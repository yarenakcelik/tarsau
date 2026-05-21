#include <stdio.h>
#include "archive.h"

int archive_files(int argc, char *argv[])
{
    printf("Arsivleme modu calisti.\n");

    for (int i = 2; i < argc; i++)
    {
        printf("Parametre %d: %s\n", i, argv[i]);
    }

    return 0;
}