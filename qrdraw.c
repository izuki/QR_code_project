#include <stdio.h>
#include "qrapp.h"
#include "qrdraw.h"


/************************************************************************/
/* マクロ定義                                                           */
/************************************************************************/

#define TIMING_PATTERN_LEN              (7) /* タイミングパターンの縦横サイズ */
#define CENTRAL_MODULE_COORDINATE_MAX   (7)
/************************************************************************/
/* 列挙体定義                                                           */
/************************************************************************/
/************************************************************************/
/* 変数定義                                                             */
/************************************************************************/
/************************************************************************/
/* 構造体定義                                                           */
/************************************************************************/
typedef struct __tagAlignmentPattern{
    UINT32 pos_laying_pattern_num;                                      /* 位置合せパターン数(H)          */
    UINT32 central_module_coordinate_array_num;							/* central_module_coordinateの数 */
    UINT32 central_module_coordinate[CENTRAL_MODULE_COORDINATE_MAX];    /* 中心モジュールの行座標又は列座標 */
}ALIGNMENTPATTERN_T;

/************************************************************************/
/* 定数宣言                                                             */
/************************************************************************/
static const ALIGNMENTPATTERN  _T AlignmentPattern[QR_VERSION_MAX+1] = {
    {  0, 0, {0,  0,  0,  0,   0,   0,   0} },    /* Dummy */
    {  0, 0, {0,  0,  0,  0,   0,   0,   0} },
    {  1, 2, {6, 18,  0,  0,   0,   0,   0} },    
    {  1, 2, {6, 22,  0,  0,   0,   0,   0} },
    {  1, 2, {6, 26,  0,  0,   0,   0,   0} },
    {  1, 2, {6, 30,  0,  0,   0,   0,   0} },
    {  1, 2, {6, 34,  0,  0,   0,   0,   0} },
    {  6, 3, {6, 22, 38,  0,   0,   0,   0} },
    {  6, 3, {6, 24, 42,  0,   0,   0,   0} },
    {  6, 3, {6, 26, 46,  0,   0,   0,   0} },
    {  6, 3, {6, 28, 50,  0,   0,   0,   0} },
    {  6, 3, {6, 30, 54,  0,   0,   0,   0} },
    {  6, 3, {6, 32, 58,  0,   0,   0,   0} },
    {  6, 3, {6, 34, 62,  0,   0,   0,   0} },
    { 13, 4, {6, 26, 46, 66,   0,   0,   0} },
    { 13, 4, {6, 26, 48, 70,   0,   0,   0} },
    { 13, 4, {6, 26, 50, 74,   0,   0,   0} },
    { 13, 4, {6, 30, 54, 78,   0,   0,   0} },
    { 13, 4, {6, 30, 56, 82,   0,   0,   0} },
    { 13, 4, {6, 30, 58, 86,   0,   0,   0} },
    { 13, 4, {6, 34, 62, 90,   0,   0,   0} },	/* Ver.20 */
	{ 22, 5, {6, 28, 50, 72,  94,   0,   0} },
	{ 22, 5, {6, 26, 50, 74,  98,   0,   0} },
	{ 22, 5, {6, 30, 54, 78, 102,   0,   0} },
	{ 22, 5, {6, 28, 54, 80, 106,   0,   0} },
	{ 22, 5, {6, 32, 58, 84, 110,   0,   0} },
	{ 22, 5, {6, 30, 58, 86, 114,   0,   0} },
	{ 22, 5, {6, 34, 62, 90, 118,   0,   0} },
    { 33, 6, {6, 26, 50, 74,  98, 122,   0} },
    { 33, 6, {6, 30, 54, 78, 102, 126,   0} },
    { 33, 6, {6, 26, 52, 78, 104, 130,   0} },
    { 33, 6, {6, 30, 56, 82, 108, 134,   0} },
    { 33, 6, {6, 34, 60, 86, 112, 138,   0} },
    { 33, 6, {6, 30, 58, 86, 114, 142,   0} },
    { 33, 6, {6, 34, 62, 90, 118, 146,   0} },
	{ 46, 7, {6, 30, 54, 78, 102, 126, 150} },
	{ 46, 7, {6, 24, 50, 76, 102, 128, 154} },
	{ 46, 7, {6, 28, 54, 80, 106, 132, 158} },
	{ 46, 7, {6, 32, 58, 84, 110, 136, 162} },
	{ 46, 7, {6, 26, 54, 82, 110, 138, 166} },
	{ 46, 7, {6, 30, 58, 86, 114, 142, 170} },	/* Ver.40 */
};



