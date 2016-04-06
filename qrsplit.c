#include <stdio.h>
#include <stdlib.h>

#include "qrapp.h"
#include "qrsplit.h"

/************************************************************************/
/* マクロ定義                                                           */
/************************************************************************/
/************************************************************************/
/* 列挙体定義                                                           */
/************************************************************************/
/************************************************************************/
/* 構造体定義                                                           */
/************************************************************************/
/************************************************************************/
/* 変数定義                                                             */
/************************************************************************/
/************************************************************************/
/* ローカル関数プロトタイプ定義                                         */
/************************************************************************/
/************************************************************************/
/* 外部公開関数                                                         */
/************************************************************************/
/****************************************************************************/
/*  @brief  pQRparamの情報を8bitづつに分割して一連のデータを作る            */
/*  @param  pQRparam モジュールサイズ                                       */
/*  @param  data_counter    モード種別                                      */
/*                                                                          */
/*  @retval CharNumIndexLength  データのヘッダポインタ                      */
/*                                                                          */
/*  @note                                                                   */
/****************************************************************************/
char *split_8bit_data( QR_PARAM *pQRparam, int data_counter, int version, QR_ECLEVEL_N QRecLevel){

    int max_data_byte = _qr_string_length[version][QRecLevel].AllDataCapCode;	/* byte */
    int flag, i;
    int remaining_bits = 8;      /* 8bit残量カウンタ */
    int codewords_counter = 0;   /* 文字カウンタ */
    unsigned char *pQRdata = NULL;

    /* Allocate Memory */
    pQRdata = (unsigned char *)calloc( max_data_byte+_qr_code_info[version].leftover_bits_num, sizeof(unsigned char) );
    if(pQRdata == NULL){
        return NULL;
    }

    for( i=0; i < data_counter ; i++ ){
        /* データ退避 */
        int buffer      = pQRparam[i].data_value;
        int buffer_bits = pQRparam[i].data_bits;
        flag = 1;

        /* ----divide data by 8bit---- */
        while(flag){
            if( remaining_bits > buffer_bits ){
                pQRdata[codewords_counter] = ( (pQRdata[codewords_counter] << buffer_bits) | buffer);
                remaining_bits -= buffer_bits;
                flag=0;
            }
            else{
                buffer_bits -= remaining_bits;
                pQRdata[codewords_counter] = ( (pQRdata[codewords_counter] << remaining_bits) | (buffer >> buffer_bits) );
                if (buffer_bits==0) {
                    flag=0;
                }
                buffer = (buffer & ((1 << buffer_bits)-1) ); 

                codewords_counter++;
                remaining_bits = 8;
                if (codewords_counter<(max_data_byte-1)){
                    /* 次の配列を初期化*/
                    pQRdata[codewords_counter]=0;
                }
            }
        }
    }

    /* 最後の列が8bit未満の場合0で埋める */
//  if(remaining_bits != 0){
    if(remaining_bits < 8){
        pQRdata[codewords_counter] = pQRdata[codewords_counter] << remaining_bits;
    } else {
       codewords_counter--;
    }

    /* 得られたコード数がシンボルのデータコード数に満たない場合は 11101100 および 00010001を交互に付加してシンボル容量にあわせる */
    flag = 1;
    while( codewords_counter < (_qr_string_length[version][QRecLevel].DataCode-1) ){
        codewords_counter++;
        if(flag == 1){
            pQRdata[codewords_counter]=236; /* 11101100 */
        }
        else{
            pQRdata[codewords_counter]=17;  /* 00010001 */
        }
        flag = !flag;
    }
    return pQRdata;
}
