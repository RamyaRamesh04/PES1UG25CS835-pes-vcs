#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include "pes.h"
#include "index.h"
// ---------------- FIX: index_load ----------------
int index_load(Index *index) {
    FILE *f = fopen(".pes/index", "r");
    index->count = 0;

    if (!f) return 0;

    while (index->count < MAX_INDEX_ENTRIES) {
        IndexEntry *e = &index->entries[index->count];

        int r = fscanf(f, "%o %64s %lu %lu %511s",
                       &e->mode,
                       (char *)e->hash.hash,
                       &e->mtime_sec,
                       &e->size,
                       e->path);

        if (r != 5) break;

        index->count++;
    }

    fclose(f);
    return 0;
}

// ---------------- FIX: index_save (minimal) ----------------
int index_save(const Index *index) {
    FILE *f = fopen(".pes/index", "w");
    if (!f) return -1;

    for (int i = 0; i < index->count; i++) {
        const IndexEntry *e = &index->entries[i];

        fprintf(f, "%o %s %lu %lu %s\n",
                e->mode,
                (char *)e->hash.hash,
                (unsigned long)e->mtime_sec,
                (unsigned long)e->size,
                e->path);
    }

    fclose(f);
    return 0;
}
int index_add(Index *index, const char *path) {
    ObjectID id;
    // Hash the file first to get the ObjectID
    if (object_hash_file(path, &id) != 0) {
        return -1;
    }

    // Check if file already exists in index
    for (int i = 0; i < index->count; i++) {
        if (strcmp(index->entries[i].path, path) == 0) {
            memcpy(&index->entries[i].hash, &id, sizeof(ObjectID));
            struct stat st;
            if (stat(path, &st) == 0) {
                index->entries[i].mode = st.st_mode;
                index->entries[i].size = st.st_size;
            }
            return 0;
        }
    }

    // Add new entry
    if (index->count < MAX_INDEX_ENTRIES) {
        IndexEntry *e = &index->entries[index->count++];
        strncpy(e->path, path, sizeof(e->path) - 1);
        memcpy(&e->hash, &id, sizeof(ObjectID));
        struct stat st;
        if (stat(path, &st) == 0) {
            e->mode = st.st_mode;
            e->size = st.st_size;
        }
        return 0;
    }
    return -1;
}

void index_status(Index *index) {
    printf("On branch main\n");
    if (index->count == 0) {
        printf("nothing to commit, working tree clean\n");
        return;
    }
    printf("Changes to be committed:\n");
    for (int i = 0; i < index->count; i++) {
        printf("\tnew file: %s\n", index->entries[i].path);
    }
}
