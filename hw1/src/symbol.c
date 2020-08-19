#include "const.h"
#include "sequitur.h"

/*
 * Symbol management.
 *
 * The functions here manage a statically allocated array of SYMBOL structures,
 * together with a stack of "recycled" symbols.
 */

/*
 * Initialization of this global variable that could not be performed in the header file.
 */
int next_nonterminal_value = FIRST_NONTERMINAL;

/*
 * Recycled symbols list, pointer to the list head
 */
SYMBOL *recycled_symbols = NULL;

/**
 * Initialize the symbols module.
 * Frees all symbols, setting num_symbols to 0, and resets next_nonterminal_value
 * to FIRST_NONTERMINAL;
 */
void init_symbols(void) {
    // To be implemented.
    num_symbols = 0;
    next_nonterminal_value = FIRST_NONTERMINAL;
    recycled_symbols = NULL;
}

/**
 * Get a new symbol.
 *
 * @param value  The value to be used for the symbol.  Whether the symbol is a terminal
 * symbol or a non-terminal symbol is determined by its value: terminal symbols have
 * "small" values (i.e. < FIRST_NONTERMINAL), and nonterminal symbols have "large" values
 * (i.e. >= FIRST_NONTERMINAL).
 * @param rule  For a terminal symbol, this parameter should be NULL.  For a nonterminal
 * symbol, this parameter can be used to specify a rule having that nonterminal at its head.
 * In that case, the reference count of the rule is increased by one and a pointer to the rule
 * is stored in the symbol.  This parameter can also be NULL for a nonterminal symbol if the
 * associated rule is not currently known and will be assigned later.
 * @return  A pointer to the new symbol, whose value and rule fields have been initialized
 * according to the parameters passed, and with other fields zeroed.  If the symbol storage
 * is exhausted and a new symbol cannot be created, then a message is printed to stderr and
 * abort() is called.
 *
 * When this function is called, if there are any recycled symbols, then one of those is removed
 * from the recycling list and used to satisfy the request.
 * Otherwise, if there currently are no recycled symbols, then a new symbol is allocated from
 * the main symbol_storage array and the num_symbols variable is incremented to record the
 * allocation.
 */
SYMBOL *new_symbol(int value, SYMBOL *rule) {
    // To be implemented.

    // if symbol storage is exhuasted and new symbol cannot be created, message print to stderr and call abort()
    if (num_symbols == MAX_SYMBOLS) {
        fprintf(stderr, "Symbol storage exhuasted.\n");
        abort();
    }

    if (value < FIRST_NONTERMINAL) { // terminal  symbol
        if (rule != NULL) { // rule is null
            return NULL;
        }
    } else { // nonterminal
        // rule having a nonterminal at its head
        // reference count of the rule is increased by one and a pointer to the rule is stored in the symbol
        if (rule != NULL) {
            rule->refcnt = (rule->refcnt) + 1;
        }
        // rule can also be null if rule is not known or assigned later
    }

    SYMBOL *symbolPtr = 0;
    // recycled_symbols
    if (recycled_symbols != NULL) { // there exists recycled symbols
        // one is removed, take the symbol and 0 all values, use to satisfy request
        symbolPtr = recycled_symbols;
        // move pointer to next
        recycled_symbols = recycled_symbols->next; // next on stack
    } else { // otherwise use symbol_storage and num_symbols++
        // return a pointer to the new symbol, which values and fields have been initialized based on parameters
        // other fields are zeroed
        symbolPtr = symbol_storage + num_symbols; // pointer into storage
        num_symbols++;
    }

    symbolPtr->value = value;
    symbolPtr->rule = rule;
    symbolPtr->refcnt = 0; // reference count if symbol is head of a rule, otherwise 0
    symbolPtr->next = 0;
    symbolPtr->prev = 0;
    symbolPtr->nextr = 0;
    symbolPtr->prevr = 0;
    return symbolPtr;
}

/**
 * Recycle a symbol that is no longer being used.
 *
 * @param s  The symbol to be recycled.  The caller must not use this symbol any more
 * once it has been recycled.
 *
 * Symbols being recycled are added to the recycled_symbols list, where they will
 * be made available for re-allocation by a subsequent call to new_symbol.
 * The recycled_symbols list is managed as a LIFO list (i.e. a stack), using the
 * next field of the SYMBOL structure to chain together the entries.
 */
void recycle_symbol(SYMBOL *s) {
    // To be implemented.
    debug("Recycle symbol");
    s->next = recycled_symbols; // add to recycle list
    recycled_symbols = s;
}
