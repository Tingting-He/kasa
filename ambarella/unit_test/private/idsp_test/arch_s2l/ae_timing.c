#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "unistd.h"
#include "iav_ioctl.h"
#include "ambas_imgproc_arch.h"
#include "AmbaDSP_Img3aStatistics.h"
#include "AmbaDSP_ImgUtility.h"

#include "img_adv_struct_arch.h"
#include "img_api_adv_arch.h"

#include "time.h"
#include <sched.h>
#include"math.h"
#include <sys/ioctl.h>
#include<pthread.h>
/////////////////////////////////////////////////////////////////////
//used to check ae timing when flash bug happened in linear mode
////////////////////////////////////////////////////////////////////
extern int hal_get_vin_tick();
extern int hal_init_vin_tick();
extern int get_tick(void);

static amba_img_dsp_mode_cfg_t ik_mode;
static amba_dsp_aaa_statistic_data_t g_stat;
static img_aaa_stat_t aaa_stat_info[MAX_HDR_EXPOSURE_NUM];
static amba_img_dsp_wb_gain_t awb_gain[MAX_HDR_EXPOSURE_NUM];

static struct rgb_aaa_stat rgb_stat[4];
static struct cfa_aaa_stat cfa_stat[4];
static struct cfa_pre_hdr_stat hdr_cfa_pre_stat[MAX_PRE_HDR_STAT_BLK_NUM];
static u8 rgb_stat_valid;
static u8 cfa_stat_valid;
static u8 hdr_cfa_pre_valid;
static pthread_cond_t stat_done_cond =PTHREAD_COND_INITIALIZER;
static pthread_mutex_t stat_mutex =PTHREAD_MUTEX_INITIALIZER;
static aaa_tile_report_t act_tile;

static u8 vin_id_exit_flag =0;
static u8 exit_flag =0;
static u32 frame_n =0;
static u32 luma_array[100];
static u32 rgb_luma_array[100];
static int t_vdsp[100];
static int t_vin[100];
static struct vindev_aaa_info vin_aaa_info;
static struct vindev_fps active_fps;

extern int  parse_aaa_data(amba_dsp_aaa_statistic_data_t * p_data, hdr_pipeline_t hdr_pipe,
	img_aaa_stat_t * p_stat,aaa_tile_report_t * p_act_tile);
