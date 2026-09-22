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
#include "SocketUtil.h"
#include "SocketUdp.h"
#include "UdpProcServer.h"
#include "UdpProcClient.h"
//#include "DtlsNwServer.h"
//#include "DtlsNwClient.h"
#include "UdpNwServer.h"
#include "UdpNwClient.h"
#include "UdpMcNwServer.h"
#include "UdpMcNwClient.h"
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
    // UDP
    const std::string UDP_IV_FILE = "cs_aes/iv.bin";
    // log4cxx
    const std::string CONF_FILE_NWRCV_LOG = "log4cxx/dm2cs_recv.properties";
    const std::string CONF_FILE_PROCRCV_LOG = "log4cxx/dm2cs_send.properties";
}
#endif
