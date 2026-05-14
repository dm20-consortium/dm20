#include "I2vSignatureProcessor.h"
#include <iostream>
#include <fstream>
namespace SEC{

    bool I2vSignatureProcessor::checkClientSignature(const std::string &conf_dir_path, const unsigned long long src_station_id) {
        struct stat buffer;
        std::string key_path = conf_dir_path + "cs_udp_pki/" + std::to_string(src_station_id) + ".private.pem";
		if (stat(key_path.c_str(), &buffer) != 0) {
			std::cout << "\nERROR: no private key found Path:[" << key_path << "]" << std::endl;
			return false;
		}
		return true;
	}
    bool I2vSignatureProcessor::checkServerSignature(const std::string &conf_dir_path) {
        struct stat buffer;
		std::string ca_path = conf_dir_path + "cs_udp_pki/ca.cert.pem";
		if (stat(ca_path.c_str(), &buffer) != 0) {
			std::cout << "\nERROR: no ca-certificate found Path:[" << ca_path << "]" << std::endl;
			return false;
		}
		return true;
	}
    size_t I2vSignatureProcessor::makeSignature(std::string buf_str, unsigned char *sig, std::string &conf_dir_path, unsigned long long src_station_id) {
        // 署名作成
        std::string key_file = conf_dir_path + "cs_udp_pki/" + std::to_string(src_station_id) + ".private.pem";
		struct stat buffer;
        if(stat(key_file.c_str(), &buffer) == 0){
            BIO* pkey_bio = BIO_new_file(key_file.c_str(), "r");
			if(!pkey_bio) {
                std::cout << "[makeSignature]file read error: " << key_file << std::endl;
				return 0;
			}
            EVP_PKEY* pkey = PEM_read_bio_PrivateKey(pkey_bio, NULL, NULL, NULL);
            if (pkey == NULL){
                //LOG4CXX_WARN(logger, "pkey NULL");
                std::cout << "pkey NULL" << std::endl;
            
            } else {
                //LOG4CXX_WARN(logger, EVP_PKEY_bits(pkey));
                //std::cout << EVP_PKEY_bits(pkey) << std::endl;
            }
            BIO_free(pkey_bio);
            EVP_MD_CTX *mctx = NULL;
            mctx = EVP_MD_CTX_create();
            EVP_MD_CTX_init(mctx);

            if (1 != EVP_DigestSignInit(mctx, NULL, EVP_sha256(), NULL, pkey)){
                //LOG4CXX_WARN(logger, "Init error");
                std::cout << "Init error" << std::endl;
            }
            if (1 != EVP_DigestSignUpdate(mctx, (unsigned char *)buf_str.c_str(), buf_str.length())){
                //LOG4CXX_WARN(logger, "Update error");
                std::cout << "Update error" << std::endl;
            }

            size_t slen;
            if (1 != EVP_DigestSignFinal(mctx, nullptr, &slen)){
                //LOG4CXX_WARN(logger, "Final1 error");
                std::cout << "Final1 error" << std::endl;
            }			
            if (slen == 0){
                //LOG4CXX_WARN(logger, "slen 0");
                std::cout << "slen 0" << std::endl;
            }

            //unsigned char *sig = (unsigned char *)OPENSSL_malloc(slen);
            void *_sig = OPENSSL_malloc(slen);

            if (1 != EVP_DigestSignFinal(mctx, (unsigned char *)_sig, &slen)){
                //LOG4CXX_WARN(logger, "Final2 error");
                std::cout << "Final2 error" << std::endl;
            }
            //LOG4CXX_WARN(logger, slen);
            //std::cout << slen << std::endl;

            memcpy(sig, (unsigned char *)_sig, slen);//char[72]

            OPENSSL_free(_sig);
            if(mctx){
                EVP_MD_CTX_destroy(mctx);
            }
			//std::cout << "[I2vSignatureProcessor::makeSignature] SIGN SIZE=" << buf_str.size()	<< std::endl;
			//for (size_t i = 0; i < buf_str.size(); i++) printf("%02x ",	(unsigned char)buf_str[i]);
			//printf("\n");
            return slen;
        }else{
            return 0;
        }

    }

    bool I2vSignatureProcessor::verifySignature(unsigned long long signer_station_id,short sign_size, unsigned char *sign, short message_size, unsigned char *message, std::string &conf_dir_path) {
        std::string cert_path = conf_dir_path + "cs_udp_pki/" + std::to_string(signer_station_id) + ".cert.pem";
		// CA証明書
		std::string ca_path = conf_dir_path + "cs_udp_pki/ca.cert.pem";
		// 証明書存在確認
		//--------------------------------------------------
		struct stat buffer;

		//std::cout << "[I2vSignatureProcessor::verifySignature]VERIFY SIZE=" << message_size << std::endl;
		//for (size_t i = 0; i < message_size; i++) printf("%02x ", (unsigned char)message[i]);
		//printf("\n");

		if (stat(cert_path.c_str(), &buffer) != 0) {
			std::cout << "certificate not found" << std::endl;
			return false;
		}
		// 証明書読み込み
		BIO* cert_bio = BIO_new_file(cert_path.c_str(), "r");
		//EVP_PKEY* pubkey = PEM_read_bio_PUBKEY(pubkey_bio, NULL, NULL, NULL);
		X509* cert = PEM_read_bio_X509(cert_bio, NULL, NULL, NULL);
		BIO_free(cert_bio);
		if(cert == NULL){
			std::cout << "pubkey NULL" << std::endl;
			return false;
		}
			
		// CA証明書読み込み
		BIO* ca_bio = BIO_new_file(ca_path.c_str(), "r");
		X509* ca_cert = PEM_read_bio_X509(ca_bio, NULL, NULL, NULL);
		BIO_free(ca_bio);
		if (ca_cert == NULL) {
			std::cout << "CA cert NULL" << std::endl;
			X509_free(cert);
			return false;
		}

    	// CA公開鍵取得
	    EVP_PKEY* ca_pubkey =
        X509_get_pubkey(ca_cert);
		
    	// CA署名検証
		if (X509_verify(cert, ca_pubkey) != 1) {
			std::cout << "certificate verify failed" << std::endl;
			EVP_PKEY_free(ca_pubkey);
			X509_free(cert);
			X509_free(ca_cert);
			return false;
		}
    	EVP_PKEY_free(ca_pubkey);

		// 公開鍵取得
		EVP_PKEY* pubkey = X509_get_pubkey(cert);

		if (pubkey == NULL) {
			std::cout << "pubkey NULL" << std::endl;
			X509_free(cert);
			X509_free(ca_cert);
			return false;
		}

		// 署名検証
		EVP_MD_CTX * mctx = EVP_MD_CTX_new();

		EVP_DigestVerifyInit(mctx, NULL, EVP_sha256(), NULL, pubkey);
		EVP_DigestVerifyUpdate(mctx, message, message_size);
		int verifyResult = EVP_DigestVerifyFinal(mctx, sign, sign_size);
		EVP_MD_CTX_free(mctx);
		EVP_PKEY_free(pubkey);
		X509_free(cert);
		X509_free(ca_cert);

		if (verifyResult != 1) {
			std::cout << "Verify Faild" << std::endl;
			return false;
		}
		
		return true;
	}
}