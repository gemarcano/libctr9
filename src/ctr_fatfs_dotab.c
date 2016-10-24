#include <ctr9/io/ctr_fatfs_dotab.h>

#include <sys/iosupport.h>
#include <ctr9/io.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <string.h>
#include <errno.h>

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

static inline int process_error(struct _reent *r, int err)
{
	if (err)
	{
		r->_errno = error_map[err];
		return -1;
	}
	return 0;
}

static int dotab_initialize(const devoptab_t *tab)
{
	return -1 != AddDevice(tab);
}

static int ctr_fatfs_dotab_open_r(const char *drive, struct _reent *r, void *fileStruct, const char *path, int flags, int mode)
{
	FIL *file = fileStruct;
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

	f_chdrive(drive);
	int err = f_open(file, path, file_mode);
	if (err != FR_OK)
	{
		r->_errno = error_map[err]; /*FIXME, map fatfs errors to whatever devoptabs use*/
		return -1;
	}

	if (flags & O_APPEND)
		err = f_lseek(file, f_size(file));

	if (err != FR_OK)
	{
		return process_error(r, err);
	}

	return (int)file;
}

static int ctr_fatfs_dotab_close_r(struct _reent *r, int fd)
{
	FIL *file = (FIL*)fd;
	int err = f_close(file);

	return process_error(r, err);
}

static ssize_t ctr_fatfs_dotab_write_r(const char *drive, struct _reent *r, int fd, const char *ptr, size_t len)
{
	FIL *file = (FIL*)fd;
	UINT wb;
	f_chdrive(drive);
	int err = f_write(file, ptr, len, &wb);

	if (process_error(r, err)) return -1;

	return (ssize_t)wb;
}

static ssize_t ctr_fatfs_dotab_read_r(const char *drive, struct _reent *r, int fd, char *ptr, size_t len)
{
	FIL *file = (FIL*)fd;
	UINT rb;
	f_chdrive(drive);
	int err = f_read(file, ptr, len, &rb);

	if (process_error(r, err)) return -1;

	return (ssize_t)rb;
}

static off_t ctr_fatfs_dotab_seek_r(struct _reent *r, int fd, off_t pos, int dir)
{
	FIL *file = (FIL*)fd;
	size_t offset = 0;
	switch (dir)
	{

	case SEEK_END:
		offset = f_size(file) - (size_t)pos;
		break;
	case SEEK_CUR:
		offset = f_tell(file) + (size_t)pos; /*Even if pos is negative, modular arithmetic takes care of this working*/
		break;
	case SEEK_SET:
		offset = (size_t)pos;
		break;
	default:
		r->_errno = EINVAL;
		return -1;
	}

	int err = f_lseek(file, offset);

	if (process_error(r, err)) return -1;

	return (off_t)f_tell(file);
}

static int ctr_fatfs_dotab_fstat_r(struct _reent *r, int fd, struct stat *st)
{
	r->_errno = ENOSYS;
	return -1;
}

static int ctr_fatfs_dotab_stat_r(const char *drive, struct _reent *r, const char *file, struct stat *st)
{
	FILINFO info;
	f_chdrive(drive);
	int err = f_stat(file, &info);

	return process_error(r, err);
}

static int ctr_fatfs_dotab_link_r(struct _reent *r, const char *existing, const char  *newLink)
{
	r->_errno = ENOSYS;
	return -1;
}

static int ctr_fatfs_dotab_unlink_r(const char *drive, struct _reent *r, const char *name)
{
	f_chdrive(drive);
	int err = f_unlink(name);

	return process_error(r, err);
}

static int ctr_fatfs_dotab_chdir_r(const char *drive, struct _reent *r, const char *name)
{
	f_chdrive(drive);
	int err = f_chdir(name);

	return process_error(r, err);
}

static int ctr_fatfs_dotab_rename_r(const char *drive, struct _reent *r, const char *oldName, const char *newName)
{
	f_chdrive(drive);
	int err = f_rename(oldName, newName);

	return process_error(r, err);
}

static int ctr_fatfs_dotab_mkdir_r(const char *drive, struct _reent *r, const char *path, int mode)
{
	f_chdrive(drive);
	int err = f_mkdir(path);

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
	int err = f_truncate((FIL*)fd);

	return process_error(r, err);
}

static int ctr_fatfs_dotab_fsync_r(struct _reent *r, int fd)
{
	int err = f_sync((FIL*)fd);

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
	sizeof(FIL),\
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
	return ctr_fatfs_dotab_open_r(#DEV, r, fileStruct, path, flags, mode);\
}\
\
static ssize_t DEV##_dotab_write_r(struct _reent *r, int fd, const char *ptr, size_t len)\
{\
	return ctr_fatfs_dotab_write_r(#DEV, r, fd, ptr, len);\
}\
\
static ssize_t DEV##_dotab_read_r(struct _reent *r, int fd, char *ptr, size_t len)\
{\
	return ctr_fatfs_dotab_read_r(#DEV, r, fd, ptr, len);\
}\
\
static int DEV##_dotab_stat_r(struct _reent *r, const char *file, struct stat *st)\
{\
	return ctr_fatfs_dotab_stat_r(#DEV, r, file, st);\
}\
\
static int DEV##_dotab_unlink_r(struct _reent *r, const char *name)\
{\
	return ctr_fatfs_dotab_unlink_r(#DEV, r, name);\
}\
\
static int DEV##_dotab_chdir_r(struct _reent *r, const char *name)\
{\
	return ctr_fatfs_dotab_chdir_r(#DEV, r, name);\
}\
\
static int DEV##_dotab_rename_r(struct _reent *r, const char *oldName, const char *newName)\
{\
	return ctr_fatfs_dotab_rename_r(#DEV, r, oldName, newName);\
}\
\
static int DEV##_dotab_mkdir_r(struct _reent *r, const char *path, int mode)\
{\
	return ctr_fatfs_dotab_mkdir_r(#DEV, r, path, mode);\
}

PREPARE_DOTAB(SD)
PREPARE_DOTAB(CTRNAND)
PREPARE_DOTAB(TWLN)
PREPARE_DOTAB(TWLP)

int ctr_fatfs_dotab_initialize(void)
{
	return dotab_initialize(&SD_tab)
		| dotab_initialize(&TWLN_tab)
		| dotab_initialize(&SD_tab)
		| dotab_initialize(&TWLP_tab);
}

