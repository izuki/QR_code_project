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
#ifndef __QR_SPLIT_H__
#define __QR_SPLIT_H__

#include "qrapp.h"

char *split_8bit_data( QR_PARAM *pQRparam, int data_counter, int version, QR_ECLEVEL_N QRecLevel);


#endif /* __QR_SPLIT_H__ */