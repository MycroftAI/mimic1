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
#include "cst_tokenstream.h"
#include "cst_alloc.h"

const cst_string *const cst_ts_default_whitespacesymbols = " \t\n\r";
const cst_string *const cst_ts_default_singlecharsymbols = "(){}[]";
const cst_string *const cst_ts_default_prepunctuationsymbols = "\"'`({[";
const cst_string *const cst_ts_default_postpunctuationsymbols =
    "\"'`.,:;!?(){}[]";

#define TS_BUFFER_SIZE 256

static void ts_getc(cst_tokenstream *ts);
static void internal_ts_getc(cst_tokenstream *ts);

int ts_charclass(const cst_string *const utf8char, int class,
                 cst_tokenstream *ts)
{
    unsigned char c1, c2, c3, c4;
    unsigned char *c = (unsigned char *) utf8char;
    switch (cst_strlen(c))
    {
    case 1:
        /* 1-byte must be [0, 128), we mask with b01111111 = 0x7f */
        return (ts->charclass[c[0] & 0x7f] & class);
    case 2:
        /* 1st byte must be 110xxxxx so we mask with b00011111 = 0x1f */
        c1 = c[0] & 0x1f;
        /* 2nd byte must be 10xxxxxx so we mask with b00111111 = 0x3f */
        c2 = c[1] & 0x3f;
        if (ts->charclass2[c1] != NULL)
        {
            return (ts->charclass2[c1][c2] & class);
        }
        else
        {
            return 0;
        }
    case 3:
        /* 1st byte must be 1110xxxx so we mask with b00011111 = 0x0f */
        c1 = c[0] & 0x0f;
        /* 2nd byte must be 10xxxxxx so we mask with b00111111 = 0x3f */
        c2 = c[1] & 0x3f;
        /* 3rd byte must be 10xxxxxx so we mask with b00111111 = 0x3f */
        c3 = c[2] & 0x3f;
        if (ts->charclass3[c1] != NULL && ts->charclass3[c1][c2] != NULL)
        {
            return (ts->charclass3[c1][c2][c3] & class);
        }
        else
        {
            return 0;
        }
    case 4:
        /* 1st byte must be 11110xxx so we mask with b00011111 = 0x07 */
        c1 = c[0] & 0x07;
        /* 2nd byte must be 10xxxxxx so we mask with b00111111 = 0x3f */
        c2 = c[1] & 0x3f;
        /* 3rd byte must be 10xxxxxx so we mask with b00111111 = 0x3f */
        c3 = c[2] & 0x3f;
        /* 4th byte must be 10xxxxxx so we mask with b00111111 = 0x3f */
        c4 = c[3] & 0x3f;
        if (ts->charclass4[c1] != NULL &&
            ts->charclass4[c1][c2] != NULL &&
            ts->charclass4[c1][c2][c3] != NULL)
        {
            return (ts->charclass4[c1][c2][c3][c4] & class);
        }
        else
        {
            return 0;
        }
    default:
        return 0;
    }
}

/** This function modifies the cst_tokenstream by setting the UTF-8
    characters given in symbols as symbols of class symbol_value.
  */
