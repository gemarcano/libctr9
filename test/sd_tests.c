#include "sd_tests.h"
#include <ctr9/io.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

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
	FILE *otp = fopen("SD:/otp.bin", "rb");
	if (otp)
	{
		struct stat st;
		fstat(fileno(otp), &st);
		off_t size = st.st_size; //FIXME fstat not implemented
		fclose(otp);
		return size == 256;
	}
	return false;
}

static bool sd_test4(void *ctx)
{
	const char * string = "HELLO WORLD!!!";

	FILE *file = fopen("SD:/test_data.txt", "wrb");
	if (file)
	{
		struct stat st;
		fwrite(string, strlen(string), 1, file);
		fflush(file);
		fstat(fileno(file), &st);
		size_t size = (size_t)(st.st_size); //fstat not yet implemented FIXME
		fclose(file);
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