static void vin_loop(void* arg)
{
	printf("vin_loop\n");
	int fd_iav;
	fd_iav = (int)arg;

	while(!vin_id_exit_flag)
	{
	//	hal_get_vin_tick();
		pthread_cond_wait (&stat_done_cond, &stat_mutex);

		if(frame_n ==10)
		{
			img_set_sensor_shutter(fd_iav, 600);
			printf("t=%d,set shutter_row =600\n",get_tick());
		}
		else if(frame_n ==20)
		{
			img_set_sensor_shutter(fd_iav, 1200);
			printf("t=%d,set shutter_row =1200\n",get_tick());
		}
		else if(frame_n ==30)
		{
			img_set_sensor_agc(fd_iav, 0);
			printf("t=%d,set agc =0db\n",get_tick());
		}
		else if(frame_n ==40)
		{
			img_set_sensor_agc(fd_iav, (6<<24)/vin_aaa_info.agc_step);
			printf("t=%d,set agc =6db\n",get_tick());
		}
		else if(frame_n ==50)
		{
			awb_gain[0].AeGain =4096;
			amba_img_dsp_set_wb_gain(fd_iav,&ik_mode,awb_gain);
			printf("t=%d,set dgain =4096\n",get_tick());
		}
		else if(frame_n ==60)
		{
		//	awb_gain[0].AeGain = 16383;
		//	amba_img_dsp_set_wb_gain(fd_iav,&ik_mode,awb_gain);
		//	printf("t=%d,set dgain =16383\n",get_tick());
		}
		else if(frame_n ==70)
			vin_id_exit_flag =1;
		t_vin[++frame_n]  =get_tick();
		printf("frame_n =%d\n",frame_n);
	}
	printf("exit vin_loop\n");
	return;
}
static void get_luma_stat(int fd_iav,u32 *luma,u32 *rgb_luma)
{
	memset(rgb_stat, 0, sizeof(struct rgb_aaa_stat)*4);
	memset(cfa_stat, 0, sizeof(struct cfa_aaa_stat)*4);
	memset(hdr_cfa_pre_stat, 0, sizeof(struct cfa_pre_hdr_stat) * MAX_HDR_EXPOSURE_NUM);
	rgb_stat_valid = 0;
	cfa_stat_valid = 0;
	hdr_cfa_pre_valid = 0;
	memset(&g_stat, 0, sizeof(amba_dsp_aaa_statistic_data_t));
	g_stat.CfaAaaDataAddr = (u32)cfa_stat;
	g_stat.RgbAaaDataAddr = (u32)rgb_stat;
	g_stat.CfaPreHdrDataAddr = (u32)hdr_cfa_pre_stat;
	g_stat.CfaDataValid = (u32)&cfa_stat_valid;
	g_stat.RgbDataValid = (u32)&rgb_stat_valid;
	g_stat.CfaPreHdrDataValid = (u32)&rgb_stat_valid;
	if(amba_img_dsp_3a_get_aaa_stat(fd_iav,&ik_mode,&g_stat)<0){
		printf("amba_img_dsp_3a_get_aaa_stat fail\n");
		return;
	}
	if(parse_aaa_data(&g_stat, HDR_PIPELINE_OFF, aaa_stat_info, &act_tile)<0){
		printf("parse_aaa_data fail\n");
		return;
	}

	int tile_num =aaa_stat_info->tile_info.ae_tile_num_col*aaa_stat_info->tile_info.ae_tile_num_row;
	int i=0;
	u32 luma_sum =0,rgb_luma_sum=0;
	for(i = 0; i < tile_num; i++) {
		luma_sum += aaa_stat_info->ae_info[i].lin_y;
		rgb_luma_sum += aaa_stat_info->ae_info[i].non_lin_y;
	}
	*luma=luma_sum/tile_num;
	*rgb_luma =rgb_luma_sum /tile_num;
	}
static void calc_result(u32* luma_arr,u32* rgb_luma_arr,u8 type)// type=1,0
{
	int i =0;
	int shutter_delay =0;
	int agc_delay =0;
	int dgain_delay =0;
	int dgain_delay_base =type?2:1;
	printf("\n**************************************\n");
	printf("type =%d\n",type);//type =1: vdsp in VB
	for(i=1;i<4;i++)
	{
		u32 index,base,rgb_base;
		int tmp =i*20;
		base =luma_arr[tmp];
		rgb_base=rgb_luma_arr[tmp];
		for(index =i*20+1;index<i*20+10;index++)
		{
			if((luma_arr[index]>(base*3/2))||(abs(rgb_luma_arr[index]-rgb_base)>20))
			{
				if(i ==1){
					shutter_delay=(index -(i*20+1)-type);
					printf("shutter timing	=======>N+%d......\n",index -(i*20+1)-type);
				}
				else if(i ==2){
					agc_delay =(index -(i*20+1)-type);
					printf("agc timing	=======>N+%d......\n",index -(i*20+1)-type);
				}
				else if(i ==3){
					dgain_delay =(index -i*20-1);
					printf("dgain timing	=======>N+%d......\n",index -i*20-1);
				}
			//	printf("i=%d,index=%d,luma_arr[index] =%d,base =%d\n",i,index,luma_arr[index],base);
				break;
			}
		}
	}

	printf("pls modify agc_delay and shutter_delay in ambarella\\prebuild\\img_data\\xxx_aeb_param.c\n\n");
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("@	shutter_delay=%d		@\n",shutter_delay-dgain_delay+dgain_delay_base);
	printf("@	agc_delay=%d		@\n",agc_delay-dgain_delay+dgain_delay_base);
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("\n**************************************\n");
	printf("\n**************************************\n");
	printf("\n**************************************\n");

}
static int kbhit(void)
{
	fd_set rfds;
	struct timeval tv;
	int retval;
	FD_ZERO(&rfds);
	FD_SET(0,&rfds);

	tv.tv_sec=1;
	tv.tv_usec=0;
	retval = select(1, &rfds, NULL, NULL, &tv);
	if (retval == -1) {
		perror("select()");
		return -1;
	}
	else if (retval)
	{
		return 1;
	}
	return 0;
}