static int set_charclass_table_symbol(cst_tokenstream *ts,
                                      const cst_string *symbols,
                                      const unsigned char symbol_value)
{
    cst_val *utflets;
    const cst_val *v;
    const unsigned char *utf8char;
    unsigned char c1;
    int utf8char_len;

    cst_string *cl1;
    cst_string **cl2;
    cst_string ***cl3;
    cst_string ****cl4;
    int idx;
    utflets = cst_utf8_explode(symbols);
    /* For each UTF-8 character */
    for (v = utflets; v; v = val_cdr(v))
    {
        /* The character as an unsigned char* */
        utf8char = (const unsigned char *) val_string(val_car(v));
        /* The number of UTF-8 bytes required to express this char */
        utf8char_len = cst_strlen(utf8char);
        idx = 0;
        if (utf8char_len > 4)
        {
            /* Invalid UTF-8 symbol */
            delete_val(utflets);
            return -1;
        }
        if (utf8char_len == 4)
        {
            cl4 = ts->charclass4;
            /* First character: 11110xxx */
            c1 = utf8char[idx] & 0x07;
            if (cl4[c1] == NULL)
            {
                /* It has 10xxxxx -> 2^6 = 64 options */
                cl4[c1] = cst_alloc(cst_string **, 64);
                memset(cl4[c1], 0, 64 * sizeof(cst_string **));
            }
            cl3 = cl4[c1];
            idx++;
        }
        if (utf8char_len >= 3)
        {
            if (utf8char_len == 3)
            {
                cl3 = ts->charclass3;
                /* First character: 1110xxxx */
                c1 = utf8char[idx] & 0x0f;
            }
            else
            {
                /* Cont. char must be 10xxxxxx so we mask with b00111111 = 0x3f */
                c1 = utf8char[idx] & 0x3f;
            }
            if (cl3[c1] == NULL)
            {
                /* It has 10xxxxx -> 2^6 = 64 options */
                cl3[c1] = cst_alloc(cst_string *, 64);
                memset(cl3[c1], 0, 64 * sizeof(cst_string *));
            }
            cl2 = cl3[c1];
            idx++;
        }
        if (utf8char_len >= 2)
        {
            if (utf8char_len == 2)
            {
                cl2 = ts->charclass2;
                /* 1st byte must be 110xxxxx so we mask with b00011111 = 0x1f */
                c1 = utf8char[idx] & 0x1f;
            }
            else
            {
                /* Cont. char must be 10xxxxxx so we mask with b00111111 = 0x3f */
                c1 = utf8char[idx] & 0x3f;
            }
            if (cl2[c1] == NULL)
            {
                /* It has 10xxxxx -> 2^6 = 64 options */
                cl2[c1] = cst_alloc(cst_string, 64);
                memset(cl2[c1], 0, 64 * sizeof(cst_string));
            }
            cl1 = cl2[c1];
            idx++;
        }
        if (utf8char_len >= 1)
        {
            if (utf8char_len == 1)
            {
                cl1 = ts->charclass;
                /* 1st byte must be 0xxxxxxx so we mask with b01111111 = 0x7f */
                c1 = utf8char[idx] & 0x7f;
            }
            else
            {
                /* Cont. char must be 10xxxxxx so we mask with b00111111 = 0x3f */
                c1 = utf8char[idx] & 0x3f;
            }
            cl1[c1] |= symbol_value;
        }
    }
    delete_val(utflets);
    return 0;
}

static void free_tables(cst_tokenstream *ts)
{
    int i, j, k;
    for (i = 0; i < 32; i++)
    {
        if (ts->charclass2[i] != NULL)
        {
            cst_free(ts->charclass2[i]);
        }
    }
    for (i = 0; i < 16; i++)
    {
        if (ts->charclass3[i] != NULL)
        {
            for (j = 0; j < 64; j++)
            {
                if (ts->charclass3[i][j] != NULL)
                {
                    cst_free(ts->charclass3[i][j]);
                }
            }
            cst_free(ts->charclass3[i]);
        }
    }
    for (i = 0; i < 8; i++)
    {
        if (ts->charclass4[i] != NULL)
        {
            for (j = 0; j < 64; j++)
            {
                if (ts->charclass4[i][j] != NULL)
                {
                    for (k = 0; k < 64; k++)
                    {
                        if (ts->charclass4[i][j][k] != NULL)
                        {
                            cst_free(ts->charclass4[i][j][k]);
                        }
                    }
                    cst_free(ts->charclass4[i][j]);
                }
            }
            cst_free(ts->charclass4[i]);
        }
    }
}

static void reset_tables(cst_tokenstream *ts)
{
    memset(ts->charclass, 0, 128 * sizeof(cst_string));
    memset(ts->charclass2, 0, 32 * sizeof(cst_string *));
    memset(ts->charclass3, 0, 16 * sizeof(cst_string **));
    memset(ts->charclass4, 0, 8 * sizeof(cst_string ***));
}

