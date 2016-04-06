#include <string.h>
#include <stdio.h>

#include "GaloisList.h"
#include "qrapp.h"
#include "qrecc.h"
#include "qrsplit.h"
#include "qrdraw.h"
#include "qrdata.h"

/************************************************************************/
/* マクロ定義                                                           */
/************************************************************************/
/************************************************************************/
/* 列挙体定義                                                           */
/************************************************************************/
/************************************************************************/
/* 変数定義                                                             */
/************************************************************************/
/************************************************************************/
/* 構造体定義                                                           */
/************************************************************************/
/************************************************************************/
/* 定数宣言                                                             */
/************************************************************************/
/************************************************************************/
/* ローカル関数プロトタイプ定義                                         */
/************************************************************************/
int _make_AN_data( QR_PARAM *pQRparam, UINT8 *pMsg, QR_MODE_N Mode, UINT version );
int _make_BYTE_data( QR_PARAM *pQRparam, UINT8 *pMsg, QR_MODE_N Mode, UINT version );
int _make_NUM_data( QR_PARAM *pQRparam, UINT8 *pMsg, QR_MODE_N Mode, UINT version );
int _make_KANJI_data( QR_PARAM *pQRparam, UINT8 *pMsg, QR_MODE_N Mode, UINT version );

int _AddTerminalPattern(QR_PARAM *pQRparam, int data_counter, int version);


/************************************************************************/
/* 外部公開関数                                                         */
/************************************************************************/
/****************************************************************************/
/*  @brief  QRコードの生成                                                  */
/*                                                                          */
/*  @param  pMsg      QRコードに埋め込む文字列                              */
/*  @param  Mode      QRモード種別                                          */
/*  @param  QRecLevel 誤り訂正レベル                                        */
/*  @param  version   モジュールサイズ(QRコード型番)                        */
/*                                                                          */
/*  @retval なし                                                            */
/*  @note   位置検出パターンに被る座標の位置合せパターンは省略する          */
/*                                                                          */
/****************************************************************************/
void make_qr_code(char *pMsg, QR_MODE_N Mode, QR_ECLEVEL_N QRecLevel, int version )
{
    QR_PARAM *pQRparam = NULL;
    BYTE *pQRcode = NULL;
    int data_counter=0;

    if( version < QR_VERSION_MIN || QR_VERSION_MAX < version ){
        printf("Error!!! version is Over Value!! Range 1-40. \n");
        return ;
    }
    if( QRecLevel <= QR_ECLEVEL_NUL || QR_ECLEVEL_MAX <= QRecLevel ){
        printf("Error!!! QRecLevel is Over Range!! \n");
        return ;
    }
    
    /* util.c で pMsg 内に何が入っているか確認する(文字のタイプを判別する)機能を後日追加したい(2015/01/27) */
    if(strlen(pMsg) <= 0){
        printf("Error!!! Message is none!! \n");
        return ;
    }

    /* Allocate Memory */
    pQRparam = (QR_PARAM *)calloc( (size_t)_qr_string_length[version][QRecLevel].AllDataCapCode, sizeof(QR_PARAM) );
    if( pQRparam == NULL ){
        return;
    }

    /* データ・誤り訂正コードを算出 */
    switch(Mode){
        case QR_MODE_AN:    data_counter = _make_AN_data(pQRparam, pMsg, Mode, version);        break;  /* 英数字モード */
        case QR_MODE_NUM:   data_counter = _make_NUM_data(pQRparam, pMsg, Mode, version);       break;  /* 数字モード */
        case QR_MODE_BYTE:  data_counter = _make_BYTE_data(pQRparam, pMsg, Mode, version);      break;  /* BYTEモード */
        case QR_MODE_KANJI: data_counter = _make_KANJI_data(pQRparam, pMsg, Mode, version);     break;  /* 漢字モード */
        default:
            /* Not Support */
            printf("Not Support!! \n");
            free(pQRparam);
            exit(1);
            break;
    }
    /* コード語変換( 8bit区切り ) */
    pQRcode = split_8bit_data( pQRparam, data_counter, version, QRecLevel);

    /* 誤り訂正コード語の付加 */
    make_ecc_code( pQRcode, version, QRecLevel);

    free(pQRparam);

    /*** 画像の埋め込み ***//* 後で別関数に分けよう */
    {
        int mask_pattern;

        /* Allocate Memory */
        BYTE *pQRdata = NULL;

        /* シンボルの固定部分を埋め込み */
        int size = base_draw(version, &pQRdata);

        /*データ・誤り訂正コードを埋め込み */
        qrdata_draw(pQRcode, pQRdata, version );    /* draw_QRCode */

        /* マスク処理 */
        mask_pattern = mask_processing(pQRdata, version, QRecLevel);

        /* 形式情報を埋め込み */
        info_input(pQRdata, version, QRecLevel, mask_pattern);

        /* ファイル出力 */
        {
            FILE *pf;       /* tmp */

            if ((pf = fopen("QR_sample.raw", "wb")) == NULL) {
                printf("file open error!!\n");
                exit(EXIT_FAILURE);
            }
            /* ファイルの書き込み */
            fwrite(pQRdata, sizeof(UINT8), size, pf);
        }
		free(pQRdata);
    }
    /* 領域解放 */
    free(pQRcode);
}

