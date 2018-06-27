#include "common.h"

extern String_Pair_Method spm[];
extern int spm_len;

void do_client(char *url, char *method, int repeat, int rmax) {

  int i , sc ;
  String_Pair_Method *sptr;
  bool match = FALSE;

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

    sc = test_socket(AF_SP, sptr->nn_method);
    test_connect (sc, url);

  } while(0);


  return;

}
