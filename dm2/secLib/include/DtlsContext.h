#ifndef DTLSCONTXT_H
#define DTLSCONTXT_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <netinet/in.h>   //sockaddr_in用
#include <arpa/inet.h>    //inet_addr用
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/engine.h>
#include <sys/stat.h>


namespace SEC{
    #define COOKIE_SECRET_LENGTH 16
    class DtlsContext
	{
	private:
        const std::string SSL_CERTIFICATE_FILE_ABS_PATH = "cs_dtls/certs/server-cert.pem";
		const std::string SSL_PRIVATE_KEY_FILE_ABS_PATH = "cs_dtls/certs/server-key.pem";
		const std::string SSL_CERTIFICATE_CLIENT_PATH = "cs_dtls/certs/client-cert.pem";
		const std::string SSL_PRIVATE_KEY_CLIENT_PATH = "cs_dtls/certs/client-key.pem";
		const std::string SSL_CERTIFICATE_VERIFY_PATH = "cs_dtls/certs/ca-cert.pem";

        static unsigned char cookie_secret[COOKIE_SECRET_LENGTH];
        static int cookie_initialized;
        static int generate_cookie(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len);
		static int verify_cookie(SSL *ssl, const unsigned char *cookie, unsigned int cookie_len);

    public:

        bool CheckClientContext(std::string confDirPath);
        SSL_CTX* CreateServerContext(std::string confDirPath);
		SSL_CTX* CreateClientContext(std::string confDirPath);
    };
}
#endif