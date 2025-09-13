#include "RoutineLib.h"

int main(int argc, char** argv)
{
    if (!RoutLibInit())
    {
        return 1;
    }
    return RoutHandle("Mythe", argc, argv);
}
