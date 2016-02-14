
#ifndef _DEV_APP_RESAMPLE_H_
#define _DEV_APP_RESAMPLE_H_
//#include "cs_types.h"
#include "stdefs.h"
typedef enum
{
    DEV_APP_ID_1 = 0,
    DEV_APP_ID_NUM,
}DEV_APP_ID_ENUM;

typedef struct
{
    INT32 in_sample_rate;
    INT32 out_sample_rate;
    INT32 frame_length;
    INT32 channel_num;
    INT8  sample_format;
}DEV_APP_CONFIG_STRUCT;

#define USE_SMALLFILTER
#define Nhc       8
#define Na        7
#define Np       (Nhc+Na)
#define Npc      (1<<Nhc)
#define Amask    ((1<<Na)-1)
#define Pmask    ((1<<Np)-1)
#define Nh       16
#define Nb       16
#define Nhxn     16
#define Nhg      (Nh-Nhxn)
#define NLpScl   13

#define IBUFFSIZE 4096*16 //dzy
typedef struct{
    UHWORD LpScl;               /* Unity-gain scale factor */
    UHWORD Nwing;               /* Filter table size */
    UHWORD Nmult;               /* Filter length for up-conversions */
	UHWORD Xoff;
	UHWORD Xread;
    HWORD *Imp;               /* Filter coefficients */
    HWORD *ImpD;              /* ImpD[n] = Imp[n+1]-Imp[n] */
	UWORD factor;
	UWORD dhb, dtb;
	UWORD Time;
	HWORD X1[IBUFFSIZE];
}  ParameterResample;
typedef struct
{
	int		nInputSampleRate;
	int     nOutputSampleRate;
	int     nInputbufferszie;//samples number of mono channel input buffer 
	int     nOutputbuffersize;//samples number of mono channel output buffer 
	void    *pSRCHandler;
}	PassSRC;



#endif