/************************************************************************/
/* ローカル関数                                                         */
/************************************************************************/
/****************************************************************************/
/*  @brief  英数字モードのデータ・誤り訂正コードの生成                      */
/*  @param  pData   埋め込みデータ先頭アドレス(1pix/byte)                   */
/*  @param  version   モジュールサイズ(QRコード型番)                        */
/*  @retval データ・誤り訂正コードの先頭アドレス                            */
/*  @note                                                                   */
/*  英数字モードの２進化                                                    */
/*  ① Ascii文字 → 既定の数値に各文字を数字化する                          */
/*  ② データを2桁づつ区切って1桁目の文字の値を45倍したものと2桁目の文字の値を足す*/
/*  ③ 算出された値を11bitの2進数で表記する                                 */
/*  備考) なお２桁に満たない場合は残った場合は対応する値を 6bit で表記する  */
/****************************************************************************/
int _make_AN_data( QR_PARAM *pQRparam, UINT8 *pMsg, QR_MODE_N Mode, UINT version )
{
    int data_counter = 0;   /* pQRparam配列の総数 */
    int i;                  /* counter */
    int str_length;         /* 文字数 */
    int remain_digit;		/* 余り桁 */

    /* データ分析 *//* ここで文字数がシンボル内に収まるかチェックする必要がある(2015/01/27) */
    str_length = strlen(pMsg);

    /* モード指示子 */
    pQRparam[data_counter].data_value = GetModeDscrpt( Mode );
    pQRparam[data_counter].data_bits  = 4;  /* 固定 */
    data_counter++; /* next subscript */
                        
    /* 文字数指示子 */
    pQRparam[data_counter].data_value = str_length;
    pQRparam[data_counter].data_bits  = GetModeBits( version, Mode );
    data_counter++; /* next subscript */

    /* データの2進数化 */
    remain_digit = str_length % 2;
    for(i=0 ; i < (str_length-remain_digit) ; i+=2){
        /* 2桁毎に計算 */
        pQRparam[data_counter].data_value = ChangeFontKindValue( pMsg[i] ) * 45 + ChangeFontKindValue( pMsg[i+1] ) ;
        pQRparam[data_counter].data_bits  = 11; 
        data_counter++; /* next subscript */
    }
    if(remain_digit){
        pQRparam[data_counter].data_value = ChangeFontKindValue( pMsg[i] ) ;
        pQRparam[data_counter].data_bits  = 6;
        data_counter++; /* next subscript */
    }
    /* 終端パターンの付加 */
    data_counter = _AddTerminalPattern( pQRparam, data_counter, version);

    return data_counter;
}

