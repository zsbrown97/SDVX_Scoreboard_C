#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PARTS 10
#define MAX_PART_LEN 256
#define MAX_FILENAME 1024

typedef struct {
    char parts[MAX_PARTS][MAX_PART_LEN];
    int  count;
} ParsedFilename;

ParsedFilename parse_filename(const char *filename) {
    ParsedFilename result = {0};

    // Strip file extension if present
    char base[MAX_FILENAME];
    strncpy(base, filename, MAX_FILENAME - 1);
    char *dot = strrchr(base, '.');
    if (dot) *dot = '\0';

    // Tokenize on underscores
    char *token = strtok(base, "_");
    while (token != NULL && result.count < MAX_PARTS) {
        strncpy(result.parts[result.count++], token, MAX_PART_LEN - 1);
        token = strtok(NULL, "_");
    }

    return result;
}

int find_max_parts(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) return 0;

    int max = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        ParsedFilename pf = parse_filename(entry->d_name);
        if (pf.count > max) max = pf.count;
    }

    closedir(dir);
    return max;
}

int main(int argc, char *argv[]) {
    const char *dir_path = argc > 1 ? argv[1] : ".";

    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("Error opening directory");
        return 1;
    }

    // First pass: find the max number of parts to size the columns
    int num_cols = find_max_parts(dir_path);
    if (num_cols == 0) {
        printf("No files found.\n");
        closedir(dir);
        return 0;
    }

    // Print header row
    printf("%-30s", "Filename");
    for (int i = 0; i < num_cols; i++) {
        printf("| Part %-2d ", i + 1);
    }
    printf("\n");

    // Print divider
    int divider_len = 40 + (num_cols * 10);
    for (int i = 0; i < divider_len; i++) printf("-");
    printf("\n");

    // Second pass: print each file and its parts
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        ParsedFilename pf = parse_filename(entry->d_name);

        printf("%-30s", entry->d_name);
        for (int i = 0; i < num_cols; i++) {
            if (i < pf.count)
                printf("| %-8s ", pf.parts[i]);
            else
                printf("| %-8s ", "-");   // pad missing parts with a dash
        }
        printf("\n");
    }

    closedir(dir);
    return 0;
}