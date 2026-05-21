#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "archive.h"

#define MAX_FILES 32

int archive_files(int argc, char *argv[])
{
    char *input_files[MAX_FILES];
    int file_count = 0;

    char output_name[256] = "a.sau";

    for (int i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0)
        {
            if (i + 1 < argc)
            {
                strcpy(output_name, argv[i + 1]);
                break;
            }
            else
            {
                printf("Arsiv dosya adi eksik!\n");
                return 1;
            }
        }

        if (file_count >= MAX_FILES)
        {
            printf("En fazla 32 dosya eklenebilir!\n");
            return 1;
        }

        input_files[file_count++] = argv[i];
    }

    printf("Arsivlenecek dosyalar:\n");

    for (int i = 0; i < file_count; i++)
    {
        printf("%s\n", input_files[i]);
    }

    printf("Cikti dosyasi: %s\n", output_name);

    printf("\nDosya kontrolleri:\n");

    for (int i = 0; i < file_count; i++)
    {
        int fd = open(input_files[i], O_RDONLY);

        if (fd == -1)
        {
            printf("%s dosyasi acilamadi!\n", input_files[i]);
            return 1;
        }

        printf("%s dosyasi basariyla acildi.\n", input_files[i]);

        close(fd);
    }

    return 0;
}