/************************************************************************/
/* ローカル関数プロトタイプ定義                                         */
/************************************************************************/
static void _draw_format_information( int version, char *pData );    // 形式情報
static void _draw_finder_pattern(int version, char *pData );        // 位置検出パターン (Finder Pattern)
static void _draw_timing_pattern(int version, char *pData );         // タイミングパターン (Timing Pattern)
static void _draw_alignment_pattern( int version, char *pData );     // 位置合せパターン (Alignment Pattern)


// mask
static void _mask_draw_pattern0(int version, char *pData );
static void _mask_draw_pattern1(int version, char *pData );
static void _mask_draw_pattern2(int version, char *pData );
static void _mask_draw_pattern3(int version, char *pData );
static void _mask_draw_pattern4(int version, char *pData );
static void _mask_draw_pattern5(int version, char *pData );
static void _mask_draw_pattern6(int version, char *pData );
static void _mask_draw_pattern7(int version, char *pData );


/************************************************************************/
/* 外部公開関数                                                         */
/************************************************************************/

/************************************************************************/
/*  @brief  QRコードの基盤を作る                                        */
/*  @param  version     QRコードの大きさ                                */
/*  @param  QRImageData イメージデータのバッファ                        */
/*  @retval size        イメージデータのバッファサイズ                  */
/*  @note                                                               */
/************************************************************************/
int base_draw( int version, char **ppQRImageData)
{
    if(version > QR_VERSION_MAX ){
        printf("ERROR :QRcode version size %d.\n", version);
        return -1;
    }

    if(ppQRImageData == NULL ){
        printf("ERROR :ppQRImageData is null.\n");
        return -1;
    }

    int size = _qr_code_info[version].side_module_num * _qr_code_info[version].side_module_num;

    /* Allocate Memory */
    *ppQRImageData = (BYTE *)calloc( (size_t)size, sizeof(BYTE) );   /* zero clear */
    if(*ppQRImageData == NULL){
        printf("Error!!! No Memory!! \n");
        return -1;
    }
    memset(*ppQRImageData, DATA_MODULE, sizeof(BYTE) * size);

    /* 既に決まっている部分の領域 */
    _draw_format_information( version, *ppQRImageData );
    _draw_finder_pattern( version, *ppQRImageData );
    _draw_timing_pattern( version, *ppQRImageData );
    _draw_alignment_pattern( version, *ppQRImageData );

    return size;

}



