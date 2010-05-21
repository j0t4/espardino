
#ifndef __MICRO214X_TYPES_H
#define __MICRO214X_TYPES_H

#ifndef u32
typedef unsigned int    u32;
#endif

#ifndef s32
typedef int	            s32;
#endif

#ifndef u16
typedef unsigned short  u16;
#endif 

#ifndef s16
typedef short		    s16;
#endif

#ifndef u8
typedef unsigned char    u8;
#endif

#ifndef s8
typedef signed char	     s8;
#endif

#ifndef __cplusplus
		#ifndef bool
		typedef u32				bool;
		#endif
		#ifndef true
		#define true 1
		#endif
		
		#ifndef false
		#define false 0
		#endif
#endif

#endif