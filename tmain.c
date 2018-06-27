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
struct pthreadpool_t *tpool;

String_Pair_Method spm[]= {
    { "rep", NN_REP, FALSE },
    { "req", NN_REQ, FALSE },
    { "pub", NN_PUB, FALSE },
    { "sub", NN_SUB, TRUE },
    { "bus", NN_BUS, FALSE },
    { "push", NN_PUSH, FALSE },
    { "pull", NN_PULL, FALSE },
    { "pair", NN_PAIR, FALSE },
};

int spm_len = asizeof(spm);

void usage(char *name) {
     printf("%s -n [ncount/10] -u [URL] -m [method] -b [bson length/100] \n",name); 
     exit(1); 
}

int main(int argc, char **argv)
{

	int ncount=10,i,opt,blen=20,xlen;
	struct timeval tm;
    char *buff, *url=NULL, *method=NULL;
    uint64_t xval;
    _debug = 0;

    if ( argc < 2 ) usage(argv[0]);

	while ((opt = getopt(argc, argv, "Dm:u:n:b:")) != -1) {
		switch (opt) {
		case 'D':
			_debug = 1;
            break;
		case 'b':
			blen = atoi(optarg);
            break;
		case 'm':
			method = strdup(optarg);
            break;
		case 'n':
			ncount = atoi(optarg);
            break;
		case 'u':
			url = strdup(optarg);
            break;
        default :
            break;
        }
   }


   if ( strstr(argv[0],"tserver") != 0 ) do_server(url, method);
   if ( strstr(argv[0],"tclient") != 0 ) do_client(url, method, ncount, blen);

 exit(0);

}

__attribute__((constructor))
void run_before_main()
{
    tpool = malloc(sizeof(*tpool));
    tpool_init( tpool, 100,20);
}
