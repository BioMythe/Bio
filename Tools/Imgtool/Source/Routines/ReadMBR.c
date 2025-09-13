#include "RoutineLib.h"
#include "Imgtool.h"

#include <stdio.h>
#include <stdlib.h>

#define PREFIX "Imgtool.ReadMBR: "
#define print(...) printf(PREFIX __VA_ARGS__)

int ItRoutineReadMBR(const Routine* pSelf, int argc, char** argv)
{
    char* pImagePath = argv[0];
    FILE* pImage = fopen(pImagePath, "rb");
    if (NULL == pImage)
    {
        print("Couldn't open file '%s' for reading. Check for existence and/or permissions?\n", pImagePath);
        return ROUTINE_FAIL;
    }

    MASTER_BOOT_RECORD MBR;
    if (MBR_SIZE != fread(&MBR, 1, MBR_SIZE, pImage))
    {
        print("Couldn't read the Master Boot Record from file '%s'.", pImagePath);
        return ROUTINE_FAIL;
    }

    ItPrintMasterBootRecord(&MBR, "Master Boot Record");
    return ROUTINE_OK;
}
