#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
    Print(L"The Biological Bootloader!");

    __asm__ __volatile__(
        "1:\n\t"
        "hlt\n\t"
        "jmp 1b\n\t"
    );

    return EFI_SUCCESS;
}
