/************************************************************************/
/* INCLUDE                                                              */
/************************************************************************/
#include <stdio.h>
#include "GaloisList.h"

/************************************************************************/
/* ファイル内定数定義                                                   */
/************************************************************************/
#define GALOISLIST_MAX  (255 + 1) /* GF(2^8) の最大数 + 1 */

/************************************************************************/
/* デバッグ定義                                                         */
/************************************************************************/
/************************************************************************/
/* 構造体定義                                                           */
/************************************************************************/
/************************************************************************/
/* ファイル内変数定義                                                   */
/************************************************************************/
/* Table */
static int _lookupTable[GALOISLIST_MAX];    /* 係数(Coe)から指数(Index) */
static int _RvslookupTable[GALOISLIST_MAX]; /* 指数(Index)から係数(Coe) */

/************************************************************************/
/* ローカル関数プロトタイプ定義                                         */
/************************************************************************/
static int _cal_gx( unsigned char *pGx, int n);

/************************************************************************/
/*  @brief  GF(2^8)αの指数と整数の対応表作成                           */
/*  @param  なし                                                        */
/*  @retval なし                                                        */
/*  @note   GetCalc_Gx()を呼び出す前に呼んでおく                        */
/************************************************************************/
void InitGaloisList(void){
    int Coe;    /* 係数 */
    int Index;  /* 指数 */

    /* 係数が0の場合は指数が存在しない(α^?=0)ので係数が0の場合は指数は-1とする */
    _RvslookupTable[0] = -1;

    for(Coe=1, Index=0; Index < GALOISLIST_MAX ; Index++ ){

        _lookupTable[Index] = Coe;

        /* α^0=1 なので係数が1の場合は指数0 */
        _RvslookupTable[Coe] = (Coe != 1)? Index : 0;

        Coe <<= 1;

        /* 8ビット目が存在した場合 */
        if( Coe & (1<<8) ){
            /* 0x11D = 100011101 */
            Coe = (Coe^0x11D); /* 符号理論(ガロア体)の基本概念では加算はXOR */
        }
    }
}

/************************************************************************/
/*  @brief  エラー訂正コードの生成多項式g(x)を求める                    */
/*  @param  **ppGx(O) 生成多項式の指数格納                              */
/*  @param  ECCode(I) エラー訂正コード語数                              */
/*  @retval index                                                       */
/*  @note                                                               */
/*      この関数を使用する前にInitGaloisList() を必ず呼んでおく         */
/************************************************************************/
int GetCalc_Gx( unsigned char **ppGx, int ECCode ){

    int ret=0;

    /* 引数チェック */
    if(ppGx == NULL || ECCode < 0){
        printf("Illegal Parameter Error!!\n");
        return ret;
    }

    /* Allocate Memory */
    *ppGx = (unsigned char*)calloc(ECCode+1, sizeof(unsigned char) );

    if(*ppGx != NULL){
        /* 初期化 */
        memset( *ppGx, 0xFF, sizeof(unsigned char)*(ECCode+1) );
        /* エラー訂正コード語数繰り返す */
        for(int i=0 ; i < ECCode ; i++){
            /* ex) ECCode(2) ⇒ g1(x) = (x+a^0)(x + a^1)          = x^2a^0 + x^1a^25 + x^0+a^1              ⇒ {0, 25, 1} */
            /*     ECCode(3) ⇒ g2(x) = (x+a^0)(x + a^1)(x + a^2) = x^3a^0 + x^2a^198 + x^1+a^199 + x^0+a^3 ⇒ (0, 198, 199, 3) */
            ret = _cal_gx(*ppGx, i);
        }
    }
    return ret;
}

/************************************************************************/
/*  @brief  GF(2^8)リストより指数から係数を引く                         */
/*  @param  Index   0～256の範囲                                        */
/*  @retval GF(2^8)の値                                                 */
/*  @note                                                               */
/*      この関数を呼び出す前にInitGaloisList()を必ず呼び出す事          */
/************************************************************************/
int GetlookupTable(unsigned int Index){

	return (Index < GALOISLIST_MAX)? _lookupTable[Index] : -1;

}

/************************************************************************/
/*  @brief  GF(2^8)リストより係数から指数を引く                         */
/*  @param  Index   0～256の範囲                                        */
/*  @retval GF(2^8)の指数                                               */
/*  @note                                                               */
/*      この関数を呼び出す前にInitGaloisList()を必ず呼び出す事          */
/************************************************************************/
int GetRvslookupTable(unsigned int Coe){

	return (Coe < GALOISLIST_MAX)? _RvslookupTable[Coe] : -1;

}

/************************************************************************/
/* ローカル関数                                                         */
/************************************************************************/

/************************************************************************/
/*  @brief  GF(2^8)リストより係数から指数を引く                         */
/*  @param  *pGx(I/O)   ｇn-1(x)の値                                    */
/*  @param  n(I)        ｇn(x)のn値                                     */
/*  @retval GF(2^8)の指数                                               */
/*  @note                                                               */
/*      この関数を呼び出す前にInitGaloisList()を必ず呼び出す事          */
/*      pGxの中はｇn-1(x)の値が入っている事                             */
/*      ex)  g2(x)の値をこの関数で求める場合はppGxの参照先には          */
/*           g1(x)の値{0,25,1}が存在する必要がある                      */
/************************************************************************/
static int _cal_gx( unsigned char *pGx, int n){
	int i;
    int base_coe;   /* 係数 */
    unsigned char *pGxtmp;

    if(n == 0){
        memset(pGx, 0x00, sizeof(unsigned char)*2);
        return 2;
    }

    /* Allocate Memory */
    pGxtmp = (unsigned char*)calloc(n+2, sizeof(unsigned char) );
    if(pGxtmp == NULL){
        printf("No Memory!!\n");
        return 0;
    }else{
        /* 初期化 */
        memset(pGxtmp, 0xFF, sizeof(unsigned char)*(n+2));
    }
    for( i=1; i < (n+2); i++){ /*  */
        pGxtmp[i] = (pGx[i-1] + n ) % 255;  /* 指数 */
    }
    for( i=0; i < (n+2); i++){ /*  */
        if(pGx[i] == 0xFF){
            pGx[i] = pGxtmp[i];
        }else if(pGxtmp[i] == 0xFF){
            /* do nothing */
        }else{
            base_coe = _lookupTable[pGx[i]]^_lookupTable[pGxtmp[i]];    /* 指数→係数へ */
            pGx[i] = _RvslookupTable[base_coe];
        }
    }

    /* Free */
    free(pGxtmp);

    return i;
}
