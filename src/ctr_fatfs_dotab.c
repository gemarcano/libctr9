#include <ctr9/io/ctr_fatfs_dotab.h>
#include <ctr9/io/ctr_fatfs.h>

#include <ctr9/io/fatfs/ff.h>
#include <sys/iosupport.h>
#include <ctr9/io.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

static int error_map[FR_INVALID_PARAMETER+1] = {
	0, //OK
	EIO, //hard error
	EIO, //assert fail
	EIO, //physical drive cannot work
	ENOENT, //File not found
	ENOENT, //path not found
	ENOENT, //Invalid path
	EACCES, //Permision denied or full
	EACCES, //Permision denied
	EBADF, //file or directory object in bad state
	EACCES, //write protected
	ENODEV, //invalid drive
	EIO, //not enabled
	EIO, //no filesystem
	EIO, //mkfs error
	EIO, //timeout trying to access volume
	ENOLCK, //not able to get lock
	ENOMEM, //Not enough memory available to allocate working buffer
	ENOLCK, //Too many open files
	EINVAL // invalid parameter
};

static_assert(_VOLUMES == 10, "_VOLUMES in fatfs must be 10!");

static FATFS fatfs[_VOLUMES];
static ctr_nand_interface nand;
static ctr_sd_interface sd;
static ctr_nand_crypto_interface ctr;
static ctr_nand_crypto_interface twl;

typedef struct
{
	const char *drive;
	char filename[_MAX_LFN + 1];
	FIL file;
} FIL_extension;

static inline int process_error(struct _reent *r, int err)
{
	if (err)
	{
		r->_errno = error_map[err];
		return -1;
	}
	return 0;
}

static int ctr_fatfs_dotab_open_r(const char *drive, struct _reent *r, void *fileStruct, const char *path, int flags, int mode)
{
	FIL_extension *file_ex = fileStruct;
	file_ex->drive = drive;
	BYTE file_mode;
	switch(flags & O_ACCMODE)
	{
		case O_WRONLY:
			file_mode = FA_WRITE;
			break;

		case O_RDONLY:
			file_mode = FA_READ;
			break;

		case O_RDWR:
			file_mode = FA_READ | FA_WRITE;
			break;

		default:
			r->_errno = EINVAL;
			return -1;
	}

	if (flags & O_TRUNC)
		file_mode |= FA_CREATE_ALWAYS;

	if (flags & O_CREAT)
	{
		if (flags & O_EXCL)
			file_mode |= FA_CREATE_NEW;
		else if (!(flags & O_TRUNC))
			file_mode |= FA_OPEN_ALWAYS;
	}

	f_chdrive_(drive);
	int err = f_open_(&file_ex->file, path, file_mode);
	if (err != FR_OK)
	{
		r->_errno = error_map[err]; /*FIXME, map fatfs errors to whatever devoptabs use*/
		return -1;
	}

	if (flags & O_APPEND)
		err = f_lseek_(&file_ex->file, f_size_(&file_ex->file));

	if (err != FR_OK)
	{
		return process_error(r, err);
	}

	strncpy(file_ex->filename, path, sizeof(file_ex->filename) - 2);
	file_ex->filename[sizeof(file_ex->filename) - 1] = '\0'; //Make sure path is null terminated

	return (int)file_ex;
}

static int ctr_fatfs_dotab_close_r(struct _reent *r, int fd)
{
	FIL_extension *file_ex = (FIL_extension*)fd;
	int err = f_close_(&file_ex->file);

	return process_error(r, err);
}

static ssize_t ctr_fatfs_dotab_write_r(const char *drive, struct _reent *r, int fd, const char *ptr, size_t len)
{
	FIL_extension *file_ex = (FIL_extension*)fd;
	UINT wb;
	f_chdrive_(drive);
	int err = f_write_(&file_ex->file, ptr, len, &wb);

	if (process_error(r, err)) return -1;

	f_sync_(&file_ex->file);
	return (ssize_t)wb;
}