/****************************************************************************/
/*  @brief  バイトモードのデータ・誤り訂正コードの生成                      */
/*  @param  pData   埋め込みデータ先頭アドレス(1pix/byte)                   */
/*  @param  version   モジュールサイズ(QRコード型番)                        */
/*  @retval pQRparam配列の総数                                              */
/*  @note                                                                   */
/*        バイトモード時の２進化                                            */
/*        ① Ascii文字をそのまま格納(8bit固定)                              */
/****************************************************************************/
int _make_BYTE_data( QR_PARAM *pQRparam, UINT8 *pMsg, QR_MODE_N Mode, UINT version )
{
    int data_counter = 0;   /* pQRparam配列の総数 */
    int i;                  /* counter */
    int str_length;         /* 文字数 */

    /* データ分析 *//* ここで文字数がシンボル内に収まるかチェックする必要がある(2015/01/27) */
    str_length = strlen(pMsg);

    /* モード指示子 */
    pQRparam[data_counter].data_value = GetModeDscrpt( Mode );
    pQRparam[data_counter].data_bits  = 4;  /* 固定 */
    data_counter++; /* next subscript */
                        
    /* 文字数指示子 */
    pQRparam[data_counter].data_value = str_length;
    pQRparam[data_counter].data_bits  = GetModeBits( version, Mode );
    data_counter++; /* next subscript */

    /* データの２進化 */
    for(i=0 ; i < str_length ; i++){
        pQRparam[data_counter].data_value = pMsg[i];
        pQRparam[data_counter].data_bits  = 8;
        data_counter++; /* next subscript */
    }

    /* 終端パターンの付加 */
    data_counter = _AddTerminalPattern( pQRparam, data_counter, version);

    return data_counter;
}

/****************************************************************************/
/*  @brief  数字モードのデータ・誤り訂正コードの生成                        */
/*  @param  pData   埋め込みデータ先頭アドレス(1pix/byte)                   */
/*  @param  version   モジュールサイズ(QRコード型番)                        */
/*  @retval pQRparam配列の総数                                              */
/*  @note                                                                   */
/*        数字モード時の２進化                                              */
/*        ① 入力値を3桁のグループに分割( 01234567 ⇒ 012 345 67)           */
/*        ② 各グループを2進数に変換                                        */
/*        ③ 算出された値を11bitの2進数で表記する                           */
/*        ④ なお２桁に満たない場合は残った値を4bit/7bitで表記する          */
/****************************************************************************/
int _make_NUM_data( QR_PARAM *pQRparam, UINT8 *pMsg, QR_MODE_N Mode, UINT version )
{
    int data_counter = 0;   /* pQRparam配列の総数 */
    int str_length;         /* 文字数 */
    int i;
    int remain_digit;   /* 余り桁 */

    /* データ分析 */
    str_length = strlen(pMsg);

    /* モード指示子 */
    pQRparam[data_counter].data_value = GetModeDscrpt( Mode );
    pQRparam[data_counter].data_bits  = 4;  /* 固定 */
    data_counter++; /* next subscript */
                        
    /* 文字数指示子 */
    pQRparam[data_counter].data_value = str_length;
    pQRparam[data_counter].data_bits  = GetModeBits( version, Mode );
    data_counter++; /* next subscript */

    /* データの２進化 */
    /* 3桁毎に分割する */
    remain_digit = str_length % 3;
    for(i=0; i < (str_length-remain_digit) ;i+=3){
        pQRparam[data_counter].data_value = (pMsg[i] - '0') * 100 + (pMsg[i+1] - '0') * 10 + (pMsg[i+2] - '0');
        pQRparam[data_counter].data_bits = 10;
        data_counter++;
    }
    /* 余りが出た場合 */
    if(remain_digit == 1){
        /* 1桁 */
        pQRparam[data_counter].data_value = (pMsg[i] - '0');
        pQRparam[data_counter].data_bits = 4;
        data_counter++;
    }
    else if(remain_digit == 2){
        /* 2桁 */
        pQRparam[data_counter].data_value = (pMsg[i] - '0') * 10 + (pMsg[i+1] - '0');
        pQRparam[data_counter].data_bits = 7;
        data_counter++;
    }
    /* 終端パターンの付加 */
    data_counter = _AddTerminalPattern( pQRparam, data_counter, version);

    return data_counter;
}

