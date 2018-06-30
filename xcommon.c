#include "common.h"

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

/* Subtracts time values to determine run time */
int timeval_subtract (struct timeval *result, struct timeval *t2, struct timeval *t1)
{
  long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
  result->tv_sec = diff / 1000000;
  result->tv_usec = diff % 1000000;

  return (diff < 0);
}

/* Starts timer */
void tic (struct timeval *timer) { 
    memset(timer,0x0,sizeof(struct timeval));
	gettimeofday(timer, NULL); 
}


/* Stops timer and prints difference to the screen */
void toc (struct timeval *timer) {
  struct timeval tv_end, tv_diff;

  memset(&tv_end,0x0,sizeof(struct timeval));
  memset(&tv_diff,0x0,sizeof(struct timeval));
  gettimeofday(&tv_end, NULL);
  timeval_subtract (&tv_diff, &tv_end, timer);
  if ( _quiet == 1 ) 
  printf("Running time is %ld.%06lds\n", tv_diff.tv_sec, tv_diff.tv_usec);

}

void dump_bson_display(bson *data,int depth, char *exkey)
{

	bson_iterator it;
	bson_type t;
	char *key;
	char *strBuf;
	char hex_oid[25];
    int  i, temp;
    double *dbuf;

	bson_iterator_init(&it, data);

	while (bson_iterator_next(&it)) {

		t = bson_iterator_type(&it);

		key = (char *)bson_iterator_key(&it);
        if ( ( exkey != NULL ) &&
            (strcmp(key, exkey, max( strlen(key), strlen(exkey) ) ) == 0) )  break;

		printf("(key) \"%s\"",key);
        for(temp=0;temp<depth;temp++) printf("\t");

		switch (t) {
		case BSON_DOUBLE:
			printf("(double) %6.2f\n", bson_iterator_double(&it));
			break;
		case BSON_INT:
			printf("(int) %d\n", bson_iterator_int(&it));
			break;
		case BSON_STRING:
			strBuf = strdup((char *)bson_iterator_string(&it));
			printf("(string) \"%s\"\n", strBuf);
            free(strBuf);
			break;
		case BSON_OID:
			bson_oid_to_string(bson_iterator_oid(&it), hex_oid);
			printf("(oid) \"%s\"\n", hex_oid);
			break;
		case BSON_OBJECT:
			printf("(subobject) {...}\n");
            bson_print_raw( bson_iterator_value( &it ) , depth+1 );
			break;
		case BSON_ARRAY:
			printf("(array) [...]\n");
            bson_print_raw( bson_iterator_value( &it ) , depth+1 );
			break;
		case BSON_TIMESTAMP:
			printf("(timestamp) [...]\n");
			break;
        case BSON_BINDATA:
            bson_printf( "(BINDATA) [...] len %d\n" ,temp = bson_iterator_bin_len(&it) );
            dbuf = (double *)bson_iterator_bin_data(&it); 
            for ( i=0; i<temp/sizeof(double); i++ ) {
                printf("%12.2lf",dbuf[i]);
                if ( (i+1)%8  == 0 ) printf("\n");
                if ( (i+1)%16  == 0 ) printf("\n");
                }
            break;
		default:
        /*
		    BSON_EOO = 0,
			BSON_BINDATA = 5,
			BSON_UNDEFINED = 6,
			BSON_BOOL = 8,
			BSON_DATE = 9,
			BSON_NULL = 10,
			BSON_REGEX = 11,
			BSON_DBREF = 12, 
			BSON_CODE = 13,
			BSON_SYMBOL = 14,
			BSON_CODEWSCOPE = 15,
	*/
			printf("Note (type %d)\n", bson_iterator_type(&it));
			break;
		}

	}

	return;
}

bzMSG *bson_pump(int sock, bson * b)
{
	char *buff = NULL, *zbuff = NULL;
	int rc, sendlen, copylen;
	bzMSG *m = NULL;
	unsigned long crc;

	while (1) {
		if ((m = Calloc(1, bzMSG)) == NULL)
			break;
		m->iszip |= 1L << SENDMSG;
		sendlen = (b->dataSize + sizeof(bzMSG) + 15) & 0xFFFFFFF0;
		m->msgsize = m->rawsize = copylen = b->dataSize;

		if (b->dataSize > 1024) {
			if ((zbuff = Calloc(sendlen, char)) == NULL) {
				free(m);
				break;
			}
			m->iszip |= 1L << ZIPMSG;
			m->msgsize = copylen = LZ4_compress(b->data, zbuff, b->dataSize);
		} else {
			zbuff = b->data;
			m->msgsize = copylen = b->dataSize;
		}

		sendlen = copylen + sizeof(bzMSG);

		if ((buff = Calloc(sendlen, char)) == NULL) {
			if (m->iszip & (1L << ZIPMSG)) free(zbuff);
			zbuff = NULL;
			free(m);
			break;
		}

		memcpy(buff + offsetof(bzMSG, headsize), zbuff, copylen);
		m->crc = Murmurhash64A(buff + offsetof(bzMSG, headsize), m->msgsize, CONFUSECODE);
        printf("msg->len %lu m->CRC is %lx\n",m->msgsize,m->crc); 
		memcpy(buff, m, offsetof(bzMSG, headsize));

		if ((rc = nn_send(sock, buff, sendlen, 0)) != sendlen) {
			break;
		}
		break;
	}

	if (buff != NULL) free(buff);
	if (m->iszip & (1L << ZIPMSG)) free(zbuff);
	// return a m should be free;
	return (m);
}

bson *recv_a_bson(int sock) {
        char *buf,*zbuf;
        bzMSG *msg;
        bson *b=NULL;

    do {
        fprintf(stderr,"%s O",__func__);
		int bytes = nn_recv(sock, &buf, NN_MSG, 0);

		b = Calloc(1, bson);
		bson_init_empty(b);
        fprintf(stderr,"O");

		msg = Calloc(1, bzMSG);
		memcpy(msg, buf, offsetof(bzMSG, headsize));

		if (msg->crc != Murmurhash64A(buf + offsetof(bzMSG, headsize), msg->msgsize, CONFUSECODE)) {
			printf("Error hash %lXH != %lXH ", msg->crc,
					 (uint64_t) Murmurhash64A(buf + offsetof(bzMSG, headsize), msg->msgsize, CONFUSECODE));
			nn_freemsg(buf);
			free(msg);
			break;
		}

		zbuf = Calloc(msg->msgsize, char);
		if (msg->iszip & (1L << ZIPMSG)) {
			LZ4_uncompress(buf + offsetof(bzMSG, headsize), zbuf, msg->rawsize);
		} else {
			memcpy(zbuf, buf + offsetof(bzMSG, headsize), msg->rawsize);
		}

		b->dataSize = msg->rawsize;
		b->data = zbuf;
		bson_finish(b);
		nn_freemsg(buf);

    } while(0);
    return(b);
}
