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