/****************************************************************************/
/*  @brief  漢字モードのデータ・誤り訂正コードの生成                        */
/*  @param  pData   埋め込みデータ先頭アドレス(1pix/byte)                   */
/*  @param  version   モジュールサイズ(QRコード型番)                        */
/*  @retval pQRparam配列の総数                                              */
/*  @note                                                                   */
/*        漢字モード時の２進化                                              */
/*        ① 入力値を3桁のグループに分割( 01234567 ⇒ 012 345 67)           */
/*        ② 各グループを2進数に変換                                        */
/*        ③ 算出された値を11bitの2進数で表記する                           */
/*        ④ なお２桁に満たない場合は残った値を4bit/7bitで表記する          */
/****************************************************************************/
int _make_KANJI_data( QR_PARAM *pQRparam, UINT8 *pMsg, QR_MODE_N Mode, UINT version )
{
    int data_counter = 0;   /* pQRparam配列の総数 */
    int str_length;         /* 文字数 */
    int i;

    /* データ分析 */
    str_length = strlen(pMsg);

    /* モード指示子 */
    pQRparam[data_counter].data_value = GetModeDscrpt( Mode );
    pQRparam[data_counter].data_bits  = 4;  /* 固定 */
    data_counter++; /* next subscript */

    /* 文字数指示子 */
    pQRparam[data_counter].data_value = (str_length / 2) ;/* 超々暫定対応 */
    pQRparam[data_counter].data_bits  = GetModeBits( version, QR_MODE_KANJI );;
    data_counter++; /* next subscript */

    
    /* データの２進化 */
    for(i=0; i < str_length ;i+=2){
        /* 1byte + 2byte */
        UINT32 div, mod;
        UINT32 conv_val = ((UINT32)pMsg[i]<<8 | pMsg[i+1]);
        
        if(33088 <= conv_val && conv_val <= 40956){
            conv_val -= 33088;
        }
        else if(57408 <= conv_val && conv_val <= 60351){
            conv_val -= 49472;
        }
        div = conv_val / 256;
        mod= conv_val % 256;
    
        pQRparam[data_counter].data_value = (div * 192) + mod;
        pQRparam[data_counter].data_bits  = 13;
        data_counter++;
    }
    /* 終端パターンの付加 */
    data_counter = _AddTerminalPattern( pQRparam, data_counter, version);

    return data_counter;
}

/****************************************************************************/
/*  @brief  終端パターンの追加                                              */
/*  @param                                                                  */
/*  @retval                                                                 */
/*  @note                                                                   */
/*        シンボル容量を満たしていない場合に0000を付加する                  */
/*        データビット列がシンボル容量を満たしている場合は必要なし          */
/****************************************************************************/

/* 終端パターン追加 */
int _AddTerminalPattern(QR_PARAM *pQRparam, int data_counter, int version)
{
    int i;
    int total_bits;
    int remain_bits;
    
    /* 総bit数の算出 */
    for(total_bits=0, i=0 ; i < data_counter ; i++){
        total_bits += pQRparam[i].data_bits;
    }

    if( _qr_code_info[version].data_capacity_bits_num >= total_bits ){

        remain_bits = _qr_code_info[version].data_capacity_bits_num - total_bits;   /* 残bits */

        if( remain_bits >= 4 ){
            /* 残容量が4bit以上 */
            pQRparam[data_counter].data_value = 0;
            pQRparam[data_counter].data_bits  = 4;
        }else{
            /* 残容量が4bit未満 */
            pQRparam[data_counter].data_value = 0;
            pQRparam[data_counter].data_bits  = remain_bits;
        }
        data_counter++; /* next subscript */
    }
    else{
        /* データがシンボル容量を超えた場合 */
        printf("ERROR :QRcode too large data.\n");
        data_counter = -1;
    }
    return data_counter;
}
