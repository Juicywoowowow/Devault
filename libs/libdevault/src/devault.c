#include "devault.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

static char WORK_DIR[1024] = ".devault";
static char HISTORY_FILE[1024];

int devault_init(const char *output_dir) {
    if (output_dir && strlen(output_dir) > 0) {
        snprintf(WORK_DIR, sizeof(WORK_DIR), "%s", output_dir);
    }

    struct stat st = {0};
    if (stat(WORK_DIR, &st) == -1) {
        if (mkdir(WORK_DIR, 0755) != 0) {
            perror("Failed to create .devault directory");
            return -1;
        }
    }

    snprintf(HISTORY_FILE, sizeof(HISTORY_FILE), "%s/history.log", WORK_DIR);
    return 0;
}

bool devault_is_attempted(const char *unique_id) {
    FILE *f = fopen(HISTORY_FILE, "r");
    if (!f) return false;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        // Strip newline
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, unique_id) == 0) {
            fclose(f);
            return true;
        }
    }
    fclose(f);
    return false;
}

int devault_log_attempt(const char *unique_id) {
    FILE *f = fopen(HISTORY_FILE, "a");
    if (!f) return -1;
    fprintf(f, "%s\n", unique_id);
    fclose(f);
    return 0;
}

int devault_save_dec(const char *unique_id, const uint8_t *content, size_t len) {
    char filepath[1024];
    // Sanitize unique_id for filename if needed, but for now assume it's safe (algo_key)
    snprintf(filepath, sizeof(filepath), "%s/%s.dec", WORK_DIR, unique_id);

    FILE *f = fopen(filepath, "wb");
    if (!f) return -1;
    fwrite(content, 1, len, f);
    fclose(f);
    return 0;
}

// Helper to track file info for sorting
typedef struct {
    char name[256];
    time_t mtime;
} FileInfo;

int compare_file_info(const void *a, const void *b) {
    FileInfo *fa = (FileInfo *)a;
    FileInfo *fb = (FileInfo *)b;
    // Sort ascending by mtime (oldest first)
    if (fa->mtime < fb->mtime) return -1;
    if (fa->mtime > fb->mtime) return 1;
    return 0;
}

int devault_enforce_buffer(int limit) {
    DIR *d;
    struct dirent *dir;
    d = opendir(WORK_DIR);
    if (!d) return -1;

    // First count .dec files and gather metadata
    // For simplicity in C, we'll assume a max number of files to track in memory for sorting
    // or we'll just implement a simple approach: if count > limit, delete the oldest.

    // To avoid massive memory usage on "thousands", we might need a better approach,
    // but for this assignment, a dynamic array of struct is likely fine.

    FileInfo *files = NULL;
    size_t count = 0;
    size_t capacity = 0;

    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, ".dec")) {
            if (count >= capacity) {
                capacity = capacity == 0 ? 1024 : capacity * 2;
                files = realloc(files, capacity * sizeof(FileInfo));
                if (!files) { closedir(d); return -1; }
            }

            strncpy(files[count].name, dir->d_name, sizeof(files[count].name) - 1);
            files[count].name[sizeof(files[count].name)-1] = '\0';

            struct stat st;
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", WORK_DIR, dir->d_name);
            if (stat(fullpath, &st) == 0) {
                files[count].mtime = st.st_mtime;
                count++;
            }
        }
    }
    closedir(d);

    if ((int)count > limit) {
        // Sort files by modification time
        qsort(files, count, sizeof(FileInfo), compare_file_info);

        int to_delete = count - limit;
        for (int i = 0; i < to_delete; i++) {
             char fullpath[1024];
             snprintf(fullpath, sizeof(fullpath), "%s/%s", WORK_DIR, files[i].name);
             remove(fullpath);
        }
    }

    free(files);
    return 0;
}
