#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "archive.h"

#define MAX_FILES 32
#define MAX_METADATA_SIZE 8192
#define BUFFER_SIZE 4096

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

    long total_size = 0;
    char metadata[MAX_METADATA_SIZE] = "";

    for (int i = 0; i < file_count; i++)
    {
        int fd = open(input_files[i], O_RDONLY);

        if (fd == -1)
        {
            printf("%s dosyasi acilamadi!\n", input_files[i]);
            return 1;
        }

        struct stat file_info;

        if (stat(input_files[i], &file_info) == -1)
        {
            printf("%s dosya bilgileri okunamadi!\n", input_files[i]);
            close(fd);
            return 1;
        }

        total_size += file_info.st_size;

        char record[512];
        sprintf(record, "|%s,%o,%ld|", input_files[i], file_info.st_mode & 0777, file_info.st_size);
        strcat(metadata, record);

        close(fd);
    }

    if (total_size > 200 * 1024 * 1024)
    {
        printf("Giris dosyalarinin toplam boyutu 200 MB'i gecemez!\n");
        return 1;
    }

    int metadata_size = strlen(metadata);

    int out_fd = open(output_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (out_fd == -1)
    {
        printf("Arsiv dosyasi olusturulamadi!\n");
        return 1;
    }

    char header[11];
    sprintf(header, "%010d", metadata_size);

    write(out_fd, header, 10);
    write(out_fd, metadata, metadata_size);

    char buffer[BUFFER_SIZE];

    for (int i = 0; i < file_count; i++)
    {
        int in_fd = open(input_files[i], O_RDONLY);

        if (in_fd == -1)
        {
            printf("%s dosyasi tekrar acilamadi!\n", input_files[i]);
            close(out_fd);
            return 1;
        }

        ssize_t bytes_read;

        while ((bytes_read = read(in_fd, buffer, BUFFER_SIZE)) > 0)
        {
            write(out_fd, buffer, bytes_read);
        }

        close(in_fd);
    }

    close(out_fd);

    printf("Dosyalar birlestirildi.\n");
    printf("Olusturulan arsiv: %s\n", output_name);

    return 0;
}