void qrdata_draw(const BYTE *pQRData, BYTE *pQRImageData, int version )
{
	int Line = _qr_code_info[version].side_module_num;		/* 1行の大きさ */
	int x,y,i,b, max, flag;
	int debug=0;
	int total=0;
	i=0;
	b=0;
	flag = 1;

	// x = Line -> 0
	for(x= Line - 1 ; x >= 0 ; x-=2){

		if(flag == 1){
			/* 下から上へ(-(Line) -> 0) */
			max = 1;
			y = -(Line);
		}else{
			/* 上から下へ(0 -> Line) */
			max = Line;
			y = 0;
		}
		
		// 垂直のトリミングパターンに引っ掛かる時は一つずらす
		// JIS X0510(2004)を参照
		if(x == (TIMING_PATTERN_WH-1)){
			x = x - 1;
			//break; // 以降はECCデータ部分を塗りつぶしたくない場合はここを有効にしてね(後で消す)
		}
		
		for(; y < max ; y++){

			// QRDataは1エレメント当たり1byte = 8bit / bが0-7を超えたら0に戻して次のエレメントを参照する
			// 右側
			if(b>7){
				b = 0;
				i++;
			}

			if( *(pQRImageData + (Line * abs(y)) + x) == DATA_MODULE ){
				if( pQRData[i] & (1<<(7-b)) ){
					*(pQRImageData + (Line * abs(y)) + x) = BLACK_MODULE;
					debug = 1;
				}else{
					*(pQRImageData + (Line * abs(y)) + x) = WHITE_MODULE;
					debug = 0;
				}
				total++;
				b++;
				/* debug */
				//printf("[%d][%d]=(%d)%d bit\n", abs(y), x, i, b);			
			}
			// 左側
			if(b>7){
//				printf("pQRData[%d]=0x%x\n", i, &pQRData[i]);
				b = 0;
				i++;
			}
			if( *(pQRImageData + (Line * abs(y)) + x-1)  == DATA_MODULE ){
				if( pQRData[i] & (1<<(7-b)) ){
					*(pQRImageData + (Line * abs(y) ) + x-1) = BLACK_MODULE;
					debug = 1;
				}else{
					*(pQRImageData + (Line * abs(y) ) + x-1) = WHITE_MODULE;
					debug = 0;
				}
				total++;
				b++;
				/* debug */
				//printf("[%d][%d]=(%d)%d bit\n", abs(y), x-1, i, b);			
			}
	
		}
		flag = !flag;
	}
	printf("total=%d\n", total);
}



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
/************************************************************************/
/*  @brief  QRコードのマスクデータを作る                                */
/*  @param  version     QRコードの大きさ                                */
/*  @param  ppMaskData マスクファイルのバッファ                        */
/*  @retval size        マスクファイルのバッファサイズ                  */
/*  @note                                                               */
/************************************************************************/
int mask_draw( int version, int pattern, char **ppMaskData)
{
    if(version > QR_VERSION_MAX ){
        printf("ERROR :QRcode version size %d.\n", version);
        return -1;
    }

    if(ppMaskData == NULL ){
        printf("ERROR :ppMaskData is null.\n");
        return -1;
    }

    int size = _qr_code_info[version].side_module_num * _qr_code_info[version].side_module_num;

    /* Allocate Memory */
    *ppMaskData = (BYTE *)calloc( (size_t)size, sizeof(BYTE) );   /* zero clear */
    if(*ppMaskData == NULL){
        printf("Error!!! No Memory!! \n");
        return -1;
    }
    memset(*ppMaskData, WHITE_MODULE, sizeof(BYTE) * size);

    switch(pattern)
    {
        case 0: _mask_draw_pattern0( version, *ppMaskData );	break;
        case 1:	_mask_draw_pattern1( version, *ppMaskData );	break;
        case 2:	_mask_draw_pattern2( version, *ppMaskData );	break;
        case 3: _mask_draw_pattern3( version, *ppMaskData );	break;
        case 4: _mask_draw_pattern4( version, *ppMaskData );	break;
        case 5: _mask_draw_pattern5( version, *ppMaskData );	break;
        case 6: _mask_draw_pattern6( version, *ppMaskData );	break;
        case 7: _mask_draw_pattern7( version, *ppMaskData );	break;
    }

    return size;
}













/************************************************************************/
/* ローカル関数定義                                                     */
/************************************************************************/
static void _mask_draw_pattern0(int version, char *pData )
{
    int line = _qr_code_info[version].side_module_num;// 1辺のサイズ(幅高さ共通)

    for(int y=0 ; y < line ; y++){
        for(int x=0 ; x < line ; x++){
            *pData = ( ( (y+x)%2 ) == 0 )? BLACK_MODULE : WHITE_MODULE;
            pData++;
        }
    }
}

