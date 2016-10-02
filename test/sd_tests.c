#include "sd_tests.h"
#include <ctr9/io.h>
#include <string.h>

static bool sd_test1(void *ctx)
{
	sd_test_data *data = ctx;

	int res = ctr_sd_interface_initialize(&data->io);

	return !res;
}

static bool sd_test2(void *ctx)
{
	sd_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	uint32_t loc = 0;
	int res = ctr_io_read(&data->io, buffer, buffer_size/2, loc, buffer_size/2);
	res |= ctr_io_read_sector(&data->io, buffer+buffer_size/2, buffer_size/2, loc /0x200, buffer_size/2/512);
	bool test1 = memcmp(buffer, buffer + buffer_size/2, buffer_size/2) == 0;

	res |= ctr_io_read(&data->io, buffer, buffer_size/2, loc+3, buffer_size/2-4);

	bool test2 = memcmp(buffer, buffer + buffer_size/2 + 3, buffer_size/2 - 4) == 0;

	return !res && test1 && test2;
}

static bool sd_test3(void *ctx)
{
	FATFS fs = { 0 };
	FIL test_file = { 0 };

	sd_test_data *data = ctx;
	ctr_setup_disk_parameters params = {&data->io, 0, ctr_io_disk_size(&data->io)};
	disk_ioctl(3, CTR_SETUP_DISK, &params);

	int res2 = 0;
	if ((res2 = f_mount(&fs, "SD:", 1)) == FR_OK &&
	(res2 = f_open(&test_file, "SD:/otp.bin", FA_READ)) == FR_OK)
	{
		size_t size = f_size(&test_file);
		f_close(&test_file);
		return size == 256;
	}
	return false;
}

static bool sd_test4(void *ctx)
{
	FATFS fs = { 0 };
	FIL test_file = { 0 };

	sd_test_data *data = ctx;
	ctr_setup_disk_parameters params = {&data->io, 0, ctr_io_disk_size(&data->io)};
	disk_ioctl(3, CTR_SETUP_DISK, &params);

	const char * string = "HELLO WORLD!!!";

	int res2 = 0;
	if ((res2 = f_mount(&fs, "SD:", 1)) == FR_OK &&
	(res2 = f_open(&test_file, "SD:/test_data.txt", FA_WRITE | FA_READ  | FA_OPEN_ALWAYS)) == FR_OK)
	{
		UINT written;
		f_write(&test_file, string, strlen(string), &written);
		size_t size = f_size(&test_file);
		f_close(&test_file);
		return size == strlen(string);
	}
	return false;

}

void sd_tests_initialize(ctr_unit_tests *sd_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *sd_ctx)
{
	ctr_unit_tests_initialize(sd_tests, "SD tests", funcs, 4);
	ctr_unit_tests_add_test(sd_tests, (ctr_unit_test){ "ctr_sd_initialize", sd_ctx, sd_test1});
	ctr_unit_tests_add_test(sd_tests, (ctr_unit_test){ "ctr_sd_read* compare", sd_ctx, sd_test2});
	ctr_unit_tests_add_test(sd_tests, (ctr_unit_test){ "ctr_sd_interface FATFS read", sd_ctx, sd_test3});
	ctr_unit_tests_add_test(sd_tests, (ctr_unit_test){ "ctr_sd_interface FATFS write", sd_ctx, sd_test4});
}
