/*************************************************************************/
/*                                                                       */
/*                  Language Technologies Institute                      */
/*                     Carnegie Mellon University                        */
/*                        Copyright (c) 2000                             */
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
/*  Text expander test (nums etc)                                        */
/*                                                                       */
/*************************************************************************/
#include <string.h>

#include "mimic.h"
#include "usenglish.h"
#include "us_text.h"

#include "cutest.h"

void digits(const char *input, const char *expected)
{
    int mismatches = 0;
    const cst_val *p, *v;
    char *tok;
    char *expected_str = malloc(strlen(expected) + 1);
    v = en_exp_digits(input);
    strcpy(expected_str, expected);
    tok = strtok(expected_str, " ");
    for (p = v; p;)
    {
        mismatches += strcmp(val_string(val_car(p)), tok);
        p = val_cdr(p);
        if (p)
            tok = strtok(NULL, " ");
        if (tok == NULL)
            break;
    }
    TEST_CHECK_(mismatches == 0, "%s != %s", input, expected);
}

void nums(const char *input, const char *expected)
{
    int mismatches = 0;
    const cst_val *p, *v;
    char *tok;
    char *expected_str = malloc(strlen(expected) + 1);
    v = en_exp_number(input);
    strcpy(expected_str, expected);
    tok = strtok(expected_str, " ");
    for (p = v; p;)
    {
        mismatches += strcmp(val_string(val_car(p)), tok);
        p = val_cdr(p);
        if (p)
            tok = strtok(NULL, " ");
        if (tok == NULL)
        {
            mismatches++;
            break;
        }
    }
    TEST_CHECK_(mismatches == 0, "%s != %s", input, expected);
}

void test_nums(void)
{
    nums("13", "thirteen");
    nums("1986", "one thousand nine hundred eighty six");
    nums("1234567890",
         "one billion two hundred thirty four million five hundred sixty seven thousand eight hundred ninety");
    nums("100", "one hundred");
    nums("10001", "ten thousand one");
    nums("10101", "ten thousand one hundred one");
    nums("432567",
         "four hundred thirty two thousand five hundred sixty seven");
    nums("432500", "four hundred thirty two thousand five hundred");
    nums("1000523", "one million five hundred twenty three");
    nums("1111111111111",
         "one one one one one one one one one one one one one");
}

void test_digits(void)
{

    digits("123", "one two three");
    digits("1", "one");
    digits("1234567809", "one two three four five six seven eight nine zero");
}

TEST_LIST = {
    {"numbers", test_nums},
    {"digits", test_digits},
    {0}
};
