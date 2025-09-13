#ifndef BIO_TOOLS_ROUTINELIB_H
#define BIO_TOOLS_ROUTINELIB_H

#include <stdbool.h>
#include <stddef.h>

#define ROUTINE_USAGE_AS_IS    "As-Is (Provide No Arguments)"
#define ROUTINE_OK              0
#define ROUTINE_FAIL            1
#define ROUTINE_NOT_IMPLEMENTED 2

typedef struct Routine
{
    const char* Identifier;
    const char* Description;
    const char* Usage;
    int         MinArguments;
    int         MaxArguments;
    int      (* Handler)(const struct Routine* pSelf, int argc, char** argv); // argc and argv are adjusted relative to post-action section.
} Routine;

bool     RoutLibInit(void);
bool     RoutRegisterRoutine(Routine pRoutine);
bool     RoutUnregisterRoutine(const char* pIdentifier);
bool     RoutUnregisterRoutineAt(size_t index);
size_t   RoutFindRoutine(const char* pIdentifier);
bool     RoutDoesRoutineExist(const char* pIdentifier);
size_t   RoutGetNumberOfRoutines(void);
Routine* RoutGetRoutines(void);
Routine* RoutGetRoutinesEx(size_t* pOutNumberOfRoutines);

int      RoutHandle(const char* pPrintPrefix, int argc, char** argv);

#endif
