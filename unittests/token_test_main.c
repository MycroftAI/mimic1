#include "cutest.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cst_tokenstream.h"
#include "cst_lexicon.h"

void test_token(void)
{
    cst_tokenstream *fd;
    const char *token;

    fd = ts_open("data.one", " \n\t", NULL, "\"!", NULL);

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
    const char *singl = "€(){}[]😊";    /* EURO SIGN + defaults + emoji */
    const char *prepunc = "¿¡\"!";    /* INVERTED QUESTION MARK, INV. EXCL. MARK, ", ! */
    const char *postpunc = "\"'`.,:;!?(){}[]";
    fd = ts_open("data_utf8.txt", " \n\t", singl, prepunc, postpunc);

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
