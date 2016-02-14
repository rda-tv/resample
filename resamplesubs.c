/* resamplesubs.c - sampling rate conversion subroutines */
// Altered version
#include "dev_app_resample.h"

#include "smallfilter.h"
#include "largefilter.h"


/* Sampling rate up-conversion only subroutine;
 * Slightly faster than down-conversion;
 */
static int SrcUp(HWORD X[], HWORD Y[], UWORD dtb, UWORD *Time,
                 UHWORD Nx, UHWORD Nwing, UHWORD LpScl,
                 HWORD Imp[], HWORD ImpD[],int ch)
{
    HWORD *Xp, *Ystart;
    HWORD *Hp, *Hdp = 0, *End, Ph;
    HWORD a = 0;
    int v, t;

    UWORD endTime;              /* When Time reaches EndTime, return to user */

    Ystart =(HWORD *)Y;
    endTime =/* *Time + */(1<<Np)*(int)Nx;
    while (*Time < endTime) {
        Xp = &X[*Time>>Np];      /* Ptr to current input sample */
        /* Perform left-wing inner product */
//        v = FilterUp(Imp, ImpD, Nwing, Interp, Xp, (HWORD)(*Time&Pmask),-1);
        Ph = (HWORD)(*Time&Pmask);
        v=0;
        Hp = &Imp[Ph>>Na];
        End = &Imp[Nwing];
        Hdp = &ImpD[Ph>>Na];
        a = Ph & Amask;

        while (Hp < End) {
            t = *Hp;      /* Get filter coeff */
            t += (((int)*Hdp)*a)>>Na; /* t is now interp'd filter coeff */
            Hdp += Npc;       /* Filter coeff differences step */
            t *= *Xp;     /* Mult coeff by input sample */
            /* Round, if needed */
            t += (1<<(Nhxn-1));
            t >>= Nhxn;       /* Leave some guard bits, but come back some */
            v += t;           /* The filter output */
            Hp += Npc;        /* Filter coeff step */
            Xp -= 1;      /* Input signal step. NO CHECK ON BOUNDS */
        }
        /* Perform right-wing inner product */
//        v += FilterUp(Imp, ImpD, Nwing, Interp, Xp+1, (HWORD)((((*Time)^Pmask)+1)&Pmask),1);

        Ph = (HWORD)((((*Time)^Pmask)+1)&Pmask);
        Xp = &X[(*Time>>Np)+1];
        Hp = &Imp[Ph>>Na];
//      End = &Imp[Nwing];
        Hdp = &ImpD[Ph>>Na];
        a = Ph & Amask;
        End--;          /*    0.5, we don't do too many mult's */
        if (Ph == 0) {      /* If the phase is zero...           */
            /* ...then we've already skipped the */
            Hp += Npc;      /*    first sample, so we must also  */
            Hdp += Npc;     /*    skip ahead in Imp[] and ImpD[] */
        }
        while (Hp < End) {
            t = *Hp;      /* Get filter coeff */
            t += (((int)*Hdp)*a)>>Na; /* t is now interp'd filter coeff */
            Hdp += Npc;       /* Filter coeff differences step */
            t *= *Xp;     /* Mult coeff by input sample */
            /* Round, if needed */
            t += (1<<(Nhxn-1));
            t >>= Nhxn;       /* Leave some guard bits, but come back some */
            v += t;           /* The filter output */
            Hp += Npc;        /* Filter coeff step */
            Xp += 1;      /* Input signal step. NO CHECK ON BOUNDS */
        }

        v >>= Nhg;              /* Make guard bits */
        v *= LpScl;             /* Normalize for unity filter gain */

        v += (1<<(NLpScl-1));
        v >>= NLpScl;
        if(v>MAX_HWORD)
            v = MAX_HWORD;
        else if(v<MIN_HWORD)
            v = MIN_HWORD;
        v=(v*9394)>>13;               //resample alg have -1.892db attenuation.

//        *Y++ = WordToHword(v,NLpScl);
        // *Y++ = v & (0x00ffffff) | ((1)<<31);
        //*Y++ = v & (0x00ffffff);
#if 1
        //*Y = (HWORD)v;
        //*Y++ |= ((ch&1)<<31);
        *Y++ = (HWORD)v;
#endif

        *Time += dtb;           /* Move to next sample by time increment */
    }
    return (Y - Ystart);        /* Return the number of output samples */
}