int calc_ae_timing(int fd_iav)
{
	printf("waiting 3\n");
	usleep(1000*1000);
	printf("waiting 2\n");
	usleep(1000*1000);
	printf("waiting 1\n");
	usleep(1000*1000);
	img_enable_ae(0);
	img_enable_awb(0);
	printf("ae,awb is disable now!\n\n");

	memset(&vin_aaa_info, 0, sizeof(vin_aaa_info));
	vin_aaa_info.vsrc_id = 0;
	if (ioctl(fd_iav, IAV_IOC_VIN_GET_AAAINFO, &vin_aaa_info) < 0) {
		perror("IAV_IOC_VIN_GET_AAAINFO error\n");
		return -1;
	}
	memset(&active_fps, 0, sizeof(active_fps));
	active_fps.vsrc_id = 0;
	if (ioctl(fd_iav, IAV_IOC_VIN_GET_FPS, &active_fps) < 0) {
		perror("IAV_IOC_VIN_GET_FPS");
		return -1;
	}

	pthread_t vin_id;
	img_set_sensor_agc(fd_iav, 0);
	img_set_sensor_shutter(fd_iav, 1000);
	amba_img_dsp_get_wb_gain(&ik_mode,awb_gain);
	if(awb_gain[0].GainR ==0||awb_gain[0].GainG==0||awb_gain[0].GainB==0){
		awb_gain[0].GainR =4096;
		awb_gain[0].GainG =4096;
		awb_gain[0].GainB =4096;
	}
	awb_gain[0].AeGain =4096;
	awb_gain[0].GlobalDGain =4096;
	amba_img_dsp_set_wb_gain(fd_iav,&ik_mode,awb_gain);

	while(1)
	{
		u32 curr_luma =0,curr_rgb_luma =0;
		get_luma_stat(fd_iav,&curr_luma,&curr_rgb_luma);
		printf("current luma=%d, pls make luma =800--->1200.press 'g' key to continue\n",curr_luma);
		if(kbhit()){
			char key =getchar();
			if(key =='g')
				break;
		}
	}
	printf("\n\n\n >>>>>>>ae timing test start now!!!<<<<<\n\n");

	if(pthread_create(&vin_id, NULL, (void*)vin_loop, (void*)fd_iav)!=0)
		printf("thread create failed!\n");
	while(!exit_flag)
	{
		u32 luma =0,rgb_luma =0;
		get_luma_stat(fd_iav,&luma,&rgb_luma);
		luma_array[frame_n] =luma;
		rgb_luma_array[frame_n] =rgb_luma;
		int t =get_tick();
		printf("t=%d, frame =%d,luma =%d, rgb =%d\n\n",t,frame_n,luma,rgb_luma);
		t_vdsp[frame_n] =t;

		if(frame_n ==60)
		{
			awb_gain[0].AeGain = 16383;
			amba_img_dsp_set_wb_gain(fd_iav,&ik_mode,awb_gain);
			printf("t=%d,set dgain =16383\n",get_tick());
		}

		if(pthread_cond_signal (&stat_done_cond)) {
			printf("cond error\n");
		}
		if(frame_n ==70)
			exit_flag =1;
	}
	u8 type =0;
	u64 frame_time=(u64)1000000*512000000/active_fps.fps;
	u32 vb_time=vin_aaa_info.vb_time/1000;
	u32 offset =3*1000000;//fix me
	u32 t_threshold =frame_time -vb_time -offset;//us
	u32 delta_t =((t_vdsp[11] -t_vin[11])+(t_vdsp[21] -t_vin[21])+(t_vdsp[31] -t_vin[31])+(t_vdsp[41] -t_vin[41]))/4;
	if(delta_t*1000000<t_threshold/*ms*/)
		type =1;

	printf("frame %lld,vb %d,threshold %d,delta %d\n",frame_time,vb_time,t_threshold,delta_t*1000000);
	calc_result(luma_array,rgb_luma_array,type);
	printf("\n\n\n >>>>>>>ae timing test done!!!<<<<<\n\n");


	return 0;

}
