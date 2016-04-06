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
#ifndef __QR_ECC_H__
#define __QR_ECC_H__

#include "qrapp.h"

char *make_ecc_code( char *pQRdata, int version, QR_ECLEVEL_N QRecLevel);

#endif /* __QR_ECC_H__ */