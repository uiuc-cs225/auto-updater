#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "config.h"
#include "autoup/common.h"

#define BUFF 256

//Returns the stdout from the executed process
char *openProc(char *command)
{
    FILE *proc;
    int read, len, size;
    char *ret, *tmp, buff[BUFF];

    proc = popen(command, "r");
    if(proc == NULL)
        return NULL;
    size = BUFF;
    ret = (char*)malloc(size);
    len = 0;
    ret[len] = 0;
    while(read = fread(buff, 1, BUFF, proc))
    {
        if(size <= read + len)
        {
            while(size <= read+len) size<<=1;
            tmp = (char*)malloc(size);
            memcpy(tmp, ret, len+1);
            free(ret);
            ret = tmp;
        }
        memcpy(ret+len, buff, read);
        len += read;
        ret[len] = 0;
    }
    pclose(proc);
    return ret;
}

int main(int argc, char **argv)
{
    FILE *updated;
    char *out;
    int new = 0;

    //Check to make sure we are running as root
    if(getuid() != 0)
    {
        fprintf(stderr, "This script must be run as root.\n");
        return EXIT_FAILURE;
    }

    //Update databse files
    out = openProc("/usr/bin/pacman -Sy --noconfirm 2>&1");
    if(out == NULL)
    {
        printf("Failed to update to the latest database version.\n");
        return EXIT_FAILURE;
    }
    free(out);

    //Check for new packages
    out = openProc("/usr/bin/pacman -Qu 2>&1 | /usr/bin/wc -l");
    if(out == NULL)
    {
        printf("Failed to get the number of updated packages.\n");
        return EXIT_FAILURE;
    }
    new = strcmp(out, "0\n") != 0;
    free(out);

    //Finalize Updating the System
    out = openProc("/usr/bin/yes | /usr/bin/pacman -Suuf 2>&1");
    if(out == NULL)
    {
        printf("Failed to install updated packages.\n");
        return EXIT_FAILURE;
    }
    free(out);
    out = openProc("/usr/bin/yes | /usr/bin/pacman -Suuf 2>&1");
    if(out == NULL)
    {
        printf("Failed to install updated packages.\n");
        return EXIT_FAILURE;
    }
    free(out);

    //If new packages were installed, write to the reboot file
    if(new)
    {
        //Fixes a kernel build error by automatically rebuilding the kernel
        out = openProc("/usr/bin/yes | /usr/bin/pacman -Sf linux 2>&1");
        if(out == NULL)
            printf("Failed to process kernel upgrade.\n");
        free(out);

        //Writes a 1 to the update file for client info
        updated = fopen(UPDATE_FILE, "w");
        if(updated == NULL)
        {
            printf("Failed to write update file!\n");
            return EXIT_FAILURE;
        }
        fputc('1', updated);
        fclose(updated);

        //Allows the client to writeback to the log
        chmod(UPDATE_FILE, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);


        printf("Installed new packages.\nPlease Reboot your system.\n");
    }
    else
    {
        printf("No new packages.\n");
    }
    return EXIT_SUCCESS;
}
