#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

#define MAX_NAME_LEN 36
typedef struct __Record {
    unsigned int    id;
    char    name[MAX_NAME_LEN + 1];
}Record;

int main() {
    Record rec;
    const char *file_path = "/tmp/mmap_test.txt";
    const int RECORD_COUNT = 60;

    // Write records
    {
        FILE* fd = fopen(file_path, "w+");
        for(int i = 0; i < RECORD_COUNT; ++i) {
            rec.id = i;
            sprintf(rec.name, "Record with id of %d", i);
            fwrite(&rec, sizeof(rec), 1, fd);
        }
        fclose(fd);
    }

    // Use mmap to modify
    {
        int fd = open(file_path, O_RDWR);
        Record *recArr = (Record*)mmap(NULL, RECORD_COUNT * sizeof(Record), 
                          PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        
        for(int i = 0; i < RECORD_COUNT; ++i) {
            recArr[i].id += 100;
            sprintf(recArr[i].name, "Record with id of %d", recArr[i].id);
        }
        munmap(recArr, RECORD_COUNT * sizeof(Record));
        close(fd);   
    }

    // Verify the mmap result
    {
        FILE* fd = fopen(file_path, "r");
        for(int i = 0; i < RECORD_COUNT; ++i) {
            fread(&rec, sizeof(Record), 1, fd);
            assert(rec.id == i + 100);
        }
        fclose(fd);
    }

    return 0;
}
