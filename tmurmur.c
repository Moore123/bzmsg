#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <ncurses.h>
#include <sys/time.h>

#include "../include/MurmurHash3.h"

#define TRUE 1
#define FALSE 0

#define Calloc(n, t)   (t *) calloc( (size_t) (n), sizeof(t) )
#define asizeof(a)     (int)(sizeof (a) / sizeof ((a)[0]))
#define CONFUSION  0xadc83b19ULL
extern uint64_t Murmurhash64A(const void *, int, unsigned int);

int randi(int max)
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
    return (int)(max * cof);
}

int main(int argc, char **argv)
{

	int ncount=10,i,opt,blen=20,xlen;
	struct timeval tm;
    char *buff;
    uint64_t xval;

    if ( argc < 2 )  { printf("%s -n [] \n",argv[0]); exit(1); }
	while ((opt = getopt(argc, argv, "n:b:")) != -1) {
		switch (opt) {
		case 'n':
			ncount = atoi(optarg);
            break;
		case 'b':
			blen = atoi(optarg);
            break;
        default :
            break;
        }
   }

    while(ncount--) {
       xlen = randi(blen)+1;
       buff = Calloc(xlen,char);
       for( i=0; i<xlen; i++) buff[i]=0x41+randi(26);
       xval = Murmurhash64A(buff, xlen, CONFUSION); 
       if ( xlen < 20 )
         fprintf(stderr,"%4d] %16lu\t%s\n",xlen,xval,buff);
       else 
         fprintf(stderr,"%4d] %16lu\t\n",xlen,xval);
       free(buff);
      }	
	exit(1);

}