static ssize_t ctr_fatfs_dotab_read_r(const char *drive, struct _reent *r, int fd, char *ptr, size_t len)
{
	FIL_extension *file_ex = (FIL_extension*)fd;
	UINT rb;
	f_chdrive_(drive);
	int err = f_read_(&file_ex->file, ptr, len, &rb);

	if (process_error(r, err)) return -1;

	return (ssize_t)rb;
}

static off_t ctr_fatfs_dotab_seek_r(struct _reent *r, int fd, off_t pos, int dir)
{
	FIL_extension *file_ex = (FIL_extension*)fd;
	size_t offset = 0;
	switch (dir)
	{

	case SEEK_END:
		offset = f_size_(&file_ex->file) - (size_t)pos;
		break;
	case SEEK_CUR:
		offset = f_tell_(&file_ex->file) + (size_t)pos; /*Even if pos is negative, modular arithmetic takes care of this working*/
		break;
	case SEEK_SET:
		offset = (size_t)pos;
		break;
	default:
		r->_errno = EINVAL;
		return -1;
	}

	int err = f_lseek_(&file_ex->file, offset);

	if (process_error(r, err)) return -1;

	return (off_t)f_tell_(&file_ex->file);
}

static int ctr_fatfs_dotab_stat_r(const char *drive, struct _reent *r, const char *file, struct stat *st);
static int ctr_fatfs_dotab_fstat_r(struct _reent *r, int fd, struct stat *st)
{
	FIL_extension *file_ex = (FIL_extension*)fd;
	return ctr_fatfs_dotab_stat_r(file_ex->drive, r, file_ex->filename, st);
}

static time_t fatfs_time_to_time_t(uint16_t time, uint16_t date)
{
	struct tm t;
	t.tm_year = (date >> 9) + 80;
	t.tm_mon = ((date >> 5) & 0x0F) - 1;
	t.tm_mday = date & 0x1F;
	t.tm_hour = time >> 11;
	t.tm_min = (time >> 5) & 0x3F;
	t.tm_sec = (time & 0x1F) << 1;
	t.tm_isdst = 0;

	return mktime(&t);
}

static int ctr_fatfs_dotab_stat_r(const char *drive, struct _reent *r, const char *file, struct stat *st)
{
	FILINFO info;
	f_chdrive_(drive);
	int err = f_stat_(file, &info);
	if (process_error(r, err)) return -1;

	memset(st, 0, sizeof(*st));
	st->st_mode = (info.fattrib & AM_DIR) ? S_IFDIR : 0;
	st->st_nlink = 1;
	st->st_uid = 1;
	st->st_gid = 1;
	st->st_rdev = st->st_dev;
	st->st_size = (off_t)info.fsize;
	st->st_mtime = fatfs_time_to_time_t(info.ftime, info.fdate);
	st->st_spare1 = info.fattrib;

	return process_error(r, err);
}

static int ctr_fatfs_dotab_link_r(struct _reent *r, const char *existing, const char  *newLink)
{
	r->_errno = ENOSYS;
	return -1;
}

static int ctr_fatfs_dotab_unlink_r(const char *drive, struct _reent *r, const char *name)
{
	f_chdrive_(drive);
	int err = f_unlink_(name);

	return process_error(r, err);
}

static int ctr_fatfs_dotab_chdir_r(const char *drive, struct _reent *r, const char *name)
{
	f_chdrive_(drive);
	int err = f_chdir_(name);

	return process_error(r, err);
}

static int ctr_fatfs_dotab_rename_r(const char *drive, struct _reent *r, const char *oldName, const char *newName)
{
	f_chdrive_(drive);
	int err = f_rename_(oldName, newName);

	return process_error(r, err);
}

static int ctr_fatfs_dotab_mkdir_r(const char *drive, struct _reent *r, const char *path, int mode)
{
	f_chdrive_(drive);
	int err = f_mkdir_(path);

	return process_error(r, err);
}

static DIR_ITER* ctr_fatfs_dotab_diropen_r(struct _reent *r, DIR_ITER *dirState, const char *path)
{
	return NULL;/*FIXME*/
}

