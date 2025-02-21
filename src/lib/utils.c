#include "utils.h"
#include "stdio.h"
#include "stdlib.h"

char *ReadFile(char *filename) {
    char * buffer = 0;
    long length;
    FILE *f = fopen (filename, "rb");

    if (f)
    {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = malloc (length + 1);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        fclose (f);
    }

    if (buffer)
    {
        buffer[length] = '\0';
        return buffer;
    } else {
        return NULL;
    }
}