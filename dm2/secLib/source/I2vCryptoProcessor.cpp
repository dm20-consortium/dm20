#include "I2vCryptoProcessor.h"

namespace SEC{

	/**
	* @fn	std::string I2vCryptoProcessor::generateHMAC(const std::string& data, const std::string& key)
	*
	* @brief	HMAC生成
	* @author	Shinichi Kusayama
	* @date		2026/05/08
	*
	*/
	std::string I2vCryptoProcessor::generateHMAC(const std::string& data, const std::string& key) {
		unsigned char hmac[EVP_MAX_MD_SIZE];
		unsigned int hmacLen = 0;

		HMAC(EVP_sha256(), key.c_str(), key.length(), reinterpret_cast<const unsigned char*>(data.data()),
			data.length(), hmac, &hmacLen);
		return std::string(reinterpret_cast<char*>(hmac), hmacLen);
	}

	/**
	* @fn	std::string I2vCryptoProcessor::encryptMessage(const std::string& message, std::string iv_, const std::string& aesKey) 
	*
	* @brief	メッセージをAES128で暗号化した後、MACを付与する関数
	* @author	Shinichi Kusayama
	* @date		2026/05/08
	*
	*/
	std::string I2vCryptoProcessor::encryptThenMACMessage(const std::string& message, const std::string &key) {
		unsigned char iv[IV_LEN];
		// IV生成
		if (RAND_bytes(iv, IV_LEN) != 1) {
			throw std::runtime_error("RAND_bytes failed");
		}

		//std::cout << "[Encrypt] IV: ";
		//for (int i = 0; i < IV_LEN; i++) printf("%02x ", iv[i]);
		//std::cout << std::endl;

		// AES Encrypt
		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.data()), iv);

		int maxCipherLen = message.length() + EVP_CIPHER_block_size(EVP_aes_128_cbc());

		unsigned char* cipherText = new unsigned char[maxCipherLen];

		int cipherLen1 = 0;
		int cipherLen2 = 0;

		EVP_EncryptUpdate(ctx, cipherText, &cipherLen1, reinterpret_cast<const unsigned char*>(message.data()),
			message.length());

		EVP_EncryptFinal_ex(ctx, cipherText + cipherLen1, &cipherLen2);

		int totalCipherLen = cipherLen1 + cipherLen2;

		EVP_CIPHER_CTX_free(ctx);

		// IV + CipherText
		std::string encryptedData(reinterpret_cast<char*>(iv), IV_LEN);

		encryptedData.append(reinterpret_cast<char*>(cipherText), totalCipherLen);

		delete[] cipherText;

		// HMAC生成
		std::string hmac = generateHMAC(encryptedData, key);

		// 最終データ
		// [ IV ][ CipherText ][ HMAC ]
		encryptedData.append(hmac);

		return encryptedData;

	}

	/**
	* @fn	std::string I2vCryptoProcessor::decryptEtMMessage(const std::string& message, std::string iv_, const std::string& aesKey) 
	*
	* @brief	HMACを検証した後、暗号化メッセージをAES128で復号化する関数
	* @author	Shinichi Kusayama
	* @date		2026/05/08
	*
	*/
	std::string I2vCryptoProcessor::decryptEtMMessage(const std::string& encryptedMessage, const std::string& key) {
		// サイズチェック
		if (encryptedMessage.length() < IV_LEN + HMAC_LEN) {
			throw std::runtime_error("message too short");
		}
		// HMAC分離
		int encryptedDataLen = encryptedMessage.length() - HMAC_LEN;
		std::string encryptedData = encryptedMessage.substr(0, encryptedDataLen);
		std::string receivedHMAC = encryptedMessage.substr(encryptedDataLen, HMAC_LEN);

		// HMAC検証
		std::string computedHMAC = generateHMAC(encryptedData, key);
		if (computedHMAC != receivedHMAC) {
			throw std::runtime_error("HMAC verification failed");
		}
		//std::cout << "[Decrypt] HMAC OK" << std::endl;

		// IV取り出し
		unsigned char iv[IV_LEN];
		memcpy(iv, encryptedData.data(), IV_LEN);
		//std::cout << "[Decrypt] IV: ";
		//for (int i = 0; i < IV_LEN; i++) printf("%02x ", iv[i]);
		//std::cout << std::endl;

		// CipherText取り出し
		int cipherLen = encryptedData.length() - IV_LEN;

		const unsigned char* cipherText = reinterpret_cast<const unsigned char*>(encryptedData.data() + IV_LEN);

		// AES Decrypt
		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.data()), iv);

		unsigned char* plainText = new unsigned char[cipherLen + AES_BLOCK_SIZE];

		int plainLen1 = 0;
		int plainLen2 = 0;

		EVP_DecryptUpdate(ctx, plainText, &plainLen1, cipherText, cipherLen);
		EVP_DecryptFinal_ex(ctx, plainText + plainLen1, &plainLen2);
		int totalPlainLen = plainLen1 + plainLen2;
		EVP_CIPHER_CTX_free(ctx);

		std::string result(reinterpret_cast<char*>(plainText), totalPlainLen);

		delete[] plainText;

		return result;
	}

}