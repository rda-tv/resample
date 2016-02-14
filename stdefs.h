/* stdefs.h */
#ifndef _STDEFS_H
#define _STDEFS_H
//#include "plm_type.h"

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef PI
#define PI (3.14159265358979232846)
#endif

#ifndef PI2
#define PI2 (6.28318530717958465692)
#endif

#define D2R (0.01745329348)          /* (2*pi)/360 */
#define R2D (57.29577951)            /* 360/(2*pi) */

#ifndef MAX
#define MAX(x,y) ((x)>(y) ?(x):(y))
#endif
#ifndef MIN
#define MIN(x,y) ((x)<(y) ?(x):(y))
#endif

#ifndef ABS
#define ABS(x)   ((x)<0   ?(-(x)):(x))
#endif

#ifndef SGN
#define SGN(x)   ((x)<0   ?(-1):((x)==0?(0):(1)))
#endif

//typedef char           BYTE;
typedef short          HWORD;
typedef unsigned short UHWORD;
//typedef int            WORD;
typedef unsigned int   UWORD;

#define MAX_HWORD (32767)
#define MIN_HWORD (-32768)

typedef unsigned int               UINT32;
typedef unsigned short              UINT16;
typedef int                        INT32;
typedef short                       INT16;
typedef unsigned char               UINT8;
typedef char                        INT8;
typedef char                       BOOL;
typedef void                        VOID;
typedef unsigned long long          UINT64;
#endif /* _STDEFS_H */
