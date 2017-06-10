/*
 * string tests
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

#include "cutest.h"

void test_uregex_match()
{
    int match;
    cst_uregex *uregex;
    uregex = new_cst_uregex("^[àé]$", PCRE2_CASELESS);
    TEST_CHECK(uregex != NULL);
    match = cst_uregex_match(uregex, "a");
    TEST_CHECK(match == 0);
    match = cst_uregex_match(uregex, "à");
    TEST_CHECK(match == 1);
    match = cst_uregex_match(uregex, "À");
    TEST_CHECK(match == 1);
    delete_cst_uregex(uregex);
    uregex = new_cst_uregex("^🐨.*$", 0);
    TEST_CHECK(uregex != NULL);
    match = cst_uregex_match(uregex, "🐨 is a koala");
    TEST_CHECK(match == 1);
    match = cst_uregex_match(uregex, "😀 is not a koala");
    TEST_CHECK(match == 0);
    delete_cst_uregex(uregex);
    fprintf(stderr, "\n Test expected error message below:\n");
    fflush(stderr);
    uregex = new_cst_uregex(NULL, 0);
    fprintf(stderr, "Test expected error message above:\n");
    fflush(stderr);
    TEST_CHECK(uregex == NULL);
    return;
}

void test_change_case_utf8()
{
    cst_string in[] = "¡hola MUNDO!";
    cst_string *out;
    out = cst_tolower_utf8(in);
    printf("\nin: '%s'\tout: '%s'\n", in, out);
    TEST_CHECK(cst_streq(out, "¡hola mundo!"));
    cst_free(out);
    out = cst_toupper_utf8(in);
    printf("in: '%s'\tout: '%s'\n", in, out);
    TEST_CHECK(cst_streq(out, "¡HOLA MUNDO!"));
    cst_free(out);
    // Test NULL input
    out = cst_tolower_utf8(NULL);
    TEST_CHECK(out == NULL);
    // Test empty input
    out = cst_toupper_utf8("");
    TEST_CHECK(cst_streq(out, ""));
    cst_free(out);
    return;
}

TEST_LIST =
{
    {"uregex_match", test_uregex_match},
    {"test_change_case_utf8", test_change_case_utf8},
    {0}
};
