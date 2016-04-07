#include <stdio.h>
#include <stdlib.h>
#include "qrapp.h"
#include "GaloisList.h"

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
/*  @brief  誤り訂正コードの生成                                            */
/*                                                                          */
/*  @param  version   モジュールサイズ(QRコード型番)                        */
/*  @param  QRecLevel 誤り訂正レベル                                        */
/*  @param  pQRdata   QRデータの先頭アドレス                                */
/*                                                                          */
/*  @retval なし                                                            */
/*  @note   pQRdata は AllDataCapCode サイズ分確保しておく事                */
/*                                                                          */
/****************************************************************************/
int make_ecc_code( char version, QR_ECLEVEL_N QRecLevel, char *pQRdata ){

    BYTE* pGx;

    // Error Check
    if( pQRdata == NULL ){
        return 0;
    }
    if(version < QR_VERSION_MIN || QR_VERSION_MAX < version){
        return 0;
    }
    if(QRecLevel < QR_ECLEVEL_L || QR_ECLEVEL_H < QRecLevel){
        return 0;
    }

    int datawords_counter = _qr_string_length[version][QRecLevel].DataCode;         /* データ長 */
    int eccwords_counter  = _qr_string_length[version][QRecLevel].ECCode;           /* 誤り訂正コード長 */
    int max_data_byte     = _qr_string_length[version][QRecLevel].AllDataCapCode;   /* データの全体長(データ + 誤り訂正コード) */

    /* Allocate Memory(データの全容量分) */
    BYTE *pECCode = (BYTE *)calloc( (size_t)max_data_byte, sizeof(BYTE) );
    if( pECCode == NULL ){
        exit(1);    /* 強制終了 */
    }
    memcpy(pECCode, pQRdata, max_data_byte);

    /* Initialize */
    InitGaloisList();

    int GfCnt = GetCalc_Gx( &pGx, eccwords_counter );   /* 生成多項式(語数) */

    for(int i=0; i < datawords_counter; i++){
        /* 指数から係数を逆引き */
        int val = GetRvslookupTable( pECCode[i] );

        for(int j=i, k=0 ; k < GfCnt ; j++, k++){
            /* 係数から指数を正引き(α^255==1) */
            pECCode[j] = GetlookupTable(  (pGx[k] + val) % 255 )^pECCode[j];
        }
    }

    /* pQRdataに誤り訂正コードを入れる */
    while(datawords_counter < max_data_byte){
        pQRdata[datawords_counter] = pECCode[datawords_counter];
        datawords_counter++;
    }
    return eccwords_counter;
}
