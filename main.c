#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qrapp.h"

void main(int argc, char *argv[]){
	
	int mode, version, level;
	unsigned char *pQRMsg;

    if(argc <= 4 || 6 <= argc ){
	}else{
		printf( "[usage]\n" );
        printf("Usage UniteRawFiles.exe [I]mode [I]version [I]level [I]message\n");
		printf( "------------------------------------------------------------------------\n" );
		printf( "[example]\n" );
		printf( "  UniteRawFiles.exe 0 10 0 ABCDEF123 \n" );
		printf( "mode       0 :QR mode kind of Number type\n" );
		printf( "\t         1 :QR mode kind of AN     type\n" );
		printf( "\t         2 :QR mode kind of BINARY type\n" );
		printf( "\t         3 :QR mode kind of KANJI  type\n" );
		printf( "version   1-40 :QR version type\n" );
		printf( "level     0-3 :QR ecc level \n" );
	    exit(1);
    }

    /* 引数を格納 *//* memcpyの+1は'\0' */
    mode = atoi(argv[1]);
    version = atoi(argv[2]);
    level = atoi(argv[3]);
	printf("mesage_len=%d\n", strlen(argv[4]));
    pQRMsg = (unsigned char*)calloc((strlen(argv[4])+1), sizeof(unsigned char));

    memcpy(pQRMsg, argv[4], strlen(argv[4]) * sizeof(unsigned char));

	if(mode < QR_MODE_NUM || QR_MODE_KANJI < mode){
		printf( "Error!! QR mode over range!!!!! \n" );
		return ;
	}
	if(version < 1 || 40 < version){
		printf( "Error!! QR version over range!!!!! \n" );
		return ;
	}
	if(level < QR_ECLEVEL_L || QR_ECLEVEL_H < level){
		printf( "Error!! QR eclevel over range!!!!! \n" );
		return ;
	}
	printf("mode=%d, version=%d, level=%d\n", mode, version, level);
	printf("QRMsg=%s\n", pQRMsg);

    /* QRコード作成のAPIを呼び出す */
	make_qr_code(pQRMsg, mode, level, version );

    return ;
}