static int set_charclass_table(cst_tokenstream *ts)
{
    int i = 0;
    free_tables(ts);
    reset_tables(ts);
    i += set_charclass_table_symbol(ts, ts->p_whitespacesymbols,
                                    TS_CHARCLASS_WHITESPACE);
    i += set_charclass_table_symbol(ts, ts->p_singlecharsymbols,
                                    TS_CHARCLASS_SINGLECHAR);
    i += set_charclass_table_symbol(ts, ts->p_prepunctuationsymbols,
                                    TS_CHARCLASS_PREPUNCT);
    i += set_charclass_table_symbol(ts, ts->p_postpunctuationsymbols,
                                    TS_CHARCLASS_POSTPUNCT);
    return i;
}

void set_charclasses(cst_tokenstream *ts,
                     const cst_string *whitespace,
                     const cst_string *singlecharsymbols,
                     const cst_string *prepunctuation,
                     const cst_string *postpunctuation)
{
    ts->p_whitespacesymbols =
        (whitespace ? whitespace : cst_ts_default_whitespacesymbols);
    ts->p_singlecharsymbols =
        (singlecharsymbols ? singlecharsymbols :
         cst_ts_default_singlecharsymbols);
    ts->p_prepunctuationsymbols =
        (prepunctuation ? prepunctuation :
         cst_ts_default_prepunctuationsymbols);
    ts->p_postpunctuationsymbols =
        (postpunctuation ? postpunctuation :
         cst_ts_default_postpunctuationsymbols);

    set_charclass_table(ts);
    return;
}

static void extend_buffer(cst_string **buffer, int *buffer_max)
{
    int new_max;
    int increment;
    cst_string *new_buffer;

    /* Extend buffer 20% */
    increment = (*buffer_max) / 5;
    /* We need at least 5 bytes increment, as the longest UTF-8 char
     * requires 4 bytes and there may be an ending 0 */
    if (increment < 5)
    {
        /* We could set it to 5, but incrementing a power of two
         * seems more "alignment friendly" */
        increment = 8;
    }
    new_max = (*buffer_max) + increment;
    new_buffer = cst_alloc(cst_string, new_max);
    memmove(new_buffer, *buffer, *buffer_max);
    cst_free(*buffer);
    *buffer = new_buffer;
    *buffer_max = new_max;
}

static cst_tokenstream *new_tokenstream(const cst_string *whitespace,
                                        const cst_string *singlechars,
                                        const cst_string *prepunct,
                                        const cst_string *postpunct)
{                               /* Constructor function */
    cst_tokenstream *ts = cst_alloc(cst_tokenstream, 1);
    ts->fd = NULL;
    ts->file_pos = 0;
    ts->line_number = 0;
    ts->eof_flag = 0;
    ts->string_buffer = NULL;
    ts->token_pos = 0;
    ts->whitespace = cst_alloc(cst_string, TS_BUFFER_SIZE);
    ts->ws_max = TS_BUFFER_SIZE;
    if (prepunct && prepunct[0])
    {
        ts->prepunctuation = cst_alloc(cst_string, TS_BUFFER_SIZE);
        ts->prep_max = TS_BUFFER_SIZE;
    }
    ts->token = cst_alloc(cst_string, TS_BUFFER_SIZE);
    ts->token_max = TS_BUFFER_SIZE;
    if (postpunct && postpunct[0])
    {
        ts->postpunctuation = cst_alloc(cst_string, TS_BUFFER_SIZE);
        ts->postp_max = TS_BUFFER_SIZE;
    }

    reset_tables(ts);
    set_charclasses(ts, whitespace, singlechars, prepunct, postpunct);
    ts->current_char[0] = 0;

    return ts;
}

