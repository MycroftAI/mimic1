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
/*               Date:  January 2000                                     */
/*************************************************************************/
/*                                                                       */
/*  Various regex tests                                                  */
/*                                                                       */
/*************************************************************************/
#include <stdio.h>
#include "cst_regex.h"
#include "cutest.h"
char *rtests[] = {
    "1",
    " \n ",
    "hello",
    "Hello",
    "1and2",
    "oneandtwo",
    "-1.34",
    "235",
    "034",
    "1,234,235",
    "1,2345",
    NULL
};

// match white spaces
void test_match_whitespace(void)
{
    int i;
    for (i = 0; rtests[i] != NULL; i++)
        TEST_CHECK(cst_regex_match(cst_rx_white, rtests[i]) == 0);
    TEST_CHECK(cst_regex_match(cst_rx_white, " ") == 1);
}

// match upper case strings
void test_match_upper(void)
{
    int i;
    int expected[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (i = 0; rtests[i] != NULL; i++)
        TEST_CHECK(cst_regex_match(cst_rx_uppercase, rtests[i]) ==
                   expected[i]);

    TEST_CHECK(cst_regex_match(cst_rx_uppercase, "HELLO") == 1);
}

// match strings containing only lower case letters
void test_match_lower(void)
{
    int i;
    int expected[] = { 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 };
    for (i = 0; rtests[i] != NULL; i++)
        TEST_CHECK(cst_regex_match(cst_rx_lowercase, rtests[i]) ==
                   expected[i]);
}

// match strings containing letters and numericals
void test_match_alphanum(void)
{
    int i;
    int expected[] = { 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0 };
    for (i = 0; rtests[i] != NULL; i++)
        TEST_CHECK(cst_regex_match(cst_rx_alphanum, rtests[i]) ==
                   expected[i]);
}

// match identifiers
void test_match_ident(void)
{
    int i;
    int expected[] = { 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0 };
    for (i = 0; rtests[i] != NULL; i++)
        TEST_CHECK(cst_regex_match(cst_rx_identifier, rtests[i]) ==
                   expected[i]);
}

// match strings containing only letters
void test_match_alpha(void)
{
    int i;
    int expected[] = { 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0 };
    for (i = 0; rtests[i] != NULL; i++)
        TEST_CHECK(cst_regex_match(cst_rx_alpha, rtests[i]) == expected[i]);
}

// match integer
void test_match_int(void)
{
    int i;
    int expected[] = { 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 };
    for (i = 0; rtests[i] != NULL; i++)
        TEST_CHECK(cst_regex_match(cst_rx_int, rtests[i]) == expected[i]);
}

// match double, i.e. 1.34 or 3.14
void test_match_double(void)
{
    int i;
    int expected[] = { 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0 };
    for (i = 0; rtests[i] != NULL; i++)
        TEST_CHECK(cst_regex_match(cst_rx_double, rtests[i]) == expected[i]);
}

// match [numerical],[numerical]
void test_match_commaint(void)
{
    int i;
    int expected[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };
    cst_regex *commaint;
    commaint =
        new_cst_regex
        ("[0-9][0-9]?[0-9]?,\\([0-9][0-9][0-9],\\)*[0-9][0-9][0-9]\\(\\.[0-9]+\\)?");

    for (i = 0; rtests[i] != NULL; i++)
        TEST_CHECK(cst_regex_match(commaint, rtests[i]) == expected[i]);

    delete_cst_regex(commaint);
}

TEST_LIST = {
    {"regex match commaint", test_match_commaint},
    {"regex match double", test_match_double},
    {"regex match integer", test_match_int},
    {"regex match identifier", test_match_ident},
    {"regex match alphanumerical", test_match_alphanum},
    {"regex match lower", test_match_lower},
    {"regex match upper", test_match_upper},
    {"regex match alpha", test_match_alpha},
    {"regex match whitespace", test_match_whitespace},
    {0}
};
