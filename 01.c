#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "01.h"

int main(int argc, char *argv[])
{
    if (argc < 3 || strncmp(argv[1], "-h", 2) == 0 || strncmp(argv[1], "--help", 6) == 0)
    {
        printf("Usage: ccp [-r] source_file target_file");
        return -1;
    }
    if (argc == 3)
    {
        return ccp(argv[1], argv[2]);
    }

    if (argc == 4)
    {
        return ccpo(argv[2], argv[3], argv[1]);
    }

    return 0;
}

int is_dir(char *path)
{
    struct stat statbuf;
    stat(path, &statbuf);
    return S_ISDIR(statbuf.st_mode);
}

int createNotExist(char *path)
{
    struct stat statbuf;
    int ret = stat(path, &statbuf);
    if (ret == -1)
    {
        mkdir(path, 0700);
    }
    return 0;
}

int ccpo(char *src, char *dest, char *option)
{
    if (is_dir(src))
    {
        createNotExist(dest);
        DIR *dir;
        struct dirent *ent;
        dir = opendir(src);
        int i = 0;
        while ((ent = readdir(dir)) != NULL)
        {
            if (i < 2)
            {
                i++;
                continue;
            }
            char *new_src = malloc(strlen(src) + 2 + strlen(ent->d_name));
            char *new_dest = malloc(strlen(dest) + 2 + strlen(ent->d_name));
            strcpy(new_src, src);
            strcpy(new_dest, dest);
            strcat(new_src, "/");
            strcat(new_dest, "/");
            strcat(new_src, ent->d_name);
            strcat(new_dest, ent->d_name);
            DIR *new_dir = opendir(new_src);
            ccpo(new_src, new_dest, option);
        }
    }
    else
    {
        ccp(src, dest);
    }
    return 0;
}

int ccp(char *src, char *dest)
{
    if (is_dir(src))
    {
        printf("Use the -r option to copy directories");
        return -1;
    }
    char buffer[1024];
    int sourceFile = open(src, O_RDONLY);
    if (sourceFile == -1)
    {
        return -1;
    }
    int destFile = open(dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (destFile == -1)
    {
        close(sourceFile);
        return -1;
    }
    int currBlock;
    while ((currBlock = read(sourceFile, buffer, 1024)) > 0)
    {
        write(destFile, buffer, currBlock);
    }
    printf("Successfully copied %s to %s\n", src, dest);
    return 0;
}