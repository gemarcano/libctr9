#include <ctr9/io/ctr_console_dotab.h>

#include <sys/iosupport.h>
#include <ctr9/io.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <ctr/console.h>
#include <ctr/printf.h>

static ssize_t ctr_console_dotab_write_r(struct _reent *r, int fd, const char *ptr, size_t len);

static const devoptab_t tab =
{
	"con",
	0,
	NULL,
	NULL,
	ctr_console_dotab_write_r,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

int ctr_console_dotab_initialize(void)
{
	devoptab_list[STD_OUT] = &tab;
	devoptab_list[STD_ERR] = &tab;
	setvbuf(stdout, NULL , _IONBF, 0);
	setvbuf(stderr, NULL , _IONBF, 0);;
	return 0;
}

static ssize_t ctr_console_dotab_write_r(struct _reent *r, int fd, const char *ptr, size_t len)
{
	for (size_t i = 0; i < len; ++i)
	{
		console_putc(NULL, ptr[i]);
	}
	return (ssize_t)len;
}


