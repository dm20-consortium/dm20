#ifndef STRUCT_H
#define STRUCT_H

#include <unistd.h> //close
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <netinet/in.h>   //sockaddr_in用
#include <arpa/inet.h>    //inet_addr用
#include <netdb.h>        //addrinfo, AI_PASSIVE, NI_MAXHOST等
#include <typeinfo>     //typeid用
#include <array>
#include <vector>
	

namespace CS{

	/**
	 * @enum	MSG_ERROR
	 *
	 * @brief	エラーコード
	 *
	 * @author	Nagoya University
	 * @date	2020/03/23
	 */
	enum MSG_ERROR{
		/** @brief	-100 */
		BIND_ERROR = -100
	};


	/**
	 * @enum	clientdata
	 *
	 * @brief	DM2.0ヘッダに載せるメッセージ種別
	 *
	 * @author	Nagoya University
	 * @date	2020/03/23
	 */
	enum MSG_TYPE{
		/** @brief	1 */
		DM2TYPE_CS = 1, 
		/** @brief	2 */
		DM2TYPE_IS = 2, 
		/** @brief	3 */
		DM2TYPE_APL = 3,
		/** @brief	4 */
		DM2TYPE_DMMNG = 4,
		/** @brief	5 */
		DM2TYPE_SEC = 5,
		/** @brief	7 */
		DM2Type_MNG_CONN_REGIST = 7
	};


	/**
	 * @enum	CS_message_detail
	 *
	 * @brief	DM2.0ヘッダに載せるメッセージ種別
	 *
	 * @author	Ryosuke Takeuchi 
	 * @date	2024/10/04
	 */
	enum CS_MESSAGE_DETAIL{
		/** @brief	1 */
		REQUEST = 1, 
		/** @brief	2 */
		RESPONSE = 2
	};

	/**
	 * @enum	PROCESS_NAME
	 *
	 * @brief	トレースポイントのプロセス名称
	 *
	 * @author	Nagoya University
	 * @date	2020/03/23
	 */
	enum PROCESS_NAME{
		/** @brief	CloudNwRcvMain */
		CNWMN = 1,
		/** @brief	CloudNwRcvWorkerThread */
		CNWWT = 2,
		/** @brief	CloudProcRcvMain */
		CPRMN = 3,
		/** @brief	CloudProcRcvWorkerThread */
		CPRWT = 4,
		/** @brief	CloudProcRcvRetrydataThread */
		CPRRT = 5,
		/** @brief	EdgeNwRcvMain */
		ENWMN = 6,
		/** @brief	EdgeNwRcvWorkerThread */
		ENWWT = 7,
		/** @brief	EdgeProcRcvMain */
		EPRMN = 8,
		/** @brief	EdgeProcRcvWorkerThread */
		EPRWT = 9,
		/** @brief	EdgeProcRcvRetrydataThread */
		EPRRT = 10,
		/** @brief	VehicleNwRcvMain */
		VNWMN = 11,
		/** @brief	VehicleNwRcvWorkerThread */
		VNWWT = 12,
		/** @brief	VehicleProcRcvMain */
		VPRMN = 13,
		/** @brief	VehicleProcRcvWorkerThread */
		VPRWT = 14,
		/** @brief	VehicleProcRcvRetrydataThread */
		VPRRT = 15
	};


	/**
	 * @enum	MSG_DETAIL
	 *
	 * @brief	ACCEANヘッダのcs_message_detailの値
	 *
	 * @author	Nagoya University
	 * @date	2020/03/23
	 */
	enum MSG_DETAIL{
		/** @brief	4 */
		DM2CS_MSG_ACK = 4,
		/** @brief	5 */
		DM2CS_MSG_TRACELOG_SET = 5,
		/** @brief	6 */
		DM2CS_MSG_TRACELOG_GET = 6,
		/** @brief	7 */
		DM2CS_MSG_DMMNG_TRACE_LOG_GET = 7,
		/** @brief	8 */
		DM2CS_MSG_DMMNG_TRACE_LOG_RES = 8,
		/** @brief	9 */
		DM2CS_MSG_LID_WAVE_QUALITY = 9,
		/** @brief	10 */
		DM2CS_MSG_SET_RCV_EDGE = 10,
		/** @brief	11 */
		DM2CS_MSG_GET_RCV_EDGE = 11,
		/** @brief	12 */
		DM2CS_MSG_RES_RCV_EDGE = 12,
		/** @brief	13 */
		DM2CS_MSG_DELETE_VEHICLE = 13
	};

