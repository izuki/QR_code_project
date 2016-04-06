#include <stdio.h>
#include <stdlib.h>

#include "GaloisList.h"
#include "qrapp.h"
#include "qrmask.h"
#include "qrdraw.h"

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

int _mask_info_part( BYTE *pMask, int version );

/************************************************************************/
/* 外部公開関数                                                         */
/************************************************************************/

/****************************************************************************/
/*  @brief  マスク処理                                                      */
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
int mask_processing(BYTE *pQRdata, int version, QR_ECLEVEL_N QRecLevel)
{
        // マスクパラメータの失点をチェックする

	// 一番失点の小さいマスクを選択する
}


/************************************************************************/
/* ローカル関数                                                         */
/************************************************************************/



	// 同色の行又は列の隣接モジュールが5以上の場合は失点を課す(3+i)
// マスクの失点を計算する
int Mask_evaluateSymbol(BYTE *pQRdata, int version)
{
    int Eret=0; /* エラーポイント */
    int i,j;
    int blacks, whites;
    int LineCol, RowCol;
    int line = _qr_code_info[version].side_module_num;

    // 列
    for(i=0;i<line;i++){
        blacks = 0;
        whites = 0;
        for(j=0;j<line;j++){
            LineCol = *(pQRdata + line * i + j);
            if(LineCol == BLACK_MODULE){
                blacks++;
                whites=0;
            }
            else if(LineCol == WHITE_MODULE){
                blacks=0;
                whites++;
            }
            
            // 同色モジュールが複数並んでいた場合
            if(blacks == 5 || whites == 5){
                Eret += 3;
            }
            else if(blacks > 5 || whites > 5){
                Eret++;
            }
        }
    }
    // 行
    for(i=0;i<line;i++){
        blacks = 0;
        whites = 0;
        for(j=0;j<line;j++){
            RowCol = *(pQRdata +  i + line * j);
            if(RowCol == BLACK_MODULE){
                blacks++;
                whites=0;
            }
            else if(RowCol == WHITE_MODULE){
                blacks=0;
                whites++;
            }
            // 同色モジュールが複数並んでいた場合
            if(blacks == 5 || whites == 5){
                Eret += 3;
            }
            else if(blacks > 5 || whites > 5){
                Eret++;
            }
        }
    }
    printf("Total Error Point = %d\n", Eret);

    return Eret;
}

// 2x2のブロックがある場合は失点を課す(+3)
int ErrPointBlock(unsigned char *pQRdata, int version)
{
	int Eret =0; /* エラーポイント */
	int i,j,k;
	int blacks, whites;
	int Module_Col[4]; /* 2x2 */
	int line = _qr_code_info[version].side_module_num;

	for(i=0;i<(line-1);i++){
		blacks = 0;
		whites = 0;
		for(j=0;j<(line-1);j++){
			Module_Col[0] = *(pQRdata + line * i + j);
			Module_Col[1] = *(pQRdata + line * i + j+1);
			Module_Col[2] = *(pQRdata + line * (i+1) + j);
			Module_Col[3] = *(pQRdata + line * (i+1) + (j+1));
			
			for(k=0;k<4;k++){
				if(Module_Col[k] == BLACK_MODULE){
					blacks++;
				}
				else if(Module_Col[k] == WHITE_MODULE){
					whites++;
				}
			}
			if(blacks==4 || whites==4){
				Eret += 3;
			}
		}
	}
	printf("Total Error Point = %d\n", Eret);

	return Eret;
}

// 暗ブロックが5%増減する度に10点を加点する(+10)
// 50%(0点)
// 45～55%(10点)
// 40～60%(20点)

int ErrPointPer(unsigned char *pQRdata, int version)
{
	
	int Eret =0; /* エラーポイント */
	int i,j;
	int blacks=0;
	int Module_Col;
	int Val;
	int line = _qr_code_info[version].side_module_num;
	

	for(i=0;i<line;i++){
		for(j=0;j<line;j++){
			Module_Col = *(pQRdata + line * i + j);

			if(Module_Col == BLACK_MODULE){
				blacks++;
			}
		}
	}
	Val = (blacks * 100) / (line*line);
	
	Eret = ((abs(Val-50) + 4) / 5) * 10;

	printf("Total Error Point = %d\n", Eret);

	return Eret;
}