static int ctr_fatfs_dotab_dirreset_r(struct _reent *r, DIR_ITER *dirState)
{
	return -1;/*FIXME*/
}

static int ctr_fatfs_dotab_dirnext_r(struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat)
{
	return -1; /*FIXME*/
}

static int ctr_fatfs_dotab_dirclose_r(struct _reent *r, DIR_ITER *dirState)
{
	return -1;/*FIXME*/
}

static int ctr_fatfs_dotab_statvfs_r(struct _reent *r, const char *path, struct statvfs *buf)
{
	return -1;
}

static int ctr_fatfs_dotab_ftruncate_r(struct _reent *r, int fd, off_t len)
{
	FIL_extension *file_ex = (FIL_extension*)fd;
	int err = f_truncate_(&file_ex->file);

	return process_error(r, err);
}

static int ctr_fatfs_dotab_fsync_r(struct _reent *r, int fd)
{
	FIL_extension *file_ex = (FIL_extension*)fd;
	int err = f_sync_(&file_ex->file);

	return process_error(r, err);
}

static int ctr_fatfs_dotab_chmod_r(struct _reent *r, const char *path, mode_t mode)
{
	return -1;
}

static int ctr_fatfs_dotab_fchmod_r(struct _reent *r, int fd, mode_t mode)
{
	return -1;
}

static int ctr_fatfs_dotab_rmdir_r(struct _reent *r, const char *name)
{
	return -1; /*FIXME this can be figure out*/
}

