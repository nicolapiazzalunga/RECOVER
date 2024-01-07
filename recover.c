#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// create building block for memory usage
typedef uint8_t BYTE;
typedef BYTE MEMORYBLOCK[512]; // credit: Tol3iaß von Löweberg

int main(int argc, char *argv[])
{
    // ensure input validity
    if (argc != 2)
    {
        fprintf(stderr, "Usage recover file.raw\n");
        return 1;
    }

    // remember memory card image file name
    char *infile = argv[1];

    // open memory card image file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Coud not open: %s.\n", infile);
        return 2;
    }

    // allocate space for memory buffer
    BYTE *buffer = malloc(sizeof(MEMORYBLOCK));
    if (buffer == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        return 3;
    }

    // declare outfile pointer
    FILE *outptr = NULL;

    // initialize file counter
    int fileCounter;

    // initialize filename string
    char filename[8];

    // iterate over memeory 512B blocks
    while (fread(buffer, sizeof(MEMORYBLOCK), 1, inptr) > 0)
    {
        // if new file tag is spotted
        if (buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            // if no JPEG file is currently open
            if (outptr == NULL)
            {
                // make a name for new JPEG file (000.jpg)
                fileCounter = 0;
                sprintf(filename, "%03i.jpg", fileCounter);

                // open a new JPEG file
                outptr = fopen(filename, "w");
                if (outptr == NULL)
                {
                    fprintf(stderr, "Coud not open: %s.\n", filename);
                    return 2;
                }
            }
            // else if a JPEG file is currently open
            else
            {
                // close previous JPEG file
                fclose(outptr);

                // make a name for new JPEG file
                fileCounter = fileCounter + 1;
                sprintf(filename, "%03i.jpg", fileCounter);

                // open a new JPEG file
                outptr = fopen(filename, "w");
                if (outptr == NULL)
                {
                    fprintf(stderr, "Coud not open: %s.\n", filename);
                    return 2;
                }
            }
        }

        // if a JPEG file exists, write
        if (outptr != NULL)
        {
            // write buffer into current JPEG file
            fwrite(buffer, sizeof(MEMORYBLOCK), 1, outptr);
        }
    }

    // free buffer
    free(buffer);

    // close infile
    fclose(inptr);
    fclose(outptr);

}
