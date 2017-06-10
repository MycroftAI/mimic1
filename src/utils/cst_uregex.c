/*
 * pcre2 functions
 * 
 * Copyright 2016 Sergio Oller <sergioller@gmail.com>

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <stdio.h>
#include "cst_uregex.h"
#include "cst_alloc.h"
#include "cst_string.h"
#include "cst_error.h"

static cst_string *case_conv(const cst_string *in, int to_upper)
{
    if (in == NULL)
        return NULL;
    size_t out_len = 2 * strlen(in);
    unsigned char *output = cst_alloc(unsigned char, out_len);
    if (output == NULL)
    {
        return NULL;
    }
    cst_uregex *ureg = new_cst_uregex("(.*)", 0);
    const char *repl;
    if (to_upper == 1)
    {
        repl = "\\U$1";
    }
    else
    {
        repl = "\\L$1";
    }
    int result = pcre2_substitute(ureg, (const unsigned char *) in, -1, 0,
                                  PCRE2_SUBSTITUTE_EXTENDED, NULL, NULL,
                                  (const unsigned char *) repl,
                                  -1, output, &out_len);
    if (result < 0)
    {
        PCRE2_UCHAR8 buffer[120];
        pcre2_get_error_message(result, buffer, 120);
        fprintf(stderr, "Error case converting: %s\n", buffer);
    }
    delete_cst_uregex(ureg);
    return (cst_string *) output;
}



cst_string *cst_tolower_utf8(const cst_string *in)
{
    return case_conv(in, 0);
}

cst_string *cst_toupper_utf8(const cst_string *in)
{
    return case_conv(in, 1);
}

/* Compile regex */
cst_uregex *new_cst_uregex(cst_string *pattern, uint32_t options)
{
    int errorcode = 0;
    PCRE2_SIZE erroroffset;
    cst_uregex *ureg = pcre2_compile((const unsigned char *) pattern,
                                     PCRE2_ZERO_TERMINATED,
                                     options | PCRE2_UTF, &errorcode,
                                     &erroroffset, NULL);
    if (ureg == NULL)
    {
        PCRE2_UCHAR8 buffer[256];
        pcre2_get_error_message(errorcode, buffer, 256);
        fprintf(stderr, "Error creating uregex: %s\n", buffer);
        /* Handle error */
        return NULL;
    }
    return ureg;
}

/* free regex */
void delete_cst_uregex(cst_uregex *uregex)
{
    pcre2_code_free(uregex);
    return;
}

/* match regex string */
int cst_uregex_match(cst_uregex *uregex, const cst_string *str)
{
    pcre2_match_data *match_data =
        pcre2_match_data_create_from_pattern(uregex, NULL);
    int rc =
        pcre2_match(uregex, (const unsigned char *) str, -1, 0, 0, match_data,
                    NULL);
    pcre2_match_data_free(match_data);
    if (rc == PCRE2_ERROR_NOMATCH)
    {
        return 0;
    }
    else if (rc < 0)
    {
        PCRE2_UCHAR8 buffer[256];
        pcre2_get_error_message(rc, buffer, 256);
        fprintf(stderr, "Error matching uregex: %s\n", buffer);
        return -1;
    }
    else if (rc == 0)
    {
        fprintf(stderr,
                "Error in cst_uregex_match. This should not happen\n");
        return -1;
    }
    else
    {
        return 1;
    }
}
