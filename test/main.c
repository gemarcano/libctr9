#include <ctr9/io.h>

#include <string.h>
#include <stdbool.h>
#include <ctr/console.h>
#include <ctr/printf.h>
#include <ctr/draw.h>
#include <ctr/headers.h>
#include <ctr/hid.h>

#include <ctr9/i2c.h>
#include <ctr9/io/fatfs/ff.h>
#include <ctr9/io/fatfs/diskio.h>
#include <ctr9/ctr_system.h>
#include <ctr9/ctr_interrupt.h>
#include <ctr9/ctr_screen.h>

#include <ctr9/ctr_rtc.h>
#include <ctr9/io/ctr_cart_interface.h>
#include <ctr9/ctr_timer.h>
#include <ctr9/ctr_system_clock.h>
#include <ctr9/ctr_irq.h>
#include <ctr9/ctr_gfx.h>
#include <ctr9/ctr_cache.h>

int DEBUG = 0;

#include "test.h"

void scribble_screen(void);

#include "nand_tests.h"
#include "nand_crypto_tests.h"
#include "sd_tests.h"
#include "twl_tests.h"
#include "memory_tests.h"
#include "crypto_memory_tests.h"


typedef struct
{
	char *buffer;
	size_t buffer_size;

	ctr_crypto_interface io;
	ctr_io_interface *lower_io;
	uint32_t ctr[4];

} crypto_test_data;

#include <ctr9/aes.h>
#include <stdalign.h>

static bool crypto_tests1(void *ctx)
{
	crypto_test_data *data = ctx;
	int res = ctr_crypto_interface_initialize(&(data->io), 0x04, 1, CTR_CRYPTO_ENCRYPTED, CRYPTO_CTR, data->ctr, data->lower_io);
	return false;
}


#include <ctr9/io/ctr_fatfs.h>

extern void(*ctr_interrupt_handlers[7])(const uint32_t*);
void abort_interrupt(uint32_t*);
void prefetch_abort(uint32_t*);
void undefined_instruction(uint32_t*);

