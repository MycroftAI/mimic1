#include "cutest.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cst_tokenstream.h"
#include "cst_lexicon.h"

#ifndef TEST_FILE
 #define TEST_FILE "data.one"
#endif

#ifndef TEST_FILE_UTF8
 #define TEST_FILE_UTF8 "data_utf8.txt"
#endif


void test_token(void)
{
    cst_tokenstream *fd;
    const char *token;

    fd = ts_open(TEST_FILE, " \r\n\t", NULL, "\"!", NULL, 0);
    TEST_CHECK(fd != NULL);

    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "Hello") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "world") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "Yahoo") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "who") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "(") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "s") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, ")") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "A") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "small") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "test") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "") == 0);
    TEST_CHECK(ts_eof(fd));
    ts_close(fd);
}

void test_token_utf8(void)
{
    cst_tokenstream *fd;
    const char *token;
    const char *singl = "€(){}[]";    /* EURO SIGN + defaults */
    const char *prepunc = "¿¡\"!";    /* INVERTED QUESTION MARK, INV. EXCL. MARK, ", ! */
    const char *postpunc = "\"'`.,:;!?(){}[]";
    int emoji_as_singlecharsymbols = 1;
    fd = ts_open(TEST_FILE_UTF8, " \r\n\t", singl, prepunc, postpunc, emoji_as_singlecharsymbols);
    TEST_CHECK(fd != NULL);

    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "Hello") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "world") == 0);
    TEST_CHECK(strcmp(fd->postpunctuation, ".") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "😊") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "Yahoo") == 0);
    TEST_CHECK(strcmp(fd->postpunctuation, "!\"") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "who") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "(") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "s") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, ")") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "Good") == 0);
    TEST_CHECK(strcmp(fd->prepunctuation, "¡") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "News") == 0);
    TEST_CHECK(strcmp(fd->postpunctuation, "!") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "Åke") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "€") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "Über") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "漢字") == 0);
    token = ts_get(fd);
    TEST_CHECK(strcmp(token, "") == 0);
    TEST_CHECK(ts_eof(fd));
    ts_close(fd);
}

TEST_LIST =
{
    {
    "tokens", test_token},
    {
    "tokens_utf8", test_token_utf8},
    {
    0}
};