#define PREPARE_DOTAB(DEV) \
static int DEV##_dotab_open_r(struct _reent *r, void *fileStruct, const char *path, int flags, int mode);\
static ssize_t DEV##_dotab_write_r(struct _reent *r, int fd, const char *ptr, size_t len);\
static ssize_t DEV##_dotab_read_r(struct _reent *r, int fd, char *ptr, size_t len);\
static int DEV##_dotab_stat_r(struct _reent *r, const char *file, struct stat *st);\
static int DEV##_dotab_unlink_r(struct _reent *r, const char *name);\
static int DEV##_dotab_chdir_r(struct _reent *r, const char *name);\
static int DEV##_dotab_rename_r(struct _reent *r, const char *oldName, const char *newName);\
static int DEV##_dotab_mkdir_r(struct _reent *r, const char *path, int mode);\
\
static const devoptab_t DEV##_tab =\
{\
	#DEV,\
	sizeof(FIL_extension),\
	DEV##_dotab_open_r,\
	ctr_fatfs_dotab_close_r,\
	DEV##_dotab_write_r,\
	DEV##_dotab_read_r,\
	ctr_fatfs_dotab_seek_r,\
	ctr_fatfs_dotab_fstat_r,\
	DEV##_dotab_stat_r,\
	ctr_fatfs_dotab_link_r,\
	DEV##_dotab_unlink_r,\
	DEV##_dotab_chdir_r,\
	DEV##_dotab_rename_r,\
	DEV##_dotab_mkdir_r,\
	sizeof(int), /*FIXME DIR element size, include FILINFO inside*/\
	ctr_fatfs_dotab_diropen_r,\
	ctr_fatfs_dotab_dirreset_r,\
	ctr_fatfs_dotab_dirnext_r,\
	ctr_fatfs_dotab_dirclose_r,\
	ctr_fatfs_dotab_statvfs_r,\
	ctr_fatfs_dotab_ftruncate_r,\
	ctr_fatfs_dotab_fsync_r,\
	NULL,\
	NULL,\
	NULL,\
	NULL\
};\
\
static int DEV##_dotab_open_r(struct _reent *r, void *fileStruct, const char *path, int flags, int mode)\
{\
	return ctr_fatfs_dotab_open_r(#DEV":", r, fileStruct, path, flags, mode);\
}\
\
static ssize_t DEV##_dotab_write_r(struct _reent *r, int fd, const char *ptr, size_t len)\
{\
	return ctr_fatfs_dotab_write_r(#DEV":", r, fd, ptr, len);\
}\
\
static ssize_t DEV##_dotab_read_r(struct _reent *r, int fd, char *ptr, size_t len)\
{\
	return ctr_fatfs_dotab_read_r(#DEV":", r, fd, ptr, len);\
}\
\
static int DEV##_dotab_stat_r(struct _reent *r, const char *file, struct stat *st)\
{\
	return ctr_fatfs_dotab_stat_r(#DEV":", r, file, st);\
}\
\
static int DEV##_dotab_unlink_r(struct _reent *r, const char *name)\
{\
	return ctr_fatfs_dotab_unlink_r(#DEV":", r, name);\
}\
\
static int DEV##_dotab_chdir_r(struct _reent *r, const char *name)\
{\
	return ctr_fatfs_dotab_chdir_r(#DEV":", r, name);\
}\
\
static int DEV##_dotab_rename_r(struct _reent *r, const char *oldName, const char *newName)\
{\
	return ctr_fatfs_dotab_rename_r(#DEV":", r, oldName, newName);\
}\
\
static int DEV##_dotab_mkdir_r(struct _reent *r, const char *path, int mode)\
{\
	return ctr_fatfs_dotab_mkdir_r(#DEV":", r, path, mode);\
}

PREPARE_DOTAB(SD)
PREPARE_DOTAB(CTRNAND)
PREPARE_DOTAB(TWLN)
PREPARE_DOTAB(TWLP)
PREPARE_DOTAB(DISK0)
PREPARE_DOTAB(DISK1)
PREPARE_DOTAB(DISK2)
PREPARE_DOTAB(DISK3)
PREPARE_DOTAB(DISK4)
PREPARE_DOTAB(DISK5)

static int dotab_initialize(const devoptab_t *tab)
{
	return -1 == AddDevice(tab);
}

static const char *valid_drives[_VOLUMES] = {
	"CTRNAND:",
	"TWLN:",
	"TWLP:",
	"SD:",
	"DISK0:",
	"DISK1:",
	"DISK2:",
	"DISK3:",
	"DISK4:",
	"DISK5:"
};

int ctr_fatfs_dotab_check_ready(const char *drive)
{
	size_t index = _VOLUMES; //select an invalid index
	for (size_t i = 0; i < _VOLUMES && index == _VOLUMES; ++i)
	{
		if (strcmp(drive, valid_drives[i]) == 0)
			index = i;
	}

	if (index >= _VOLUMES) return -1;

	return f_mount_(&fatfs[index], drive, 1);
}

int ctr_fatfs_dotab_chdrive(const char *drive)
{
	//Check that the drive requested is in the devoptab table
	int index = FindDevice(drive);
	if (index == -1) return -1;

	setDefaultDevice(index);
	return 0;
}

int ctr_fatfs_dotab_initialize(void)
{
	//FIXME allow for mounting as needed at f_open
	//Specifically, this is to allow for SD removal and such...
	ctr_fatfs_initialize(&nand, &ctr, &twl, &sd);

	f_mount_(&fatfs[0], "CTRNAND:", 0);
	f_mount_(&fatfs[1], "TWLN:", 0);
	f_mount_(&fatfs[2], "TWLP:", 0);
	f_mount_(&fatfs[3], "SD:", 0);
	f_mount_(&fatfs[4], "DISK0:", 0);
	f_mount_(&fatfs[5], "DISK1:", 0);
	f_mount_(&fatfs[6], "DISK2:", 0);
	f_mount_(&fatfs[7], "DISK3:", 0);
	f_mount_(&fatfs[8], "DISK4:", 0);
	f_mount_(&fatfs[9], "DISK5:", 0);

	return dotab_initialize(&SD_tab)
		| dotab_initialize(&TWLN_tab)
		| dotab_initialize(&SD_tab)
		| dotab_initialize(&TWLP_tab)
		| dotab_initialize(&DISK0_tab)
		| dotab_initialize(&DISK1_tab)
		| dotab_initialize(&DISK2_tab)
		| dotab_initialize(&DISK3_tab)
		| dotab_initialize(&DISK4_tab)
		| dotab_initialize(&DISK5_tab);
}

