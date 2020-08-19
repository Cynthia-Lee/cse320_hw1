#include "const.h"
#include "sequitur.h"

/*
 * Digram hash table.
 *
 * Maps pairs of symbol values to first symbol of digram.
 * Uses open addressing with linear probing.
 * See, e.g. https://en.wikipedia.org/wiki/Open_addressing
 */

/**
 * Clear the digram hash table.
 */
void init_digram_hash(void) {
    // To be implemented.
    // clearing the digram_table
    for (int i = 0; i < MAX_DIGRAMS; i++) {
        *(digram_table + i) = NULL;
    }
}

/**
 * Look up a digram in the hash table.
 *
 * @param v1  The symbol value of the first symbol of the digram.
 * @param v2  The symbol value of the second symbol of the digram.
 * @return  A pointer to a matching digram (i.e. one having the same two
 * symbol values) in the hash table, if there is one, otherwise NULL.
 */
SYMBOL *digram_get(int v1, int v2) {
    // To be implemented.
    int index = DIGRAM_HASH(v1, v2);
    // a[i] == *(a+i)
    // SYMBOL *ptr = (*(digram_table + index));
    while ((*(digram_table + index)) != NULL) {
        // check symbol values of each diagram
        if ((*(digram_table + index)) != TOMBSTONE && (*(digram_table + index))->value == v1 && (*(digram_table + index))->next->value == v2) {
            return (*(digram_table + index));
        }
        (*(digram_table + index)) = *(digram_table + index);
        index++;

        if (index == MAX_DIGRAMS) { // loop through beginning
            index = 0;
        }
    }

    return NULL;
}

/**
 * Delete a specified digram from the hash table.
 *
 * @param digram  The digram to be deleted.
 * @return 0 if the digram was found and deleted, -1 if the digram did
 * not exist in the table.
 *
 * Note that deletion in an open-addressed hash table requires that a
 * special "tombstone" value be left as a replacement for the value being
 * deleted.  Tombstones are treated as vacant for the purposes of insertion,
 * but as filled for the purpose of lookups.
 *
 * Note also that this function will only delete the specific digram that is
 * passed as the argument, not some other matching digram that happens
 * to be in the table.  The reason for this is that if we were to delete
 * some other digram, then somebody would have to be responsible for
 * recycling the symbols contained in it, and we do not have the information
 * at this point that would allow us to be able to decide whether it makes
 * sense to do it here.
 */
int digram_delete(SYMBOL *digram) {
    // To be implemented.
    int index = DIGRAM_HASH(digram->value, digram->next->value);
    SYMBOL *ptr = digram_get(digram->value, digram->next->value);
    if (ptr == NULL) {
        debug("Deleting digram - not found");
        return -1;
    }

    while ((*(digram_table + index)) != NULL) {
        if ((*(digram_table + index)) == digram) {
            debug("Delete digram from table");
            (*(digram_table + index)) = TOMBSTONE;
            return 0;
        }
        index++;

        if (index == MAX_DIGRAMS) {
            index = 0;
        }
    }
    return -1;
}

/**
 * Attempt to insert a digram into the hash table.
 *
 * @param digram  The digram to be inserted.
 * @return  0 in case the digram did not previously exist in the table and
 * insertion was successful, 1 if a matching digram already existed in the
 * table and no change was made, and -1 in case of an error, such as the hash
 * table being full or the given digram not being well-formed.
 */
int digram_put(SYMBOL *digram) {
    // To be implemented.
    int ptr = DIGRAM_HASH(digram->value, digram->next->value);

    if (digram == NULL) {
        return -1;
    }
    int index = DIGRAM_HASH(digram->value, digram->next->value);
    // a[i] == *(a+i)
    // SYMBOL *ptr = (*(digram_table + index));
    // search for vacant entry without first seeing a matching entry
    if (digram_get(digram->value, digram->next->value)) {
        return 1;
    }
    while ((*(digram_table + index)) != NULL && (*(digram_table + index)) != TOMBSTONE) {
        // check symbol values of each diagram
        index++;
        (*(digram_table + index)) = *(digram_table + index);

        if (index == MAX_DIGRAMS) {
            index = 0;
        }

        if (index == ptr) {
            return -1;
        }
    }

    (*(digram_table + index)) = digram; // put digram into the table
    return 0;
}
