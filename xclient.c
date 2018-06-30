#include "common.h"
#define MIN_STR 16

extern String_Pair_Method spm[];
extern int spm_len;

bson *CallocBson()
{
	bson *r;
	if ((r = Calloc(1, bson)) == NULL)
		return (r);
	bson_init(r);
	return (r);
}

bool self_req_client( int sc, bson *b ){
    bson_pump(sc, b);        
    return(TRUE);
}

list *gen_bson_list(char *kfix,int nc, int rmax) {

 list *retval=NULL;
 bson *b;
 char *kstr, *buff;
 int i, j, k, cnt=0, lmt;

 do {
    if ( ( retval = ListCreate() ) == NULL ) break;
    retval->free = bson_free;
    kstr = Calloc(MIN_STR,char);
    buff = Calloc(rmax+1,char);
    cnt = 0;
    if ( ( b = CallocBson() ) == NULL ) break;
    while( cnt++ < nc) {
        snprintf(kstr,MIN_STR,"%s%05d",kfix,cnt);
        for(k=0; k<rmax; k++) buff[k]=randi(26)+41;
	    bson_append_string(b,kstr, buff);
    }
	bson_finish(b);
    ListAddNodeTail(retval,b);
    if ( i++ >= rmax ) break;
 } while(0);

 return(retval);
}

void do_client(char *url, char *method,char *k, int repeat, int ncount, int rmax) {

  int i , j=0 , sc ;
  String_Pair_Method *sptr;
  bool match = FALSE, loop=TRUE;
  list *dl;
  listIter *iter;
  listNode *node;
  bson *b, *rb;

  printf("%s %s %s\n",__func__,url,method);
    
  do {
    if ( url == NULL ) break;
    if ( method == NULL) break;
    for ( i=0 ; i<spm_len; i++) {
      if ( strncmp(spm[i].str,method,max( strlen(spm[i].str), strlen(method) ) ) == 0 )  {
          match = TRUE;
          sptr = &spm[i];
          break;
      }
    }
    if ( match == FALSE ) break;
    dl = gen_bson_list(k, ncount, rmax);
    sc = test_socket(AF_SP, sptr->nn_method);
    test_connect (sc, url);
    while( j++ < repeat && ( loop == TRUE ) ) {
	    ListEachFromHead(dl, iter, node) {
	        b = (bson *)listNodeValue(node);
	        if ( sptr->udf ) loop = sptr->udf(sc, b);
            if ( loop == FALSE ) break;
	    } ListEachEnd(iter);

    }

  } while(0);


  return;

}
