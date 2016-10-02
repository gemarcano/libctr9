#include "twl_tests.h"
#include "nand_crypto_tests.h"

#include <ctr9/aes.h>
#include <stdalign.h>
#include <ctr9/ctr_system.h>
#include <ctr/printf.h>

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
	FATFS fs = { 0 };
	FIL test_file = { 0 };

	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	ctr_setup_disk_parameters params = { &data->io, 0x00012E00/0x200, 0x08FB5200/0x200 };
	disk_ioctl(1, CTR_SETUP_DISK, &params);
	bool test1 = false, test2 = false;
	int res2 = 0;
	if ((res2 = f_mount(&fs, "TWLN:", 1)) == FR_OK &&
	(res2 |= f_open(&test_file, "TWLN:/sys/TWLFontTable.dat", FA_READ)) == FR_OK)
	{
		size_t size = f_size(&test_file);
		f_close(&test_file);
		test1 = size == 863296;
		ctr_fatfs_interface io;
		f_open(&test_file, "TWLN:/shared2/0000", FA_READ | FA_WRITE);
		ctr_fatfs_interface_initialize(&io, &test_file);

		ctr_io_read(&io, buffer, buffer_size, 0, 513);
	}

	params = (ctr_setup_disk_parameters){&data->io, 0x09011A00/0x200, 0x020B6600/0x200};
	disk_ioctl(2, CTR_SETUP_DISK, &params);

	if ((res2 |= f_mount(&fs, "TWLP:", 1)) == FR_OK &&
	(res2 |= f_open(&test_file, "TWLP:/photo/private/ds/app/484E494A/pit.bin", FA_READ)) == FR_OK)
	{
		size_t size = f_size(&test_file);
		f_close(&test_file);
		test2 = size == 8032;
	}
	return !res2 && test1 && test2;
}

static bool twl_test3(void *ctx)
{
	FATFS fs = { 0 }, fs2 = { 0 };
	FIL test_file = { 0 };
	FIL test_file2 = { 0 };

	nand_crypto_test_data *data = ctx;
	char *buffer = data->buffer;
	size_t buffer_size = data->buffer_size;

	ctr_io_read_sector(&data->io, buffer, buffer_size, 0x00012E00/0x200 + 1, 1);


	bool test1 = false;
	int res2 = 0;
	if ((res2 = f_mount(&fs, "TWLN:", 1)) == FR_OK &&
	(res2 |= f_open(&test_file, "TWLN:/shared2/0000", FA_READ | FA_WRITE)) == FR_OK)
	{
		ctr_fatfs_interface io;
		ctr_fatfs_interface_initialize(&io, &test_file);

		ctr_setup_disk_parameters params = { &io, 0, ctr_io_disk_size(&data->io)/0x200 };
		disk_ioctl(4, CTR_SETUP_DISK, &params);

		ctr_io_read(&io, buffer, buffer_size, 0, 513);
		test1 = FR_OK == f_mount(&fs2, "DISK1:", 1);

		int res3 = f_open(&test_file2, "DISK1:/foobar.txt", FA_WRITE | FA_OPEN_ALWAYS);
		printf("Did it open: %d\n", res3);
		UINT written;
		int res4 = f_write(&test_file2, "FOOBAR\nhello world!!!", sizeof("FOOBAR\nhello world!!!")-1,&written);
		printf("this much got written: %d\n, status: %d\n", written, res4);
		f_close(&test_file2);

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