	enum NODE_TYPE : short {
		CAR = 0,
		EDGE = 1,
		CLOUD = 2
	};

	/** @def dm2_payloadのサイズ(byte) */
	//#define MSGSIZE 1280
	#define MSGSIZE 64000

	/** @def トレースログ最大ポイント数 */
	#define TRACEPOINT 20

	/**
	 * @struct	tracelog
	 *
	 * @brief	トレースログ構造体
	 *
	 * @author	Nagoya University
	 * @date	2020/03/23
	 */
	struct tracelog{
		/** @brief	トレースポイントのSID */
		//int station_id;
		unsigned long long station_id;
		/** @brief	トレースポイントのプロセス名称： 0(xxNwRcv), 1(xxProcRcv) xxはCloud, Edge, Vehicleのいずれか*/
		short process_name;
		/** @brief	トレース対象のキューサイズ */
		int queue_size;
		/** @brief	tracelog取得時刻 */
		unsigned long long log_time;
	};


	/**
	 * @struct	lid_wave_quality
	 *
	 * @brief	LID毎電波強度情報格納構造体
	 *
	 * @author	Nagoya University
	 * @date	2020/03/23
	 */
	struct lid_wave_quality{
		/** @brief	Lane ID */
		//int lid;
		unsigned long long lid;
		/** @brief	Lane IDの電波強度*/
		float wave_quality;
	};

