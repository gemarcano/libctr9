export PATH	:=	$(DEVKITARM)/bin:$(PATH)

CC=arm-none-eabi-gcc
CP=arm-none-eabi-g++
OC=arm-none-eabi-objcopy 
LD=arm-none-eabi-ld
AR=arm-none-eabi-gcc-ar

LIBNAME=n3ds

SRC_DIR:=src
OBJ_DIR:=obj
LIB_DIR:=lib
DEP_DIR:=obj

INCPATHS=-Iinclude

THUMBFLAGS=-mthumb -mthumb-interwork
CFLAGS= -flto -std=c11 -Os -g -mword-relocations -fomit-frame-pointer -ffast-math \
	-Wpedantic -Wall -Wextra -Wcast-align -Wcast-qual \
	-Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op \
	-Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls \
	-Wshadow -Wsign-conversion -Wstrict-overflow=5 -Wswitch-default \
	-Wundef -Wno-unused $(INCPATHS)

C9FLAGS=-mcpu=arm946e-s -march=armv5te -mlittle-endian
LDFLAGS=
OCFLAGS=--set-section-flags .bss=alloc,load,contents

OBJS:=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/**/*.c))
OBJS+=$(patsubst $(SRC_DIR)/%.s, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/**/*.s))
OBJS+=$(patsubst $(SRC_DIR)/%.S, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/**/*.S))
OBJS+=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
OBJS+=$(patsubst $(SRC_DIR)/%.s, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.s))
OBJS+=$(patsubst $(SRC_DIR)/%.S, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.S))

OUT_DIR:=$(LIB_DIR) $(OBJ_DIR) $(OBJ_DIR)/sdmmc $(OBJ_DIR)/option $(OBJ_DIR)/fatfs
LIB:=$(LIB_DIR)/lib$(LIBNAME).a

.PHONY: clean all install

all: $(LIB)

$(LIB): $(OBJS) | dirs
	@echo $(OBJS)
	$(AR) -rcs $@ $(OBJS)

obj/%.o: src/%.c | dirs
	@echo Compiling $<
	$(CC) -c $(CFLAGS) $(C9FLAGS) $< -o $@

obj/%.o: src/%.s | dirs
	@echo Compiling $<
	$(CC) -c $(CFLAGS) $(C9FLAGS) $< -o $@

obj/%.o: src/%.S | dirs
	@echo Compiling $<
	$(CC) -c $(CFLAGS) $(C9FLAGS) $< -o $@

dirs: ${OUT_DIR}

${OUT_DIR}:
	mkdir -p ${OUT_DIR}

install: all
	cp -r include/* $(DEVKITARM)/arm-none-eabi/include
	cp $(LIB) $(DEVKITARM)/arm-none-eabi/lib

uninstall:
	rm -rf $(DEVKITARM)/arm-none-eabi/include/${LIB}
	rm -rf $(DEVKITARM)/arm-none-eabi/lib/${LIB}

clean:
	rm -rf lib/* obj/*
