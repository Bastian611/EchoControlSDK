#ifndef _____FUNCTION_LIST_H____
#define _____FUNCTION_LIST_H____

//for system control config
#define MAX_SYSTEM_CONTROL_STRING_LEN	2048

#define FUNCTION_SUPPORT_STORAGE  		 "storage_support"

#define FUNCTION_WIRELESS_STATION			"wireless_station"
#define FUNCTION_WIFI_AP 					"wifi_ap"
#define FUNCTION_WIFI_AP_STATION_SAMETIME		"ap_station"

#define FUNCTION_NETWORK_STORAGE		"network_storage"
#define FUNCTION_FTPEMAIL_STORAGE			"ftpemail_aj"
#define FUNCTION_EMAIL_SSL			"ssl_email"
#define FUNCTION_SCHEDULE_RECORD		"schedule_record"
#define FUNCTION_PICTURE_CAPTURE		"picture_capture"
#define FUNCTION_PTZ_CONTROL			"ptz_control"
#define FUNCTION_GPIO_INPUT				"gpio_input"
#define FUNCTION_GPIO_OUTPUT			"gpio_output"
#define FUNCTION_PTZ_ACTION			"ptz_action"
#define FUNCTION_AUDIOPLAY_ACTION	"audio_action"
#define FUNCTION_RA_ANSWER	"ra_answer"
#define FUNCTION_RA_PCM	"ra_pcm"	
#define FUNCTION_RA_MP3STREAM	"ra_mp3"	
#define FUNCTION_UPNP	"enable_upnp"	


#define FUNCTION_THREE_VIDEO			"three_video"	//是否支持VIDEO第三码流
#define FUNCTION_YUV_VIDEO				"yuv_video"		//是否支持YUV VIDEO 跨进程读取
#define FUNCTION_ONLY_AUDIO				"only_audio"	//是否仅运行音频编码
#define FUNCTION_VIDEO_FORBIT			"forbid_video"	//是否可进制视频流
#define FUNCTION_VIDEO_ROI				"video_roi"		//是否可设置视频兴趣区域
#define FUNCTION_LED_TYPE				"ledtype_set" 	//是否可设置白光/红外光
#define FUNCTION_VIDEO_ENCODE_MODE		"vencodemode_set" //是否可设置视频编码模式(图像模式)
#define FUNCTION_USEROSD		"userosd_set" //是否可设置自定义OSD
#define FUNCTION_TITLE_BMP		"bmplogo_set" //是否可设置BMP LOGO图片
#define FUNCTION_TITLE_COLOR	"titlecolor_set" //是否可设置title颜色图片
#define FUNCTION_TITLE_TRANSPARENT	"alpha_set" //是否可设置透明度

#define FUNCTION_FRONT_REPLAY			"front_replay"
#define FUNCTION_REPLAY_BYTIME		"replay_bytime"
#define FUNCTION_MEDIA_CAPABILITY		"media_capabiltiy"
#define FUNCTION_IRCUT_SETTING			"ircut_setting"   //IRCUT 工作模式(自动， 定时， 外部控制， 手动)
#define FUNCTION_IRCUT_LED_DELAY		"ircut_leddelay" // 补光延时配置，用于软光敏方式， 可以开补光灯的灵敏度
#define FUNCTION_PROFLE_SETTING		"profile_setting"
#define FUNCTION_WDR_SETTING			"wdr_setting"  //DSP 数字宽动态 设置
#define FUNCTION_HDR_SETTING			"hdr_setting"  //SENSOR 宽动态设置
#define FUNCTION_VIDEO_MASK			"video_mask"    //视频遮挡设置
#define FUNCTION_SYSTEM_MAINTAIN		"system_maitain"
#define FUNCTION_LINKDOWN_RECORD		"linkdown_record"
#define FUNCTION_PPTP					"pptp_support"

#define FUNCTION_AUDIO					"audio_support"

#define FUCNTION_3G_EVDO				"evdo_support"
#define FUCNTION_3G_WCDMA				"wcdma_support"
#define FUCNTION_3G_TDSCDMA			"tdscdma_support"


#define FUNCTION_LANGUAGE_ZH_CN		"zh_cn"
#define FUNCTION_LANGUAGE_ZH_TW		"zh_tw"
#define FUNCTION_LANGUAGE_ZH_HK		"zh_hk"
#define FUNCTION_LANGUAGE_EN_US		"en_us"
#define FUNCTION_LANGUAGE_RU_PY		"ru_py"
#define FUNCTION_LANGUAGE_TR_TR		"tr-tr"
#define FUNCTION_LANGUAGE_KO_KO		"ko-ko"
#define FUNCTION_LANGUAGE_CZ_CZEKH	"cz-czekh"


#define FUNCTION_SEARCH_WIFIAP		 	"SEARCH_WIFIAP"
#define FUNCTION_LONG_TITLE		 	"LONG_TITLE"
#define FUNCTION_TIMEZONE_HALFHOUR	"timezone_halfhour"
#define FUNCTION_P2P_CFG				"p2p_cfg_support"

