// resample.cpp : Defines the entry point for the console application.
//

//#include "cs_types.h"
#include "dev_app_resample.h"
#include "stdio.h"
INT16 *in_data[2];
INT16 *out_data[2];
PassSRC *pPassSrcM[2];
PassSRC PassSrcM[2];

#define APP_INPUT_BUFF_SIZE  (0x1000)
#define APP_OUTPUT_BUFF_SIZE (APP_INPUT_BUFF_SIZE*8)
#define OUT_SAMPLE_RATE     (48000)
#define OUT_SAMPLE_RATE_96k     (96000)
#define OUT_SAMPLE_RATE_44_1k     (44100)

#define IN_SAMPLE_RATE      (48000)
#define IN_SAMPLE_RATE_8k      (8000)
#define IN_SAMPLE_RATE_16k      (16000)
#define IN_SAMPLE_RATE_441      (44100)
#define IN_SAMPLE_RATE_2205      (22050)
#define IN_SAMPLE_RATE_96      (96000)
#define IN_SAMPLE_RATE_70100      (70100)
#define TEST (0)
//UINT16 av_ifc_buff_in[APP_INPUT_BUFF_SIZE];//0x4000 0x2000
//UINT32 av_ifc_buff_out[APP_OUTPUT_BUFF_SIZE];

UINT16 audio_process_buff_left[APP_OUTPUT_BUFF_SIZE];
UINT16 audio_process_buff_right[APP_OUTPUT_BUFF_SIZE];

INT16 app_in_buff_left[APP_INPUT_BUFF_SIZE];//0x1000
INT16 app_in_buff_right[APP_INPUT_BUFF_SIZE];//0x1000
BOOL dev_app_updateinsamprate(PassSRC **pPassSrcM,int inLen, int sampRate);
int dev_app_process(PassSRC **pPassSrcM, INT16 ** const pIn, UINT16 ** const pOut, int const channel);
void dev_app_clear_initm();
int main(void)
{
	int cnt,n;
	FILE *fp_in,*fp_out,*fp_test;
	UINT32 valid_sample_num;
	UINT32 in_resample=IN_SAMPLE_RATE;//IN_SAMPLE_RATE_8k IN_SAMPLE_RATE_16k
	in_data[0] = app_in_buff_left;
	in_data[1] = app_in_buff_right;
	out_data[0] =(INT16 *)audio_process_buff_left;
	out_data[1] =(INT16 *)audio_process_buff_right;
	for(cnt = 0 ; cnt<2 ; cnt++) 
	{
            PassSrcM[cnt].nOutputSampleRate = OUT_SAMPLE_RATE ;//OUT_SAMPLE_RATE_96k OUT_SAMPLE_RATE OUT_SAMPLE_RATE_44_1k
            pPassSrcM[cnt] = &PassSrcM[cnt];
    }
#if TEST
	fp_test  = fopen("test.bin","wb");//48_1K_16bit.bin 44_1_1K_16bit.bin
	if (fp_test == NULL)
	{
		printf("cccc\n");
		return -1;
	}
	fwrite("abcd",4,1,fp_test);
	fclose(fp_test);
#endif
	fp_in  = fopen("48_1K_16bit.bin","rb");//48_1K_16bit.bin 44_1_1k_16bit.bin 96_1k_16bit.bin resampe_test_8K.bin 16k_16bit.bin Aa22050-l-channel.pcm
	if (fp_in == NULL)
	{
		printf("aaaaab\n");
		return -1;
	}
	fp_out  = fopen("48_1K_16bit_out.bin","wb");//48_1K_16bit_out.bin 44_1K_16bit_out.bin 96_1K_16bit_out.bin 
	if (fp_out == NULL)
	{	
		printf("ddddd\n");
		return -2;
	}
	 //LINEIN
	dev_app_clear_initm();
	printf("%d,%d\n",sizeof(app_in_buff_left),sizeof(app_in_buff_left)/2);
    dev_app_updateinsamprate(pPassSrcM, sizeof(app_in_buff_left), in_resample); //in_stream.sample_rate
	while(1)
	{
		n = fread(&app_in_buff_left,2,APP_INPUT_BUFF_SIZE,fp_in);
		printf("read n = %d\n",n);
		if (n != APP_INPUT_BUFF_SIZE)
		{
			printf("finished \n");
			break;
		}
		printf("00000\n");
		valid_sample_num=dev_app_process(pPassSrcM, in_data,(UINT16 **)out_data, 1); 
		printf("valid_sample_num =%d\n",valid_sample_num);
		printf("xxxxx\n");
		fwrite(audio_process_buff_left,2,valid_sample_num,fp_out);

		printf("%d\n",valid_sample_num);
		
	}
	//dev_app_clear_initm();
    dev_app_updateinsamprate(pPassSrcM, n*2, in_resample); //in_stream.sample_rate
    valid_sample_num=dev_app_process(pPassSrcM, in_data,(UINT16 **)out_data, 1); 
	fwrite(audio_process_buff_left,2,valid_sample_num,fp_out);
	printf("%d\n",valid_sample_num);
	fclose(fp_in);
	fclose(fp_out);

	printf("world!\n");

	return 0;

}