static void _mask_draw_pattern1(int version, char *pData )
{
    int line = _qr_code_info[version].side_module_num;// 1辺のサイズ(幅高さ共通)

    for(int y=0 ; y < line ; y++){
        for(int x=0 ; x < line ; x++){
            *pData = ( ( y%2 ) == 0 )? BLACK_MODULE : WHITE_MODULE;
            pData++;
        }
    }
}

static void _mask_draw_pattern2(int version, char *pData )
{
    int line = _qr_code_info[version].side_module_num;// 1辺のサイズ(幅高さ共通)

    for(int y=0 ; y < line ; y++){
        for(int x=0 ; x < line ; x++){
            *pData = ( ( x%3 ) == 0 )? BLACK_MODULE : WHITE_MODULE;
            pData++;
        }
    }
}

static void _mask_draw_pattern3(int version, char *pData )
{
    int line = _qr_code_info[version].side_module_num;// 1辺のサイズ(幅高さ共通)

    for(int y=0 ; y < line ; y++){
        for(int x=0 ; x < line ; x++){
            *pData = ( ( (y+x)%3 ) == 0 )? BLACK_MODULE : WHITE_MODULE;
            pData++;
        }
    }
}

static void _mask_draw_pattern4(int version, char *pData )
{
    int line = _qr_code_info[version].side_module_num;// 1辺のサイズ(幅高さ共通)

    for(int y=0 ; y < line ; y++){
        for(int x=0 ; x < line ; x++){
            *pData = ( ( ((y/2)+(x/3))%2 ) == 0 )? BLACK_MODULE : WHITE_MODULE;
            pData++;
        }
    }
}

static void _mask_draw_pattern5(int version, char *pData )
{
    int line = _qr_code_info[version].side_module_num;// 1辺のサイズ(幅高さ共通)

    for(int y=0 ; y < line ; y++){
        for(int x=0 ; x < line ; x++){
            *pData = ( ( ((y*x)%2)+((y*x)%3) ) == 0 )? BLACK_MODULE : WHITE_MODULE;
            pData++;
        }
    }
}

static void _mask_draw_pattern6(int version, char *pData )
{
    int line = _qr_code_info[version].side_module_num;// 1辺のサイズ(幅高さ共通)

    for(int y=0 ; y < line ; y++){
        for(int x=0 ; x < line ; x++){
            *pData = ( ( (y*x)%2 ) == 0 )? BLACK_MODULE : WHITE_MODULE;
            pData++;
        }
    }
}

static void _mask_draw_pattern7(int version, char *pData )
{
    int line = _qr_code_info[version].side_module_num;// 1辺のサイズ(幅高さ共通)

    for(int y=0 ; y < line ; y++){
        for(int x=0 ; x < line ; x++){
            *pData = (  (((y*x)%3) + ((y+x)%2) )%2 == 0 )? BLACK_MODULE : WHITE_MODULE;
            pData++;
        }
    }
}

/****************************************************************************/
/*  @brief  形式情報の埋め込み                                              */
/*  @param  pData   埋め込みデータ先頭アドレス(1pix/byte)                   */
/*  @param  version   モジュールサイズ(QRコード型番)                        */
/*  @retval なし                                                            */
/*  @note   INFO_MODULEで塗りつぶす                                         */
/*                                                                          */
/****************************************************************************/

