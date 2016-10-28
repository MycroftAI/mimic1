#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mimic.h"

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

void current_utc_time(struct timespec *ts)
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
   clock_serv_t cclock;
   mach_timespec_t mts;
   host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
   clock_get_time(cclock, &mts);
   mach_port_deallocate(mach_task_self(), cclock);
   ts->tv_sec = mts.tv_sec;
   ts->tv_nsec = mts.tv_nsec;
#else
   clock_gettime(CLOCK_REALTIME, ts);
#endif
}

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
    if (argc < 3)
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
        current_utc_time(&t1);
        v = mimic_voice_load(argv[1]);
        current_utc_time(&t2);
        acc += (t2.tv_sec - t1.tv_sec) * 1000000000 + (t2.tv_nsec - t1.tv_nsec);
    }
    if (v != NULL)
    {
        printf("voice name: %s, %ld\n", v->name, acc);
    }
    return 0;
}
