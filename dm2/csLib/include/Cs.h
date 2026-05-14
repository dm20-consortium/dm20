#ifndef CS_H
#define CS_H

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <unistd.h> //close
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <netinet/in.h>   //sockaddr_in用
#include <arpa/inet.h>    //inet_addr用
#include <netdb.h>        //addrinfo, AI_PASSIVE, NI_MAXHOST等
#include <typeinfo>     //typeid用
#include <net/if.h>     //if_nametoindex用
#include <time.h>
#include <ifaddrs.h>    //getifaddrs用
#include <errno.h>

//ここから電波強度測定用
#include <netinet/icmp6.h>
#include <cstdio>
#include <ext/stdio_filebuf.h>
#include <linux/wireless.h>
#include <netinet/ether.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/wait.h>
// ここまで

//RADWIN読み込み用
#include <ext/stdio_filebuf.h>


#include "Struct.h"
#include "Socket.h"
#include "SocketUdp.h"
#include "UdpProcServer.h"
#include "UdpProcClient.h"
//#include "DtlsNwServer.h"
//#include "DtlsNwClient.h"
#include "UdpNwServer.h"
#include "UdpNwClient.h"
#include "UdpMcNwServer.h"
#include "UdpMcNwClient.h"
#include "UnorderedMap.h"
#include "Map.h"
#include "Queue.h"
#include "Util.h"
#include <sys/epoll.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

namespace CS{
	/** @brief dm2.confファイルの指定 */
    const std::string CONF_FILE = "dm2.conf";

    // UDP
    const std::string UDP_IV_FILE = "cs_aes/iv.bin";
    
    // log4cxx
    const std::string CONF_FILE_CSMNG_LOG = "log4cxx/csmng.properties";
    const std::string CONF_FILE_CLOUDNWRCV_LOG = "log4cxx/cloudnwrcv.properties";
    const std::string CONF_FILE_CLOUDPROCRCV_LOG = "log4cxx/cloudprocrcv.properties";
    const std::string CONF_FILE_CLOUDRCVEDGEMNGRCV_LOG = "log4cxx/cloudrcvedgemngrcv.properties";
    const std::string CONF_FILE_EDGENWRCV_LOG = "log4cxx/edgenwrcv.properties";
    const std::string CONF_FILE_EDGEPROCRCV_LOG = "log4cxx/edgeprocrcv.properties";
    const std::string CONF_FILE_VEHICLEPROCRCV_LOG = "log4cxx/vehicleprocrcv.properties";
    const std::string CONF_FILE_VEHICLENWRCV_LOG = "log4cxx/vehiclenwrcv.properties";
    const std::string CONF_FILE_NWRCV_LOG = "log4cxx/dm2cs_recv.properties";
    const std::string CONF_FILE_PROCRCV_LOG = "log4cxx/dm2cs_send.properties";

    
    const std::string EDGE_SID_EDGE_IP_PATH = "cs_map/edge_sid_edge_ip.csv";
    const std::string CLOUD_SID_CLOUD_IP_PATH = "cs_map/cloud_sid_cloud_ip.csv";
    const std::string EDGE_LID_EDGE_SID_PATH = "cs_map/edge_lid_edge_sid.csv";

    const std::string LID_PRIORITY_PATH = "cs_map/lid_priority.csv";
    const std::string PRIORITY_POLICINGTIME_PATH = "cs_map/priority_policingtime.csv";
}
#endif
