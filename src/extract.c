#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

#include "extract.h"

#define BUFFER_SIZE 4096
#define MAX_FILES 32

typedef struct
{
    char name[256];
    int permission;
    long size;
} FileInfo;

int extract_archive(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Arsiv dosyasi belirtilmedi!\n");
        return 1;
    }

    if (argc > 4)
    {
        printf("Arsiv acma islemi icin fazla parametre girildi!\n");
        return 1;
    }

    char *archive_name = argv[2];

    char *extension = strrchr(archive_name, '.');

    if (extension == NULL || strcmp(extension, ".sau") != 0)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        return 1;
    }

    char output_dir[256] = "";

    int fd = open(archive_name, O_RDONLY);

    if (fd == -1)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        return 1;
    }

    if (argc >= 4)
    {
        strcpy(output_dir, argv[3]);

      if (mkdir(output_dir, 0755) == -1)
{
    if (errno == EEXIST)
    {
        struct stat dir_info;

        if (stat(output_dir, &dir_info) == -1 || !S_ISDIR(dir_info.st_mode))
        {
            printf("Belirtilen yol bir dizin degildir!\n");
            close(fd);
            return 1;
        }
    }
    else
    {
        printf("Dizin olusturulamadi!\n");
        close(fd);
        return 1;
    }
}
    }

    char header[11];

    if (read(fd, header, 10) != 10)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        close(fd);
        return 1;
    }

    header[10] = '\0';

    int metadata_size = atoi(header);

    if (metadata_size <= 0)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        close(fd);
        return 1;
    }

    char *metadata = malloc(metadata_size + 1);

    if (metadata == NULL)
    {
        printf("Bellek hatasi!\n");
        close(fd);
        return 1;
    }

    if (read(fd, metadata, metadata_size) != metadata_size)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        free(metadata);
        close(fd);
        return 1;
    }

    metadata[metadata_size] = '\0';

    FileInfo files[MAX_FILES];
    int file_count = 0;

    char *token = strtok(metadata, "|");

    while (token != NULL)
    {
        if (file_count >= MAX_FILES)
        {
            printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
            free(metadata);
            close(fd);
            return 1;
        }

        int parsed = sscanf(token, "%255[^,],%o,%ld",
                            files[file_count].name,
                            &files[file_count].permission,
                            &files[file_count].size);

        if (parsed != 3)
        {
            printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
            free(metadata);
            close(fd);
            return 1;
        }

        if (files[file_count].size < 0)
        {
            printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
            free(metadata);
            close(fd);
            return 1;
        }

        file_count++;

        token = strtok(NULL, "|");
    }

    char buffer[BUFFER_SIZE];

    for (int i = 0; i < file_count; i++)
    {
        char output_path[512];

        if (strlen(output_dir) > 0)
        {
            sprintf(output_path, "%s/%s", output_dir, files[i].name);
        }
        else
        {
            strcpy(output_path, files[i].name);
        }

        int out_fd = open(output_path,
                          O_WRONLY | O_CREAT | O_TRUNC,
                          files[i].permission);

        if (out_fd == -1)
        {
            printf("%s dosyasi olusturulamadi!\n", files[i].name);
            free(metadata);
            close(fd);
            return 1;
        }

        long remaining = files[i].size;

        while (remaining > 0)
        {
            int to_read;

            if (remaining > BUFFER_SIZE)
                to_read = BUFFER_SIZE;
            else
                to_read = remaining;

            int bytes_read = read(fd, buffer, to_read);

            if (bytes_read <= 0)
            {
                printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
                close(out_fd);
                free(metadata);
                close(fd);
                return 1;
            }

           if (write(out_fd, buffer, bytes_read) != bytes_read)
{
    printf("Dosyaya yazma hatasi!\n");
    close(out_fd);
    free(metadata);
    close(fd);
    return 1;
}

            remaining -= bytes_read;
        }

        close(out_fd);

        chmod(output_path, files[i].permission);
    }

    free(metadata);
    close(fd);

    printf("Dosyalar acildi.\n");

    return 0;
}