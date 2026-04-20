#include "index.h"
#include <stdio.h>
#include <string.h>

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
