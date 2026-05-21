#include <stdio.h>
#include "extract.h"

int extract_archive(int argc, char *argv[])
{
    printf("Acma modu calisti.\n");

    for (int i = 2; i < argc; i++)
    {
        printf("Parametre %d: %s\n", i, argv[i]);
    }

    return 0;
}