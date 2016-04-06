
#include "qrapp.h"
#include "qrutil.h"

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
/*  @brief  QRコードのモジュールサイズとモード種別から文字数指示子を返す    */
/*  @param  version モジュールサイズ                                        */
/*  @param  Mode    モード種別                                              */
/*                                                                          */
/*  @retval CharNumIndexLength  文字数指示子                                */
/*                                                                          */
/*  @note                                                                   */
/*      モジュールサイズ：QRコード画像の大きさ                              */
/*      モード種別：格納する文字の種類                                      */
/*      文字数指示子：格納する文字数を定義するbit数                         */
/****************************************************************************/
PUBLIC int GetModeBits( char version, QR_MODE_N Mode ){

    int CharNumIndexLength = -1;

    if(1 <= version && version <= 9){
        switch(Mode){
            case QR_MODE_NUM:   CharNumIndexLength = 10;    break;
            case QR_MODE_AN:    CharNumIndexLength = 9;     break;
            case QR_MODE_BYTE:  CharNumIndexLength = 8;     break;
            case QR_MODE_KANJI: CharNumIndexLength = 8;     break;
        }
    }
    else if(10 <= version && version <= 26){
        switch(Mode){
            case QR_MODE_NUM:   CharNumIndexLength = 12;    break;
            case QR_MODE_AN:    CharNumIndexLength = 11;    break;
            case QR_MODE_BYTE:  CharNumIndexLength = 16;    break;
            case QR_MODE_KANJI: CharNumIndexLength = 10;    break;
        }
    }
    else if(27 <= version && version <= 40){
        switch(Mode){
            case QR_MODE_NUM:   CharNumIndexLength = 14;    break;
            case QR_MODE_AN:    CharNumIndexLength = 13;    break;
            case QR_MODE_BYTE:  CharNumIndexLength = 16;    break;
            case QR_MODE_KANJI: CharNumIndexLength = 12;    break;
        }
    }else{
        /* do nothing */
    }
    return CharNumIndexLength;
}
/****************************************************************************/
/*  @brief  QRコードのモード種別から を返す                                 */
/*  @param  Mode    モード種別                                              */
/*                                                                          */
/*  @retval iRet                                                            */
/*                                                                          */
/*  @note                                                                   */
/****************************************************************************/
PUBLIC int GetModeDscrpt( QR_MODE_N Mode )
{
    int iRet = -1;

    switch(Mode){
        case QR_MODE_NUM:   iRet = NUM_MODE_DSCRPT;     break;
        case QR_MODE_AN:    iRet = AN_MODE_DSCRPT;      break;
        case QR_MODE_BYTE:  iRet = BYTE_MODE_DSCRPT;    break;
        case QR_MODE_KANJI: iRet = KANJI_MODE_DSCRPT;   break;
    }
    return iRet;
}


/****************************************************************************/
/*  @brief  各文字を数値化する                                              */
/*  @param  c   Ascii文字                                                   */
/*  @retval 文字を数値化した値                                              */
/*  @note                                                                   */
/*      英数字モードのみ文字を数値化する                                    */
/****************************************************************************/
PUBLIC int ChangeFontKindValue( char c ){

    int Val = -1;

    switch( c ){
        case '0': Val = 0;  break;
        case '1': Val = 1;  break;
        case '2': Val = 2;  break;
        case '3': Val = 3;  break;
        case '4': Val = 4;  break;
        case '5': Val = 5;  break;
        case '6': Val = 6;  break;
        case '7': Val = 7;  break;
        case '8': Val = 8;  break;
        case '9': Val = 9;  break;
        case 'A': Val = 10; break;
        case 'B': Val = 11; break;
        case 'C': Val = 12; break;
        case 'D': Val = 13; break;
        case 'E': Val = 14; break;
        case 'F': Val = 15; break;
        case 'G': Val = 16; break;
        case 'H': Val = 17; break;
        case 'I': Val = 18; break;
        case 'J': Val = 19; break;
        case 'K': Val = 20; break;
        case 'L': Val = 21; break;
        case 'M': Val = 22; break;
        case 'N': Val = 23; break;
        case 'O': Val = 24; break;
        case 'P': Val = 25; break;
        case 'Q': Val = 26; break;
        case 'R': Val = 27; break;
        case 'S': Val = 28; break;
        case 'T': Val = 29; break;
        case 'U': Val = 30; break;
        case 'V': Val = 31; break;
        case 'W': Val = 32; break;
        case 'X': Val = 33; break;
        case 'Y': Val = 34; break;
        case 'Z': Val = 35; break;
        case ' ': Val = 36; break;
        case '$': Val = 37; break;
        case '%': Val = 38; break;
        case '*': Val = 39; break;
        case '+': Val = 40; break;
        case '-': Val = 41; break;
        case '.': Val = 42; break;
        case '/': Val = 43; break;
        case ':': Val = 44; break;
    }
    return Val;
}