static void _draw_format_information( int version, char *pData ){

    int x,y;
    int line = _qr_code_info[version].side_module_num;// 1辺のサイズ(幅高さ共通)
    int PosX = TIMING_PATTERN_LEN+1;
    int PosY = TIMING_PATTERN_LEN+1;
        
    // 左上
    for(x=0, PosY=TIMING_PATTERN_LEN+1;x < TIMING_PATTERN_LEN+2 ;x++){
        *(pData + (line * PosY) + x) = INFO_MODULE;
    }

    // 右上
    for(x=line-(TIMING_PATTERN_LEN+1), PosY=TIMING_PATTERN_LEN+1;x < line ;x++){
        *(pData + (line * PosY) + x) = INFO_MODULE;
    }
    

    // 右上(垂直)
    for(PosX=TIMING_PATTERN_LEN+1, y=0;y < TIMING_PATTERN_LEN+2 ;y++){
        *(pData + (line * y) + PosX) = INFO_MODULE;
    }

    // 右下(垂直)
    for(PosX=TIMING_PATTERN_LEN+1, y=line-(TIMING_PATTERN_LEN+1);y < line ;y++){
        *(pData + (line * y) + PosX) = INFO_MODULE;
    }   


    *(pData+8+(line*(line-8))) = BLACK_MODULE;
}

/****************************************************************************/
/*  @brief  位置検出パターンの埋め込み                                      */
/*  @param  pData   埋め込みデータ先頭アドレス(1pix/byte)                   */
/*  @param  version   モジュールサイズ(QRコード型番)                        */
/*  @retval なし                                                            */
/*  @note                                                                   */
/****************************************************************************/
static void _draw_finder_pattern( int version, char *pData ){
    
    int x,y; /* counter */
    int cur_pos = _qr_code_info[version].side_module_num - 8;
    UINT8 *pCurPos;

/* 位置検出パターンの埋め込み */
    /* 左上 */
    for(y = 0; y < 8 ; y++){
        for(x = 0; x < 8 ; x++){
            pCurPos = pData + (x + (y * _qr_code_info[version].side_module_num));
            *pCurPos = WHITE_MODULE;

            if( (2 <= x && 2 <=y) && (x < 5 && y < 5 ) ){
                *pCurPos = BLACK_MODULE;
                continue;
            }

            if( (1 <= x && 1 <=y) && (x < 6 && y < 6 ) ){
                *pCurPos =  WHITE_MODULE;
                continue;
            }

            if( (0 <= x && 0 <= y) && (x < 7 && y < 7 ) ){
                *pCurPos = BLACK_MODULE;
                continue;
            }
        }
    }
    /* 右上 */
    for(y = 0; y < 8 ; y++){
        for(x = 0; x < 8 ; x++){

            pCurPos = pData + (cur_pos + x + (y * _qr_code_info[version].side_module_num));
            *pCurPos = WHITE_MODULE;

            if( (3 <= x && 2 <= y) && (x <= 5 && y < 5 ) ){
                *pCurPos = BLACK_MODULE;
                continue;
            }

            if( (2 <= x && 1 <= y) && (x <= 6 && y < 6 ) ){
                *pCurPos = WHITE_MODULE;
                continue;
            }

            if( (1 <= x && 0 <= y) && (x <= 7 && y < 7 ) ){
                *pCurPos = BLACK_MODULE;
                continue;
            }
        }
    }
    /* 左下 */
    for(y = 0; y < 8 ; y++){
        for(x = 0; x < 8 ; x++){

            pCurPos = pData +( x + ((cur_pos + y) * _qr_code_info[version].side_module_num));
            *pCurPos = WHITE_MODULE;

            if( (2 <= x && 3 <=y) && (x < 5 && y <= 5 ) ){
                *pCurPos = BLACK_MODULE;
                continue;
            }

            if( (1 <= x && 2 <=y) && (x < 6 && y <= 6 ) ){
                *pCurPos = WHITE_MODULE;
                continue;
            }

            if( (0 <= x && 1 <= y) && (x < 7 && y <= 7 ) ){
                *pCurPos = BLACK_MODULE;
                continue;
            }
        }
    }
}



