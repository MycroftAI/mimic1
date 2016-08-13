/*
 * icu functions
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

#include "cst_icu.h"
#include "cst_alloc.h"
#include "cst_string.h"
#include "cst_error.h"

static cst_string *cst_to_l(const cst_string *in, const char *locale,
                            int32_t (*func)(const UCaseMap *, char *, int32_t,
                                            const char *, int32_t,
                                            UErrorCode *))
{
    if (locale == NULL)
    {
        locale = "C";
    }
    if (in == NULL)
    {
        return NULL;
    }
    cst_string *output = NULL;
    int32_t output_size;
    UCaseMap *csm;
    UErrorCode status = U_ZERO_ERROR;
    csm = ucasemap_open(locale, 0, &status);
    output_size = func(csm, NULL, 0, in, -1, &status);
    if (status == U_BUFFER_OVERFLOW_ERROR)
    {
        status = U_ZERO_ERROR;
    }
    output = cst_alloc(char, output_size + 1);
    func(csm, output, output_size + 1, in, -1, &status);
    ucasemap_close(csm);
    if (U_FAILURE(status))
    {
        cst_free(output);
        cst_errmsg
            ("Error lowering (with locale %s) the string %s.\nError message: %s",
             locale, in, u_errorName(status));
        return NULL;
    }
    return output;
}


cst_string *cst_tolower_l(const cst_string *in, const char *locale)
{
    return cst_to_l(in, locale, ucasemap_utf8ToLower);
}

cst_string *cst_toupper_l(const cst_string *in, const char *locale)
{
    return cst_to_l(in, locale, ucasemap_utf8ToUpper);
}

/* Compile regex */
URegularExpression *new_cst_uregex(cst_string *pattern, uint32_t flags)
{
    URegularExpression *output;
    UErrorCode status = U_ZERO_ERROR;
    output = uregex_openC((const char *) pattern, flags, NULL, &status);
    if (U_FAILURE(status))
    {
        cst_errmsg("Error creating uregex: %s", u_errorName(status));
        return NULL;
    }
    return output;
}

/* free regex */
void delete_cst_uregex(URegularExpression * uregex)
{
    uregex_close(uregex);
    return;
}

/* match regex string */
int cst_uregex_match(URegularExpression * uregex, const cst_string *str)
{
    UErrorCode status = U_ZERO_ERROR;
    UText *ut = NULL;
    UBool ismatch;
    ut = utext_openUTF8(ut, str, -1, &status);
    uregex_setUText(uregex, ut, &status);
    ismatch = uregex_matches(uregex, 0, &status);
    uregex_reset(uregex, 0, &status);
    if (U_FAILURE(status))
    {
        cst_errmsg("Error cst_uregex_match: %s", u_errorName(status));
        return -1;
    }
    return ((int) ismatch);
}
