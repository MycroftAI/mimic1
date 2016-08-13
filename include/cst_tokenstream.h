/*************************************************************************/
/*                                                                       */
/*                  Language Technologies Institute                      */
/*                     Carnegie Mellon University                        */
/*                        Copyright (c) 1999                             */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*             Author:  Alan W Black (awb@cs.cmu.edu)                    */
/*               Date:  July 1999                                        */
/*************************************************************************/
/*                                                                       */
/*  Tokenizer for strings and files                                      */
/*                                                                       */
/*************************************************************************/
#ifndef _CST_TOKENSTREAM_H__
#define _CST_TOKENSTREAM_H__

#include "cst_alloc.h"
#include "cst_string.h"
#include "cst_file.h"
#include "cst_features.h"

typedef struct cst_tokenstream_struct {
    cst_file fd;
    int file_pos;
    int line_number;
    int eof_flag;
    cst_string *string_buffer;

    cst_string current_char[5]; /* UTF-8 character: 4 bytes + '\0' */

    int token_pos;
    int ws_max;
    cst_string *whitespace;
    int prep_max;
    cst_string *prepunctuation;
    int token_max;
    cst_string *token;
    int postp_max;
    cst_string *postpunctuation;

    cst_features *tags;         /* e.g xml tags */
    /* if set will find token boundaries at every utf8 character */
    int utf8_explode_mode;

    void *streamtype_data;

    /* Should only be set through set_charclasses as charclass table needs */
    /* to be updated when you reset these                                  */
    const cst_string *p_whitespacesymbols;
    const cst_string *p_singlecharsymbols;
    const cst_string *p_prepunctuationsymbols;
    const cst_string *p_postpunctuationsymbols;

    /* 1-byte long UTF-8 characters are in [0, 128) */
    cst_string charclass[128];
    /* 2-byte long UTF-8 characters have a first UTF-8 character like
     *  110xxxxx, so 32 possibilities are only possible*/
    cst_string *charclass2[32];
    /* 3-byte long UTF-8 characters have a first UTF-8 character like
     * 1110xxxx, so 16 possibilities are only possible*/
    cst_string **charclass3[16];
    /* 4-byte long UTF-8 characters have a first UTF-8 character like
     * 11110xxx, so 8 possibilities are only possible*/
    cst_string ***charclass4[8];

    /* To allow externally specified reading functions e.g. epub/xml */
    int (*open) (struct cst_tokenstream_struct *ts, const char *filename);
    void (*close) (struct cst_tokenstream_struct *ts);
    int (*eof) (struct cst_tokenstream_struct *ts);
    int (*seek) (struct cst_tokenstream_struct *ts, int pos);
    int (*tell) (struct cst_tokenstream_struct *ts);
    int (*size) (struct cst_tokenstream_struct *ts);
    int (*getc) (struct cst_tokenstream_struct *ts);
} cst_tokenstream;

#define TS_CHARCLASS_NONE        0
#define TS_CHARCLASS_WHITESPACE  2
#define TS_CHARCLASS_SINGLECHAR  4
#define TS_CHARCLASS_PREPUNCT    8
#define TS_CHARCLASS_POSTPUNCT  16
#define TS_CHARCLASS_QUOTE      32

/**
 * Receives an UTF-8 character and a class (for instance:
 * `TS_CHARCLASS_WHITESPACE` or `TS_CHARCLASS_POSTPUNCT`) and returns
 * whether or not the UTF-8 character given belongs to that class.
 * If an invalid UTF-8 character is given the returned value is 
 * undefined.
 * 
 * If you want to understand how the classes are set, read
 * `set_charclass_table_symbol` in `src/utils/cst_tokenstream.c`.
 * 
 * @see set_charclasses, set_charclass_table_symbol
 * 
 * @param utf8char A cst_string containing a utf-8 character, that can
 *                 be 1 to 4 bytes long.
 * @param class    An integer, usually one of the `TS_CHARCLASS_*` macros
 * @param ts       The tokenstream that has defined which characters
 *                 belong to what classes.
 * 
 * @return Returns 0 if the given character does not belong to the given
 *         class. It returns the class otherwise.
 */
int ts_charclass(const cst_string *const utf8char, int cclass, cst_tokenstream *ts);

extern const cst_string *const cst_ts_default_whitespacesymbols;
extern const cst_string *const cst_ts_default_prepunctuationsymbols;
extern const cst_string *const cst_ts_default_postpunctuationsymbols;
extern const cst_string *const cst_ts_default_singlecharsymbols;

/* Public functions for tokenstream manipulation */
cst_tokenstream *ts_open(const char *filename,
                         const cst_string *whitespacesymbols,
                         const cst_string *singlecharsymbols,
                         const cst_string *prepunctsymbols,
                         const cst_string *postpunctsymbols);
cst_tokenstream *ts_open_string(const cst_string *string,
                                const cst_string *whitespacesymbols,
                                const cst_string *singlecharsymbols,
                                const cst_string *prepunctsymbols,
                                const cst_string *postpunctsymbols);
cst_tokenstream *ts_open_generic(const char *filename,
                                 const cst_string *whitespacesymbols,
                                 const cst_string *singlecharsymbols,
                                 const cst_string *prepunctsymbols,
                                 const cst_string *postpunctsymbols,
                                 void *streamtype_data,
                                 int (*open) (cst_tokenstream *ts,
                                              const char *filename),
                                 void (*close) (cst_tokenstream *ts),
                                 int (*eof) (cst_tokenstream *ts),
                                 int (*seek) (cst_tokenstream *ts,
                                              int pos),
                                 int (*tell) (cst_tokenstream *ts),
                                 int (*size) (cst_tokenstream *ts),
                                 int (*getc) (cst_tokenstream *ts));
void ts_close(cst_tokenstream *ts);

int ts_eof(cst_tokenstream *ts);
const cst_string *ts_get(cst_tokenstream *ts);

const cst_string *ts_get_quoted_token(cst_tokenstream *ts,
                                      char quote, char escape);

void set_charclasses(cst_tokenstream *ts,
                     const cst_string *whitespace,
                     const cst_string *singlecharsymbols,
                     const cst_string *prepunctuation,
                     const cst_string *postpunctuation);


int ts_set_stream_pos(cst_tokenstream *ts, int pos);
int ts_get_stream_pos(cst_tokenstream *ts);
int ts_get_stream_size(cst_tokenstream *ts);

#endif