void delete_tokenstream(cst_tokenstream *ts)
{
    cst_free(ts->whitespace);
    cst_free(ts->token);
    if (ts->tags)
        delete_features(ts->tags);
    if (ts->prepunctuation)
        cst_free(ts->prepunctuation);
    if (ts->postpunctuation)
        cst_free(ts->postpunctuation);
    free_tables(ts);
    cst_free(ts);
}

cst_tokenstream *ts_open(const char *filename,
                         const cst_string *whitespace,
                         const cst_string *singlechars,
                         const cst_string *prepunct,
                         const cst_string *postpunct)
{
    cst_tokenstream *ts = new_tokenstream(whitespace,
                                          singlechars,
                                          prepunct,
                                          postpunct);

#ifndef UNDER_CE
    if (cst_streq("-", filename))
        ts->fd = stdin;
    else
#endif
        ts->fd = cst_fopen(filename, CST_OPEN_READ | CST_OPEN_BINARY);
    ts_getc(ts);

    if (ts->fd == NULL)
    {
        delete_tokenstream(ts);
        return NULL;
    }
    else
        return ts;
}

cst_tokenstream *ts_open_string(const cst_string *string,
                                const cst_string *whitespace,
                                const cst_string *singlechars,
                                const cst_string *prepunct,
                                const cst_string *postpunct)
{
    cst_tokenstream *ts = new_tokenstream(whitespace,
                                          singlechars,
                                          prepunct,
                                          postpunct);

    ts->string_buffer = cst_strdup(string);
    ts_getc(ts);

    return ts;
}

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
                                 int (*seek) (cst_tokenstream *ts, int pos),
                                 int (*tell) (cst_tokenstream *ts),
                                 int (*size) (cst_tokenstream *ts),
                                 int (*getc) (cst_tokenstream *ts))
{                               /* Its a generic token stream where user has specified the low level */
    /* file/stream access functions                                      */
    cst_tokenstream *ts = new_tokenstream(whitespacesymbols,
                                          singlecharsymbols,
                                          prepunctsymbols,
                                          postpunctsymbols);

    ts->streamtype_data = streamtype_data;
    ts->open = open;
    ts->close = close;
    ts->eof = eof;
    ts->seek = seek;
    ts->tell = tell;
    ts->size = size;
    ts->getc = getc;

    if ((ts->open) (ts, filename) != 0)
    {
        (ts->getc) (ts);
        return ts;
    }
    else
    {
        delete_tokenstream(ts);
        return NULL;
    }
}

void ts_close(cst_tokenstream *ts)
{
    if (ts->fd != NULL)
    {
#ifndef UNDER_CE
        if (ts->fd != stdin)
#endif
            cst_fclose(ts->fd);
        ts->fd = NULL;          /* just in case close gets called twice */
    }
    if (ts->string_buffer != NULL)
    {
        cst_free(ts->string_buffer);
        ts->string_buffer = NULL;
    }
    if (ts->open)
        (ts->close) (ts);
    delete_tokenstream(ts);
}

static void get_token_sub_part(cst_tokenstream *ts,
                               int charclass,
                               cst_string **buffer, int *buffer_max)
{
    int p;
    int curr_char_len = 0;

    for (p = 0; ((!ts_eof(ts)) &&
                 (ts_charclass(ts->current_char, charclass, ts)) &&
                 (!ts_charclass(ts->current_char,
                                TS_CHARCLASS_SINGLECHAR, ts)));
         p += curr_char_len)
    {
        curr_char_len = cst_strlen(ts->current_char);
        if (p + curr_char_len >= *buffer_max)
            extend_buffer(buffer, buffer_max);
        memcpy(&((*buffer)[p]), ts->current_char, curr_char_len);
        ts_getc(ts);
    }
    (*buffer)[p] = '\0';
}

static inline int ts_utf8_sequence_length(char c0)
{
    // Get the expected length of UTF8 sequence given its most
    // significant byte
    return ((0xE5000000 >> ((c0 >> 3) & 0x1E)) & 3) + 1;
}

