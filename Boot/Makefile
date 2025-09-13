GNU_EFI_DIR ?= gnu-efi
BUILD_PATH  ?= Binaries
TARGET_NAME ?= BioBoot.efi
TARGET      := $(BUILD_PATH)/$(TARGET_NAME)

SHARED_OBJECT := $(BUILD_PATH)/Int/BioBoot.so
OBJECT        := $(BUILD_PATH)/Int/BioBoot.o
SOURCE        ?= BioBoot.c

CFLAGS   ?= -I$(GNU_EFI_DIR)/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar \
			-mno-red-zone -maccumulate-outgoing-args
LDFLAGS  ?= -shared -Bsymbolic -L$(GNU_EFI_DIR)/x86_64/lib -L$(GNU_EFI_DIR)/x86_64/gnuefi -T$(GNU_EFI_DIR)/gnuefi/elf_x86_64_efi.lds

RM    ?= rm
LD    ?= ld
CC    ?= gcc
OCP   ?= objcopy
ECHO  ?= echo
MKDIR ?= mkdir

$(TARGET): $(SHARED_OBJECT)
	@$(MKDIR) -p $(dir $@)
	@$(ECHO) BioBoot: Creating final EFI PE executable \"$@\" from \"$<\"...
	@$(OCP) -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* \
			-j .reloc --target efi-app-x86_64 --subsystem=10 $< $@

$(SHARED_OBJECT): $(OBJECT)
	@$(MKDIR) -p $(dir $@)
	@$(ECHO) BioBoot: Compiling \"$<\" to shared object \"$@\"...
	@$(LD) $(LDFLAGS) $(GNU_EFI_DIR)/x86_64/gnuefi/crt0-efi-x86_64.o $< -o $@ -lgnuefi -lefi

$(OBJECT): $(SOURCE)
	@$(MKDIR) -p $(dir $@)
	@$(ECHO) BioBoot: Compiling \"$<\" to \"$@\"...
	@$(CC) $(CFLAGS) -c $< -o $@
