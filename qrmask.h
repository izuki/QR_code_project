/*======================================================================*/
/* File Name	: qrutil.h												*/
/* Component	: QR Code												*/
/* Author		: $Author: Sato_t $										*/
/* Mail			: 														*/
/* Date			: $Date: XX/XX/XX XX:XX $								*/
/* Revision		: $Revision: 0 $										*/
/* Design Doc	: 														*/
/* Unpublished Copyright.												*/
/*======================================================================*/
#ifndef __QR_MASK_H__
#define __QR_MASK_H__

#include "qrapp.h"

int qr_mask_reserve_part( BYTE *pQRImageData, int version );
void make_mask(unsigned char *pData, int version, QR_ECLEVEL_N QRecLevel);

int mask_pos_search_pattern_module( BYTE *pQRdata, int version, UINT8 mask_col );



int Maskevaluation(unsigned char *pData, int version);
int ErrPointBlock(unsigned char *pData, int version);
int ErrPointPer(unsigned char *pData, int version);

#endif /* __QR_MASK_H__ */