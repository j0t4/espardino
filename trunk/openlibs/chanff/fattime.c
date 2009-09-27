#include "integer.h"
#include "fattime.h"
#include <rtc.h>

/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */
/* This is not required in read-only configuration.        */

DWORD get_fattime (void)
{
	DWORD res;
	RTC   rtc;

	RTC_get( &rtc );
	
	res =  (((DWORD)rtc.year - 1980) << 25)
			| ((DWORD)rtc.month << 21)
			| ((DWORD)rtc.dom << 16)
			| (WORD)(rtc.hour << 11)
			| (WORD)(rtc.min << 5)
			| (WORD)(rtc.sec >> 1);

	return res;
}

