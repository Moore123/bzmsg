#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <ncurses.h>
#include <sys/time.h>

#include "common.h"

int _debug;
int _quiet;
struct pthreadpool_t *tpool;


//change user defined function of Pair_Method for one's testing 
String_Pair_Method spm[]= {
    { "rep", NN_REP, FALSE, self_rep_server },
    { "req", NN_REQ, FALSE, self_req_client },
    { "pub", NN_PUB, FALSE, NULL },
    { "sub", NN_SUB, TRUE, NULL },
    { "bus", NN_BUS, FALSE, NULL },
    { "push", NN_PUSH, FALSE, NULL },
    { "pull", NN_PULL, FALSE, NULL },
    { "pair", NN_PAIR, FALSE, NULL },
};

int spm_len = asizeof(spm);

void usage(char *name) {
     printf("%s -n [ncount/10] -u [URL] -m [method]  -b [bson length/100] \n",name); 
     printf("-r [repeat] -k [keyprefix] \n");
     exit(1); 
}

int main(int argc, char **argv)
{
    struct timeval rec_time;
	int rep=10, ncount=10,i,opt,blen=20,xlen;
	struct timeval tm;
    char *buff, *url=NULL, *method=NULL, *kpre=NULL;
    uint64_t xval;
    _quiet = 1;  
    _debug = 0;
    

    if ( argc < 2 ) usage(argv[0]);

	while ((opt = getopt(argc, argv, "Dqk:m:u:r:n:b:")) != -1) {
		switch (opt) {
		case 'D':
			_debug = 1;
            break;
		case 'q':
			_quiet = 0;
            break;
		case 'b':
			blen = atoi(optarg);
            break;
		case 'k':
			kpre = strdup(optarg);
            break;
		case 'm':
			method = strdup(optarg);
            break;
		case 'n':
			ncount = atoi(optarg);
            break;
		case 'r':
			rep = atoi(optarg);
            break;
		case 'u':
			url = strdup(optarg);
            break;
        default :
            break;
        }
   }
   
   if ( kpre == NULL ) kpre = strdup("key");
   tic(&rec_time);
   if ( strstr(argv[0],"tserver") != 0 ) do_server(url, method);
   if ( strstr(argv[0],"tclient") != 0 ) do_client(url, method, kpre, rep, ncount, blen);
   toc(&rec_time);

 exit(0);

}

__attribute__((constructor))
void run_before_main()
{
    tpool = malloc(sizeof(*tpool));
    tpool_init( tpool, 100,20);
}
