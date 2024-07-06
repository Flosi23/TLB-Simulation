#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "types.h"

#define INITIAL_CAPACITY 100
#define MAX_LINE 1024 // max size for a line

struct Request *parseCSVFile(const char *filename, size_t *requestCount) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: The File %s could not be opened\n", filename);
        exit(EXIT_FAILURE);
    }

    size_t capacity = INITIAL_CAPACITY;
    struct Request *requests = malloc(capacity * sizeof(struct Request));
    if (!requests) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];

    // reset
    *requestCount = 0;

    while (fgets(line, MAX_LINE, file)) {

        // remove any newline characters
        char *newline = strchr(line, '\n');
        if (newline) {
            *newline = '\0';
        }

        // don't overflow line size
        if (!newline && !feof(file)) {
            fprintf(stderr, "Error: Line %zu is too long\n", *requestCount + 1);
            // ignore rest of line
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF);
            continue;
        }

        if (*requestCount >= capacity) {
            capacity *= 2;
            struct Request *temp = realloc(requests, capacity * sizeof(struct Request));
            if (!temp) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                free(requests);
                fclose(file);
                exit(EXIT_FAILURE);
            }
            requests = temp;
        }

        // start parsing line string
        char *token;
        token = strtok(line, ",");
        if (token == NULL || (strcmp(token, "R") != 0 && strcmp(token, "W") != 0)) {
            fprintf(stderr, "Error: Invalid operation type %zu\n", *requestCount + 1);
            continue;
        }
        requests[*requestCount].we = (strcmp(token, "W") == 0);

        token = strtok(NULL, ",");
        if (token == NULL) {
            fprintf(stderr, "Error: Address is missing in line %zu\n", *requestCount + 1);
            continue;
        }
        requests[*requestCount].addr = (uint32_t) strtoul(token, NULL, 0);  // 0 allows for both decimal and hex

        token = strtok(NULL, ",");
        if (requests[*requestCount].we) {
            if (token == NULL) {
                fprintf(stderr, "Error: Data for write is missing in line %zu\n", *requestCount + 1);
                continue;
            }
            requests[*requestCount].data = (uint32_t) strtoul(token, NULL, 0);
        } else {
            if (token != NULL && !isspace((unsigned char) *token)) {
                fprintf(stderr, "Error: Unexpected Data when reading line %zu\n", *requestCount + 1);
                continue;
            }
            requests[*requestCount].data = 0;  // Initialize to 0 for read operations
        }

        (*requestCount)++;
    }

    // cleanup
    fclose(file);

    return requests;
}
