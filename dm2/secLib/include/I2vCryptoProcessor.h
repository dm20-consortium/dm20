#ifndef I2VCRYPTOPROCESSOR_H
#define I2VCRYPTOPROCESSOR_H


#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/aes.h>


namespace SEC{
	class I2vCryptoProcessor{
		private:
			static const std::string aesKey;
			static constexpr int AES_KEY_LEN = 16;   // AES-128
			static constexpr int IV_LEN      = 16;   // AES block size
			static constexpr int HMAC_LEN    = 32;   // SHA256

		public:
			static std::string generateHMAC(const std::string& data, const std::string& key);
			static std::string encryptThenMACMessage(const std::string& message, const std::string &key);
			static std::string decryptEtMMessage(const std::string& encryptedMessage, const std::string& key);
	};
}

#endif