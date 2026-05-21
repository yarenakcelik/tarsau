#include <stdio.h>
#include <string.h>
#include "archive.h"
#include "extract.h"

void print_usage()
{
    printf("Kullanim:\n");
    printf("  tarsau -b dosya1 dosya2 ... -o arsiv.sau\n");
    printf("  tarsau -a arsiv.sau [dizin]\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "-b") == 0)
    {
        return archive_files(argc, argv);
    }
    else if (strcmp(argv[1], "-a") == 0)
    {
        return extract_archive(argc, argv);
    }
    else
    {
        printf("Hatali parametre!\n");
        print_usage();
        return 1;
    }
}