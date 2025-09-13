BUILD_PATH  ?= Binaries
BOOT_PATH   ?= Boot
KERNEL_PATH ?= Kernel
TOOLS_PATH  ?= Tools
VENDOR_PATH ?= Vendor

BOOT_BUILD_PATH   := $(BUILD_PATH)/$(BOOT_PATH)
KERNEL_BUILD_PATH := $(BUILD_PATH)/$(KERNEL_PATH)
TOOLS_BUILD_PATH  := $(BUILD_PATH)/$(TOOLS_PATH)

ROUTINE_LIB_PATH       ?= $(TOOLS_PATH)/RoutineLib
ROUTINE_LIB_BUILD_PATH ?= $(TOOLS_BUILD_PATH)/RoutineLib
ROUTINE_LIB_NAME       ?= libroutinelib.a
ROUTINE_LIB            := $(ROUTINE_LIB_BUILD_PATH)/$(ROUTINE_LIB_NAME)

OS_IMAGE_BLOCK_SIZE  ?= 512
OS_EFI_PART_SIZE     ?= 100
OS_EFI_PART_SIZE_RAW ?= $$(($(OS_EFI_PART_SIZE) * 1024 * 1024))
TEMP_EFI_FAT32       ?= $(BUILD_PATH)/FAT32sect.bin
OS_IMAGE_SIZE        ?= 1024 # In MiB.
OS_IMAGE_NAME        ?= Bio.img
OS_IMAGE             := $(BUILD_PATH)/$(OS_IMAGE_NAME)
MEMORY_SIZE          ?= 512M # RAM (Random Access Memory/System Memory) size for emulation with 'run' target.
BOOTLOADER_NAME      ?= BioBoot.efi

DD      ?= dd
RM      ?= rm
MMD     ?= mmd
MCPY    ?= mcopy
QEMU    ?= qemu-system-x86_64
ECHO    ?= echo
MKDIR   ?= mkdir
IMGTOOL ?= $(TOOLS_BUILD_PATH)/Imgtool/Imgtool

os-image: $(OS_IMAGE)
$(OS_IMAGE): boot tools
	@$(MKDIR) -p $(dir $@)
	@$(ECHO) Os-Image: Creating standard BIO OS GPT disk using Imgtool...
	@$(IMGTOOL) MakeStd $@ $(OS_IMAGE_SIZE) 512
	@$(ECHO) Os-Image: Creating FAT32 file system on EFI partition and adding the bootloader...
## We have to create temporary image containing the FAT32 file system.
	@$(DD) if=/dev/zero of=$(TEMP_EFI_FAT32) bs=$(OS_EFI_PART_SIZE_RAW) count=1
	@mkfs.fat -F 32 $(TEMP_EFI_FAT32)
	@$(MMD) -i $(TEMP_EFI_FAT32) ::/EFI
	@$(MMD) -i $(TEMP_EFI_FAT32) ::/EFI/Boot
	@$(MCPY) -i $(TEMP_EFI_FAT32) $(BOOT_BUILD_PATH)/$(BOOTLOADER_NAME) ::/EFI/Boot/Bootx64.efi
	@$(DD) if=$(TEMP_EFI_FAT32) of=$@ bs=1MiB seek=1 count=$(OS_EFI_PART_SIZE) conv=notrunc
	@$(RM) $(TEMP_EFI_FAT32)

run:
	@$(QEMU) \
	-drive if=none,file=$(OS_IMAGE),id=hd0,format=raw \
	-device ide-hd,drive=hd0 \
	-drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
	-drive if=pflash,format=raw,file=OVMF_VARS.fd \
	-m $(MEMORY_SIZE) \
	-cpu qemu64 \
	-boot menu=on \
	-serial stdio

boot:
	@$(MAKE) -C $(BOOT_PATH) BUILD_PATH=$(abspath $(BOOT_BUILD_PATH)) TARGET_NAME=$(BOOTLOADER_NAME) GNU_EFI_DIR=$(abspath $(VENDOR_PATH)/gnu-efi)

tools: routinelib imgtool mythe
routinelib: $(ROUTINE_LIB)
$(ROUTINE_LIB):
	@$(MAKE) -C $(TOOLS_PATH)/RoutineLib BUILD_PATH=$(abspath $(ROUTINE_LIB_BUILD_PATH))
imgtool:
	@$(MAKE) -C $(TOOLS_PATH)/Imgtool BUILD_PATH=$(abspath $(TOOLS_BUILD_PATH)/Imgtool) ROUTINE_LIB_PATH=$(abspath $(ROUTINE_LIB_PATH)/Source) ROUTINE_LIB=$(abspath $(ROUTINE_LIB))
mythe:
	@$(MAKE) -C $(TOOLS_PATH)/Mythe BUILD_PATH=$(abspath $(TOOLS_BUILD_PATH)/Mythe) ROUTINE_LIB_PATH=$(abspath $(ROUTINE_LIB_PATH)/Source) ROUTINE_LIB=$(abspath $(ROUTINE_LIB))

clean:
	@$(ECHO) Removing build directory \"$(BUILD_PATH)\"...
	@$(RM) -rf $(BUILD_PATH)
