#include "const.h"
#include "sequitur.h"
#include "debug.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/*
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 *
 * IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
 * you MAY NOT declare any arrays or allocate any storage with malloc().
 * The purpose of this restriction is to force you to use pointers.
 * Variables to hold the pathname of the current file or directory
 * as well as other data have been pre-declared for you in const.h.
 * You must use those variables, rather than declaring your own.
 * IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
 *
 * IMPORTANT: You MAY NOT use floating point arithmetic or declare
 * any "float" or "double" variables.  IF YOU VIOLATE THIS RESTRICTION,
 * YOU WILL GET A ZERO!
 */

/*
 * Helper method to compare if two Strings (array of chars) are equal to each other
 */
int compare_string(char *str1, char *str2) {
    // char *str1, address of first char of the array of chars
    char *p1;
    char *p2 = str2;
    for (p1 = str1; *p1 != '\0'; p1++) {
        if (*p1 != *p2 || *p2 == '\0') {
            return -1;
        }
        p2++;
    }
    if (*p2 != '\0') {
        return -1;
    }
    return 0;
}

/*
 * Helper method to get integer value from a Strings (array of chars)
 */
int string_to_int(char *str) {
    int number = 0;
    char *p;
    for (p = str; *p != '\0'; p++) {
        if (*p < '0' || *p > '9') { // if the char is not a number
            return -1;
        }
        number = number * 10 + *p - '0';
    }
    return number;
}

/*
 * Helper method to get integer value from a Strings (array of chars)
 */
int set_msb_global_options(int number) {
    number = number << 16;
    int maskLength = (1 << 16) - 1;
    int mask = ((maskLength) << 15) & number;
    global_options |= mask;
    // printf("%d", global_options);
    return 0;
}

/*
 * Helper method to read a UTF-8 encoded character from an input stream
 * and identify the special marker bytes
 */
int utf_size(int b1) {
    // each UTF-8 encoded character is a symbol, which can be 1 - 4 bytes
    // each rule has a squence of symbols (any number of symbols)

    // utf is in int form, leading zeros then char data (8) lsb
    // first byte, check msb
    if ((b1 & 0x80) == 0) { // 1 byte
        return 1;
    } else if ((b1 & 0xe0) == 0xc0) { // 2 bytes
        return 2;
    } else if ((b1 & 0xf0) == 0xe0) { // 3 bytes
        return 3;
    } else if ((b1 & 0xf8) == 0xf0) { // 4 bytes
        return 4;
    }
    return -1;
}

/*
 * Expands block to readable characters.
 */
int block_expansion(SYMBOL *rule, FILE *out) {
    // starting from the main_rule
    // traverse body of the rule, symbol by symbol
    SYMBOL *ptr = rule;
    ptr = ptr->next;
    int count = 0;

    while (ptr != rule) {
        if (IS_TERMINAL(ptr)) { // terminal
            fputc((ptr->value), out);
            count++;
        } else { // nonterminal
            // need to find rule that has symbol as its head
            count += block_expansion((*(rule_map + (ptr->value))), out);
        }
        ptr = ptr->next;
    }
    return count;

    // each time a terminal symbol is encountered, use fputc() to write to the output stream
        // a single data bye corresponding to the value of that symbol
    // nonterminal symbol is encountered, recurse and expand using the rule having
        // that nonterminal symbol as its head

    // need to be able to find, given a nonterminal symbol,
        // the rule that has that symbol as its head
    // rule_map

    // while reading a block, each time you construct a rule,
    // put a pointer to the head of the rule in the entry of rule_map
    // correspopnding to the nonterminal at the rule head
    // during expansion, when you encounter a nonterminal, you can then use the rule map
    // to go directly to the associated rule
}

/*
 * Takes in an int (char), and outputs it into UTF-8 format.
 */
