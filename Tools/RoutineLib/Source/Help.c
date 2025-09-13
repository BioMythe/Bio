#include "RoutineLib.h"

#include <stdio.h>

int RoutRoutineHelp(const Routine* pSelf, int argc, char** argv)
{
    size_t numberOfRoutines;
    Routine* pRoutines = RoutGetRoutinesEx(&numberOfRoutines);

    if (0 == numberOfRoutines || NULL == pRoutines)
    {
        return ROUTINE_FAIL;
    }

    printf("List of Available Actions (%lu in total):\n", numberOfRoutines);
    for (size_t i = 0; i < numberOfRoutines; i++)
    {
        Routine* routine = pRoutines + i;
        if (routine->MinArguments == 0 && routine->MaxArguments == 0)
        {
            printf(" %lu - %s (No Arguments Needed):\n", i+1, routine->Identifier);
        }
        else if (routine->MinArguments == routine->MaxArguments)
        {
            printf(" %lu - %s (Arguments Needed -> %d):\n", i+1, routine->Identifier, routine->MinArguments);
        }
        else
        {
            printf(" %lu - %s (Arguments Needed -> Minimum %d, Maximum %d):\n", i+1, routine->Identifier, routine->MinArguments, routine->MaxArguments);
        }
        printf("  %s\n  Usage: %s\n", routine->Description, routine->Usage);
    }

    return ROUTINE_OK;
}
