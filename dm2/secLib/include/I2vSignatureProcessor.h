#ifndef I2VSIGNATUREPROCESSOR_H
#define I2VSIGNATUREPROCESSOR_H


#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/pem.h>

namespace SEC{
	class I2vSignatureProcessor{
        private:

		public:
            static bool checkClientSignature(const std::string &conf_dir_path, const unsigned long long src_station_id);
            static bool checkServerSignature(const std::string &conf_dir_path);
            static size_t makeSignature(std::string buf_str, unsigned char *sig, std::string &conf_dir_path, unsigned long long src_station_id);
            static bool verifySignature(unsigned long long signer_station_id,short sign_size, unsigned char *sign, short message_size, unsigned char *message, std::string &conf_dir_path);
	};
}

#endif