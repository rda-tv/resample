
#include "dev_app_resample.h"

#define SAMPLE_RATE_CONVERTION  8

static UINT32  g_Nx;

static INT16 g_OldSamplel=0;
static INT16 g_OldSampler=0;

static INT32 g_dtb = 0;
static INT16 g_last_sample_l = 0;
static INT16 g_last_sample_r = 0;

BOOL dev_app_init(VOID)
{
    g_dtb = 0;
    g_last_sample_l = 0;
    g_last_sample_r = 0;
    return TRUE;
}

BOOL dev_app_exit(VOID)
{
    return TRUE;
}

BOOL dev_app_info(VOID)
{
#if(DEV_APP_DEF_INFO_ENABLE==TRUE)
    DEV_APP_EVENT(0x0000);
#endif

    return TRUE;
}

BOOL dev_app_open(DEV_APP_ID_ENUM id)
{
    if(id >= DEV_APP_ID_NUM) {
        return FALSE;
    }

    return TRUE;
}

BOOL dev_app_close(DEV_APP_ID_ENUM id)
{
    if(id >= DEV_APP_ID_NUM) {
        return FALSE;
    }

    return TRUE;
}



VOID dev_app_adjust_ratio(INT8 delta)
{
    g_dtb += delta;
}

INT32 dev_app_get_ratio(VOID)
{
    return g_dtb;
}


#define TRACE_ERR(c)
#define MAX_CHAN_NUM  1

BOOL initM[MAX_CHAN_NUM];
ParameterResample ResampParamM[MAX_CHAN_NUM];
INT32 inSampRateM;
INT32 outSampRateM;

ParameterResample pGlobal;
void dev_app_clear_initm()
{
    int i;
    for(i = 0; i<MAX_CHAN_NUM; i++)
        initM[i] = 0;

}
BOOL dev_app_updateinsamprate(PassSRC **pPassSrcM,int inLen, int sampRate)
{
    int i;

    for (i = 0; i < MAX_CHAN_NUM; i++) {
        pPassSrcM[i]->nInputbufferszie = inLen >> 1; //sample num

        if (!initM[i]) {
            pPassSrcM[i]->nInputSampleRate = sampRate;
            // printf("nInputSampleRate= %d,nOutputSampleRate = %d\n",pPassSrcM[i]->nInputSampleRate,pPassSrcM[i]->nOutputSampleRate);
            if (-1 == resample_Init(pPassSrcM[i],(char *) &ResampParamM[i])) {
                TRACE_ERR("resample init failed!\n");
                return FALSE;
            }
            initM[i] = TRUE;
        } else {
            if (sampRate != pPassSrcM[i]->nInputSampleRate) {
                pPassSrcM[i]->nInputSampleRate = sampRate;
                resample_UpdateOutSampRate(pPassSrcM[i],(char *) &ResampParamM[i]);
            }
            // printf("sampRate != pPassSrcM[i]->nInputSampleRate\n");
        }
    }

    inSampRateM = sampRate;
    return TRUE;
}

VOID dev_resample_UpdateOutSampRate(PassSRC **pPassSrcM,int sampRatedtb)
{
#if 1
    int i;
    inSampRateM += sampRatedtb;

    for (i = 0; i < MAX_CHAN_NUM; i++) {
        if (initM[i]) {
            if (inSampRateM != pPassSrcM[i]->nInputSampleRate) {
                pPassSrcM[i]->nInputSampleRate = inSampRateM;
                resample_UpdateOutSampRate(pPassSrcM[i], (char *)&ResampParamM[i]);
            }
        }
    }
#endif
}

int dev_app_process(PassSRC **pPassSrcM, INT16 ** const pIn, UINT16 ** const pOut, int const channel)
{

    int i;
    if (channel == MAX_CHAN_NUM) {


        for (i = 0; i < MAX_CHAN_NUM; i++) {

            resample_Process(pPassSrcM[i], (char *)pIn[i], (char *)pOut[i],i);
        }

    }

    return pPassSrcM[0]->nOutputbuffersize;
}



