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
#ifndef __QR_DRAWT_H__
#define __QR_DRAWT_H__

#include "qrapp.h"

int base_draw(int version, char **ppQRImageData);
void qrdata_draw(const BYTE *pQRData, BYTE *pQRImageData, int version );

int QRInfo_mask( BYTE *pQRImageData, int version );


#endif /* __QR_SPLIT_H__ */