int byte_utf(int b, FILE *out, int *count) {
    if (b >= 0x0000 && b <= 0x007f) { // 1 byte
        fputc(b, out);
        (*count)++;
        return 0;
    } else if (b >= 0x0080 && b <= 0x07ff) { // 2 bytes
        // xxyy -> 3xx 2yy
        int b2 = 0x80 | (b & 0x3f);// 10, bottom 6
        int b1 = 0xc0 | (b >> 6); // 110, first 5
        fputc(b1, out);
        (*count)++;
        fputc(b2, out);
        (*count)++;
        return 0;
    } else if (b >= 0x0800 && b <= 0xffff) { // 3 bytes
        // xyyzz -> 34x 2yy 2zz
        int b3 = 0x80 | (b & 0x3f); // 10, bottom 6
        int b2 = 0x80 | ((b >> 6) & 0x3f);// 10, next 6
        int b1 = 0xe0 | (b >> 12); // 1110, first 4
        fputc(b1, out);
        (*count)++;
        fputc(b2, out);
        (*count)++;
        fputc(b3, out);
        (*count)++;
        return 0;
    } else if (b >= 0x10000 && b <= 0x10ffff) { // 4 bytes
        // 1xyyzz -> 35x 2yy 2zz
        int b4 = 0x80 | (b & 0x3f); // 10, bottom 6
        int b3 = 0x80 | ((b >> 6) & 0x3f); // 10, next 6
        int b2 = 0x80 | ((b >> 6) & 0x3f);// 10, next 6
        int b1 = 0xe0 | (b >> 18); // 1110, first 4
        fputc(b1, out);
        (*count)++;
        fputc(b2, out);
        (*count)++;
        fputc(b3, out);
        (*count)++;
        fputc(b4, out);
        (*count)++;
        return 0;
    }
    return -1;
}

/**
 * Main compression function.
 * Reads a sequence of bytes from a specified input stream, segments the
 * input data into blocks of a specified maximum number of bytes,
 * uses the Sequitur algorithm to compress each block of input to a list
 * of rules, and outputs the resulting compressed data transmission to a
 * specified output stream in the format detailed in the header files and
 * assignment handout.  The output stream is flushed once the transmission
 * is complete.
 *
 * The maximum number of bytes of uncompressed data represented by each
 * block of the compressed transmission is limited to the specified value
 * "bsize".  Each compressed block except for the last one represents exactly
 * "bsize" bytes of uncompressed data and the last compressed block represents
 * at most "bsize" bytes.
 *
 * @param in  The stream from which input is to be read.
 * @param out  The stream to which the block is to be written.
 * @param bsize  The maximum number of bytes read per block.
 * @return  The number of bytes written, in case of success,
 * otherwise EOF.
 */
int compress(FILE *in, FILE *out, int bsize) {
    int count = 0;
    // To be implemented.
    bsize = bsize * 1024;
    fputc(0x81, out); // SOT
    count++;
    int i = 0;
    SYMBOL * currRule = NULL;
    // read blocks of data from the input stream
    int c = fgetc(in);
    while (c != EOF) {
        // printf("%c\n", c);

        // create terminal symbol from input byte
        // use insert_after() to append symbol to main rule
        // call check_diagram() on second-to-last symbol in main rule
        if (i == 0) { // SOB
            init_rules();
            init_symbols();
            init_digram_hash();
            // main rule is created and installed
            currRule = new_rule(next_nonterminal_value++);
            add_rule(currRule);
        }

        while (i < bsize && c != EOF) { // less than a block
            // byte of data read
                // append to end of main rule, insert_after()
                // check_digram()
                    // to correct violations of the two constraints that the set of rules
                    // is supposed to satisfy aaccording to the Sequitur algorithm.
                SYMBOL *sym = new_symbol(c, NULL);
                insert_after(main_rule->prev, sym); // last rule
                check_digram(sym->prev);
                i++;

                c = fgetc(in);
        }
        // sometimes less than the block size

        // rules in the block are output as a single block of compressed datas
        // SOB, -RD-RD-RD, EOB
        fputc(0x83, out); // SOB
        count++;
        currRule = main_rule;
        // each rule

        do {
            // each symbol in the rule
            SYMBOL *sym = currRule;
            do {
                byte_utf(sym->value, out, &count);
                sym = sym->next;
            } while(sym != currRule);

            if (currRule->nextr != main_rule) {
                fputc(0x85, out); // RD
                count++;
            }
            currRule = currRule->nextr;
        } while(currRule != main_rule);

        fputc(0x84, out); // EOB
        count++;
        i = 0;
        currRule = NULL;
    }

    fputc(0x82, out); // EOT
    count++;
    // debug("%d", bsize);
    // printf("%d", count);
    return count;
}