/* Can't afford dynamically generate this char class so have separated func */
/* so do the core token part -- this goes while not givenlass (while the    */
/* above function oes while is givenclass */
static void get_token_sub_part_2(cst_tokenstream *ts,
                                 int endclass1,
                                 cst_string **buffer, int *buffer_max)
{
    int p, curr_char_len;

    for (p = 0; ((!ts_eof(ts)) &&
                 (!ts_charclass(ts->current_char, endclass1, ts)) &&
                 (!ts_charclass(ts->current_char,
                                TS_CHARCLASS_SINGLECHAR, ts)));
         p += curr_char_len)
    {
        curr_char_len = cst_strlen(ts->current_char);
        if (p + curr_char_len >= *buffer_max)
            extend_buffer(buffer, buffer_max);
        memcpy(&((*buffer)[p]), ts->current_char, curr_char_len);
        /* If someone sets tags we end the token */
        /* This can't happen in standard tokenstreams, but can in user */
        /* defined ones */
        if (ts->tags)
            break;

        /* In the special utf8 char by char mode we end at end of a utf8 char */
        if ((ts->utf8_explode_mode) &&
            (p == ts_utf8_sequence_length((*buffer)[0])))
            break;

        ts_getc(ts);
    }
    (*buffer)[p] = '\0';
}

static void get_token_postpunctuation(cst_tokenstream *ts)
{
    int p, t, plast;
    const cst_string *one_cp;

    t = cst_strlen(ts->token);
    p = t;
    cst_val *utf8lets;
    const cst_val *v;
    utf8lets = val_reverse(cst_utf8_explode(ts->token));
    for (v = utf8lets; v; v = val_cdr(v))
    {
        one_cp = val_string(val_car(v));
        plast = cst_strlen(one_cp);
        p -= plast;
        if (ts_charclass(one_cp, TS_CHARCLASS_POSTPUNCT, ts) == 0)
        {
            break;
        }
    }

    if (t != p)
    {
        if (t - p >= ts->postp_max)
            extend_buffer(&ts->postpunctuation, &ts->postp_max);
        /* Copy postpunctuation from token */
        memmove(ts->postpunctuation, &ts->token[p + plast], (t - p));
        /* truncate token at postpunctuation */
        ts->token[p + plast] = '\0';
    }
    delete_val(utf8lets);
}

int ts_eof(cst_tokenstream *ts)
{
    if (ts->eof_flag)
        return TRUE;
    else
        return FALSE;
}

int ts_set_stream_pos(cst_tokenstream *ts, int pos)
{
    /* Note this doesn't preserve line_pos */
    int new_pos, l;

    if (ts->fd)
    {
        new_pos = (int) cst_fseek(ts->fd, (long) pos, CST_SEEK_ABSOLUTE);
        if (new_pos == pos)
            ts->eof_flag = FALSE;
    }
    else if (ts->string_buffer)
    {
        l = cst_strlen(ts->string_buffer);
        if (pos > l)
            new_pos = l;
        else if (pos < 0)
            new_pos = 0;
        else
            new_pos = pos;
        ts->eof_flag = FALSE;
    }
    else if (ts->open)
    {
        new_pos = (ts->seek) (ts, pos);
        if (new_pos == pos)
            ts->eof_flag = FALSE;
    }
    else
        new_pos = pos;          /* not sure it can get here */
    ts->file_pos = new_pos;
    ts->current_char[0] = ' ';  /* To be safe (but this is wrong) */
    ts->current_char[1] = '\0'; /* To be safe (but this is wrong) */

    return ts->file_pos;
}

int ts_get_stream_pos(cst_tokenstream *ts)
{
    if (ts->open)
        return (ts->tell) (ts);
    else
        return ts->file_pos;
}