/****************************************************************************/
/*  @brief  タイミングパターンの埋め込み                                    */
/*  @param  pData   埋め込みデータ先頭アドレス(1pix/byte)                   */
/*  @param  version   モジュールサイズ(QRコード型番)                        */
/*  @retval なし                                                            */
/*  @note    暗モジュールと明モジュールが1モジュールずつ交互になった直線を  */
/*           水平方向と垂直方向に1本ずつ6行目に配置する                     */
/****************************************************************************/
static void _draw_timing_pattern( int version, char *pData )
{
    int i, flag;
    int side = _qr_code_info[version].side_module_num;// 1辺のサイズ(幅高さ共通)
    char col;

    /* 開始オフセット */
    int HorOffset = (side * (TIMING_PATTERN_LEN-1)) + TIMING_PATTERN_LEN;    /* 水平方向 */
    int VerOffset = (side * TIMING_PATTERN_LEN) + TIMING_PATTERN_LEN-1;  /* 垂直方向 */

    flag = 1;   // 明モジュールから埋めていく
    for(i=0 ; i < ( side-(TIMING_PATTERN_LEN*2) ); i++){

        if(flag == 1){
            col = WHITE_MODULE;
        }else{
            col = BLACK_MODULE;
        }
        
        *(pData + HorOffset + i) = col;
        *(pData + VerOffset + (side * i)) = col;

        flag = !flag;
    }
    
    // 左下の変な所にあるタイミングパターンを埋め込む
    *(pData + (side * (side - TIMING_PATTERN_LEN - 1)) + TIMING_PATTERN_LEN+1 ) = BLACK_MODULE;

}

/****************************************************************************/
/*  @brief  位置合せパターンの埋め込み                                      */
/*  @param  pData   埋め込みデータ先頭アドレス(1pix/byte)                   */
/*  @param  version   モジュールサイズ(QRコード型番)                        */
/*  @retval なし                                                            */
/*  @note   位置検出パターンに被る座標の位置合せパターンは省略する          */
/*                                                                          */
/****************************************************************************/

static void _draw_alignment_pattern( int version, char *pData ){

    int x,y; /* counter */
    int vertical, horizon ; /* Array counter *//* vertical=行, horizon=列 */
    int posX, posY; /* 座標 */

    for(vertical=0; vertical < AlignmentPattern[version].central_module_coordinate_array_num; vertical++){
        for(horizon=0; horizon < AlignmentPattern[version].central_module_coordinate_array_num; horizon++){

            /* 位置検出パターンに被る個所は省略する */
            if( (vertical == 0 && horizon == 0) ||
                (horizon == 0 && (AlignmentPattern[version].central_module_coordinate_array_num-1 <= vertical) ) ||
                (vertical== 0 && (AlignmentPattern[version].central_module_coordinate_array_num-1 <= horizon)) ){
                continue;
            }
            /* x, y を始点に(5,5)のモジュールの埋め込みを行う */
            posX = AlignmentPattern[version].central_module_coordinate[horizon]  -2;
            posY = AlignmentPattern[version].central_module_coordinate[vertical] -2;
            for(x=0; x < 5; x++){
                for(y=0; y < 5 ; y++){
                    /* 0行目, 5行目 */
                    if(y==0 || y==4){
                        *(pData + posX + x + (posY + y) * _qr_code_info[version].side_module_num ) = BLACK_MODULE;
                    }
                    /* 2行目, 4行目 */
                    else if(y== 1 || y== 3){
                        if(x==0 || x==4){
                        *(pData + posX + x + (posY + y) * _qr_code_info[version].side_module_num ) = BLACK_MODULE;
                        }else{
                        *(pData + posX + x + (posY + y) * _qr_code_info[version].side_module_num ) = WHITE_MODULE;
                        }
                    }else{
                        if(x==0 || x==4 || x == 2){
                        *(pData + posX + x + (posY + y) * _qr_code_info[version].side_module_num ) = BLACK_MODULE;
                        }else{
                        *(pData + posX + x + (posY + y) * _qr_code_info[version].side_module_num ) = WHITE_MODULE;
                        }
                    }
                }
            }
        }
    }
}
