// Scan a directory

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>

void printdir(const char* dirname, unsigned int leadingSpaceNum) {
    struct stat curStat;
    int i = 0;
    if(dirname == NULL)
        return;

    if(strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0)
        return;

    printf("%*s%s\n", leadingSpaceNum, " ", dirname);

    if(stat(dirname, &curStat)) {
        fprintf(stderr, "Error when stating %s: %s\n", dirname, strerror(errno));
        return;
    }

    if(S_ISDIR(curStat.st_mode)) {
        DIR* dir = opendir(dirname);
        struct dirent* item = NULL;
        if(dir == NULL) {
            fprintf(stderr, "Error when openning %s: %s\n", dirname, strerror(errno));
            return;
        }
        chdir(dirname);

        while(NULL != (item = readdir(dir))) {
            printdir(item->d_name, leadingSpaceNum + 2);       
        }

        chdir("..");
        closedir(dir);
    }

    return;
}

int main(int argc, const char* argv[]) {
    char dirstr[256];
    char* parentdir = NULL, *curdir = NULL, *dirc = NULL;
    if(argc < 2) {
        getcwd(dirstr, 256);
    }
    else {
        strcpy(dirstr, argv[1]);
    }

    dirc = strdup(dirstr);
    parentdir = dirname(dirc);
    curdir = basename(dirstr);

    chdir(parentdir);
    printdir(curdir, 0);
    return 0;
}