int ts_get_stream_size(cst_tokenstream *ts)
{
    int current_pos, end_pos;
    if (ts->fd)
    {
        current_pos = ts->file_pos;
        end_pos = (int) cst_fseek(ts->fd, (long) 0, CST_SEEK_ENDREL);
        cst_fseek(ts->fd, (long) current_pos, CST_SEEK_ABSOLUTE);
        return end_pos;
    }
    else if (ts->string_buffer)
        return cst_strlen(ts->string_buffer);
    else if (ts->open)
        return (ts->size) (ts);
    else
        return 0;
}

static void ts_getc(cst_tokenstream *ts)
{
    if (ts->open)
        (ts->getc) (ts);
    else
        internal_ts_getc(ts);
    return;
}

static void internal_ts_getc(cst_tokenstream *ts)
{
    int gotchar;
    int cur_char_len, i;
    if (ts->fd)
    {
        gotchar = cst_fgetc(ts->fd);
        if (gotchar == EOF)
        {
            ts->eof_flag = TRUE;
            ts->current_char[0] = '\0';
            return;
        }
        ts->file_pos++;
        ts->current_char[0] = gotchar;
        cur_char_len = ts_utf8_sequence_length(ts->current_char[0]);
        if (cur_char_len > 4)
        {
            cst_errmsg
                ("Invalid UTF-8 sequence in tokenstream at position %s. Skipping.\n",
                 ts->file_pos);
            internal_ts_getc(ts);
            return;
        }
        for (i = 1; i < cur_char_len; i++)
        {
            gotchar = cst_fgetc(ts->fd);
            if (gotchar == -1)
            {
                ts->eof_flag = TRUE;
                cst_errmsg
                    ("End of file reached unexpectedly (UTF-8 continuation byte was expected)\n");
                ts->current_char[0] = '\0';
                return;
            }
            ts->file_pos++;
            ts->current_char[i] = gotchar;
            if ((ts->current_char[i] & 0xC0) != 0x80)
            {
                cst_errmsg
                    ("Invalid UTF-8 continuation character %d in tokenstream\n",
                     (int) ts->current_char[i]);
            }
        }
        ts->current_char[cur_char_len] = 0;
    }
    else if (ts->string_buffer)
    {
        if (ts->string_buffer[ts->file_pos] == '\0')
        {
            ts->eof_flag = TRUE;
            ts->current_char[0] = '\0';
            return;
        }
        else
        {
            ts->current_char[0] = ts->string_buffer[ts->file_pos];
            ts->file_pos++;
            if (((ts->current_char[0] & 0x80) != 0x00) &&
                ((ts->current_char[0] & 0xE0) != 0xC0) &&
                ((ts->current_char[0] & 0xF0) != 0xE0) &&
                ((ts->current_char[0] & 0xF8) != 0xF0))
            {
                cst_errmsg
                    ("Invalid UTF-8 sequence in tokenstream. Skipping\n");
                internal_ts_getc(ts);
                return;
            }
            cur_char_len = ts_utf8_sequence_length(ts->current_char[0]);
            if (cur_char_len > 4 || cur_char_len < 1)
            {
                cst_errmsg
                    ("Invalid UTF-8 sequence in tokenstream. Skipping\n");
                internal_ts_getc(ts);
                return;
            }
            for (i = 1; i < cur_char_len; i++)
            {
                ts->current_char[i] = ts->string_buffer[ts->file_pos];
                if (ts->string_buffer[ts->file_pos] == '\0')
                {
                    ts->eof_flag = TRUE;
                    ts->current_char[0] = '\0';
                    return;
                }
                ts->file_pos++;
                if ((ts->current_char[i] & 0xC0) != 0x80)
                {
                    cst_errmsg
                        ("Invalid UTF-8 continuation character in tokenstream\n");
                }
            }
            ts->current_char[cur_char_len] = '\0';
        }
    }

    if (ts->current_char[0] == '\n')
        ts->line_number++;
    return;
}

