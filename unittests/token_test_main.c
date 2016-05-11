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

TEST_LIST =
{
    {"tokens", test_token},
    {0}
};