	/**
	 * @struct	send_message
	 *
	 * @brief	DM2.0通信データフレームフォーマット
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	struct send_message{
		/** @brief	送信元SID */
		unsigned long long src_station_id;
		/** @brief	宛先SID */
		unsigned long long dst_station_id;
		/** @brief	送信元type */
		short src_station_type;
		/** @brief	宛先type */
		short dst_station_type;
		/** @brief	ペイロードサイズ */
		unsigned short payload_size;
		/** @brief	転送フラグ */
		short transmission_flag;
		/** @brief	重複チェックID */
		unsigned long long duplication_check_id;
		/** @brief	レーンID */
		unsigned long long lane_id;
		/** @brief	再送レベル */
		short retry_level;
		/** @brief	再送データID */
		unsigned long long retry_data_id;
		/** @brief	再送時間(ms) */
		int retry_lifetime;
		/** @brief	メッセージ種別 */
		short msg_type;
		/** @brief	CS詳細メッセージ  */
		short cs_message_detail;
		/** @brief	ファイルディスクリプタ名称 (署名格納用としても使用される) */
		char fd_name[72];
		/** @brief	署名サイズ */
		short sign_size;
		/** @brief	フラグメントシーケンス番号 */
		unsigned long long flagment_duplication_check_id;
		/** @brief	フラグメントトータル */
		int flagment_sum;
		/** @brief	フラグメントオフセット */
		int flagment_offset;
		#if TRACELOG == 1
		/** @brief	トレースログ */
		std::array<tracelog, TRACEPOINT> tracelogs;
		#endif
		/** @brief 優先度フラグ */
		int priority_level;
		#if LID_PRIORITY == 1
		/** @brief LID毎優先機能によるポリシング時間 */
		int policing_time;
		#endif
		/** @brief	DM2.0データペイロード */
		char dm2_payload[MSGSIZE];
	};

	struct send_message_mng {
	    unsigned long long sid;
	    char ip[NI_MAXHOST];
	    char ctl_flag[20];
	};

	/**
	 * @struct	send_message_upper
	 *
	 * @brief	IS/APLインタフェースデータ構造体
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	struct send_message_upper{
		/** @brief	送信元SID */
		//int src_station_id;
		unsigned long long src_station_id;
		/** @brief	宛先SID */
		//int dst_station_id;
		unsigned long long dst_station_id;
		/** @brief	レーンID */
		//int lane_id;
		unsigned long long lane_id;
		/** @brief	再送レベル */
		short retry_level;
		/** @brief	再送時間(s) */
		int retry_lifetime;
		/** @brief  再送間隔(ms) */
		int retry_interval;
		/** @brief  IPアドレス */
		char from_ip[NI_MAXHOST];
		/** @brief	DM2.0データペイロード */
		std::string dm2_payload;
	};


	/**
	 * @struct	csmng_message
	 *
	 * @brief	CS内通信データ構造体
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	struct csmng_message{
		/** @brief	メッセージ種別 */
		short msg_type;
		/** @brief	CS詳細メッセージ  */
		short cs_message_detail;
		/** @brief	トレースログ */
		std::array<tracelog, TRACEPOINT> tracelogs;
		/** @brief	DM2.0データペイロード */
		std::string dm2_payload;

	};


	/**
	 * @struct	mac_the_encrypt_message
	 *
	 * @brief	MtE用通信データ構造体
	 *
	 * @author	Ryosuke Takeuchi
	 * @date	2025/02/06
	 */
	 
	struct authenticated_encryption_message{
		/** @brief	署名者SID */
		unsigned long long signer_station_id;
		/** @brief	署名サイズ */
		short sign_size;
		/** @brief	署名 */
		char sign[72];
		/** @brief	メッセージサイズ */
		short message_size;
		/** @brief	メッセージ */
		//std::string message;
		char message[64168];
	};


	struct test_mac_the_encrypt_message{
		/** @brief	署名者SID */
		unsigned long long signer_station_id;
		/** @brief	署名サイズ */
		short sign_size;
		/** @brief	署名 */
		char sign[72];
		/** @brief	メッセージサイズ */
		short crt_size;
		/** @brief	署名 */
		char crt[2100];
		short message_size;
		/** @brief	メッセージ */
		//std::string message;
		char message[61966];
	};	

	/**
	 * @struct	security_crt_message
	 *
	 * @brief	security用通信データ構造体
	 *
	 * @author	Ryosuke Takeuchi
	 * @date	2025/02/10
	 */
	 struct security_crt_message{
		/** @brief	SID */
		unsigned long long station_id;
		/** @brief	bool */
		bool ea_res;
		/** @brief	crtサイズ */
		short crt_size;
		/** @brief	csrサイズ */
		short csr_size;
		/** @brief	メッセージ */
		char message[3000];
	};

	/**
	 * @struct	clientdata
	 *
	 * @brief	受信データと送信元IPアドレスの対応
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	struct clientdata{
		int priority_num;
		/** @brief	送信元IPアドレス */
		char from_ip[NI_MAXHOST];
		/** @brief	EdgeNwRcvのworkerでの処理のタイプ */
		int transfer_type;
		/** @brief	EdgeNwRcvのworkerで使用するレーンID管轄エッジSID */
		//int lid_controll_edge_sid;
		unsigned long long lid_controll_edge_sid;
		/** @brief	受信データ */
		struct send_message msg;

		bool operator<(const clientdata& rhs) const{
			return priority_num < rhs.priority_num;
		}
	};


	/**
	 * @struct	_iwconfig_status
	 *
	 * @brief	Wi-Fi電波品質情報構造体
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	struct _iwconfig_status{
		char essid[IW_ESSID_MAX_SIZE + 1];
		char mac_address[18];
		int qual;
		int max_qual;
	};

	/******************************************
	* Station ID(SID)の設定範囲
	*******************************************/
	/** @brief	車両SIDの最小設定値 */
	#define MINIMUM_VEHICLE_SID           1
	/** @brief	車両SIDの最大設定値 */
	#define MAXIMUM_VEHICLE_SID    90000000
	/** @brief	エッジSIDの最小設定値 */
	#define MINIMUM_EDGE_SID     1900000001
	/** @brief	エッジSIDの最大設定値 */
	#define MAXIMUM_EDGE_SID     1920000000
	/** @brief	クラウドSIDの最小設定値 */
	#define MINIMUM_CLOUD_SID    1920000001
	/** @brief	クラウドSIDの最大設定値 */
	#define MAXIMUM_CLOUD_SID    2000000000
}

#endif