const cst_string *ts_get_quoted_token(cst_tokenstream *ts,
                                      char quote, char escape)
{
    /* for reading the next quoted token that starts with quote and
       ends with quote, quote may appear only if preceded by escape */
    int p;

    /* Hmm can't change quotes within a ts */
    ts->charclass[(unsigned int) quote] |= TS_CHARCLASS_QUOTE;
    ts->charclass[(unsigned int) escape] |= TS_CHARCLASS_QUOTE;

    /* skipping whitespace */
    get_token_sub_part(ts, TS_CHARCLASS_WHITESPACE,
                       &ts->whitespace, &ts->ws_max);
    ts->token_pos = ts->file_pos - 1;

    if (ts->current_char[0] == quote)
    {                           /* go until quote */
        ts_getc(ts);
        for (p = 0; ((!ts_eof(ts)) && (ts->current_char[0] != quote));)
        {
            if (p >= ts->token_max)
                extend_buffer(&ts->token, &ts->token_max);
            strcpy(&(ts->token[p]), ts->current_char);
            ts_getc(ts);
            if (ts->current_char[0] == escape)
            {
                ts_get(ts);
                if (p >= ts->token_max)
                    extend_buffer(&ts->token, &ts->token_max);
                memcpy(&(ts->token[p]), ts->current_char,
                       strlen(ts->current_char));
                ts_get(ts);
            }
            p += strlen(ts->current_char);
        }
        ts->token[p] = '\0';
        ts_getc(ts);
    }
    else                        /* its not quoted, like to be careful dont you */
    {                           /* treat is as standard token                  */
        /* Get prepunctuation */
        extend_buffer(&ts->prepunctuation, &ts->prep_max);
        get_token_sub_part(ts, TS_CHARCLASS_PREPUNCT,
                           &ts->prepunctuation, &ts->prep_max);
        /* Get the symbol itself */
        if (ts_charclass(ts->current_char, TS_CHARCLASS_SINGLECHAR, ts))
        {
            if (5 >= ts->token_max)
                extend_buffer(&ts->token, &ts->token_max);
            strcpy(ts->token, ts->current_char);
            p += strlen(ts->current_char);
            ts_getc(ts);
        }
        else
            get_token_sub_part_2(ts, TS_CHARCLASS_WHITESPACE,   /* end class1 */
                                 &ts->token, &ts->token_max);
        /* This'll have token *plus* post punctuation in ts->token */
        /* Get postpunctuation */
        get_token_postpunctuation(ts);
    }

    return ts->token;
}

const cst_string *ts_get(cst_tokenstream *ts)
{
    /* Get next token */

    if (ts->tags)
    {                           /* Someone didn't delete them before -- so we delete them now */
        delete_features(ts->tags);
        ts->tags = NULL;
    }

    /* Skip whitespace */
    get_token_sub_part(ts,
                       TS_CHARCLASS_WHITESPACE, &ts->whitespace, &ts->ws_max);

    /* quoted strings currently ignored */
    ts->token_pos = ts->file_pos - 1;

    /* Get prepunctuation */
    if (!ts_eof(ts) &&
        ts_charclass(ts->current_char, TS_CHARCLASS_PREPUNCT, ts))
        get_token_sub_part(ts,
                           TS_CHARCLASS_PREPUNCT,
                           &ts->prepunctuation, &ts->prep_max);
    else if (ts->prepunctuation)
        ts->prepunctuation[0] = '\0';
    /* Get the symbol itself */
    if (!ts_eof(ts) &&
        ts_charclass(ts->current_char, TS_CHARCLASS_SINGLECHAR, ts))
    {
        if (5 >= ts->token_max)
            extend_buffer(&ts->token, &ts->token_max);
        strcpy(ts->token, ts->current_char);
        ts_getc(ts);
    }
    else
        get_token_sub_part_2(ts, TS_CHARCLASS_WHITESPACE,       /* end class1 */
                             &ts->token, &ts->token_max);
    /* This'll have token *plus* post punctuation in ts->token */
    /* Get postpunctuation */
    if (ts->postpunctuation)
        ts->postpunctuation[0] = '\0';
    if (ts->p_postpunctuationsymbols[0])
        get_token_postpunctuation(ts);

    return ts->token;
}
