#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mimic.h"

void mimic_set_lang_list();

void print_usage(char *prog_name)
{
    printf("%s voice-file [number of loads]\n", prog_name);
}


int main(int argc, char *argv[])
{
    int i, num;
    long acc = 0;
    struct timespec t1;
    struct timespec t2;
    cst_voice * v = NULL;
    mimic_init();
    mimic_set_lang_list();
    if (argc < 2)
    {
        print_usage(argv[0]);
        return -1;
    }
    else if (argc < 3)
        num = 1;
    else
        num = atoi(argv[2]);

    for(i = 0; i < num; i++)
    {
        clock_gettime(CLOCK_REALTIME, &t1);
        v = mimic_voice_load(argv[1]);
        clock_gettime(CLOCK_REALTIME, &t2);
        acc += (t2.tv_sec - t1.tv_sec) * 1000000000 + (t2.tv_nsec - t1.tv_nsec);
    }
    if (v != NULL)
    {
        printf("voice name: %s, %ld\n", v->name, acc);
    }
    return 0;
}
