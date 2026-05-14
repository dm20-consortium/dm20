#include "Cs.h"
namespace CS{
	/**
	 * @fn	int UdpProcServer::Init(const char* fd_name)
	 *
	 * @brief	初期化
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	fd_name	ファイルディスクリプタ名
	 *
	 * @return	ソケットID
	 */

	int UdpProcServer::Init(const std::string &fd_name)
	{
		return InitServer(fd_name.c_str());
	}

	/**
	 * @fn	int UdpProcServer::InitServer(const char* fd_name)
	 *
	 * @brief	接続シーケンス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	fd_name	ファイルディスクリプタ名
	 *
	 * @return	ソケットID
	 */

	int UdpProcServer::InitServer(const char* fd_name){

		//既存ソケットファイルの削除
		DeleteFd(fd_name);
		//アドレスの初期化
		server_addr_.sun_family = PF_UNIX;
		strcpy(server_addr_.sun_path,fd_name);
		
		//ソケット作成
		server_d_ = CreateSocket(PF_UNIX, SOCK_DGRAM);
		if(server_d_ < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "CreateSocket fail." << std::endl;
			exit(EXIT_FAILURE);
		}
		//Bind
		if(Bind(server_d_, server_addr_) < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Bind fail." << std::endl;
			server_d_ = BIND_ERROR;
		}

		return server_d_;

	}

	/**
	 * @fn	void UdpProcServer::DeleteFd(const char* fd_name)
	 *
	 * @brief	ソケットファイルが残っていたら消す
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	fd_name	ファイルディスクリプタ名
	 */

	void UdpProcServer::DeleteFd(const char* fd_name){
		struct stat file_stat;
		// ソケットファイルと同名のファイルがあるか
		if(stat(fd_name, &file_stat) == -1){
			if(errno != ENOENT){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "stat fail." << std::endl;
				perror("stat");
				exit(EXIT_FAILURE);
			}
		}

		// 同名のファイルがソケットファイルである場合
		if(S_ISSOCK(file_stat.st_mode)){
			if(unlink(fd_name) == -1){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "unlink fail." << std::endl;
				perror("unlink");
			}
		}
	}

}