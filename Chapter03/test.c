#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE* fd = fopen("test.txt", "w+");
    if(fd == NULL) {
        printf("Error open/create file.\n");
        return 1;
    }

    int a = 2, b = 3;
    float c = 4.5;
    fwrite(&a, sizeof(a), 1, fd);
    fwrite(&b, sizeof(b), 1, fd);
    fwrite(&c, sizeof(c), 1, fd);
    fflush(fd);

    fseek(fd, 0, SEEK_SET);
    a = b = 0; c = 0;
    fread(&a, sizeof(a), 1, fd);
    fread(&b, sizeof(b), 1, fd);
    fread(&c, sizeof(c), 1, fd);
    fclose(fd);
    
    printf("a = %d, b = %d, c = %f\n\n", a, b, c);

    return 0;
}