/**
 * Main decompression function.
 * Reads a compressed data transmission from an input stream, expands it,
 * and and writes the resulting decompressed data to an output stream.
 * The output stream is flushed once writing is complete.
 *
 * @param in  The stream from which the compressed block is to be read.
 * @param out  The stream to which the uncompressed data is to be written.
 * @return  The number of bytes written, in case of success, otherwise EOF.
 */
int decompress(FILE *in, FILE *out) {
    // To be implemented.
    init_symbols();
    init_rules();

    if (in == NULL) {
        return EOF;
    }

    int c = fgetc(in); // SOT
    if (c != 0x81) {
        return EOF;
    }

    // SOT = 0x81, EOT = 0x82
    // SOB = 0x83, EOB = 0x84, RD = 0x85

    int blockBusy = 0;
    // 1 = calulating block (busy), 0 = not calulating block (free), EOB flag
    int count = 0;
    int utfByteIndex = 0;
    int utfSize = 0;
    int value = 0;
    SYMBOL *currRule = NULL;
    SYMBOL *currSymbol = NULL;
    int head = 0;
    int ruleSize = 0;

    c = fgetc(in);
    if (c != 0x83 && c != 0x82) {
        return EOF;
    }
    // read input stream byte by byte
    while (c != EOF) { // advances pointer for the stream

        if (c == 0x82 && blockBusy == 0) { // EOT
            fflush(out);
            return count;
        }

        // check SOB, EOB, RD
        // each rule has a sequence of symbols
        // each symbol can be 1,2,3 or 4 bytes (UTF-8 encoded)
        // no UTF-8 encoding seq can start with a "continuation byte" having msb of 10
        // 0x85, 0x84, 0x85 has 10 msb

        if (c == 0x83 && blockBusy == 0) { // SOB
            init_rules();
            blockBusy = 1;
            // followed by a rule, rule has sequence of UTF-8
            c = fgetc(in);
            utfByteIndex = 0;
            utfSize = utf_size(c); // read byte 1 of UTF-8
            if (utfSize == -1) {
                return EOF;
            }
        } else if (utfByteIndex < utfSize) { // translate each utf
            // go through specified # of bytes
            if (utfByteIndex == 0) { // first symbol (utf) is head
                if (utfSize == 1) { // bottom 7
                    value = c & 0x7f;
                } else if (utfSize == 2) { // bottom 5
                    value = c & 0x1f;
                } else if (utfSize == 3) { // bottom 4
                    value = c & 0xf;
                } else if (utfSize == 4) { // bottom 3
                    value = c & 7;
                }
            } else {
                value = (value << 6 | (c & 0x3f));
            }
            utfByteIndex++;
            c = fgetc(in);
        } else if ((utfByteIndex == utfSize) && utfSize) {
            // new_rule, construct new rules specified nonterminal at the head
            if (head == 0) { // if first utf in rule
                currRule = new_rule(value); // head, nonterminal
                if (IS_TERMINAL(currRule)) {
                    return EOF;
                }
                head = 1;
            }
            // new_symbol, each instance of symbol in a rule
            if (currSymbol == NULL) { // head, nonterminal
                currSymbol = currRule;
            } else { // terminal
                currSymbol = new_symbol(value, NULL);
                // prev node is currRule->prev
                // doubly linked list
                (currRule->prev)->next = currSymbol;
                currSymbol->prev = (currRule->prev);
                currRule->prev = currSymbol; // head->prev = new symbol
                currSymbol->next = currRule; // last symbol->next = head
            }
            if (c != 0x85 && c!= 0x84) {
                utfSize = utf_size(c);
            } else {
                utfSize = 0;
            }
            utfByteIndex = 0;
            ruleSize++;
        } else if (c == 0x85 && utfSize == 0) { // RD
            // the last symbol in the rule body points to the head sentintel of the current rule
            // the prev field of the sentinel head points to the last symbol of the rule
            if (ruleSize < 3) { // at least 3 (1h, 2b) symbols in a rule
                if (head == 1 && ruleSize == 2 && IS_TERMINAL(currSymbol)) {

                } else {
                    return EOF;
                }
            }
            // add_rule, to link rules into a list of all rules by main_rule variable
            if ( (*(rule_map + (currRule->value))) != NULL) {
                return EOF;
            }
            add_rule(currRule);
            *(rule_map + (currRule->value)) = currRule; // rule_map
            // each time you construct a rule, put a pointer to the head of the rule in the entry of rule map
            // rule_map[nonterminal->value]
            // a[i] == *(a+i)
            head = 0;
            ruleSize = 0;
            currRule = NULL;
            currSymbol = NULL;
            c = fgetc(in);
            utfSize = utf_size(c); // read byte 1 of UTF-8
        } else if (c == 0x84 && utfSize == 0) { // EOB
            if (ruleSize < 3) { // at least 3 (1h, 2b) symbols in a rule
                if (head == 1 && ruleSize == 2 && IS_TERMINAL(currSymbol)) {

                } else {
                    return EOF;
                }
            }
            // add_rule, to link rules into a list of all rules by main_rule variable
            if ( (*(rule_map + (currRule->value))) != NULL) {
                return EOF;
            }
            add_rule(currRule);
            *(rule_map + (currRule->value)) = currRule; // rule_map
            blockBusy = 0;
            head = 0;
            ruleSize = 0;
            count += block_expansion(main_rule, out); // expansion
            currRule = NULL;
            currSymbol = NULL;
            c = fgetc(in);
            if (c != 0x83 && c != 0x82) {
                return EOF;
            }
        } else {
            return EOF;
        }
    }
    return EOF;
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv)
{
    // To be implemented.
    global_options = 0;

    if (argc == 1) { // invalid
        return -1;
    }

    char *p = *(argv + 1);
    int i;
    int optB = 0;
    int cdFlag = 1; // true

    // char *help = *(argv + 1);
    // printf("%s", help);
    // printf("%s", p);
    // int test = compare_string(p, "-h");
    // printf("%d", test);

    if (compare_string(p, "-h") == 0) { // must be the first flag
        global_options |= 1;
        return 0;
    }

    // -h, least sig bit (bit 0) is 1
    // -c, second least sig bit (bit 1) is 1
    // -d, third least sig bit (bit 2) is 1
    // -b, if specified = 16 most sig bit of global_options
        // if not spefified  = 16 most sig bit of global_options 0x0400

    for (i = 1; i < argc; i++) { // iterate through the arguments
        if (compare_string(p, "-h") == 0) { // not first flag
            return -1;
        } else if (compare_string(p, "-c") == 0 && cdFlag) {
            optB = 1;
            cdFlag = 0; // false
            global_options |= 1 << 1;
        } else if (compare_string(p, "-d") == 0 && cdFlag) {
            cdFlag = 0; // false
            global_options |= 1 << 2;
        } else if (compare_string(p, "-b") == 0 && optB == 1 && i != argc - 1) { // make sure it is after -c
            // check next argument exists
            char *blocksize = *(argv + i + 1); // [1, 1024]
            int numBlocksize = string_to_int(blocksize);
            if (numBlocksize < 1 || numBlocksize > 1024) {
                return -1;
            }
            set_msb_global_options(numBlocksize);
            i++;
            optB++;
        } else {
            return -1;
        }
        p = *(argv + i + 1);
    }

    if (optB == 1) { // -c flag, no -b flag
        set_msb_global_options(0x0400); // if no block size is specified, use default 1024
    }

    return 0;
}
