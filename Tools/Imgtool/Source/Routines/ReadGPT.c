#include "RoutineLib.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#define PREFIX "Imgtool.ReadGPT: "
#define print(...) printf(PREFIX __VA_ARGS__)

int ItRoutineReadGPT(const Routine* pSelf, int argc, char** argv)
{
    bool bNoSafetyChecks;
    if (0 == strcmp(argv[0], "True"))
    {
        bNoSafetyChecks = true;
    }
    else if (0 == strcmp(argv[0], "False"))
    {
        bNoSafetyChecks = false;
    }
    else
    {
        print("Invalid NoSafetyChecks value provided. Only \"False\" and \"True\" are acceptable.");
        return ROUTINE_FAIL;
    }

    if (!bNoSafetyChecks)
    {
        return ROUTINE_NOT_IMPLEMENTED;
    }

    return ROUTINE_NOT_IMPLEMENTED;
}
