#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include <sched.h>
#include <signal.h>

#include "img_abs_filter.h"
#include "img_adv_struct_arch.h"

#include "iav_ioctl.h"
#include "test_tuning.h"
#include "load_param.c"
//#include "AmbaTUNE_HdlrManager.h"
//static int cfg_id_backup =0;
static int fd_iav;
#define	IMGPROC_PARAM_PATH	"/etc/idsp"
static amba_img_dsp_anti_aliasing_t        AntiAliasing;
static BLC_PKG blc_pkg;
static amba_img_dsp_color_correction_reg_t	color_corr_reg;
static amba_img_dsp_color_correction_t color_corr;
static EC_INFO ec_info;
static AEB_SETTING_INFO aeb_setting_info;
static SA_ASF_PKG sa_asf_pkg;
static SHARPEN_PKG sharpen_pkg;
static amba_img_dsp_wb_gain_t wb_gain={4096,4096,4096,4096,4096};
static amba_img_dsp_wb_gain_t tmp_wb_gain;
static amba_img_dsp_chroma_median_filter_t chroma_median_setup;
static amba_img_dsp_video_mctf_info_t mctf_info;
static MCTF_PKG mctf_pkg;
static amba_img_dsp_rgb_to_yuv_t rgb2yuv_matrix;
static amba_img_dsp_dgain_saturation_t  d_gain_satuation_level;
static amba_img_dsp_dbp_correction_t dbp_correction_setting;
static amba_img_dsp_cfa_noise_filter_t cfa_noise_filter;
static amba_img_dsp_chroma_filter_t chroma_noise_filter;
static amba_img_dsp_chroma_filter_t wide_chroma_noise_filter;
static AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s wide_chroma_noise_filter_combine;
static amba_img_dsp_demosaic_t demosaic_info;
static amba_img_dsp_gbgr_mismatch_t mismatch_gbgr;
static amba_img_dsp_cdnr_info_t cdnr_info;
static amba_img_dsp_cfa_leakage_filter_t cfa_leakage_filter;
static amba_img_dsp_chroma_scale_t cs;
static aaa_cntl_t aaa_cntl_station;
static amba_img_dsp_mode_cfg_t ik_mode;
static img_config_info_t img_config;
static hdr_proc_data_t hdr_proc_data;
static BATCH_CMD_PKG batch_cmd_pkg;
static hdr_sensor_gp_t p_sht_agc_gp;
static u8 cc_reg[AMBA_DSP_IMG_CC_REG_SIZE];
static u8 cc_matrix[AMBA_DSP_IMG_CC_3D_SIZE];
static int debug_step, debug_mode;
//static int debug_frame;
static u8 debug_enble =0;
//static TILE_PKG tile_pkg;
static lens_cali_t lens_cali_info = {
	NORMAL_RUN,
	{92},
};

#include <pthread.h>
#include "idsp_netlink.h"

pthread_t nl_thread;
extern int  parse_aaa_data(amba_dsp_aaa_statistic_data_t * p_data, hdr_pipeline_t hdr_pipe,
						img_aaa_stat_t * p_stat,aaa_tile_report_t * p_act_tile);
extern int calc_ae_timing(int fd_iav);
static int receive_msg(int sock_fd,u8* buff,int size)
{
	int length =0;
	while(length < size)
	{
		int retv=0;
	 	retv=recv(sock_fd,buff+length,size - length,0);
		if(retv<=0)
		{
			if (retv == 0)
			{
				printf("Port  closed\n");
				return -2;
			}
			printf("recv() returns %d\n", retv);
			return -1;
		}
		length+=retv;
	//	printf("length %d , size %d\n",length,size);
	}
	return length;
}
#if 0
static int send_msg(int sock_fd,u8* buff,int size)
{
	int length =0;
	while(length < size)
	{
		int retv=0;
	 	retv=send(sock_fd,buff+length,size - length,MSG_NOSIGNAL);
		if(retv<=0)
		{
			if (retv == 0)
			{
				printf("Port  closed\n");
				return -2;
			}
			printf("send() returns %d\n", retv);
			return -1;
		}
		length+=retv;
		printf("length =%d\n",length);
	}
	return length;
}
#endif
static int send_msg_ex(int sock_fd,u8* buff,int size)
{
	int length =0;
	int send_size =0;
	while(length < size)
	{
		int remain =size -length;
		if(remain>2000)
			send_size =2000;
		else
			send_size =remain;
		int retv=0;
	 	retv=send(sock_fd,buff+length,send_size,MSG_NOSIGNAL);
		if(retv<=0)
		{
			if (retv == 0)
			{
				printf("Port  closed\n");
				return -2;
			}
			printf("send() returns %d\n", retv);
			return -1;
		}
		length+=retv;
	//	printf("length =%d\n",length);
	}
	return length;
}
static int SockServer_Setup(int sockfd,int port)
{
	int on =2048;
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket");
		return -1;
	}
	setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR,(char*) &on, sizeof(int) );

	struct sockaddr_in  my_addr;
	my_addr.sin_family=AF_INET;
	my_addr.sin_port=htons(port);
	my_addr.sin_addr.s_addr=INADDR_ANY;
	bzero(&(my_addr.sin_zero),0);

	if(bind(sockfd,(struct sockaddr*)&my_addr,sizeof(struct sockaddr))==-1)
	{
		perror("bind");
		return -1;
	}
	if(listen(sockfd,10)==-1)
	{
		perror("listen");
		return -1;
	}
	return sockfd;

}
static int SockServer_free(int sock_fd,int client_fd)
{
	if(sock_fd!=-1)
	{
		close(sock_fd);
		sock_fd =-1;
	}
	if(client_fd!=-1)
	{
		close(client_fd);
		client_fd =-1;
	}
	return 0;
}

