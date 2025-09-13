#include "RoutineLib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static Routine* g_Routines         = NULL;
static size_t   g_NumberOfRoutines = 0;

extern int RoutRoutineHelp(const Routine* pSelf, int argc, char** argv);

bool RoutLibInit(void)
{
    return RoutRegisterRoutine((Routine) {
        .Identifier   = "Help",
        .Description  = "Lists all available actions.",
        .Usage        = ROUTINE_USAGE_AS_IS,
        .MinArguments = 0,
        .MaxArguments = 0,
        .Handler      = RoutRoutineHelp
    });
}

bool RoutRegisterRoutine(Routine pRoutine)
{
    if (RoutDoesRoutineExist(pRoutine.Identifier))
    {
        return false;
    }

    if (NULL == g_Routines)
    {
        g_Routines = malloc(sizeof(Routine));
    }
    else
    {
        g_Routines = realloc(g_Routines, (g_NumberOfRoutines * sizeof(Routine)) + sizeof(Routine));
    }
    g_Routines[g_NumberOfRoutines++] = pRoutine;
    if (NULL == g_Routines)
    {
        g_NumberOfRoutines = 0;
        return false;
    }

    return true;
}

bool RoutUnregisterRoutine(const char* pIdentifier)
{
    size_t i = RoutFindRoutine(pIdentifier);
    if (i == SIZE_MAX)
    {
        return false;
    }
    return RoutUnregisterRoutineAt(i);
}

bool RoutUnregisterRoutineAt(size_t index)
{
    if (NULL == g_Routines || index >= g_NumberOfRoutines)
    {
        return false;
    }

    for (size_t i = index; i < g_NumberOfRoutines - 1; i++)
    {
        g_Routines[i] = g_Routines[i + 1];
    }
    
    g_Routines = realloc(g_Routines, (g_NumberOfRoutines * sizeof(Routine)) - sizeof(Routine));
    if (NULL == g_Routines)
    {
        g_NumberOfRoutines = 0;
        return false;
    }

    return true;
}

size_t RoutFindRoutine(const char* pIdentifier)
{
    for (size_t i = 0; i < g_NumberOfRoutines; i++)
    {
        if (0 == strcmp(g_Routines[i].Identifier, pIdentifier))
        {
            return i;
        }
    }
    return SIZE_MAX;
}

bool RoutDoesRoutineExist(const char* pIdentifier)
{
    return RoutFindRoutine(pIdentifier) != SIZE_MAX;
}

size_t RoutGetNumberOfRoutines(void)
{
    return g_NumberOfRoutines;
}

Routine* RoutGetRoutines(void)
{
    return g_Routines;
}

Routine* RoutGetRoutinesEx(size_t* pOutNumberOfRoutines)
{
    *pOutNumberOfRoutines = g_NumberOfRoutines;
    return RoutGetRoutines();
}

int RoutHandle(const char* pPrintPrefix, int argc, char** argv)
{
    if (argc < 2)
    {
        printf("%s: No action was provided. Use 'Help' to get a list of all available actions.\n", pPrintPrefix);
        return EXIT_FAILURE;
    }

    char* id = argv[1];
    size_t numberOfRoutines;
    Routine* pRoutines = RoutGetRoutinesEx(&numberOfRoutines);
    
    for (size_t i = 0; i < numberOfRoutines; i++)
    {
        Routine* pRoutine = pRoutines + i;
        if (0 == strcmp(id, pRoutine->Identifier))
        {
            int    localargc = argc - 2;
            char** localargv = argv + 2;

            if (localargc < pRoutine->MinArguments)
            {
                printf("%s: The amount of arguments you have provided for action '%s', which is %d, is below the minimum requirement of that action which is %d.\n"
                       "  Usage of '%s': %s\n", pPrintPrefix, id, localargc, pRoutine->MinArguments, id, pRoutine->Usage);
                return EXIT_FAILURE;
            }
            if (localargc > pRoutine->MaxArguments)
            {
                printf("%s: The amount of arguments you have provided for action '%s', which is %d, is above the maximum requirement of that action which is %d.\n"
                       "  Usage of '%s': %s\n", pPrintPrefix, id, localargc, pRoutine->MaxArguments, id, pRoutine->Usage);
                return EXIT_FAILURE;
            }

            int result = pRoutine->Handler(pRoutine, argc - 2, argv + 2);
            if (0 != result)
            {
                printf("%s: The action '%s' has failed with return code %d.\n", pPrintPrefix, id, result);
            }
            else
            {
                printf("%s: The action '%s' has been completed successfully.\n", pPrintPrefix, id);
            }
            return result;
        }
    }

    printf("%s: No such action as '%s' exists, which is what was provided. Use 'Help' to get a list of all available actions.\n", pPrintPrefix, id);
    return EXIT_FAILURE;
}
