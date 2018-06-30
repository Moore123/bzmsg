#include "common.h"

extern String_Pair_Method spm[];
extern int spm_len;

bool self_rep_server(int sc, char *fmt,...) {
    bson *b;
    if ( ( b = recv_a_bson(sc) ) == NULL ) return(FALSE);
    dump_bson_display(b,1,NULL);
    return(TRUE);
}

void do_server(char *url, char *method) {

  int i, sc, rc;
  bool match = FALSE, loop = TRUE;
  String_Pair_Method *sptr;
  bson *b=NULL;
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

   sc = test_socket(AF_SP, sptr->nn_method);
   test_bind(sc, url);
   while(loop) {
        if ( sptr->udf ) loop = sptr->udf(sc,sptr->fmt_str);
   }

  } while(0);

  return;
}