/* Sampling rate conversion subroutine */

static int SrcUD(HWORD X[], HWORD Y[], UWORD dhb, UWORD dtb, UWORD *Time,
                 UHWORD Nx, UHWORD Nwing, UHWORD LpScl,
                 HWORD Imp[], HWORD ImpD[],int ch)
{
    HWORD *Xp, *Ystart;
    HWORD *Hp, *End, Ph;
    HWORD a = 0;
    int v, t, Hd;
    UWORD Ho;

    UWORD endTime;              /* When Time reaches EndTime, return to user */

    Ystart = (HWORD *)Y;
    endTime =/* *Time + */(1<<Np)*(int)Nx;
    while (*Time < endTime) {
        Xp = &X[*Time>>Np];     /* Ptr to current input sample */
//        v = FilterUD(Imp, ImpD, Nwing, Interp, Xp, (HWORD)(*Time&Pmask), -1, dhb);
        Ph = (HWORD)(*Time&Pmask);


        v=0;
        Ho = (Ph*(UWORD)dhb)>>Np;
        End = &Imp[Nwing];

        while ((Hp = &Imp[Ho>>Na]) < End) {
            t = *Hp;      /* Get IR sample */
            Hd = (int)(ImpD[Ho>>Na]);  /* get interp (lower Na) bits from diff table*/
            a = Ho & Amask;   /* a is logically between 0 and 1 */
            t += (Hd*a)>>Na; /* t is now interp'd filter coeff */
            t *= *Xp;     /* Mult coeff by input sample */
            /* Round, if needed */
            t += 1<<(Nhxn-1);
            t >>= Nhxn;       /* Leave some guard bits, but come back some */
            v += t;           /* The filter output */
            Ho += dhb;        /* IR step */
            Xp -= 1;      /* Input signal step. NO CHECK ON BOUNDS */
        }
//        v += FilterUD(Imp, ImpD, Nwing, Interp, Xp+1, (HWORD)((((*Time)^Pmask)+1)&Pmask), 1, dhb);

        Xp = &X[(*Time>>Np)+1];
        Ph = (HWORD)((((*Time)^Pmask)+1)&Pmask);
        Ho = (Ph*(UWORD)dhb)>>Np;
//    End = &Imp[Nwing];
        End--;          /*    0.5, we don't do too many mult's */
        if (Ph == 0)        /* If the phase is zero...           */
            Ho += dhb;        /* ...then we've already skipped the */
        /*    first sample, so we must also  */
        /*    skip ahead in Imp[] and ImpD[] */

        while ((Hp = &Imp[Ho>>Na]) < End) {
            t = *Hp;      /* Get IR sample */
            Hd = (int)(ImpD[Ho>>Na]);  /* get interp (lower Na) bits from diff table*/
            a = Ho & Amask;   /* a is logically between 0 and 1 */
            t += (Hd*a)>>Na; /* t is now interp'd filter coeff */
            t *= *Xp;     /* Mult coeff by input sample */
            /* Round, if needed */
            t += 1<<(Nhxn-1);
            t >>= Nhxn;       /* Leave some guard bits, but come back some */
            v += t;           /* The filter output */
            Ho += dhb;        /* IR step */
            Xp += 1;      /* Input signal step. NO CHECK ON BOUNDS */
        }

        v >>= Nhg;              /* Make guard bits */
        v *= LpScl;             /* Normalize for unity filter gain */

        v += (1<<(NLpScl-1));
        v >>= NLpScl;
        if(v>MAX_HWORD)
            v = MAX_HWORD;
        else if(v<MIN_HWORD)
            v = MIN_HWORD;
        v=(v*9394)>>13;             //resample alg have -1.892db attenuation.

        //*Y++ =  v & (0x00ffffff) | ((1)<<31);
        //*Y++ =  v & (0x00ffffff) ;
#if 1
        //*Y = (HWORD)v;
        //*Y++ |=((ch&1)<<31);
        *Y++ = (HWORD)v;
#endif
        *Time += dtb;           /* Move to next sample by time increment */
    }
    return (Y - Ystart);        /* Return the number of output samples */
}