static int init_ik_mode(img_config_info_t *p_img_cfg,
	amba_img_dsp_mode_cfg_t* p_ik_mode, struct iav_enc_mode_cap * cap)
{
	amba_img_dsp_variable_range_t dsp_variable_range;

	memset(p_ik_mode, 0, sizeof(amba_img_dsp_mode_cfg_t));

	p_ik_mode->Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
	if (p_img_cfg->hdr_config.pipeline != HDR_PIPELINE_OFF) {
		p_ik_mode->FuncMode = AMBA_DSP_IMG_FUNC_MODE_HDR;
	}
	if (p_img_cfg->isp_pipeline == ISP_PIPELINE_LISO) {
		p_ik_mode->AlgoMode = AMBA_DSP_IMG_ALGO_MODE_FAST;
	} else if (p_img_cfg->isp_pipeline == ISP_PIPELINE_ADV_LISO||
		p_img_cfg->isp_pipeline == ISP_PIPELINE_MID_LISO||
		p_img_cfg->isp_pipeline == ISP_PIPELINE_B_LISO) {
		p_ik_mode->AlgoMode = AMBA_DSP_IMG_ALGO_MODE_LISO;
		p_ik_mode->BatchId = 0xff;
	}

	dsp_variable_range.max_chroma_radius = (1 << (5 + cap->max_chroma_radius));
	dsp_variable_range.max_wide_chroma_radius = (1 << (5 + cap->max_wide_chroma_radius));
	dsp_variable_range.inside_fpn_flag = 0;
	dsp_variable_range.wide_chroma_noise_filter_enable = cap->wcr_possible;
	amba_img_dsp_set_variable_range(p_ik_mode, &dsp_variable_range);

	return 0;
}
static void process_other(int sock_fd,TUNING_ID* p_tuning_id)
{
/*
	switch(p_tuning_id->tab_id)
	{
		case OnlineTuning:
		{
			switch(p_tuning_id->item_id)
			{
				case CHECK_IP:
				{
					char test[100] ="test_tuning";
					send(sock_fd,&test,strlen(test),0);
					printf("Available IP addess!\n");
					break;
				}
			}
		}
		break;
	}
*/
}
static void process_load(int sock_fd,TUNING_ID* p_tuning_id)
{

	printf("Load!\n");
	switch(p_tuning_id->tab_id)
	{
	case OnlineTuning:
	{
		switch(p_tuning_id->item_id)
		{
			case BlackLevelCorrection:
				amba_img_dsp_get_static_black_level(&ik_mode,&blc_pkg.blc_info);
				amba_img_dsp_get_deferred_black_level(&ik_mode,&blc_pkg.DefBlc);
				send(sock_fd,(char*)&blc_pkg,sizeof(BLC_PKG),0);
				printf("Black Level Correction!\n");
				break;

			case ColorCorrection:
				amba_img_dsp_get_color_correction_reg(&ik_mode,&color_corr_reg);
				amba_img_dsp_get_color_correction(&ik_mode,&color_corr);
				send_msg_ex(sock_fd,(u8*)color_corr_reg.RegSettingAddr,AMBA_DSP_IMG_CC_REG_SIZE);
				send_msg_ex(sock_fd,(u8*)color_corr.MatrixThreeDTableAddr,AMBA_DSP_IMG_CC_3D_SIZE);
				send(sock_fd,"!",1,0);
				printf("Color Correction\n");
				break;

			case ToneCurve:
				amba_img_dsp_get_tone_curve(&ik_mode,&tone_curve);
				send(sock_fd,(char*)&tone_curve,sizeof(amba_img_dsp_tone_curve_t),0);
				printf("Tone Curve!\n ");
				break;

			case RGBtoYUVMatrix:
				amba_img_dsp_get_rgb_to_yuv_matrix(&ik_mode,&rgb2yuv_matrix);
				send(sock_fd,(char*)&rgb2yuv_matrix,sizeof(amba_img_dsp_rgb_to_yuv_t),0);
				printf("RGB to YUV Matrix!\n");
				break;

			case WhiteBalanceGains:
				amba_img_dsp_get_wb_gain(&ik_mode,&wb_gain);
				send(sock_fd,(char*)&wb_gain,sizeof(amba_img_dsp_wb_gain_t),0);
				printf("White Balance Gains!\n");
				break;

			case DGainSaturaionLevel:
				amba_img_dsp_get_dgain_saturation_level(&ik_mode,&d_gain_satuation_level);
				send(sock_fd,(char*)&d_gain_satuation_level,sizeof(amba_img_dsp_dgain_saturation_t),0);
				printf("DGain Saturation Level!\n");
				break;

			case LocalExposure:
				amba_img_dsp_get_local_exposure(&ik_mode,&local_exposure);
				send(sock_fd,(char*)&local_exposure,sizeof(amba_img_dsp_local_exposure_t),0);
				printf("Local Exposure!\n");
				break;

			case ChromaScale:
				amba_img_dsp_get_chroma_scale(&ik_mode,&cs);
				send(sock_fd,(char*)&cs,sizeof(amba_img_dsp_chroma_scale_t),0);
				printf("Chroma Scale!\n");
				break;

			case FPNCorrection://not finished
				break;

			case BadPixelCorrection:
				amba_img_dsp_get_dynamic_bad_pixel_correction(&ik_mode,&dbp_correction_setting);
				send(sock_fd,(char*)&dbp_correction_setting,sizeof(amba_img_dsp_dbp_correction_t),0);
				printf("Bad Pixel Correction!\n");
				break;

			case CFALeakageFilter:
				amba_img_dsp_get_cfa_leakage_filter(&ik_mode,&cfa_leakage_filter);
				send(sock_fd,(char*)&cfa_leakage_filter,sizeof(amba_img_dsp_cfa_leakage_filter_t),0);
				printf("CFA Leakage Filter!\n");
				break;

			case AntiAliasingFilter:
				amba_img_dsp_get_anti_aliasing(&ik_mode,&AntiAliasing);
				send(sock_fd,(char*)&AntiAliasing,sizeof(amba_img_dsp_anti_aliasing_t),0);
				printf("Anti-Aliasing Filter!\n");
				break;

			case CFANoiseFilter:
				amba_img_dsp_get_cfa_noise_filter(&ik_mode,&cfa_noise_filter);
				send(sock_fd,(char*)&cfa_noise_filter,sizeof(amba_img_dsp_cfa_noise_filter_t),0);
				printf("CFA Noise Filter!\n");
				break;

			case ChromaMedianFiler:
				amba_img_dsp_get_chroma_median_filter(&ik_mode,&chroma_median_setup);
				send(sock_fd,(char*)&chroma_median_setup,sizeof(amba_img_dsp_chroma_median_filter_t),0);
				printf("Chroma Median Filer!\n");
				break;

			case SharpeningA_ASF:
				amba_img_dsp_get_luma_processing_mode(&ik_mode,&sa_asf_pkg.select_mode);
				amba_img_dsp_get_advance_spatial_filter(&ik_mode,&sa_asf_pkg.asf_info);
				amba_img_dsp_get_1st_sharpen_noise_both(&ik_mode,&sa_asf_pkg.sa_info.both);
				amba_img_dsp_get_1st_sharpen_noise_noise(&ik_mode,&sa_asf_pkg.sa_info.noise);
				amba_img_dsp_get_1st_sharpen_noise_sharpen_coring(&ik_mode,&sa_asf_pkg.sa_info.coring);
				amba_img_dsp_get_1st_sharpen_noise_sharpen_fir(&ik_mode,&sa_asf_pkg.sa_info.fir);
				amba_img_dsp_get_1st_sharpen_noise_sharpen_min_coring_result(&ik_mode,&sa_asf_pkg.sa_info.MinCoringResult);
				amba_img_dsp_get_1st_sharpen_noise_sharpen_coring_index_scale(&ik_mode,&sa_asf_pkg.sa_info.CoringIndexScale);
				amba_img_dsp_get_1st_sharpen_noise_sharpen_scale_coring(&ik_mode,&sa_asf_pkg.sa_info.ScaleCoring);
				send_msg_ex(sock_fd,(u8*)&sa_asf_pkg,sizeof(SA_ASF_PKG));
				send(sock_fd,"!",1,0);
				printf("SharpeningA_ASF!\n");
				break;

			case MCTFControl:
			        if(img_config.isp_pipeline ==ISP_PIPELINE_LISO){
        				amba_img_dsp_get_video_mctf(&ik_mode,&mctf_info);
        				send(sock_fd,(char*)&mctf_info,sizeof(amba_img_dsp_video_mctf_info_t),0);
				}else if(img_config.isp_pipeline ==ISP_PIPELINE_ADV_LISO||img_config.isp_pipeline ==ISP_PIPELINE_MID_LISO
				||img_config.isp_pipeline==ISP_PIPELINE_B_LISO){
                                    amba_img_dsp_get_video_mctf( &ik_mode, &mctf_pkg.mctf_info);
                                    amba_img_dsp_get_video_mctf_temporal_adjust( &ik_mode, &mctf_pkg.mctf_temporal_adjust);
                                    send(sock_fd,(char*)&mctf_pkg,sizeof(MCTF_PKG),0);
				}
				printf("MCTF Control!\n");
				break;

			case SharpeningBControl:
				amba_img_dsp_get_final_sharpen_noise_both(&ik_mode,&sharpen_pkg.both);
				amba_img_dsp_get_final_sharpen_noise_noise(&ik_mode,&sharpen_pkg.noise);
				amba_img_dsp_get_final_sharpen_noise_sharpen_coring(&ik_mode,&sharpen_pkg.coring);
				amba_img_dsp_get_final_sharpen_noise_sharpen_fir(&ik_mode,&sharpen_pkg.fir);
				amba_img_dsp_get_final_sharpen_noise_sharpen_min_coring_result(&ik_mode,&sharpen_pkg.MinCoringResult);
				amba_img_dsp_get_final_sharpen_noise_sharpen_coring_index_scale(&ik_mode,&sharpen_pkg.CoringIndexScale);
				amba_img_dsp_get_final_sharpen_noise_sharpen_scale_coring(&ik_mode,&sharpen_pkg.ScaleCoring);
				send(sock_fd,(char*)&sharpen_pkg,sizeof(SHARPEN_PKG),0);
				printf("SharpeningBControl!\n");
				break;

			case ColorDependentNoiseReduction:
				amba_img_dsp_get_color_dependent_noise_reduction(&ik_mode,&cdnr_info);
				send(sock_fd,(char*)&cdnr_info,sizeof(amba_img_dsp_cdnr_info_t),0);
				printf("Color Dependent Noise Reduction\n");
				break;

			case ChromaNoiseFilter:
				amba_img_dsp_get_chroma_filter(&ik_mode,&chroma_noise_filter);
				send(sock_fd,(char*)&chroma_noise_filter,sizeof(amba_img_dsp_chroma_filter_t),0);
				printf("Chroma Noise Filter!\n");
				break;
			case WideChromaFilter:
			        amba_img_dsp_get_wide_chroma_filter(&ik_mode,&wide_chroma_noise_filter);
			        send(sock_fd,(char*)&wide_chroma_noise_filter,sizeof(amba_img_dsp_chroma_filter_t),0);
			        printf("Wide Chroma Filter  !\n");
			       break;
			 case WideChromaFilterCombine:
			       amba_img_dsp_get_wide_chroma_filter_combine(&ik_mode,&wide_chroma_noise_filter_combine);
			       send(sock_fd,(char*)&wide_chroma_noise_filter_combine,sizeof(AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s),0);
                             printf("Wide Chroma Filter Combine!\n");

                             break;

			case TileConfiguration:
			//	AmbaDSP_Img3aGetAaaStatInfo( &ik_mode, &tile_pkg.stat_config_info);
			//	send(sock_fd,(char*)&tile_pkg,sizeof(TILE_PKG),0);
			//	printf("AF Tile Configuration!\n");
				break;

			case DEMOSAIC:
				amba_img_dsp_get_demosaic(&ik_mode, &demosaic_info);
				send(sock_fd,(char*)&demosaic_info,sizeof(amba_img_dsp_demosaic_t),0);
				printf("Demosaic!\n");
				break;

			case MisMatchGr_Gb:
				amba_img_dsp_get_gbgr_mismatch(&ik_mode, &mismatch_gbgr);
				send(sock_fd,(char*)&mismatch_gbgr,sizeof(amba_img_dsp_gbgr_mismatch_t),0);
				printf("MisMatchGr_Gb!\n");
				break;

			case ConfigAAAControl:
				img_get_3a_cntl_status(&aaa_cntl_station);
				send(sock_fd,(char*)&aaa_cntl_station,sizeof(aaa_cntl_t),0);
				printf(" Config AAA Control!\n");
				break;

			case AFStatisticSetupEx:
			//	img_dsp_get_af_statistics_ex(&af_statistic_setup_ex);
			//	send(sock_fd,(char*)&af_statistic_setup_ex,sizeof(af_statistics_ex_t),0);
			//	printf("AF Statistic Setup Ex!\n");
			//	break;

			case ExposureControl:
				ec_info.gain_tbl_idx=img_get_sensor_agc();
				ec_info.shutter_row=img_get_sensor_shutter();
				amba_img_dsp_get_wb_gain(&ik_mode ,&wb_gain);
				ec_info.dgain =wb_gain.AeGain;
				send(sock_fd,(char*)&ec_info,sizeof(EC_INFO),0);
				printf("Exposure Control!\n");
				break;
			case ImgConfigInfo:
				send(sock_fd,(char*)&img_config,sizeof(img_config_info_t),0);
				printf("img_config info\n");
				break;
			case AEBSetting:
			{
				img_get_ae_target_ratio(&aeb_setting_info.ae_target_ratio);
				img_get_wb_property_ratio(&aeb_setting_info.awb_r_ratio,&aeb_setting_info.awb_b_ratio);
				send(sock_fd,(char*)&aeb_setting_info,sizeof(AEB_SETTING_INFO),0);
				printf("AEB Setting\n");
				break;
			}
			case HDR_BATCH_CMD:
				img_get_hdr_alpha_value(&batch_cmd_pkg.alpha_value);
				img_get_hdr_threshold(&batch_cmd_pkg.threshold_value);
				img_get_hdr_batch(fd_iav,&hdr_proc_data);
				img_get_hdr_sensor_shutter_group(fd_iav,&p_sht_agc_gp.shutter_gp);
				img_get_hdr_sensor_agc_group(fd_iav,&p_sht_agc_gp.agc_gp);
				amba_img_dsp_get_wb_gain(&ik_mode,&wb_gain);
				batch_cmd_pkg.hdr_enable=img_get_hdr_enable();
				feed_batch_cmd_pkg(&batch_cmd_pkg,&hdr_proc_data,img_config.hdr_config.expo_num);
				memcpy(batch_cmd_pkg.shutter,&p_sht_agc_gp.shutter_gp,MAX_HDR_EXPOSURE_NUM*sizeof(u32));
				memcpy(batch_cmd_pkg.agc,&p_sht_agc_gp.agc_gp,MAX_HDR_EXPOSURE_NUM*sizeof(u32));
				batch_cmd_pkg.dgain =(wb_gain.AeGain*wb_gain.GlobalDGain)/4096;
				u16 ae_target;
				img_get_ae_target(&ae_target);
				batch_cmd_pkg.ae_target =ae_target;
				send(sock_fd,(char*)&batch_cmd_pkg,sizeof(BATCH_CMD_PKG),0);
				printf("HDR BATCH CMD!\n");
				break;

			case MoBadPixelCorrection:
				amba_img_dsp_get_mo_dynamic_bad_pixel_correction(&ik_mode,&dbp_correction_setting);
				send(sock_fd,(char*)&dbp_correction_setting,sizeof(amba_img_dsp_dbp_correction_t),0);
				printf("Motion Bad Pixel Correction!\n");
				break;

			case MoCFALeakageFilter:
				amba_img_dsp_get_mo_cfa_leakage_filter(&ik_mode,&cfa_leakage_filter);
				send(sock_fd,(char*)&cfa_leakage_filter,sizeof(amba_img_dsp_cfa_leakage_filter_t),0);
				printf("Motion CFA Leakage Filter!\n");
				break;

			case MoAntiAlisingFilter:
				amba_img_dsp_get_mo_anti_aliasing(&ik_mode,&AntiAliasing);
				send(sock_fd,(char*)&AntiAliasing,sizeof(amba_img_dsp_anti_aliasing_t),0);
				printf("Motion Anti-Aliasing Filter!\n");
				break;

			case MoCFANoiseFilter:
				amba_img_dsp_get_mo_cfa_noise_filter(&ik_mode,&cfa_noise_filter);
				send(sock_fd,(char*)&cfa_noise_filter,sizeof(amba_img_dsp_cfa_noise_filter_t),0);
				printf("Motion CFA Noise Filter!\n");
				break;

			case MoChromaMedianFilter:
				amba_img_dsp_get_mo_chroma_median_filter(&ik_mode,&chroma_median_setup);
				send(sock_fd,(char*)&chroma_median_setup,sizeof(amba_img_dsp_chroma_median_filter_t),0);
				printf("Motion Chroma Median Filer!\n");
				break;

			case MoSharpeningA_ASF:
				amba_img_dsp_get_mo_luma_processing_mode(&ik_mode,&sa_asf_pkg.select_mode);
				amba_img_dsp_get_mo_advance_spatial_filter(&ik_mode,&sa_asf_pkg.asf_info);
				amba_img_dsp_get_mo_sharpen_noise_both(&ik_mode,&sa_asf_pkg.sa_info.both);
				amba_img_dsp_get_mo_sharpen_noise_noise(&ik_mode,&sa_asf_pkg.sa_info.noise);
				amba_img_dsp_get_mo_sharpen_noise_sharpen_coring(&ik_mode,&sa_asf_pkg.sa_info.coring);
				amba_img_dsp_get_mo_sharpen_noise_sharpen_fir(&ik_mode,&sa_asf_pkg.sa_info.fir);
				amba_img_dsp_get_mo_sharpen_noise_sharpen_min_coring_result(&ik_mode,&sa_asf_pkg.sa_info.MinCoringResult);
				amba_img_dsp_get_mo_sharpen_noise_sharpen_coring_index_scale(&ik_mode,&sa_asf_pkg.sa_info.CoringIndexScale);
				amba_img_dsp_get_mo_sharpen_noise_sharpen_scale_coring(&ik_mode,&sa_asf_pkg.sa_info.ScaleCoring);
				send_msg_ex(sock_fd,(u8*)&sa_asf_pkg,sizeof(SA_ASF_PKG));
				send(sock_fd,"!",1,0);
				printf("Motion SharpeningA_ASF!\n");
				break;

			case MoChromaNoiseFilter:
				amba_img_dsp_get_mo_chroma_filter(&ik_mode,&chroma_noise_filter);
				send(sock_fd,(char*)&chroma_noise_filter,sizeof(amba_img_dsp_chroma_filter_t),0);
				printf("Motion Chroma Noise Filter!\n");
				break;

			case MoDemosaic:
					amba_img_dsp_get_mo_demosaic(&ik_mode, &demosaic_info);
					send(sock_fd,(char*)&demosaic_info,sizeof(amba_img_dsp_demosaic_t),0);
					printf("Motion Demosaic!\n");
					break;

			case MoMisMatchGr_Gb:
					amba_img_dsp_get_mo_gbgr_mismatch(&ik_mode, &mismatch_gbgr);
					send(sock_fd,(char*)&mismatch_gbgr,sizeof(amba_img_dsp_gbgr_mismatch_t),0);
					printf("Motion MisMatchGr_Gb!\n");
					break;
#ifdef ITUNER_TRANS
			case ITUNER_FILE:
				{
					#define MAX_AMGAGE_DATA_SIZE 40000
					static u8 ituner_stream[MAX_AMGAGE_DATA_SIZE];
					memset(ituner_stream,0,MAX_AMGAGE_DATA_SIZE);
					int size =AmbaTUNE_load_ituner_stream((char*)ituner_stream);
				//	printf("size =%d,load ituner file done\n",size);
				/*	int fd_bpc = -1;
					if((fd_bpc = open("load_tmp.txt", O_CREAT | O_TRUNC | O_WRONLY, 0777)) < 0) {
								printf("map file open error!\n");
					}
					write(fd_bpc, ituner_stream, size);
					close(fd_bpc);*/
					send_msg_ex(sock_fd,ituner_stream,size);
					printf("ITUNER_FILE DONE\n");
					break;
				}
#endif
			}
			break;
		}

		case HDRTuning:
			break;

		case OTHER:
			printf("to be supported\n");
			break;

		default:
			printf("Unknow tab id [%c]\n",p_tuning_id->tab_id);
			break;
	}
}
int cfg_id = 0;
//#define DSP_DEBUG
static void process_apply(int sock_fd,TUNING_ID* p_tuning_id)
{
	printf("Apply!\n");
	int rval =0;

		if(p_tuning_id->item_id!=ConfigAAAControl){
			if(p_tuning_id->item_id<ImgConfigInfo&&aaa_cntl_station.adj_enable){
				printf("pls disable auto adj\n");
				return;
			}
		}
		switch(p_tuning_id->item_id)
		{
		case TUNING_DONE:
			if(img_config.isp_pipeline ==ISP_PIPELINE_LISO)
				break;
			ik_mode.ConfigId = cfg_id;
			amba_img_dsp_post_exe_cfg(fd_iav, &ik_mode, AMBA_DSP_IMG_CFG_EXE_PARTIALCOPY, 0);
#ifdef DSP_DEBUG
			AMBA_DSP_IMG_DEBUG_MODE_s debug = {0};
			debug.Step = 0;
			debug.Mode = 0;
			amba_img_dsp_set_debug_mode(&ik_mode, &debug);
			usleep(1000*1000);
			static amba_img_dsp_cfg_info_t CfgInfo;
			CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
			CfgInfo.CfgId =ik_mode.ConfigId;

			AmbaDSP_ImgLowIsoDumpCfg(CfgInfo, "/mnt/tuning_cfg");
			AmbaDSP_ImgLowIsoPrintCfg(CfgInfo);
#endif
			cfg_id ^= 0x1;
			ik_mode.ConfigId = cfg_id;
			amba_img_dsp_post_exe_cfg(fd_iav, &ik_mode, AMBA_DSP_IMG_CFG_EXE_FULLCOPY, 0);
			printf("TUNING DONE!!!!!!!!!!!!!!!!!\n\n");
			break;
		case BlackLevelCorrection:
			rval=recv(sock_fd,(char*)&blc_pkg,sizeof(BLC_PKG),0);
			amba_img_dsp_set_static_black_level(fd_iav,&ik_mode, &blc_pkg.blc_info);
			amba_img_dsp_set_deferred_black_level( fd_iav,&ik_mode, &blc_pkg.DefBlc);
			if (img_config.hdr_config.pipeline != HDR_PIPELINE_OFF) {
				amba_img_dsp_set_hdr_video_post_proc( fd_iav, &ik_mode);
			}
			printf("Black Level Correction!\n");
			break;

		case ColorCorrection:
		{
			receive_msg(sock_fd,cc_reg,AMBA_DSP_IMG_CC_REG_SIZE);
			receive_msg(sock_fd,cc_matrix,AMBA_DSP_IMG_CC_3D_SIZE);
			color_corr_reg.RegSettingAddr= (u32)cc_reg;
			color_corr.MatrixThreeDTableAddr = (u32)cc_matrix;
			rval = amba_img_dsp_set_color_correction_reg(&ik_mode,&color_corr_reg);
			CHECK_RVAL
			rval = amba_img_dsp_set_color_correction(fd_iav,&ik_mode, &color_corr);
			CHECK_RVAL
			printf("Color Correction!\n");
			break;
		}
		case ToneCurve:
			{
				rval=recv(sock_fd,(char*)&tone_curve,sizeof(amba_img_dsp_tone_curve_t),0);
				rval = amba_img_dsp_set_tone_curve(fd_iav,&ik_mode,&tone_curve);
				CHECK_RVAL
				printf("Tone Curve!\n ");
			}
			break;

		case RGBtoYUVMatrix:
			rval=recv(sock_fd,(char*)&rgb2yuv_matrix,sizeof(amba_img_dsp_rgb_to_yuv_t),0);
			rval = amba_img_dsp_set_rgb_to_yuv_matrix(fd_iav,&ik_mode,&rgb2yuv_matrix);
			CHECK_RVAL
			printf("RGB to YUV Matrix!\n");
			break;

		case WhiteBalanceGains:
			rval=recv(sock_fd,(char*)&tmp_wb_gain,sizeof(amba_img_dsp_wb_gain_t),0);
			wb_gain.GainR =tmp_wb_gain.GainR;
			wb_gain.GainG =tmp_wb_gain.GainG;
			wb_gain.GainB=tmp_wb_gain.GainB;
			wb_gain.GlobalDGain =tmp_wb_gain.GlobalDGain;
			rval = amba_img_dsp_set_wb_gain(fd_iav,&ik_mode, &wb_gain);
			CHECK_RVAL
			printf("White Balance !\n");
			break;

		case DGainSaturaionLevel:
			rval=recv(sock_fd,(char*)&d_gain_satuation_level,sizeof(amba_img_dsp_dgain_saturation_t),0);
			rval = amba_img_dsp_set_dgain_saturation_level( fd_iav,&ik_mode,&d_gain_satuation_level);
			CHECK_RVAL
			printf("DGain Saturation Level!\n");
			break;

		case LocalExposure:
			rval=recv(sock_fd,(char*)&local_exposure,sizeof(amba_img_dsp_local_exposure_t),0);
			rval = amba_img_dsp_set_local_exposure(fd_iav,&ik_mode, &local_exposure);
			CHECK_RVAL
			printf("Local Exposure!\n");
			break;

		case ChromaScale:
			rval=recv(sock_fd,(char*)&cs,sizeof(amba_img_dsp_chroma_scale_t),0);
			rval = amba_img_dsp_set_chroma_scale(fd_iav,&ik_mode,&cs);
			CHECK_RVAL
			printf("Chroma Scale!\n");
			break;

		case FPNCorrection://not finished

			break;
		case BadPixelCorrection:
			rval=recv(sock_fd,(char*)&dbp_correction_setting,sizeof(amba_img_dsp_dbp_correction_t),0);
			amba_img_dsp_set_dynamic_bad_pixel_correction(fd_iav,&ik_mode,&dbp_correction_setting);
			printf("Bad Pixel Correction!\n");
			break;

		case CFALeakageFilter:
			rval=recv(sock_fd,(char*)&cfa_leakage_filter,sizeof(amba_img_dsp_cfa_leakage_filter_t),0);
			amba_img_dsp_set_cfa_leakage_filter(fd_iav,&ik_mode,&cfa_leakage_filter);
			printf("CFA Leakage Filter!\n");
			break;

		case AntiAliasingFilter:
			rval=recv(sock_fd,(char*)&AntiAliasing,sizeof(amba_img_dsp_anti_aliasing_t),0);
			amba_img_dsp_set_anti_aliasing(fd_iav,&ik_mode, &AntiAliasing);
			printf("Anti-Aliasing Filter!\n");
			break;

		case CFANoiseFilter:
			rval=recv(sock_fd,(char*)&cfa_noise_filter,sizeof(amba_img_dsp_cfa_noise_filter_t),0);
			amba_img_dsp_set_cfa_noise_filter(fd_iav,&ik_mode,&cfa_noise_filter);
			printf("CFA Noise Filter!\n");
			break;

		case ChromaMedianFiler:
			rval=recv(sock_fd,(char*)&chroma_median_setup,sizeof(amba_img_dsp_chroma_median_filter_t),0);
			amba_img_dsp_set_chroma_median_filter(fd_iav,&ik_mode,&chroma_median_setup);
			printf("Chroma Median Filer!\n");
			break;

		case SharpeningA_ASF:
		{
			rval=recv(sock_fd,(char*)&sa_asf_pkg,sizeof(SA_ASF_PKG),0);
			amba_img_dsp_set_luma_processing_mode(fd_iav,&ik_mode,&sa_asf_pkg.select_mode);

			if(sa_asf_pkg.select_mode.UseSharpenNotAsf==0)
			{
				amba_img_dsp_set_advance_spatial_filter(fd_iav, &ik_mode, &sa_asf_pkg.asf_info);
				printf("ASF!\n");
			}
			else if(sa_asf_pkg.select_mode.UseSharpenNotAsf ==1)
			{
				amba_img_dsp_set_1st_sharpen_noise_both(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.both);
				amba_img_dsp_set_1st_sharpen_noise_noise(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.noise);
				amba_img_dsp_set_1st_sharpen_noise_sharpen_fir(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.fir);
				amba_img_dsp_set_1st_sharpen_noise_sharpen_coring(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.coring);
				amba_img_dsp_set_1st_sharpen_noise_sharpen_min_coring_result(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.MinCoringResult);
				amba_img_dsp_set_1st_sharpen_noise_sharpen_coring_index_scale(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.CoringIndexScale);
				amba_img_dsp_set_1st_sharpen_noise_sharpen_scale_coring(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.ScaleCoring);
				printf("SA!\n");
			}
		}
		break;

		case MCTFControl:
		        if(img_config.isp_pipeline ==ISP_PIPELINE_LISO){
        			rval=recv(sock_fd,(char*)&mctf_info,sizeof(amba_img_dsp_video_mctf_info_t),0);
			       amba_img_dsp_set_video_mctf(fd_iav,&ik_mode,&mctf_info);
			}else if(img_config.isp_pipeline ==ISP_PIPELINE_ADV_LISO||img_config.isp_pipeline ==ISP_PIPELINE_MID_LISO||img_config.isp_pipeline == ISP_PIPELINE_B_LISO){
			        rval=recv(sock_fd,(char*)&mctf_pkg,sizeof(MCTF_PKG),0);
			        amba_img_dsp_set_video_mctf(fd_iav,&ik_mode,&mctf_pkg.mctf_info);
			        amba_img_dsp_set_video_mctf_temporal_adjust( fd_iav, &ik_mode,&mctf_pkg.mctf_temporal_adjust);
			}
			printf("MCTF Control!\n");
			break;
		case SharpeningBControl:
			rval=recv(sock_fd,(char*)&sharpen_pkg,sizeof(SHARPEN_PKG),0);
			amba_img_dsp_set_final_sharpen_noise_both(fd_iav,&ik_mode,&sharpen_pkg.both);
			amba_img_dsp_set_final_sharpen_noise_noise(fd_iav,&ik_mode,&sharpen_pkg.noise);
			amba_img_dsp_set_final_sharpen_noise_sharpen_coring(fd_iav,&ik_mode,&sharpen_pkg.coring);
			amba_img_dsp_set_final_sharpen_noise_sharpen_fir(fd_iav,&ik_mode,&sharpen_pkg.fir);
			amba_img_dsp_set_final_sharpen_noise_sharpen_min_coring_result(fd_iav,&ik_mode,&sharpen_pkg.MinCoringResult);
			amba_img_dsp_set_final_sharpen_noise_sharpen_coring_index_scale(fd_iav,&ik_mode,&sharpen_pkg.CoringIndexScale);
			amba_img_dsp_set_final_sharpen_noise_sharpen_scale_coring(fd_iav,&ik_mode,&sharpen_pkg.ScaleCoring);
			printf("SharpeningBControl!\n");
			break;

		case ColorDependentNoiseReduction:
			rval=recv(sock_fd,(char*)&cdnr_info,sizeof(amba_img_dsp_cdnr_info_t),0);
			amba_img_dsp_set_color_dependent_noise_reduction( fd_iav, &ik_mode, &cdnr_info);
			printf("Color Dependent Noise Reduction!\n");
			break;

		case ChromaNoiseFilter:
			rval=recv(sock_fd,(char*)&chroma_noise_filter,sizeof(amba_img_dsp_chroma_filter_t),0);
			amba_img_dsp_set_chroma_filter(fd_iav,&ik_mode,&chroma_noise_filter);
			printf("Chroma Noise Filter!\n");
			break;
		case WideChromaFilter:
			rval=recv(sock_fd,(char*)&wide_chroma_noise_filter,sizeof(amba_img_dsp_chroma_filter_t),0);
			if(img_config.isp_pipeline ==ISP_PIPELINE_ADV_LISO){
				amba_img_dsp_set_wide_chroma_filter(fd_iav,&ik_mode,&wide_chroma_noise_filter);
				printf("Wide Chroma Filter!\n");
			}
			break;
		 case WideChromaFilterCombine:
			rval=recv(sock_fd,(char*)&wide_chroma_noise_filter_combine,sizeof(AMBA_DSP_IMG_HISO_CHROMA_FILTER_COMBINE_s),0);
			if(img_config.isp_pipeline ==ISP_PIPELINE_ADV_LISO){
				amba_img_dsp_set_wide_chroma_filter_combine(fd_iav,&ik_mode,&wide_chroma_noise_filter_combine);
				printf("Wide Chroma Filter Combine!\n");
			}
                      break;
		case TileConfiguration:
		//	rval=recv(sock_fd,(char*)&tile_pkg.stat_config_info,sizeof(TILE_PKG),0);
		//	amba_img_dsp_3a_config_aaa_stat( &ik_mode, &tile_pkg.stat_config_info);
		//	printf("Tile Configuration!\n");
			break;
		case DEMOSAIC:
			rval=recv(sock_fd,(char*)&demosaic_info,sizeof(amba_img_dsp_demosaic_t),0);
			amba_img_dsp_set_demosaic(fd_iav, &ik_mode, &demosaic_info);
			printf("Demosaic!\n");
			break;
		case MisMatchGr_Gb:
			rval=recv(sock_fd,(char*)&mismatch_gbgr,sizeof(amba_img_dsp_gbgr_mismatch_t),0);
			amba_img_dsp_set_gbgr_mismatch(fd_iav,&ik_mode, &mismatch_gbgr);
			printf("MisMatchGr_Gb!\n");
			break;

		case ConfigAAAControl:
			rval=recv(sock_fd,(char*)&aaa_cntl_station,sizeof(aaa_cntl_t),0);
			img_enable_ae(aaa_cntl_station.ae_enable);
			img_enable_awb(aaa_cntl_station.awb_enable);
			img_enable_af(aaa_cntl_station.af_enable);
			img_enable_adj(aaa_cntl_station.adj_enable);
			printf("Config AAA Control!\n");
			break;

		case AFStatisticSetupEx:
		//	rval=recv(sock_fd,(char*)&af_statistic_setup_ex,sizeof(af_statistics_ex_t),0);
		//	img_dsp_set_af_statistics_ex(fd_iav,&af_statistic_setup_ex,1);
		//	printf("AF Statistic Setup Ex!\n");
		//	break;

		case ExposureControl:
			rval=recv(sock_fd,(char*)&ec_info,sizeof(EC_INFO),0);
		 	img_set_sensor_shutter(fd_iav,ec_info.shutter_row);
		  	img_set_sensor_agc(fd_iav,ec_info.gain_tbl_idx);
			wb_gain.AeGain =ec_info.dgain;
			amba_img_dsp_set_wb_gain(fd_iav,&ik_mode,&wb_gain);
			printf("Exposure Control!\n");
			break;
		case AEBSetting:
			{
				rval=recv(sock_fd,(char*)&aeb_setting_info,sizeof(AEB_SETTING_INFO),0);
				if(aaa_cntl_station.ae_enable&&aaa_cntl_station.adj_enable)
					img_set_ae_target_ratio(&aeb_setting_info.ae_target_ratio);
				if(aaa_cntl_station.awb_enable&&aaa_cntl_station.adj_enable){
					img_set_wb_property_ratio(aeb_setting_info.awb_r_ratio,aeb_setting_info.awb_b_ratio);
				}
				printf("AEB Setting!\n");
				break;
			}
		case HDR_BATCH_CMD:
			{
				receive_msg(sock_fd,(u8*)&batch_cmd_pkg,sizeof(BATCH_CMD_PKG));
				img_set_hdr_enable(batch_cmd_pkg.hdr_enable);
				img_get_hdr_batch(fd_iav,&hdr_proc_data);
				feed_hdr_proc_data(&hdr_proc_data,&batch_cmd_pkg,img_config.hdr_config.expo_num);
				memcpy(&p_sht_agc_gp.agc_gp,batch_cmd_pkg.agc,MAX_HDR_EXPOSURE_NUM*sizeof(u32));
				memcpy(&p_sht_agc_gp.shutter_gp,batch_cmd_pkg.shutter,MAX_HDR_EXPOSURE_NUM*sizeof(u32));
				img_set_hdr_batch(fd_iav,&ik_mode,&img_config,&hdr_proc_data);
				img_set_hdr_alpha_value(&batch_cmd_pkg.alpha_value);
				img_set_hdr_threshold(&batch_cmd_pkg.threshold_value);
				img_set_hdr_sensor_shutter_agc_group(fd_iav, &p_sht_agc_gp);
				u16 ae_target;
				u16 ae_target_ratio;
				img_get_ae_target(&ae_target);
				img_get_ae_target_ratio(&ae_target_ratio);
				u16 fixed_ae_target =ae_target*1024/ae_target_ratio;
				ae_target_ratio =batch_cmd_pkg.ae_target*1024/fixed_ae_target;
				img_set_ae_target_ratio(&ae_target_ratio);
				printf("HDR BATCH CMD!\n");
				break;
			}


		case MoBadPixelCorrection:
			rval=recv(sock_fd,(char*)&dbp_correction_setting,sizeof(amba_img_dsp_dbp_correction_t),0);
			amba_img_dsp_set_mo_dynamic_bad_pixel_correction(fd_iav,&ik_mode,&dbp_correction_setting);
			printf("Motion Bad Pixel Correction!\n");
			break;

		case MoCFALeakageFilter:
			rval=recv(sock_fd,(char*)&cfa_leakage_filter,sizeof(amba_img_dsp_cfa_leakage_filter_t),0);
			amba_img_dsp_set_mo_cfa_leakage_filter(fd_iav,&ik_mode,&cfa_leakage_filter);

			printf("Motion CFA Leakage Filter!\n");
			break;

		case MoAntiAlisingFilter:
			rval=recv(sock_fd,(char*)&AntiAliasing,sizeof(amba_img_dsp_anti_aliasing_t),0);
			amba_img_dsp_set_mo_anti_aliasing(fd_iav,&ik_mode, &AntiAliasing);
			printf("Motion Anti-Aliasing Filter!\n");
			break;

		case MoCFANoiseFilter:
			rval=recv(sock_fd,(char*)&cfa_noise_filter,sizeof(amba_img_dsp_cfa_noise_filter_t),0);
			amba_img_dsp_set_mo_cfa_noise_filter(fd_iav,&ik_mode,&cfa_noise_filter);

			printf("Motion CFA Noise Filter!\n");
			break;

		case MoChromaMedianFilter:
			rval=recv(sock_fd,(char*)&chroma_median_setup,sizeof(amba_img_dsp_chroma_median_filter_t),0);
			amba_img_dsp_set_mo_chroma_median_filter(fd_iav,&ik_mode,&chroma_median_setup);
			printf("Motion Chroma Median Filer!\n");
			break;

		case MoSharpeningA_ASF:
		{
			rval=recv(sock_fd,(char*)&sa_asf_pkg,sizeof(SA_ASF_PKG),0);
			amba_img_dsp_set_mo_luma_processing_mode(fd_iav,&ik_mode,&sa_asf_pkg.select_mode);

			if(sa_asf_pkg.select_mode.UseSharpenNotAsf==0)
			{
				amba_img_dsp_set_mo_advance_spatial_filter(fd_iav, &ik_mode, &sa_asf_pkg.asf_info);
				printf("Motion ASF!\n");
			}
			else if(sa_asf_pkg.select_mode.UseSharpenNotAsf ==1)
			{
				amba_img_dsp_set_mo_sharpen_noise_both(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.both);
				amba_img_dsp_set_mo_sharpen_noise_noise(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.noise);
				amba_img_dsp_set_mo_sharpen_noise_sharpen_fir(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.fir);
				amba_img_dsp_set_mo_sharpen_noise_sharpen_coring(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.coring);
				amba_img_dsp_set_mo_sharpen_noise_sharpen_min_coring_result(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.MinCoringResult);
				amba_img_dsp_set_mo_sharpen_noise_sharpen_coring_index_scale(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.CoringIndexScale);
				amba_img_dsp_set_mo_sharpen_noise_sharpen_scale_coring(fd_iav,&ik_mode,&sa_asf_pkg.sa_info.ScaleCoring);
				printf("Motion SA!\n");
			}
		}
		break;

		case MoChromaNoiseFilter:
			rval=recv(sock_fd,(char*)&chroma_noise_filter,sizeof(amba_img_dsp_chroma_filter_t),0);
			amba_img_dsp_set_mo_chroma_filter(fd_iav,&ik_mode,&chroma_noise_filter);
			printf("Motion Chroma Noise Filter!\n");
			break;

		case MoDemosaic:
			rval=recv(sock_fd,(char*)&demosaic_info,sizeof(amba_img_dsp_demosaic_t),0);
			amba_img_dsp_set_mo_demosaic(fd_iav, &ik_mode, &demosaic_info);
			printf("Motion Demosaic!\n");
			break;

		case MoMisMatchGr_Gb:
			rval=recv(sock_fd,(char*)&mismatch_gbgr,sizeof(amba_img_dsp_gbgr_mismatch_t),0);
			amba_img_dsp_set_mo_gbgr_mismatch(fd_iav,&ik_mode, &mismatch_gbgr);
			printf("Motion MisMatchGr_Gb!\n");
			break;

		case CHECK_IP:
			printf("Check IP!\n");
			break;

#ifdef ITUNER_TRANS
		case ITUNER_FILE:
			{

			u32 size =p_tuning_id->size;
			printf("size  ===%d\n",size);
			u8* ituner_buf =malloc(size);//sizeof
			receive_msg(sock_fd,ituner_buf,size);
	/*		int fd_bpc = -1;
			if((fd_bpc = open("apply_tmp.txt", O_CREAT | O_TRUNC | O_WRONLY, 0777)) < 0) {
						printf("map file open error!\n");
			}
			write(fd_bpc, ituner_buf, size);
			close(fd_bpc);*/

			AmbaTUNE_apply_ituner_stream(ituner_buf,size);

			ITUNER_INFO_s ItunerInfo;
			if (0 != AmbaTUNE_Get_ItunerInfo(&ItunerInfo)) {
				printf("%s() %d, call AmbaTUNE_Get_ItunerInfo() Fail", __func__, __LINE__);
				break ;
			}
			memcpy(&ik_mode, &ItunerInfo.TuningAlgoMode, sizeof(amba_img_dsp_mode_cfg_t));
			AMBA_ITUNER_PROC_INFO_s ProcInfo;
			memset(&ProcInfo, 0, sizeof(AMBA_ITUNER_PROC_INFO_s));
			ProcInfo.HisoBatchId = 0;
			ik_mode.BatchId =0xff; // FIXME

			if (0 != AmbaTUNE_Execute_IDSP(fd_iav, ik_mode, &ProcInfo)) {
				printf("%s() %d, call AmbaTUNE_Execute_IDSP() Fail", __func__, __LINE__);
				break;
			}
			free(ituner_buf);
			// to add post
			if(img_config.isp_pipeline ==ISP_PIPELINE_LISO)
				break;
			cfg_id_backup =ik_mode.ConfigId= cfg_id;
			amba_img_dsp_post_exe_cfg(fd_iav, &ik_mode,AMBA_DSP_IMG_CFG_EXE_PARTIALCOPY, 0);
			cfg_id ^= 0x1;
			ik_mode.ConfigId = cfg_id;
			amba_img_dsp_post_exe_cfg(fd_iav, &ik_mode, AMBA_DSP_IMG_CFG_EXE_FULLCOPY, 0);
			printf("TUNING DONE!!!!!!!!!!!!!!!!!\n\n");
			printf("ITUNER FILE done\n");
			break;
		}
#endif
	}
}
static int process_tuning_id(int sock_fd,TUNING_ID* p_tuning_id)
{
	switch(p_tuning_id->req_id)
	{
	case APPLY:
		process_apply(sock_fd,p_tuning_id);
		break;
	case LOAD:
		process_load(sock_fd,p_tuning_id);
		break;
	case OTHER:
		process_other(sock_fd,p_tuning_id);
		break;
	default:
		printf("Unknown req_id [%c]\n",p_tuning_id->req_id);
		break;
	}
	return 0;
}
static const char* short_options = "adI:D:";
static struct option long_options[] = {
	{"aaa",0,0,'a'},
	{"debug",0,0,'d'},
	{"lens_mount_id", HAS_ARG, 0, 'I'},
	{"lens_cali_mode", HAS_ARG, 0, 'D'},
};
static int init_param(int argc, char **argv)
{
	int ch;
	int option_index = 0;

	opterr = 0;
	while ((ch = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
		switch(ch) {
		 case 'a':
		        break;
		case 'd'://only for debug
			debug_enble=1;
			break;
		 case 'I':
				lens_mount_id = atoi(optarg);
				break;
		 case 'D':
				lens_cali_info.act_mode = atoi(optarg);
				break;
		default:
			printf("unknown option %c\n", ch);
			return -1;
		}
	}
	return 0;
}
struct hint_s {
	const char *arg;
	const char *str;
};
static const struct hint_s hint[] = {
	{"-a","\tauto config"},
	{"-d","\tdebug IK for internal"}
};
static void usage()
{
	int cnt = sizeof(hint)/sizeof(hint[0]);
	int i;
	for(i=0; i<cnt; i++) {
		printf("%s %s\n", hint[i].arg, hint[i].str);
	}
	printf("\nExamples:\n");
	printf("    test_tuning -a\n");
	printf("	test_tuning -a -d\n");

	printf("\n");
}

int check_iav_work(void)
{
	u32 state;

	memset(&state, 0, sizeof(state));
	if (ioctl(fd_iav, IAV_IOC_GET_IAV_STATE, &state) < 0) {
		perror("IAV_IOC_GET_IAV_STATE");
		return -1;
	}
	if (state == IAV_STATE_PREVIEW || state == IAV_STATE_ENCODING) {
		return 1;
	}

	return 0;
}

int do_init_netlink(void)
{
	init_netlink();
	pthread_create(&nl_thread, NULL, (void *)netlink_loop, (void *)NULL);

	while (1) {
		if (check_iav_work() > 0) {
			break;
		}
		usleep(10000);
	}

	return 0;
}

int do_prepare_aaa(void)
{
	#define	PIXEL_IN_MB			(16)
	struct vindev_video_info video_info;
	struct iav_enc_mode_cap mode_cap;
	struct iav_system_resource system_resource;
	struct iav_srcbuf_setup	srcbuf_setup;
	struct vindev_aaa_info vin_aaa_info;
	int sensor_id = 0x0;
	img_config_info_t* p_img_config = &img_config;

	memset(p_img_config, 0, sizeof(img_config_info_t));

	// video info
	memset(&video_info, 0, sizeof(video_info));
	video_info.vsrc_id = 0;
	video_info.info.mode = AMBA_VIDEO_MODE_CURRENT;
	if (ioctl(fd_iav, IAV_IOC_VIN_GET_VIDEOINFO, &video_info) < 0) {
		perror("IAV_IOC_VIN_GET_VIDEOINFO");
		return 0;
	}
	p_img_config->raw_width = ROUND_UP(video_info.info.width, PIXEL_IN_MB);
	p_img_config->raw_height = ROUND_UP(video_info.info.height, PIXEL_IN_MB);
	p_img_config->raw_resolution =video_info.info.bits;

	// encode mode capability
	memset(&mode_cap, 0, sizeof(mode_cap));
	mode_cap.encode_mode = DSP_CURRENT_MODE;
	if (ioctl(fd_iav, IAV_IOC_QUERY_ENC_MODE_CAP, &mode_cap)) {
		perror("IAV_IOC_QUERY_ENC_MODE_CAP");
		return -1;
	}

	// system resource
	memset(&system_resource, 0, sizeof(system_resource));
	system_resource.encode_mode = DSP_CURRENT_MODE;
	if (ioctl(fd_iav, IAV_IOC_GET_SYSTEM_RESOURCE, &system_resource) < 0) {
		perror("IAV_IOC_GET_SYSTEM_RESOURCE\n");
		return -1;
	}
	p_img_config->hdr_config.expo_num = system_resource.exposure_num;
	p_img_config->hdr_config.pipeline = system_resource.hdr_type;
	p_img_config->isp_pipeline = system_resource.iso_type;
	p_img_config->raw_pitch =system_resource.raw_pitch_in_bytes;

	// source buffer
	memset(&srcbuf_setup, 0, sizeof(srcbuf_setup));
	if (ioctl(fd_iav, IAV_IOC_GET_SOURCE_BUFFER_SETUP, &srcbuf_setup) < 0) {
		printf("IAV_IOC_GET_SOURCE_BUFFER_SETUP error\n");
		return -1;
	}

	p_img_config->main_width = ROUND_UP(
		srcbuf_setup.size[IAV_SRCBUF_MN].width, PIXEL_IN_MB);
	p_img_config->main_height = ROUND_UP(
		srcbuf_setup.size[IAV_SRCBUF_MN].height, PIXEL_IN_MB);

	// vin aaa info
	memset(&vin_aaa_info, 0, sizeof(vin_aaa_info));
	vin_aaa_info.vsrc_id = 0;
	if (ioctl(fd_iav, IAV_IOC_VIN_GET_AAAINFO, &vin_aaa_info) < 0) {
		perror("IAV_IOC_VIN_GET_AAAINFO error\n");
		return -1;
	}

	p_img_config->raw_bayer = vin_aaa_info.bayer_pattern;
	sensor_id = vin_aaa_info.sensor_id;
	if(vin_aaa_info.dual_gain_mode){
		p_img_config->hdr_config.method = HDR_DUAL_GAIN_METHOD;
	}else{
		switch (vin_aaa_info.hdr_mode){
			case AMBA_VIDEO_LINEAR_MODE:
				p_img_config->hdr_config.method = HDR_NONE_METHOD;
				break;
			case AMBA_VIDEO_2X_HDR_MODE:
			case AMBA_VIDEO_3X_HDR_MODE:
			case AMBA_VIDEO_4X_HDR_MODE:
				if(sensor_id == SENSOR_AR0230 ){
					p_img_config->hdr_config.method = HDR_RATIO_FIX_LINE_METHOD;
				}else{
					p_img_config->hdr_config.method = HDR_RATIO_VARY_LINE_METHOD;
				}
				break;
			case AMBA_VIDEO_INT_HDR_MODE:
				p_img_config->hdr_config.method = HDR_BUILD_IN_METHOD;
				break;
			default:
				printf("error: invalid vin HDR sensor info.\n");
				return -1;
		}
	}

	img_lib_init(fd_iav, p_img_config->defblc_enable, p_img_config->sharpen_b_enable);
	img_config_pipeline(fd_iav, p_img_config);
	init_ik_mode(p_img_config, &ik_mode, &mode_cap);
	if (binding_param_with_sensor(fd_iav, &vin_aaa_info, p_img_config, &ik_mode) < 0)
		return -1;
	if (img_prepare_isp(fd_iav) < 0) {
		printf("prepare isp fail\n");
		return -1;
	}
	return 0;
}

int do_start_aaa(void)
{
	if (img_start_aaa(fd_iav) < 0) {
		printf("start_aaa error!\n");
		return -1;
	}
	if (img_set_work_mode(0)) {
		printf("img_set_work_mode error!\n");
		return -1;
	}
	return 0;
}

int do_stop_aaa(void)
{
	if (img_stop_aaa() < 0) {
		printf("stop aaa error!\n");
		return -1;
	}
	usleep(1000);
	if (img_lib_deinit() < 0) {
		printf("img_lib_deinit error!\n");
		return -1;
	}
	return 0;
}

static void sigstop(int signo)
{
	if (send_image_msg_exit() < 0) {
		perror("send_image_msg_stop_aaa\n");
		exit(-1);
	}
	pthread_join(nl_thread, NULL);
	nl_thread = -1;

	close(fd_iav);
	exit(1);
}
static u8 exit_flag =0;
void handle_pipe()
{
	printf("histo thread exit\n");
	exit_flag =1;
}
static amba_dsp_aaa_statistic_data_t g_stat;
static struct rgb_aaa_stat rgb_stat[MAX_SLICE_NUM];
static struct cfa_aaa_stat cfa_stat[MAX_SLICE_NUM];
static struct cfa_pre_hdr_stat hdr_cfa_pre_stat[MAX_PRE_HDR_STAT_BLK_NUM];
static u8 rgb_stat_valid;
static u8 cfa_stat_valid;
static u8 hdr_cfa_pre_valid;
static aaa_tile_report_t act_tile;


static int prepare_to_get_statistics(amba_dsp_aaa_statistic_data_t* stat)
{
	memset(rgb_stat, 0, sizeof(rgb_stat));
	memset(cfa_stat, 0, sizeof(cfa_stat));
	memset(hdr_cfa_pre_stat, 0, sizeof(struct cfa_pre_hdr_stat)*MAX_PRE_HDR_STAT_BLK_NUM);
	rgb_stat_valid = 0;
	cfa_stat_valid = 0;
	hdr_cfa_pre_valid = 0;
	memset(stat, 0, sizeof(amba_dsp_aaa_statistic_data_t));

	stat->CfaAaaDataAddr = (u32)cfa_stat;
	stat->RgbAaaDataAddr = (u32)rgb_stat;
	stat->CfaPreHdrDataAddr = (u32)hdr_cfa_pre_stat;
	stat->CfaDataValid = (u32)&cfa_stat_valid;
	stat->RgbDataValid = (u32)&rgb_stat_valid;
	stat->CfaPreHdrDataValid = (u32)&hdr_cfa_pre_valid;

	return 0;
}
static HISTO_PKG histo_pkg;
void feed_histo_pkg(HISTO_PKG* pkg,img_aaa_stat_t* stat,int expo_num)
{
	int i=0;
	for(i=0;i<expo_num;i++){
		memcpy(&pkg->cfa_pre_histogram[i],&stat[i].cfa_pre_hdr_hist,sizeof(cfa_pre_hdr_histogram_t));
	}
	memcpy(&(pkg->cfa_histogram),&stat[0].cfa_hist,sizeof(cfa_histogram_stat_t));
	memcpy(&(pkg->rgb_histogram),&stat[0].rgb_hist,sizeof(rgb_histogram_stat_t));
/*	int sum1 =0;
	int sum2 =0;
	for(i =0;i<64;i++)
	{
		sum1+= pkg->cfa_histogram.his_bin_g[i];
		sum2+= pkg->rgb_histogram.his_bin_g[i];
	}
	printf("\nsum =%d, %d\n",sum1,sum2);*/
}
static int show_cmd_menu(void)
{
	printf("\n================ Global Settings ================\n");
	printf("  d -- dsp dump\n");
	printf("  r  -- test_ituner -r\n");
	printf("  t  -- test_ae timing\n");
	printf("  q -- Return to upper level\n");
	printf("\n================================================\n\n");
	printf("G > ");
	return 0;
}

void menucmd_thread_proc()
{
	char ch;
	int  exit_flag, error_opt;
	show_cmd_menu();
	ch = getchar();
	while (ch) {
		exit_flag = 0;
		error_opt = 0;
		switch (ch) {
		case 'd':
			printf("pls input lisocfg dump step,mode\n");
			scanf("%d %d",&debug_step,&debug_mode);
			AMBA_DSP_IMG_DEBUG_MODE_s debug = {0};
			debug.Step = debug_step;
			debug.Mode = debug_mode;
			debug.PicNum =0xFFFFFFFF;
			//   printf("mode =%d,step=%d,frame =%d\n",debug_mode,debug_step,debug_frame);
			amba_img_dsp_set_debug_mode(&ik_mode, &debug);
			cfg_id ^= 0x1;
			ik_mode.ConfigId = cfg_id;
			amba_img_dsp_set_debug_mode(&ik_mode, &debug);
			break;
		case 'r':{
			char  shell_cmd[128];
			sprintf(shell_cmd, "test_ituner -r");
			system(shell_cmd);
			break;
			}
		case 'p':
		{
			#ifdef DSP_DEBUG

				AMBA_DSP_IMG_DEBUG_MODE_s debug = {0};
				debug.Step = 0;
				debug.Mode = 0;
				amba_img_dsp_set_debug_mode(&ik_mode, &debug);
				usleep(1000*1000);
				static amba_img_dsp_cfg_info_t CfgInfo;
				CfgInfo.Pipe = AMBA_DSP_IMG_PIPE_VIDEO;
				CfgInfo.CfgId =ik_mode.ConfigId;

				AmbaDSP_ImgLowIsoDumpCfg(CfgInfo, "/mnt/tuning_cfg");
				AmbaDSP_ImgLowIsoPrintCfg(CfgInfo);
			#endif
			break;
		}
		case 'q':
			exit_flag = 1;
			break;
		case 't':
			calc_ae_timing(fd_iav);
			break;
		default:
			error_opt = 1;
			break;
		}
		if (exit_flag)
			break;
		if (error_opt == 0) {
			show_cmd_menu();
		}
		ch = getchar();
	}

}
void histo_thread_proc()
{
	int histo_sock_server = -1;
	int histo_sock_client = -1;

	static img_aaa_stat_t hdr_statis_gp[MAX_HDR_EXPOSURE_NUM];
	memset(hdr_statis_gp, 0, MAX_HDR_EXPOSURE_NUM*sizeof(img_aaa_stat_t));
	prepare_to_get_statistics(&g_stat);
	struct sockaddr_in	addr_client;
	histo_sock_server=SockServer_Setup(histo_sock_server, HISTO_SOCKET_PORT);

	while(1)
	{
		socklen_t s_size=sizeof(struct sockaddr_in);
		if((histo_sock_client=accept(histo_sock_server,(struct sockaddr*)&addr_client,&s_size))==-1)
		{
			perror("accapt");
			continue;
		}
		while(1)
		{
			if(amba_img_dsp_3a_get_aaa_stat(fd_iav,&ik_mode,&g_stat)<0)
				continue;

			if(parse_aaa_data(&g_stat, img_config.hdr_config.pipeline, hdr_statis_gp, &act_tile)<0){
				continue;
			}
			feed_histo_pkg(&histo_pkg,hdr_statis_gp,img_config.hdr_config.expo_num);
			if(send_msg_ex(histo_sock_client,(u8*)&histo_pkg,sizeof(HISTO_PKG))<0)
				break;

			char ack[10];
			while(1)
			{
				struct sigaction action;
				action.sa_handler = handle_pipe;
				sigemptyset(&action.sa_mask);
				action.sa_flags = 0;
				sigaction(SIGPIPE, &action, NULL);
				if(exit_flag==1)
				{
					if (histo_sock_client != -1)
						close(histo_sock_client);
					break;
				}
				recv(histo_sock_client, ack, 1, MSG_WAITALL);
			//	printf("ack =%c\n", ack[0]);
				if(ack[0] =='!')
					break;
			}
			if(exit_flag==1)
				break;
			usleep(1000*1200);
		}
	}
 }



int main(int argc, char **argv)
{
	signal(SIGINT, sigstop);
	signal(SIGQUIT, sigstop);
	signal(SIGTERM, sigstop);

	if(argc<=1){
		usage();
		return -1;
	}
	if (init_param(argc, argv) < 0) {
		return -1;
	}

	if ((fd_iav = open("/dev/iav", O_RDWR, 0)) < 0) {
		perror("open /dev/iav");
		return -1;
	}
	if (do_init_netlink() < 0) {
		return -1;
	}

	if(img_config_lens_cali(&lens_cali_info) < 0) {
			return -1;
	}

	int sockfd =-1;
	int new_fd =-1;
	struct sockaddr_in  their_addr;
	sockfd=SockServer_Setup(sockfd, ALL_ITEM_SOCKET_PORT);

#ifdef ITUNER_TRANS

	AmbaTUNE_Change_Parser_Mode(TEXT_TUNE);
	if (0 != AmbaTUNE_Init()) {
	    printf("%s() %d, call AmbaTUNE_Init() Fail", __func__, __LINE__);
	    return -1;
	}
#endif
	pthread_t thread_3A_info;
	if((pthread_create(&thread_3A_info,NULL,(void*)histo_thread_proc,NULL)!=0))
		printf("create a new thread fail!\n");

	pthread_t thread_menucmd;
	if(debug_enble){
		if((pthread_create(&thread_menucmd,NULL,(void*)menucmd_thread_proc,NULL)!=0))
			printf("create a new thread fail!\n");
	}
	while(1)
	{

		if(new_fd!=-1)
			close(new_fd);
		socklen_t sin_size=sizeof(struct sockaddr_in);
		signal(SIGPIPE, SIG_IGN);
		if((new_fd=accept(sockfd,(struct sockaddr*)&their_addr,&sin_size))==-1)
		{
			perror("accapt");
			continue;
		}
		TUNING_ID tuning_id;
		int rev =-1;
		rev=recv(new_fd,(char*)&tuning_id,sizeof(TUNING_ID),0);
		if(rev <0)
		{
			printf("recv error[%d]\n",rev);
			break;
		}
	//	printf("size %d,require %c,item %c,tab %c\n",sizeof(TUNING_ID),
	//		tuning_id.req_id,tuning_id.item_id,tuning_id.tab_id);
		process_tuning_id(new_fd,&tuning_id);
	}
	SockServer_free(sockfd,new_fd);
	return 0;

}

