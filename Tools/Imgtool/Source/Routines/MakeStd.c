#include "RoutineLib.h"
#include "Imgtool.h"

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <sys/types.h>

#define NO_PARTITIONS_TO_ALLOCATE 128
#define MIN_DISK_MIB 128
#define EFI_PARTITION_SIZE 100

#define PREFIX "Imgtool.MakeStd: "
#define print(...) printf(PREFIX __VA_ARGS__)

int ItRoutineMakeStd(const Routine* pSelf, int argc, char** argv)
{
    char* pImagePath =      argv[0];
    int   iDiskSize  = atoi(argv[1]);
    uint16_t iBlockSize;

    if (MIN_DISK_MIB > iDiskSize)
    {
        print("Disk size must be at least %d MiB.\n", MIN_DISK_MIB);
        return ROUTINE_FAIL;
    }

    if (argc == 3)
    {
        int blockSize = atoi(argv[2]);

        if (0 > blockSize)
        {
            print("Block size cannot be negative.\n");
            return ROUTINE_FAIL;
        }

        if (512 > blockSize)
        {
            print("Block size must be at least 512.\n");
            return ROUTINE_FAIL;
        }

        if (65024 < blockSize)
        {
            print("Block size cannot exceed 65024, as it's the last multiple of 512 before the 16-bit integer limit.\n");
            return ROUTINE_FAIL;
        }

        if (0 != blockSize % 512)
        {
            print("Block size must be a multiple of 512.\n");
            return ROUTINE_FAIL;
        }

        iBlockSize = (uint16_t) blockSize;
    }
    else
    {
        iBlockSize = 512;
    }

    uint64_t iRawDiskSize = IT_MTOB((uint64_t) iDiskSize);
    uint64_t iNoBlocks    = iRawDiskSize / iBlockSize;

    FILE* pImage = fopen(pImagePath, "wb");
    if (NULL == pImage)
    {
        print("Couldn't open file '%s' for writing. Check for existence and/or permissions?\n", pImagePath);
        return ROUTINE_FAIL;
    }

    if (0 != ftruncate(fileno(pImage), iRawDiskSize))
    {
        print("Couldn't truncate file '%s' up to %d MiBs (%lu bytes).\n", pImagePath, iDiskSize, iRawDiskSize);
        goto FailPostFile;
    }

    uint64_t iPartitionTableBlockCount = (NO_PARTITIONS_TO_ALLOCATE * sizeof(GPT_PARTITION_ENTRY)) / iBlockSize;

    // Write MBR
    {
        MASTER_BOOT_RECORD MBR;
        memset(&MBR, 0, MBR_SIZE);

        MBR_PARTITION_ENTRY* entry = MBR.PartitionTable;
        entry->Attributes = MBR_PARTITION_ATTRIBUTE_NOT_BOOTABLE;
        entry->StartLBA   = 1;
        entry->Size       = (uint32_t)(iNoBlocks - 1);
        entry->Type       = MBR_PARTITION_TYPE_GPT_PROTECTIVE;
        ItLBAToCHS(entry->StartLBA, entry->StartCHS);
        ItLBAToCHS(entry->Size, entry->EndCHS);
        
        MBR.BootSignature = MBR_BOOT_SIGNATURE;

        if (MBR_SIZE != fwrite(&MBR, 1, MBR_SIZE, pImage))
        {
            print("Failed to write the Master Boot Record onto the disk.\n");
            goto FailPostFile;
        }
    }
    // Write GPT headers & Partition Tables
    {
        GPT_HEADER GPT;
        memcpy(GPT.Signature, GPT_HEADER_SIGNATURE, GPT_HEADER_SIGNATURE_SIZE);
        memset(GPT.Reserved, 0, 4);
        GPT.Revision               = GPT_INITIAL_REVISION;
        GPT.HeaderSize             = sizeof(GPT_HEADER);
        GPT.HeaderCRC32            = 0;
        GPT.SelfLBA                = 1;
        GPT.AlternateLBA           = iNoBlocks - 1;
        GPT.FirstUsableLBA         = 2 + iPartitionTableBlockCount; // +2 -> MBR and Header.
        GPT.LastUsableLBA          = iNoBlocks - 1 - iPartitionTableBlockCount - 1; // Final -1 because GPT LBAs are inclusive.
        ItGUID(GPT.DiskGUID);
        GPT.PartitionTableLBA      = GPT.SelfLBA + 1;
        GPT.NoPartitionEntries     = NO_PARTITIONS_TO_ALLOCATE;
        GPT.BytesPerPartitionEntry = sizeof(GPT_PARTITION_ENTRY);
        GPT.PartitionTableCRC32    = 0;

        // Create intermediate Partition Table representation.
        uint64_t iPartitionTableSize = GPT.NoPartitionEntries * GPT.BytesPerPartitionEntry;
        GPT_PARTITION_ENTRY* pPartitionTable = malloc(iPartitionTableSize);

        if (NULL == pPartitionTable)
        {
            print("Failed to allocate memory for intermediate Partition Table representation.\n");
            goto FailPostFile;
        }
        memset(pPartitionTable, 0, iPartitionTableSize);

        GPT_PARTITION_ENTRY* pPartitionEFI = pPartitionTable;
        GPT_PARTITION_ENTRY* pPartitionOS  = pPartitionTable + 1;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Waddress-of-packed-member"
        // EFI System Partition
        memcpy(pPartitionEFI->PartitionTypeGUID, GUID_EFI, GUID_SIZE);
        ItGUID(pPartitionEFI->UniqueGUID);
        pPartitionEFI->Start      = IT_MTOB(1) / iBlockSize; // 1MiB mark
        pPartitionEFI->End        = pPartitionEFI->Start + (IT_MTOB(EFI_PARTITION_SIZE) / iBlockSize) - 1; // GPT LBAs are inclusive so -1.
        pPartitionEFI->Attributes = GPT_PARTITION_ATTRIBUTE_REQUIRED;
        ItWriteAnsiToPartName(pPartitionEFI->PartitionName, "EFI System Partition");

        // OS Partition
        memcpy(pPartitionOS->PartitionTypeGUID, GUID_MYTHE, GUID_SIZE);
        ItGUID(pPartitionOS->UniqueGUID);
        pPartitionOS->Start      = pPartitionEFI->End + 1;
        pPartitionOS->End        = GPT.LastUsableLBA;
        pPartitionOS->Attributes = GPT_PARTITION_ATTRIBUTE_NO_BLOCK_IO;
        ItWriteAnsiToPartName(pPartitionOS->PartitionName, "Biological Operating System");
    #pragma GCC diagnostic pop

        GPT.PartitionTableCRC32 = ItCRC32(pPartitionTable, iPartitionTableSize);
        GPT.HeaderCRC32         = ItCRC32(&GPT, GPT.HeaderSize);

        if (0 != fseek(pImage, IT_RESBLK(GPT.PartitionTableLBA, iBlockSize), SEEK_SET))
        {
            print("Failed to seek to the primary Partition Table on disk.\n");
            free(pPartitionTable);
            goto FailPostFile;
        }
        if (iPartitionTableSize != fwrite(pPartitionTable, 1, iPartitionTableSize, pImage))
        {
            print("Failed to write the primary Partition Table onto the disk.\n");
            free(pPartitionTable);
            goto FailPostFile;
        }
        free(pPartitionTable);

        if (0 != fseek(pImage, IT_RESBLK(GPT.SelfLBA, iBlockSize), SEEK_SET))
        {
            print("Failed to seek to the primary GPT header on disk.\n");
            goto FailPostFile;
        }
        if (GPT.HeaderSize != fwrite(&GPT, 1, GPT.HeaderSize, pImage))
        {
            print("Failed to write the primary GPT header onto the disk.\n");
            goto FailPostFile;
        }

        GPT_HEADER BackupGPT        = GPT;
        BackupGPT.HeaderCRC32       = 0;
        BackupGPT.SelfLBA           = GPT.AlternateLBA;
        BackupGPT.AlternateLBA      = GPT.SelfLBA;
        BackupGPT.PartitionTableLBA = GPT.LastUsableLBA + 1;
        BackupGPT.HeaderCRC32       = ItCRC32(&BackupGPT, BackupGPT.HeaderSize);

        if (0 != fseek(pImage, IT_RESBLK(BackupGPT.PartitionTableLBA, iBlockSize), SEEK_SET))
        {
            print("Failed to seek to the backup Partition Table on disk.\n");
            goto FailPostFile;
        }
        if (iPartitionTableSize != fwrite(pPartitionTable, 1, iPartitionTableSize, pImage))
        {
            print("Failed to write the backup Partition Table onto the disk.\n");
            goto FailPostFile;
        }

        if (0 != fseek(pImage, IT_RESBLK(BackupGPT.SelfLBA, iBlockSize), SEEK_SET))
        {
            print("Failed to seek to the backup GPT header on disk.");
            goto FailPostFile;
        }
        if (BackupGPT.HeaderSize != fwrite(&BackupGPT, 1, BackupGPT.HeaderSize, pImage))
        {
            print("Failed to write the backup GPT header onto the disk.");
            goto FailPostFile;
        }
    }

    return ROUTINE_OK;
FailPostFile:
    if (NULL != pImage)
    {
        fclose(pImage);
        pImage = NULL;
    }
    return ROUTINE_FAIL;
}
