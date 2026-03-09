#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILENAME 1024
#define MAX_SONGNAME 64 
#define MAX_DIFFICULTY 6 
#define MAX_SCORE 8 
#define MAX_RATING 5 
#define MAX_DATETIME 19

typedef struct {
    char song[MAX_SONGNAME];
    char difficulty[MAX_DIFFICULTY];
    char score[MAX_SCORE];
    char rating[MAX_RATING];
    char datetime[MAX_DATETIME];
    int valid;
} scoreEntry;

scoreEntry parse_files(const char *filename) {
    scoreEntry entry = {0};

    /* strip file extension */

    char base[MAX_FILENAME];
    strncpy(base, filename, MAX_FILENAME - 1); // strncpy() copies a specific number of characters from a source
                                               // Example: strncpy(destination, source, size)
    char *dot = strrchr(base, '.'); // strrchr() locates the last occurrence of a specified string
                                    // Example: strrchr(stringToSearch, characterSearchedFor)
                                    // strchr() searches for the first, strrchr() searches for the last
    if (dot) *dot = '\0';

    // Split filename into tokens
    char *tokens[64];
    int token_count = 0;

    char *p = base;
    while (*p && token_count < 64) {
        tokens[token_count++] = p;
        char *next = strchr(p, '_');
        if (!next) break;
        *next = '\0';
        p = next + 1;
    }

    // Looking for 4 tokens: song, difficulty, score, rating
    // Song
    if (token_count < 4) return entry;
    strncpy(entry.song, tokens[0], MAX_SONGNAME - 1);

    // Rating (sans DateTime from end of filename)
    char rating_raw[MAX_RATING];
    strncpy(rating_raw, tokens[token_count - 1], MAX_RATING - 1);
    char *space = strchr(rating_raw, ' ');
    if (space) {
        *space = '\0';
        strncpy(entry.datetime, space + 1, MAX_DATETIME - 1);
    }
    strncpy(entry.rating, rating_raw, MAX_RATING - 1);

    strncpy(entry.score, tokens[token_count - 2], MAX_SCORE - 1);
    strncpy(entry.difficulty, tokens[token_count - 3], MAX_DIFFICULTY - 1);

    // Song name, rejoining middle tokens with spaces
    int song_start = 1;
    int song_end = token_count - 4; // inclusive
    if (song_start > song_end) return entry;

    char song_buf[MAX_FILENAME] = {0};
    for (int i = song_start; i <= song_end; i++) {
        strncat(song_buf, tokens[i], MAX_FILENAME - strlen(song_buf) - 1);
        if (i < song_end) strncat(song_buf, " ", MAX_FILENAME - strlen(song_buf) - 1);
    }
    strncpy(entry.song, song_buf, MAX_SONGNAME - 1);

    entry.valid = 1;
    return entry;

}

void print_divider(void) {
    printf("%-68s %-9s %-12s %-8s\n", "--------------------------------------------------------------------", "---------", "------------", "--------");
}

int main(int argc, char *argv[]) {
    const char *dir_path = argc > 1 ? argv[1] : ".";

    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("Error opening directory");
        return 1;
    }

    // Header
    printf("%-68s %-9s %-12s %-8s %-20s\n", "Song", "Difficulty", "Score", "Rating", "DateTime");
    print_divider();

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        scoreEntry sdvx = parse_files(entry->d_name);

        if (!sdvx.valid) {
            printf("[skipped: %s]\n", entry->d_name);
            continue;
        }

        printf("%-68s %-9s %-12s %-8s %-20s\n", sdvx.song, sdvx.difficulty, sdvx.score, sdvx.rating, sdvx.datetime); 
    }

    closedir(dir);
    return 0;
}