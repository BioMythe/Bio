#ifndef BIO_TOOLS_IMGTOOL_H
#define BIO_TOOLS_IMGTOOL_H

#include "CommonDef.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define GUID_SIZE 16
typedef uint8_t   CHS[3];
typedef uint8_t   GUID[GUID_SIZE];
typedef uint32_t  CRC32;
typedef uint32_t  LBA32;
typedef uint64_t  LBA64;

typedef struct
{
    uint16_t Cylinder;
    uint8_t  Head;
    uint8_t  Sector;
} UnpackCHS;

extern GUID GUID_EMPTY;
extern GUID GUID_LEGACY_MBR;
extern GUID GUID_EFI;
extern GUID GUID_MYTHE; // Bio OS inhouse file system: Mythe

#define MBR_SIZE 512
#define MBR_PARTITION_ENTRY_SIZE             16
#define MBR_NO_PARTITION_ENTRIES             4
#define MBR_PARTITION_TABLE_SIZE            (MBR_PARTITION_ENTRY_SIZE * MBR_NO_PARTITION_ENTRIES)
#define MBR_BOOT_SIGNATURE                   UINT16_C(0xAA55)
#define MBR_BOOT_SIGNATURE_SIZE              2
#define MBR_BOOT_CODE_SIZE                  (MBR_SIZE - MBR_PARTITION_TABLE_SIZE - MBR_BOOT_SIGNATURE_SIZE)
#define MBR_PARTITION_ATTRIBUTE_NOT_BOOTABLE UINT8_C(0x00)
#define MBR_PARTITION_ATTRIBUTE_BOOTABLE     UINT8_C(0x80)
#define MBR_PARTITION_TYPE_UEFI_SYSTEM       UINT8_C(0xEF)
#define MBR_PARTITION_TYPE_GPT_PROTECTIVE    UINT8_C(0xEE)

typedef struct __attribute__((packed))
{
    uint8_t  Attributes; // Drive attributes.
    CHS      StartCHS;   // CHS of start adress.
    uint8_t  Type;       // Type/OS.
    CHS      EndCHS;     // CHS of end address.
    LBA32    StartLBA;   // LBA of start address.
    uint32_t Size;       // Number of sectors in partition.
} MBR_PARTITION_ENTRY;
typedef MBR_PARTITION_ENTRY MBR_PARTITION_TABLE[MBR_NO_PARTITION_ENTRIES];

typedef struct __attribute__((packed))
{
    uint8_t             BootCode[MBR_BOOT_CODE_SIZE];
    MBR_PARTITION_TABLE PartitionTable;
    uint16_t            BootSignature;
} MASTER_BOOT_RECORD;

#define GPT_HEADER_SIGNATURE_SIZE  8
#define GPT_HEADER_SIGNATURE      "EFI PART"
#define GPT_INITIAL_REVISION       0x00010000

typedef struct __attribute__((packed))
{
    char     Signature[GPT_HEADER_SIGNATURE_SIZE];
    uint32_t Revision;
    uint32_t HeaderSize;
    CRC32    HeaderCRC32;
    uint8_t  Reserved[4];
    LBA64    SelfLBA;
    LBA64    AlternateLBA;
    LBA64    FirstUsableLBA;
    LBA64    LastUsableLBA;
    GUID     DiskGUID;
    LBA64    PartitionTableLBA;
    uint32_t NoPartitionEntries;
    uint32_t BytesPerPartitionEntry;
    CRC32    PartitionTableCRC32;
    // ...pad to zeroes up to BlockSize.
} GPT_HEADER;

#define GPT_PARTITION_ATTRIBUTE_REQUIRED       1
#define GPT_PARTITION_ATTRIBUTE_NO_BLOCK_IO    2
#define GPT_PARTITION_ATTRIBUTE_BIOS_BOOTABLE  4
#define GPT_PARTITION_NAME_RAW_SIZE            72
#define GPT_PARTITION_NAME_LOGICAL_SIZE       (GPT_PARTITION_NAME_RAW_SIZE / 2)
typedef struct __attribute__((packed))
{
    GUID     PartitionTypeGUID; // Specific to each partition type, like EFI, NTFS, Linux, etc...
    GUID     UniqueGUID;        // Unique for every partition no matter what kind.
    LBA64    Start;
    LBA64    End;
    uint64_t Attributes;
    uint16_t PartitionName[GPT_PARTITION_NAME_LOGICAL_SIZE];
} GPT_PARTITION_ENTRY;

/**
 * @brief Generates a new unique GUID.
 * @param guid The GUID to write the new GUID to.
 */
void  ItGUID(GUID guid);

/**
 * @brief Calculates EFI standard compliant CRC32 checksum.
 * @param pData The data to calculate the checksum of.
 * @param szData The size of the data to calculate the checksum of, in bytes.
 */
CRC32 ItCRC32(const void* pData, size_t szData);

UnpackCHS ItUnpackCHS(CHS chs); 
void      ItLBAToCHS(LBA32 LBA, CHS chs);
void      ItWriteAnsiToPartName(uint16_t pDest[GPT_PARTITION_NAME_LOGICAL_SIZE], const char* pSrc);

void      ItPrintMasterBootRecord(MASTER_BOOT_RECORD* pMasterBootRecord, const char* pLeadingStr);
void      ItPrintGPTHeader(GPT_HEADER* pHeader, const char* pLeadingStr);

#endif
