project('libctr9', 'c',
  default_options: [
    'b_lto=true',
    'b_staticpic=false',
    'c_std=c17',
    'warning_level=3',
  ],
  license: ['GPL-2.0-or-later'],
  version: '0.0.1')

link_args = [
  '-Wl,--gc-sections', '-fno-exceptions',
]

sources = files([
  'src/ctr_init.c',
  'src/aes.c',
  'src/ctr_aeskeydb.c',
  'src/ctr_cart_interface.c',
  'src/ctr_circular_buffer.c',
  'src/ctr_console.c',
  'src/ctr_crypto_interface.c',
  'src/ctr_disks.c',
  'src/ctr_drives.c',
  'src/ctr_elf_loader.c',
  'src/ctr_fatfs.c',
  'src/ctr_file_interface.c',
  'src/ctr_firm.c',
  'src/ctr_freetype.c',
  'src/ctr_headers.c',
  'src/ctr_hid.c',
  'src/ctr_interrupt.c',
  'src/ctr_io_interface.c',
  'src/ctr_irq.c',
  'src/ctr_memory.c',
  'src/ctr_memory_interface.c',
  'src/ctr_nand_crypto_interface.c',
  'src/ctr_nand_interface.c',
  'src/ctr_pxi.c',
  'src/ctr_rtc.c',
  'src/ctr_screen.c',
  'src/ctr_sd_interface.c',
  'src/ctr_system.c',
  'src/ctr_system_clock.c',
  'src/ctr_timer.c',
  'src/fatfs/ctr_fatfs_disk.c',
  'src/fatfs/diskio.c',
  'src/fatfs/ff.c',
  'src/fatfs/option/unicode.c',
  'src/gamecart/command_ctr.c',
  'src/gamecart/command_ntr.c',
  'src/gamecart/protocol.c',
  'src/gamecart/protocol_ctr.c',
  'src/gamecart/protocol_ntr.c',
  'src/ctr_i2c.c',
  'src/sdmmc/sdmmc.c',
  'src/sha.c',
  'src/crt0.s',
  'src/ctr_cache.s',
  'src/ctr_interrupt_asm.s',
  'src/ctr_memory_asm.s',
  'src/gamecart/delay.s',
  'src/sdmmc/delay.s',
])

header_files = files([
  'include/ctr9/aes.h',
  'include/ctr9/ctr_aeskeydb.h',
  'include/ctr9/ctr_cache.h',
  'include/ctr9/ctr_circular_buffer.h',
  'include/ctr9/ctr_elf_loader.h',
  'include/ctr9/ctr_firm.h',
  'include/ctr9/ctr_freetype.h',
  'include/ctr9/ctr_headers.h',
  'include/ctr9/ctr_hid.h',
  'include/ctr9/ctr_interrupt.h',
  'include/ctr9/ctr_irq.h',
  'include/ctr9/ctr_memory.h',
  'include/ctr9/ctr_pxi.h',
  'include/ctr9/ctr_rtc.h',
  'include/ctr9/ctr_screen.h',
  'include/ctr9/ctr_system.h',
  'include/ctr9/ctr_system_clock.h',
  'include/ctr9/ctr_timer.h',
  'include/ctr9/gamecart/command_ctr.h',
  'include/ctr9/gamecart/command_ntr.h',
  'include/ctr9/gamecart/protocol.h',
  'include/ctr9/gamecart/protocol_ctr.h',
  'include/ctr9/gamecart/protocol_ntr.h',
  'include/ctr9/ctr_i2c.h',
  'include/ctr9/io/ctr_cart_interface.h',
  'include/ctr9/io/ctr_console.h',
  'include/ctr9/io/ctr_crypto_interface.h',
  'include/ctr9/io/ctr_disks.h',
  'include/ctr9/io/ctr_drives.h',
  'include/ctr9/io/ctr_file_interface.h',
  'include/ctr9/io/ctr_io_implementation.h',
  'include/ctr9/io/ctr_io_interface.h',
  'include/ctr9/io/ctr_memory_interface.h',
  'include/ctr9/io/ctr_nand_crypto_interface.h',
  'include/ctr9/io/ctr_nand_interface.h',
  'include/ctr9/io/ctr_sdmmc_implementation.h',
  'include/ctr9/io/ctr_sd_interface.h',
  'include/ctr9/io/fatfs/ctr_fatfs.h',
  'include/ctr9/io/fatfs/ctr_fatfs_disk.h',
  'include/ctr9/io/fatfs/diskio.h',
  'include/ctr9/io/fatfs/ff.h',
  'include/ctr9/io/fatfs/ffconf.h',
  'include/ctr9/io/fatfs/integer.h',
  'include/ctr9/io/sdmmc/sdmmc.h',
  'include/ctr9/io.h',
  'include/ctr9/sha.h',
])

test_sources = files([
  'test/crypto_memory_tests.c',
  'test/interrupt.c',
  'test/main.c',
  'test/memory_control_tests.c',
  'test/memory_tests.c',
  'test/nand_crypto_tests.c',
  'test/nand_tests.c',
  'test/sd_tests.c',
  'test/test.c',
  'test/twl_tests.c',
])

# FIXME do not install fatfs headers
install_subdir('include', install_dir: 'include', strip_directory: true)

ctr_core = dependency('ctr_core')
ctrelf = dependency('ctrelf')

lib = library(
  'ctr9', sources,
  dependencies: [ctr_core, ctrelf],
  include_directories: 'include',
  install: true,
)

tests = executable(
  'tests', test_sources,
  include_directories: 'include',
  link_with: lib,
  dependencies: [ctr_core, ctrelf],
  install: false,
  link_args: link_args + ['-T' + meson.source_root() / 'test/arm9loaderhax.ld']
)

pkg = import('pkgconfig')

pkg.generate(lib,
  subdirs: [ 'ctr9', '.' ],
  description: 'Baremetal ARM9 library for the Nintendo 3DS',
  url: 'https://github.com/gemarcano/' + meson.project_name(),
)
