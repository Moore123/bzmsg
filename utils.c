/*
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <string.h>
#include <time.h>
#include <sys/time.h>

typedef enum _BOOL {
	FALSE = 0,
	TRUE = 1
} bool;

#ifdef RAND_MAX
//#undef RAND_MAX
//#define RAND_MAX 0x7FFFFFFF
#endif

int randi(int max)
{
	int rand_num;
	double cof;
	static struct random_data rand_data;
	static char rand_state[256];
	static bool initialized = FALSE;

	if (!initialized) {
		initstate_r((unsigned int)time(NULL), rand_state, sizeof(rand_state), &rand_data);
		initialized = TRUE;
	}
	random_r(&rand_data, &rand_num);
	cof = rand_num / (RAND_MAX + 1.0);
    
	return (int)(max * cof);
}

double randd(double max)
{
	int rand_num;
	double cof;
	static struct random_data rand_data;
	static char rand_state[256];
	static bool initialized = FALSE;

	if (!initialized) {
		initstate_r((unsigned int)time(NULL), rand_state, sizeof(rand_state), &rand_data);
		initialized = TRUE;
	}
	random_r(&rand_data, &rand_num);
	cof = rand_num / (RAND_MAX + 1.0);
    
	return (max * cof);
}

float randf(float max)
{
    int rand_num;
    static struct random_data rand_data;
    static char rand_state[256];
    static bool initialized = FALSE;

    if (!initialized)
    {
        initstate_r(time(0), rand_state, sizeof(rand_state), &rand_data);
        initialized =  TRUE;
    }
    random_r(&rand_data, &rand_num);
    double cof = rand_num / (RAND_MAX + 1.0);
    return (float)(max * cof);
}

/* Subtracts time values to determine run time */
int timeval_subtract (struct timeval *result, struct timeval *t2, struct timeval *t1)
{
  long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) -
    (t1->tv_usec + 1000000 * t1->tv_sec);
  result->tv_sec = diff / 1000000;
  result->tv_usec = diff % 1000000;

  return (diff < 0);
}


/* Starts timer */
void tic (struct timeval *timer) {
    memset(timer,0x0,sizeof(struct timeval));
    gettimeofday (timer, NULL);
    return;
}


/* Stops timer and prints difference to the screen */
void toc (struct timeval *timer) {
  struct timeval tv_end, tv_diff;

  memset(&tv_end,0x0,sizeof(struct timeval));
  memset(&tv_diff,0x0,sizeof(struct timeval));
  gettimeofday (&tv_end, NULL);
  timeval_subtract (&tv_diff, &tv_end, timer);
  fprintf(stderr,"running time is %ld.%06ld\n", tv_diff.tv_sec, tv_diff.tv_usec);
  return;
}

void ptoc (struct timeval *timer) {
  struct timeval tv_end, tv_diff;

  memset(&tv_end,0x0,sizeof(struct timeval));
  memset(&tv_diff,0x0,sizeof(struct timeval));
  gettimeofday (&tv_end, NULL);
  timeval_subtract (&tv_diff, &tv_end, timer);
  fprintf(stderr,"running time is %ld.%06ld\n", tv_diff.tv_sec, tv_diff.tv_usec);
  return;
}

int core_count() {
#ifdef __APPLE__
    int nm[2];
    size_t len = 4;
    uint32_t count;

    nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);

    if(count < 1) {
        nm[1] = HW_NCPU;
        sysctl(nm, 2, &count, &len, NULL, 0);
        if(count < 1) { count = 1; }
    }
    return count;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

#define SZRSV 4000
#define INNRSV 3000
#define INNRSV2 1000
#define SHRSV 5000
#define STKSPACE SZRSV+INNRSV+INNRSV2+SHRSV

int name2num(char *name) {

	int retval = STKSPACE - 1;

	if (NULL == name ) return(-1);

	if (strncasecmp(name, "sz00", 4) == 0) {
		retval = (atoi(name + 2));
	} else if (strncasecmp(name, "sz30", 4) == 0) {
		retval = (atoi(name + 3) + SZRSV);
	} else if (strncasecmp(name, "sh000001", 6) == 0) {
        retval = (SZRSV+INNRSV)-1;
	} else if (strncasecmp(name, "sh60", 4) == 0) {
		retval = (atoi(name + 3) + SZRSV + INNRSV);
	} 

	return (retval);

}