int main()
{
	draw_s *cakehax_fbs = (draw_s*)0x23FFFE00;
	ctr_gfx_screen top_screen, bottom_screen;
	ctr_gfx_screen_initialize(&top_screen, cakehax_fbs->top_left, 400, 240, CTR_GFX_PIXEL_RGB8);
	ctr_gfx_screen_initialize(&bottom_screen, cakehax_fbs->sub, 320, 240, CTR_GFX_PIXEL_RGB8);

	draw_init(cakehax_fbs);
	console_init(0xFFFFFF, 0);
	draw_clear_screen(SCREEN_TOP, 0x111111);
	printf("UNIT TESTING\n");

	ctr_interrupt_prepare();
	ctr_interrupt_set(CTR_INTERRUPT_DATABRT, abort_interrupt);
	ctr_interrupt_set(CTR_INTERRUPT_UNDEF, undefined_instruction);
	ctr_interrupt_set(CTR_INTERRUPT_PREABRT, prefetch_abort);

	char buffer[0x1000] = {0};
	nand_test_data nand_ctx = {buffer, sizeof(buffer), {{0}} };
	nand_crypto_test_data nand_crypto_ctx = {buffer, sizeof(buffer), {{0}}, &nand_ctx.nand_io};
	nand_crypto_test_data twl_crypto_ctx = {buffer, sizeof(buffer), {{0}}, &nand_ctx.nand_io};
	sd_test_data sd_ctx = {buffer, sizeof(buffer), {{0}}};
	memory_test_data memory_ctx = {buffer, sizeof(buffer), {0}, {{0}}};
	nand_crypto_test_data crypto_memory_ctx = {buffer, sizeof(buffer), {{0}}, &memory_ctx.mem_io};

	ctr_unit_test nand_tests_f[11];
	ctr_unit_tests nand_tests;
	nand_tests_initialize(&nand_tests, nand_tests_f, 11, &nand_ctx);

	ctr_unit_test nand_crypto_tests_f[6];
	ctr_unit_tests nand_crypto_tests;
	nand_crypto_tests_initialize(&nand_crypto_tests, nand_crypto_tests_f, 6, &nand_crypto_ctx);

	ctr_unit_test sd_tests_f[4];
	ctr_unit_tests sd_tests;
	sd_tests_initialize(&sd_tests, sd_tests_f, 4, &sd_ctx);

	ctr_unit_test twl_crypto_tests_f[3];
	ctr_unit_tests twl_crypto_tests;
	twl_tests_initialize(&twl_crypto_tests, twl_crypto_tests_f, 3, &twl_crypto_ctx);

	ctr_unit_test memory_tests_f[5];
	ctr_unit_tests memory_tests;
	memory_tests_initialize(&memory_tests, memory_tests_f, 5, &memory_ctx);

	ctr_unit_test crypto_memory_tests_f[5];
	ctr_unit_tests crypto_memory_tests;
	crypto_memory_tests_initialize(&crypto_memory_tests, crypto_memory_tests_f, 5, &crypto_memory_ctx);

	int res = ctr_execute_unit_tests(&nand_tests);
	res |= ctr_execute_unit_tests(&nand_crypto_tests);
	res |= ctr_execute_unit_tests(&sd_tests);
	res |= ctr_execute_unit_tests(&twl_crypto_tests);
	res |= ctr_execute_unit_tests(&memory_tests);
	res |= ctr_execute_unit_tests(&crypto_memory_tests);

	FATFS fs = { 0 };
	FIL test_file = { 0 };

	ctr_setup_disk_parameters params = {&nand_crypto_ctx.io, 0x0B930000/0x200, 0x2F5D0000/0x200};
	disk_ioctl(0, CTR_SETUP_DISK, &params);

	int res2 = 0;
	printf("trying to mount\n");
	if ((res2 = f_mount(&fs, "CTRNAND:", 1)) != FR_OK)
	{
		printf("WTF MOUNT FAILED; %d\n", res2);
	}
	else if ((res2 = f_open(&test_file, "CTRNAND:/rw/sys/SecureInfo_A", FA_READ)) != FR_OK)
	{
		printf("WTF READ OPEN FAILED; %d\n", res2);
	}
	else
	{
		printf("Size: %d\n", f_size(&test_file));
	}

	int res3 = ctr_fatfs_initialize(&(nand_ctx.nand_io), NULL, NULL, &(sd_ctx.io));
	printf("ctr_fatfs_initialize result: %X\n", res3);

	ctr_sd_interface_destroy(&sd_ctx.io);
	ctr_nand_crypto_interface_destroy(&nand_crypto_ctx.io);
	ctr_nand_interface_destroy(&nand_ctx.nand_io);

	printf("Press any key to continue...\n");
	input_wait();

	printf("Preparing interrupts\n");
		printf("abort handler: %X\n", ctr_interrupt_handlers[4]);
	printf("testing abort\n");

	//Cause a data abort :P
	*(volatile u32*)0xFFFFFFF0;
	printf("Returned from the abort.\n");

	printf("Trying to turn off top screen\n");
	input_wait();
	ctr_screen_disable_backlight(CTR_SCREEN_TOP);
	input_wait();
	ctr_screen_enable_backlight(CTR_SCREEN_TOP);
	printf("done: Trying to turn off top screen\n");

	printf("Trying to turn off bottom screen\n");
	input_wait();
	ctr_screen_disable_backlight(CTR_SCREEN_BOTTOM);
	input_wait();
	ctr_screen_enable_backlight(CTR_SCREEN_BOTTOM);
	printf("done: Trying to turn off bottom screen\n");

	printf("Trying to turn off both screens\n");
	input_wait();
	ctr_screen_disable_backlight(CTR_SCREEN_BOTH);
	input_wait();
	ctr_screen_enable_backlight(CTR_SCREEN_BOTTOM | CTR_SCREEN_TOP);
	printf("Trying to turn off both screens\n");
	input_wait();

	printf("Testing i2c write crap\n");

	ctr_rtc_data rtc = ctr_rtc_gettime();
	printf("%d %d %d %d %d %d\n", rtc.seconds, rtc.minutes, rtc.hours, rtc.day, rtc.month, rtc.year);
	printf("Testing a single write\n");
	input_wait();
	i2cWriteRegister(I2C_DEV_MCU, 0x30, 0x11);
	rtc = ctr_rtc_gettime();
	printf("%d %d %d %d %d %d\n", rtc.seconds, rtc.minutes, rtc.hours, rtc.day, rtc.month, rtc.year);

	printf("Testing a single write from buffer\n");
	input_wait();
	uint8_t temp_rtc[8] = { 11, 22, 33, 44, 0xBB, 0xAA,  0x99, 0x88};
	i2cWriteRegisterBuffer(I2C_DEV_MCU, 0x30, temp_rtc, 1);
	rtc = ctr_rtc_gettime();
	printf("%d %d %d %d %d %d\n", rtc.seconds, rtc.minutes, rtc.hours, rtc.day, rtc.month, rtc.year);

	printf("Testing a multiple write from buffer\n");
	input_wait();
	i2cWriteRegisterBuffer(I2C_DEV_MCU, 0x30, temp_rtc, 8);
	rtc = ctr_rtc_gettime();
	printf("%d %d %d %d %d %d\n", rtc.seconds, rtc.minutes, rtc.hours, rtc.day, rtc.month, rtc.year);
/*
	printf("Trying to read cart header...\n");
	ctr_cart_interface cart;
	printf("Initialize cart: %d\n", ctr_cart_interface_initialize(&cart));
	printf("%c%c%c%c\n", cart.ncsd_header.magic[0], cart.ncsd_header.magic[1], cart.ncsd_header.magic[2], cart.ncsd_header.magic[3]);
	printf("%c%c%c%c\n", cart.ncch_header.magic[0], cart.ncch_header.magic[1], cart.ncch_header.magic[2], cart.ncch_header.magic[3]);

	uint8_t temporary_cart[0x8000] = {0};
	printf("Cart inserted? : %d\n", ctr_cart_inserted());

	if (!cart.media_unit_size) cart.media_unit_size = 512; //Make sure this is zero, because lazy
	ctr_io_read_sector(&cart, temporary_cart, 0x8000, 0, 0x8000 / cart.media_unit_size);

	printf("cart media unit size: %d\n", cart.media_unit_size);
	printf("Finished reading, now trying to dump to SD.\n");
	ctr_sd_interface sd_interface;
	ctr_fatfs_initialize(NULL, NULL, NULL, &sd_interface);
	//FATFS fs;
	FIL file;
	f_mount(&fs, "SD:", 0);
	int open_res = f_open(&file, "SD:/dump.dump.bin", FA_WRITE | FA_CREATE_ALWAYS);
	unsigned int bw;
	int write_res = f_write(&file, temporary_cart, 0x8000, &bw);
	ctr_io_read(&cart, temporary_cart, 0x8000, 0x100, 4);
	f_write(&file, temporary_cart, 4, &bw);
	f_close(&file);
	printf("ROM SIZE: %X\n", ctr_cart_interface_disk_size(&cart));
	printf("Finished dumping, hopefully.: %d\n %d %d\n", open_res, write_res, bw);
*/
	mmcdevice dev1= *getMMCDevice(1);
	mmcdevice dev0= *getMMCDevice(0);
	printf("SDHandle: %d\n", dev0.isSDHC);
	printf("SDHandle: %d\n", dev1.isSDHC);

	printf("Trying timer stuff\n");
	ctr_timer_disable_irq(CTR_TIMER0);
	uint16_t starting_timer = ctr_timer_get_value(CTR_TIMER0);
	ctr_timer_set_prescaler(CTR_TIMER0, CTR_TIMER_DIV1);
	ctr_timer_set_count_up(CTR_TIMER0, false);
	ctr_timer_disable(CTR_TIMER0);
	ctr_timer_set_value(CTR_TIMER0, 0);

	printf("Timer value: %u\n", starting_timer);
	printf("Timer reg: %08X\n", *(uint32_t*)CTR_TIMER_REG_BASE);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);

	ctr_timer_enable(CTR_TIMER0);

	printf("Timer effective fq: %u\n", ctr_timer_get_effective_frequency(CTR_TIMER0));
	printf("Timer reg: %08X\n", *(uint32_t*)CTR_TIMER_REG_BASE);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);

	ctr_timer_set_prescaler(CTR_TIMER0, CTR_TIMER_DIV64);

	printf("Timer effective fq: %u\n", ctr_timer_get_effective_frequency(CTR_TIMER0));
	printf("Timer reg: %08X\n", *(uint32_t*)CTR_TIMER_REG_BASE);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);

	ctr_timer_set_prescaler(CTR_TIMER0, CTR_TIMER_DIV256);

	printf("Timer effective fq: %u\n", ctr_timer_get_effective_frequency(CTR_TIMER0));
	printf("Timer reg: %08X\n", *(uint32_t*)CTR_TIMER_REG_BASE);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);

	ctr_timer_set_prescaler(CTR_TIMER0, CTR_TIMER_DIV1024);

	printf("Timer effective fq: %u\n", ctr_timer_get_effective_frequency(CTR_TIMER0));
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);
	printf("Timer reg: %08X\n", *(uint32_t*)CTR_TIMER_REG_BASE);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);
	printf("Timer value delta: %u\n", ctr_timer_get_value(CTR_TIMER0) - starting_timer);

	input_wait();
	printf("Testing clock\n");
	ctr_irq_initialize();
	ctr_system_clock clock;
	ctr_system_clock_initialize(&clock, CTR_TIMER0);
	ctr_irq_master_enable();

	ctr_gfx_screen_set_pixel(&top_screen, 0 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 2 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 4 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 0 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 0 + 100, 4, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 2 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 4 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 2 + 100, 4, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&top_screen, 4 + 100, 4, 0xFF00FFu);

	for (size_t i = 0; i < 3; ++i)
	{
		uint64_t start = ctr_system_clock_get_ms(&clock);
		uint64_t ms = start;
		while (ms - start < 1000)
		{
			ms = ctr_system_clock_get_ms(&clock);
		}
		printf("second: %d\n", i);
	}


	ctr_gfx_screen_set_pixel(&bottom_screen, 0 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 2 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 4 + 100, 0, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 0 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 0 + 100, 4, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 2 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 4 + 100, 2, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 2 + 100, 4, 0xFF00FFu);
	ctr_gfx_screen_set_pixel(&bottom_screen, 4 + 100, 4, 0xFF00FFu);


	uint8_t bitmap_data[][3] = {{0xFF, 0xFF, 0xFF}, { 0xFF, 0x00, 0x81 }, {0x80, 0x01, 0x01}, {0xFF, 0xFF, 0xFF}};
	ctr_gfx_bitmap bitmap = { 20, 4, bitmap_data };
	ctr_gfx_screen_draw_bitmap(&top_screen, 0, 0, 0xFF00FF, &bitmap);
	ctr_gfx_screen_draw_bitmap(&top_screen, 20, 4, 0x00FFFF, &bitmap);
	ctr_gfx_screen_draw_bitmap(&top_screen, 40, 8, 0x0000FF, &bitmap);
	ctr_gfx_screen_draw_bitmap(&top_screen, 20, 14, 0x00FF00, &bitmap);
	ctr_gfx_screen_draw_bitmap(&top_screen, 0, 20, 0xFF0000, &bitmap);
	ctr_gfx_screen_draw_bitmap(&bottom_screen, 200, 200, 0xFF0000, &bitmap);

	printf("Testing aes\n");

	char input_buffer[0x10 * 0x20000] = { 1,2,3,4,5,6 };
	for (size_t i = 0; i < sizeof(input_buffer); ++i)
	{
		input_buffer[i] = i;
	}
	char output_buffer[0x10 * 0x20000];
	char output_buffer2[0x10 * 0x20000];
	char output_buffer3[0x10 * 0x20000];
	uint8_t ctr[16] = {0};
	uint8_t ctr2[16] = {0};

	set_ctr(ctr);
	aes_decrypt(input_buffer, output_buffer, 5, AES_CTR_MODE);
	set_ctr(ctr);
	aes_decrypt(output_buffer, output_buffer2, 5, AES_CTR_MODE);
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer2, 0x10*4));
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer2, 0x10*5));

	memset(ctr, 0, sizeof(ctr));
	memset(ctr2, 0, sizeof(ctr2));
	ctr_decrypt(input_buffer, output_buffer, 0x20000, AES_CTR_MODE, ctr);
	set_ctr(ctr2);
	aes_decrypt(input_buffer, output_buffer2, 0x20000, AES_CTR_MODE);

	printf("Comparing mid aes results: %d\n", memcmp(output_buffer, output_buffer2, sizeof(output_buffer)));

	memset(ctr, 0, sizeof(ctr));
	ctr_decrypt(input_buffer, output_buffer, 0x20000, AES_CTR_MODE, ctr);
	memset(ctr2, 0, sizeof(ctr2));
	ctr_decrypt(output_buffer, output_buffer3, 0x20000, AES_CTR_MODE, ctr2);
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer3, 0x10*4));
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer3, sizeof(output_buffer)));

	memset(ctr2, 0, sizeof(ctr2));
	set_ctr(ctr2);
	aes_decrypt(input_buffer, output_buffer, 0x20000, AES_CTR_MODE);
	set_ctr(ctr2);
	aes_decrypt(output_buffer, output_buffer2, 0x20000, AES_CTR_MODE);
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer2, 0x10*4));
	printf("Comparing aes results: %d\n", memcmp(input_buffer, output_buffer2, sizeof(output_buffer)));
	printf("Comparing aes results: %d\n", memcmp(output_buffer2, output_buffer3, sizeof(output_buffer)));

	alignas(4) uint8_t test_key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
	alignas(4) uint8_t test_key_i[16] = { 0x3c, 0x4f, 0xcf, 0x09, 0x88, 0x15, 0xf7, 0xab, 0xa6, 0xd2, 0xae, 0x28, 0x16, 0x15, 0x7e, 0x2b };
	alignas(4) uint8_t test_iv[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
	alignas(4) uint8_t cipher_cbc[32] = { 0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
		0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2};
	alignas(4) uint8_t cipher_ecb[32] = { 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
		0xf5, 0xd3, 0xd5, 0x85, 0x03, 0xb9, 0x69, 0x9d, 0xe7, 0x85, 0x89, 0x5a, 0x96, 0xfd, 0xba, 0xaf};
	alignas(4) uint8_t outtext[16] = { 0x6b, 0xc0, 0xbc, 0xe1, 0x2a, 0x45, 0x99, 0x91, 0xe1, 0x34, 0x74, 0x1a, 0x7f, 0x9e, 0x19, 0x25};
	alignas(4) uint8_t plaintext[32] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
		0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac ,0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51};
	alignas(4) uint8_t iv[16];
	alignas(4) uint8_t zero[16] = { 0 };
	memcpy(iv, test_iv, AES_BLOCK_SIZE);

	setup_aeskey(0x11, test_key_i);
	use_aeskey(0x11);

	cbc_decrypt(cipher_cbc, output_buffer, 0x2, AES_CNT_CBC_DECRYPT_MODE, iv);
	printf("Comparing cbc results: %d\n", memcmp(output_buffer, plaintext, AES_BLOCK_SIZE * 2));
	for (size_t i = 0; i < 32; ++i)
	{
		printf("%02X ", plaintext[i]);
	}
	printf("\n");
	for (size_t i = 0; i < 32; ++i)
	{
		printf("%02X ", output_buffer[i]);
	}

	ecb_decrypt(cipher_ecb, output_buffer, 0x2, AES_CNT_ECB_DECRYPT_MODE);
	printf("Comparing ecb results: %d\n", memcmp(output_buffer, plaintext, AES_BLOCK_SIZE * 2));
	memcpy(iv, test_iv, AES_BLOCK_SIZE);
	cbc_decrypt(plaintext, output_buffer, 0x2, AES_CNT_CBC_ENCRYPT_MODE, iv);
	printf("Comparing cbc results: %d\n", memcmp(output_buffer, cipher_cbc, AES_BLOCK_SIZE * 2));
	aes_decrypt(plaintext, output_buffer, 0x2, AES_CNT_ECB_ENCRYPT_MODE);
	printf("Comparing ecb results: %d\n", memcmp(output_buffer, cipher_ecb, AES_BLOCK_SIZE * 2));


	for (size_t i = 0; i < 0x1FFFF * 16; ++i)
	{
		output_buffer[i] = i;
	}
	memcpy(iv, test_iv, AES_BLOCK_SIZE);
	cbc_decrypt(output_buffer, output_buffer2, 0x1FFFF, AES_CNT_CBC_DECRYPT_MODE, iv);

	ctr_sd_interface sd;
	ctr_fatfs_initialize(NULL, NULL, NULL, &sd);
	FATFS fs3;
	FIL dump;
	f_mount(&fs3, "SD:", 0);
	f_open(&dump, "SD:/dump.cbc", FA_WRITE | FA_READ  | FA_CREATE_ALWAYS);
	int br;
	f_write(&dump, output_buffer2, 0x1FFFF * 16, &br);
	f_close(&dump);

	input_wait();
	ctr_system_poweroff();
	return 0;
}

