#include "cutest.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cst_tokenstream.h"
#include "mimic.h"


void test_local_voice(void)
{
    if (mimic_voice_list == NULL)
        mimic_set_voice_list("../voices");
    TEST_CHECK(mimic_voice_select("rms") != NULL);
}


void test_url_voice(void)
{
    if (mimic_voice_list == NULL)
        mimic_set_voice_list("../voices");

    TEST_CHECK(mimic_voice_select("http://www.festvox.org/flite/packed/flite-2.0/voices/cmu_us_ksp.flitevox") != NULL);
}

void test_file_voice(void)
{
    if (mimic_voice_list == NULL)
        mimic_set_voice_list("../voices");
    TEST_CHECK(mimic_voice_select("../voices/cmu_us_ksp.flitevox") != NULL);
}


TEST_LIST = {
    { "local voice", test_local_voice },
    { "voice url", test_url_voice },
    { "voice file", test_file_voice },
    {0}
};