int resample_Init(PassSRC *pPassSRC,char* pGlobal)
{
    int i;
    ParameterResample *pParameterResample = (ParameterResample *)pGlobal;
    UWORD InvFactor;
    pPassSRC->pSRCHandler = pParameterResample;
    /* Set defaults */
#ifdef USE_LARGEFILTER
    pParameterResample->Nmult = LARGE_FILTER_NMULT;
    pParameterResample->Imp = LARGE_FILTER_IMP;         /* Impulse response */
    pParameterResample->ImpD = LARGE_FILTER_IMPD;       /* Impulse response deltas */
    pParameterResample->LpScl = LARGE_FILTER_SCALE;     /* Unity-gain scale factor */
    pParameterResample->Nwing = LARGE_FILTER_NWING;     /* Filter table length */
#else
    pParameterResample->Nmult = SMALL_FILTER_NMULT;
    pParameterResample->Imp = SMALL_FILTER_IMP;         /* Impulse response */
    pParameterResample->ImpD = SMALL_FILTER_IMPD;       /* Impulse response deltas */
    pParameterResample->LpScl = SMALL_FILTER_SCALE;     /* Unity-gain scale factor */
    pParameterResample->Nwing = SMALL_FILTER_NWING;     /* Filter table length */
#endif

#if DEBUG
    fprintf(stderr,"Attenuating resampler scale factor by 0.95 "
            "to reduce probability of clipping\n");
#endif
//    pParameterResample->LpScl *= 0.95;

    pParameterResample->factor = (UWORD)(((UINT64)pPassSRC->nOutputSampleRate<<24)/pPassSRC->nInputSampleRate);

    InvFactor = (UWORD)(((UINT64)pPassSRC->nInputSampleRate<<24)/pPassSRC->nOutputSampleRate);

    pParameterResample->dtb = (InvFactor+(1<<(23-Np)))>>(24-Np);     /* Fixed-point representation */
    pParameterResample->dhb = MIN((1<<Np), (pParameterResample->factor+(1<<(23-Np)))>>(24-Np));

    /* Account for increased filter gain when using factors less than 1 */
    if (pParameterResample->factor < 0x1000000)
        pParameterResample->LpScl = (UHWORD)((((UINT64)pParameterResample->LpScl*pParameterResample->factor) + 0x800000)>>24);

    /* Calc reach of LP filter wing & give some creeping room */
    pParameterResample->Xoff = (UHWORD)(((UINT64)((pParameterResample->Nmult+1)>>1) * MAX(0x1000000,InvFactor))>>24) + 10;

    pParameterResample->Time = (((UWORD)(pParameterResample->Xoff))<<Np);
    pParameterResample->Xread = (pParameterResample->Xoff<<1);
    for (i=0; i<(pParameterResample->Xread); pParameterResample->X1[i++]=0); /* Need Xoff zeros at begining of sample */
printf("factor=0x%x,InvFactor=0x%x,dtb=0x%x, dhb=0x%x,LpScl=0x%x,Xoff=0x%x,Time=0x%x,Xread=0x%x\n",
 pParameterResample->factor,
 InvFactor,
  pParameterResample->dtb,
  pParameterResample->dhb,
   pParameterResample->LpScl,
   pParameterResample->Xoff,
   pParameterResample->Time,
    pParameterResample->Xread 
);

    if (IBUFFSIZE < (2*pParameterResample->Xoff+pPassSRC->nInputbufferszie))
        return (-1);

    return(sizeof(ParameterResample));
}

