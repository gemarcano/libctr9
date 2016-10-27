#include "twl_tests.h"
#include "nand_crypto_tests.h"

#include <ctr9/aes.h>
#include <stdalign.h>
#include <ctr9/ctr_system.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ctr9/io/fatfs/diskio.h>

static bool twl_test1(void *ctx)
{
	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	ctr_twl_keyslot_setup();

	int res = ctr_nand_crypto_interface_initialize(&data->io, 0x03, NAND_TWL, &data->lower_io->base);

	return !res;
}

static bool twl_test2(void *ctx)
{
	nand_crypto_test_data *data = ctx;

	bool test1 = false, test2 = false;
	int res2 = 0;

	FILE *file = fopen("TWLN:/sys/TWLFontTable.dat", "rb");
	if (file)
	{
		struct stat st;
		fstat(fileno(file), &st);
		off_t size = st.st_size;
		fclose(file);
		test1 = size == 863296;
	}

	if ((file = fopen("TWLP:/photo/private/ds/app/484E494A/pit.bin", "rb")))
	{
		struct stat st;
		fstat(fileno(file), &st);
		off_t size = st.st_size;
		fclose(file);
		test2 = size == 8032;
	}
	return !res2 && test1 && test2;
}

static bool twl_test3(void *ctx)
{
	nand_crypto_test_data *data = ctx;

	bool test1 = false;
	int res2 = 0;
	FILE *file;
	if ((file = fopen("TWLN:/shared2/0000", "r+b")))
	{
		ctr_file_interface io;
		ctr_file_interface_initialize(&io, file);

		ctr_setup_disk_parameters params = { &io, 0, ctr_io_disk_size(&io)/0x200 };
		disk_ioctl_(4, CTR_SETUP_DISK, &params);

		FILE *file2 = fopen("DISK0:/foobar.txt", "wb");
		printf("Did it open: %d\n", file2 != NULL);
		size_t written = fwrite("FOOBAR\nhello world!!!", sizeof("FOOBAR\nhello world!!!")-1, 1, file2);
		printf("this many elements got written: %d\n", written);
		test1 = file2 != NULL && written == 1;
		fclose(file2);
	}

	return !res2 && test1;
}

void twl_tests_initialize(ctr_unit_tests *twl_crypto_tests, ctr_unit_test *funcs, size_t number_of_funcs, void *twl_crypto_ctx)
{
	ctr_unit_tests_initialize(twl_crypto_tests, "TWL tests", funcs, 3);
	ctr_unit_tests_add_test(twl_crypto_tests, (ctr_unit_test){ "twl_crypto_initialize", twl_crypto_ctx, twl_test1 });
	ctr_unit_tests_add_test(twl_crypto_tests, (ctr_unit_test){ "twl mount and read", twl_crypto_ctx, twl_test2 });
	ctr_unit_tests_add_test(twl_crypto_tests, (ctr_unit_test){ "twl mount nested", twl_crypto_ctx, twl_test3 });
}
