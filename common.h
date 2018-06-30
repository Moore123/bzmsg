
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>

#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#include <signal.h>
#include <semaphore.h>
#include <pthread.h>

#include <errno.h>
#include <syslog.h>
#include <curl/curl.h>
#include <dirent.h>
#include <lz4.h>
#include <lz4hc.h>

#include <nanomsg/nn.h>
#include <nanomsg/tcp.h>
#include <nanomsg/bus.h>
#include <nanomsg/pair.h>
#include <nanomsg/pubsub.h>
#include <nanomsg/reqrep.h>
#include <nanomsg/pipeline.h>

#include "compmark.h"
#include "bson.h"
#include "list.h"
#include "pthreadpool.h"
#include "MurmurHash3.h"

#include "nnutils.h"

#include "udf.h"

extern int _debug;
extern int _quiet;

#define TRUE 1
#define FALSE 0

// code4 | 1<<ZIPMSG;
#define ZIPMSG  62
#define SENDMSG 61

typedef struct __attribute__ ((aligned(8))) _bzMSG {
	uint64_t iszip;
	uint64_t crc;
	uint64_t msgsize;
	uint64_t rawsize;
	char *headsize;
} bzMSG;

typedef bool (*user_def_action)(int sock,bson *b);
typedef struct _String_Pair_Method{
    char *str;
    uint32_t nn_method;
    bool set_sock_opt;
    user_def_action udf;
} String_Pair_Method;

extern int randi(int max);
void tic (struct timeval *timer); 
void toc (struct timeval *timer); 

extern bzMSG *bson_pump(int sock, bson * b);
extern bson *recv_a_bson(int sock);
extern void dump_bson_display(bson *data,int depth, char *exkey);

extern void do_server(char *url, char *method);
extern void do_client(char *url, char *method,char *kpre, int repeat, int ncount,  int blen);

#define min(x,y)        ((x) < (y) ? (x) : (y))
#define max(x,y)        ((x) > (y) ? (x) : (y))
#define Calloc(n, t)   (t *) calloc( (size_t) (n), sizeof(t) )
#define asizeof(a)     (int)(sizeof (a) / sizeof ((a)[0]))
