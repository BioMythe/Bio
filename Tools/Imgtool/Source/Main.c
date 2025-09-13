#include "RoutineLib.h"
#include "Imgtool.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern int ItRoutineHelp(const Routine* pSelf, int argc, char** argv);
extern int ItRoutineReadMBR(const Routine* pSelf, int argc, char** argv);
extern int ItRoutineReadGPT(const Routine* pSelf, int argc, char** argv);
extern int ItRoutineMakeStd(const Routine* pSelf, int argc, char** argv);

int main(int argc, char** argv)
{
    srand((uint32_t) time(NULL));
    
    // Initialize RoutineLib.
    RoutLibInit();
    // Initialize all standard routines.   
    RoutRegisterRoutine((Routine) {
        .Identifier   = "ReadMBR",
        .Description  = "Reads the Master Boot Record and prints out the Partition Table and signature info.",
        .Usage        = "[DiskPath:Str](The path to the disk to read relevant information from.)",
        .MinArguments = 1,
        .MaxArguments = 1,
        .Handler      = ItRoutineReadMBR
    });
    RoutRegisterRoutine((Routine) {
        .Identifier   = "ReadGPT",
        .Description  = "Reads the GUID Partition Table and prints out relevant information.",
        .Usage        = "[DiskPath:Str](The path to the disk to read relevant information from.) [NoSafetyChecks:StrBool]<Optional,ByDefault=False,Values={\"False\", \"True\"}>",
        .MinArguments = 1,
        .MaxArguments = 2,
        .Handler      = ItRoutineReadGPT
    });
    RoutRegisterRoutine((Routine) {
        .Identifier   = "MakeStd",
        .Description  = "Creates a standard Biological Operating System disk image file with protective MBR and 2 GPT partitions: EFI and Mythe OS Partition.",
        .Usage        = "[DiskPath:Str](The path to the disk to read relevant information from.) [DiskSize:Int32]<Min=128>(The size of the image file to be created, in MiB.) [BlockSize:Int16](The size of each block/sector of the disk.)<Optional,ByDefault=512,Min=512,Max=65024>",
        .MinArguments = 2,
        .MaxArguments = 3,
        .Handler      = ItRoutineMakeStd
    });

    return RoutHandle("Imgtool", argc, argv);
}