void resample_UpdateOutSampRate(PassSRC *pPassSRC,char* pGlobal)
{
    int i;
    ParameterResample *pParameterResample = (ParameterResample *)pGlobal;
    UWORD InvFactor;
    pParameterResample->LpScl = SMALL_FILTER_SCALE;     /* Unity-gain scale factor */
    pParameterResample->factor = (UWORD)(((UINT64)pPassSRC->nOutputSampleRate<<24)/pPassSRC->nInputSampleRate);

    InvFactor = (UWORD)(((UINT64)pPassSRC->nInputSampleRate<<24)/pPassSRC->nOutputSampleRate);//24501725 24502424 24503473 24508716 24536678

    pParameterResample->dtb = (InvFactor+(1<<(23-Np)))>>(24-Np); //(24501725+256)>>9 =47855 47856 47858 47869 47923  /* Fixed-point representation Np=(Nhc+Na)=15 */
    pParameterResample->dhb = MIN((1<<Np), (pParameterResample->factor+(1<<(23-Np)))>>(24-Np));

    /* Account for increased filter gain when using factors less than 1 */
    if (pParameterResample->factor < 0x1000000)
        pParameterResample->LpScl = (UHWORD)((((UINT64)pParameterResample->LpScl*pParameterResample->factor) + 0x800000)>>24);

    /* Calc reach of LP filter wing & give some creeping room */
    pParameterResample->Xoff = (UHWORD)(((UINT64)((pParameterResample->Nmult+1)>>1) * MAX(0x1000000,InvFactor))>>24) + 10;

    //pParameterResample->Time = (((UWORD)(pParameterResample->Xoff))<<Np);
    pParameterResample->Xread = (pParameterResample->Xoff<<1);
//    for (i=0; i<(pParameterResample->Xread); pParameterResample->X1[i++]=0); /* Need Xoff zeros at begining of sample */

//    if (IBUFFSIZE < (2*pParameterResample->Xoff+pPassSRC->nInputbufferszie))
    //return (-1);

//  return(sizeof(ParameterResample));
}


void resample_Process(PassSRC *pPassSRC,char* pInBuf, char* pOutBuf,int ch)
{
    ParameterResample *pParameterResample = (ParameterResample *)pPassSRC->pSRCHandler;
    UWORD *Time = &(pParameterResample->Time);          /* Current time/pos in input sample */
//  static BOOL IsFirst = 1;
    HWORD *pIn = (HWORD *)pInBuf, *Y1 = (HWORD *)pOutBuf, *pX1; /* I/O buffers */
    UHWORD Nout, Nx, Xread, Xoff;
    int i;

    Nx = pPassSRC->nInputbufferszie;
    pX1 = pParameterResample->X1;
    Xread = pParameterResample->Xread;
    Xoff = pParameterResample->Xoff;

    for(i=0; i<Nx; i++)
        pX1[i+Xread] = pIn[i];

    /* Resample stuff in input buffer */
    if (pParameterResample->factor >= 0x1000000) {      /* SrcUp() is faster if we can use it */
        Nout=SrcUp(pX1,Y1,pParameterResample->dtb,Time,Nx+Xoff,pParameterResample->Nwing,pParameterResample->LpScl,pParameterResample->Imp,pParameterResample->ImpD,ch);
    } else {
        Nout=SrcUD(pX1,Y1,pParameterResample->dhb, pParameterResample->dtb, Time,Nx+Xoff,pParameterResample->Nwing,pParameterResample->LpScl,pParameterResample->Imp,pParameterResample->ImpD,ch);
    }

    *Time -= (Nx<<Np);       /* Move converter Nx samples back in time */

    for (i=0; i<Xread; i++) { /* Copy part of input signal */
        pX1[i] = pX1[i+Nx]; /* that must be re-used */
    }
    pPassSRC->nOutputbuffersize = Nout;

}
