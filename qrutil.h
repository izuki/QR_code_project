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
#ifndef __QR_UTIL__
#define __QR_UTIL__

#include "type.h"


PUBLIC int GetModeBits( char version, QR_MODE_N Mode );
PUBLIC int GetModeDscrpt( QR_MODE_N Mode );
PUBLIC int ChangeFontKindValue( char c );

#endif /* __QR_MAKE__ */