#define FUNCTION_SUPPORT_RECORD_AJV  "ajv_support"
#define FUNCTION_MD_18X22				"md_18x22"
#define FUNCTION_ONLY_18X22			"only_18x22"
#define FUNCTION_AMBAR_ENCMODE		"encmode_setting"
#define FUNCTION_SUPPORT_LBR			"lbr_support"
#define FUNCTION_SUPPORT_28181			"gb28181"
#define FUNCTION_SUPPORT_IPVS			"ipvs"
#define FUNCTION_SUPPORT_RTMP	"rtmp"
#define FUNCTION_HIK_CONFIG		"hikconfig"	
#define FUNCTION_COMM_ONVIF_ENABLE		"commenable"	
#define FUNCTION_DH_CONFIG		"dhconfig"	

#define FUCNTION_VOIP			"voip"
#define FUNCTION_ROTATE		"rotate_enable"   //视频旋转
#define FUNCTION_AUDIO_AMPLIFY		"audio_amplify"
#define FUNCTION_ALOWIP_SETTING		"ipaddrlimit"
#define FUNCTION_OVERLAYFPS_SETTING		"overlayfps"//叠加帧率
#define FUNCTION_GAIN_SETTING		"gainsetting"//手动增益/自动增益
#define FUNCTION_ALARMCLOCK_SETTING		"alarmclock"
#define FUNCTION_ALARM_VIDEOGATE		"VideoGate"	//电子围栏
#define FUNCTION_ISP_MODE		"ispmode"
#define FUNCTION_ALARM_PD		"VideoPD"	//人形检测
#define FUNCTION_FACE_FD		"face_detect"	//人脸检测
#define FUNCTION_FACE_FR		"face_recognize"//人脸匹配

#define FUCTION_PD_POLYGON     "pd_polygon_area"  //人形支持多边形监控区域
#define FUCTION_PD_MADP        "pd_madp"  // mstar 的人形算法
#define FUNCTION_OSD_ANYPOSITION		"OSD_ANYPOS"	//OSD任意位置
#define FUNCTION_VIDEO_CROP		"video_crop"	//视频裁剪
#define FUNCTION_VIDEO_FORCT_ANTIFLICKER		"antiflicker"	//强制抗闪
#define FUCTION_ADVANCE_LIGHT_CONTROL         "advance_light_ctrl"     //远瞻提出的灯光控制功能， 包括告警触发亮度，值守亮度
#define FUCTION_IO_OUTPUT_SET              "io_output_set"   //IO 输出配置
#define FUNCTION_ALARM_TEMP_HUMIDITY	"temp_humidity"	//温湿度检测  temperature  humidity
//#define FUNCTION_ALARM_TEMPERATURE	"temperature_set"
//#define FUNCTION_ALARM_HUMIDITY		"humidity_set"


#define FUCTION_ONE_OUTPUT                 "one_output"
#define FUCTION_TWO_OUTPUT                 "two_output"
#define FUCTION_THREE_OUTPUT                 "three_output"
#define FUCTION_FOUR_OUTPUT                 "four_output"

#define FUCTION_ONE_INPUT                 "one_input"
#define FUCTION_TWO_INPUT                 "two_input"
#define FUCTION_THREE_INPUT                 "three_input"
#define FUCTION_FOUR_INPUT                 "four_input"


#define FUNCTION_VIDEOMASK_ONESET	"ONEVIDEOMASK"		//主子码流使用同一套隐私遮挡，不需要分开配置
#define FUNCTION_MULTICAST	"multicast"	
#define FUNCTION_VIDEOSHUTTER	"VideoShutter"	//电子快门设置

#define FUNCTION_MAC_MODIFY	"ModifyMac"	
#define FUNCTION_AUDIO_ALARM	"AudioAlarm"	


//海思编码支持宽动态、去雾功能，参数范围0-255
#define FUNCTION_HISCON_ENCMODE		"hisconenc"	

#define FUNCTION_P2P_CONFIG	"p2p_config"	
#define FUNCTION_P2P_DANALE	"p2p_danale"	
#define FUNCTION_P2P_ANKO		"p2p_anko"	
#define FUNCTION_P2P_GOOLINK	"p2p_goolink"	
#define FUNCTION_P2P_ISMART	"p2p_ismart"	
#define FUNCTION_P2P_QQ		"p2p_qq"
#define FUNCTION_P2P_EYEPLUS		"p2p_eyeplus"

#define FUNCTION_PRE_RECORD   "pre_record"
#define FUNCTION_UPLOAD_SOFTCRYPT   "crypt_upload"
#define FUNCTION_OSD_GB28181  "osd_gb"  //国标字体大小
#define FUNCTION_AF_VERSION		"af_setting"//AF设置
#define FUNCTION_AUDIO_8M_Repartition		"audio_Repartition"//AF设置

#define FUNCTION_OTA_SUPPORT		"ota_enable"
#endif       

