#include "DtlsContext.h"

using namespace log4cxx;

namespace SEC{
    unsigned char DtlsContext::cookie_secret[COOKIE_SECRET_LENGTH] = {};
    int DtlsContext::cookie_initialized = 0;
    LoggerPtr logger = Logger::getLogger("DtlsContext");


    int dtls_verify_callback(int ok, X509_STORE_CTX *ctx){
		//return 1;
		BIO *bio_err = BIO_new_fp(stdout, BIO_NOCLOSE);
		X509 *err_cert;
		int err, depth;

		err_cert = X509_STORE_CTX_get_current_cert(ctx);
		err = X509_STORE_CTX_get_error(ctx);
		depth = X509_STORE_CTX_get_error_depth(ctx);

		//BIO_printf(bio_err, "depth=%d ", depth);

		if (!ok){
			BIO_printf(bio_err, "verify error:num=%d:%s\n", err,
						X509_verify_cert_error_string(err));
			if (err_cert) {
				X509_print_ex(bio_err, err_cert, 0, 0);
				BIO_puts(bio_err, "\n");
			}
			else{
				BIO_puts(bio_err, "<no cert>\n");
			}
		}

		switch (err) {
			case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
				BIO_puts(bio_err, "issuer= ");
				X509_NAME_print_ex(bio_err, X509_get_issuer_name(err_cert),
									0, XN_FLAG_ONELINE);
				BIO_puts(bio_err, "\n");
				break;
			case X509_V_ERR_CERT_NOT_YET_VALID:
			case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
				BIO_printf(bio_err, "notBefore=");
				ASN1_TIME_print(bio_err, X509_get_notBefore(err_cert));
				BIO_printf(bio_err, "\n");
				break;
			case X509_V_ERR_CERT_HAS_EXPIRED:
			case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
				BIO_printf(bio_err, "notAfter=");
				ASN1_TIME_print(bio_err, X509_get_notAfter(err_cert));
				BIO_printf(bio_err, "\n");
				break;
		}

		//BIO_printf(bio_err, "verify return:%d\n", ok);
		return ok;
	}

    int DtlsContext::generate_cookie(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len){
		unsigned char *buffer, result[EVP_MAX_MD_SIZE];
		unsigned int length = 0, resultlength;
		union {
			struct sockaddr_storage ss;
			struct sockaddr_in6 s6;
			struct sockaddr_in s4;
		} peer;

		/* Initialize a random secret */
		if (!cookie_initialized){
			if (!RAND_bytes(cookie_secret, COOKIE_SECRET_LENGTH)){
				std::cout << "error setting random cookie secret" << std::endl;
				return 0;
			}
			cookie_initialized = 1;
		}

		/* Read peer information */
		(void) BIO_dgram_get_peer(SSL_get_rbio(ssl), &peer);

		/* Create buffer with peer's address and port */
		length = 0;
		switch (peer.ss.ss_family) {
			case AF_INET:
				length += sizeof(struct in_addr);
				break;
			case AF_INET6:
				length += sizeof(struct in6_addr);
				break;
			default:
				OPENSSL_assert(0);
				break;
		}
		length += sizeof(in_port_t);
		buffer = (unsigned char*) OPENSSL_malloc(length);

		if (buffer == NULL){
			std::cout << "out of memory" << std::endl;
			return 0;
		}

		switch (peer.ss.ss_family) {
			case AF_INET:
				memcpy(buffer,
					   &peer.s4.sin_port,
					   sizeof(in_port_t));
				memcpy(buffer + sizeof(peer.s4.sin_port),
					   &peer.s4.sin_addr,
					   sizeof(struct in_addr));
				break;
			case AF_INET6:
				memcpy(buffer,
					   &peer.s6.sin6_port,
					   sizeof(in_port_t));
				memcpy(buffer + sizeof(in_port_t),
					   &peer.s6.sin6_addr,
					   sizeof(struct in6_addr));
				break;
			default:
				OPENSSL_assert(0);
				break;
		}

		/* Calculate HMAC of buffer using the secret */
		HMAC(EVP_sha1(), (const void*) cookie_secret, COOKIE_SECRET_LENGTH,
			 (const unsigned char*) buffer, length, result, &resultlength);
		OPENSSL_free(buffer);

		memcpy(cookie, result, resultlength);
		*cookie_len = resultlength;

		return 1;
	}


	int DtlsContext::verify_cookie(SSL *ssl, const unsigned char *cookie, unsigned int cookie_len){
		unsigned char *buffer, result[EVP_MAX_MD_SIZE];
		unsigned int length = 0, resultlength;
		union {
			struct sockaddr_storage ss;
			struct sockaddr_in6 s6;
			struct sockaddr_in s4;
		} peer;

		/* If secret isn't initialized yet, the cookie can't be valid */
		if (!cookie_initialized){
			return 0;
		}

		/* Read peer information */
		(void) BIO_dgram_get_peer(SSL_get_rbio(ssl), &peer);

		/* Create buffer with peer's address and port */
		length = 0;
		switch (peer.ss.ss_family) {
			case AF_INET:
				length += sizeof(struct in_addr);
				break;
			case AF_INET6:
				length += sizeof(struct in6_addr);
				break;
			default:
				OPENSSL_assert(0);
				break;
		}
		length += sizeof(in_port_t);
		buffer = (unsigned char*) OPENSSL_malloc(length);

		if (buffer == NULL){
			std::cout << "out of memory" << std::endl;
			return 0;
		}

		switch (peer.ss.ss_family) {
			case AF_INET:
				memcpy(buffer,
					   &peer.s4.sin_port,
					   sizeof(in_port_t));
				memcpy(buffer + sizeof(in_port_t),
					   &peer.s4.sin_addr,
					   sizeof(struct in_addr));
				break;
			case AF_INET6:
				memcpy(buffer,
					   &peer.s6.sin6_port,
					   sizeof(in_port_t));
				memcpy(buffer + sizeof(in_port_t),
					   &peer.s6.sin6_addr,
					   sizeof(struct in6_addr));
				break;
			default:
				OPENSSL_assert(0);
				break;
		}

		/* Calculate HMAC of buffer using the secret */
		HMAC(EVP_sha1(), (const void*) cookie_secret, COOKIE_SECRET_LENGTH,
			 (const unsigned char*) buffer, length, result, &resultlength);
		OPENSSL_free(buffer);

		if (cookie_len == resultlength && memcmp(result, cookie, resultlength) == 0){
			return 1;
		}

		return 0;
	}
	bool DtlsContext::CheckClientContext(std::string confDirPath) {
        struct stat buffer;
		const std::string certPath = confDirPath + SSL_CERTIFICATE_CLIENT_PATH;
		const std::string keyPath = confDirPath + SSL_PRIVATE_KEY_CLIENT_PATH;
		const std::string caPath = confDirPath + SSL_CERTIFICATE_VERIFY_PATH;
		if (stat(certPath.c_str(), &buffer) != 0) {
			std::cout << "\nERROR: no certificate found! Path:[" << certPath << "]" << std::endl;
			return false;
		}
		if (stat(keyPath.c_str(), &buffer) != 0) {
			std::cout << "\nERROR: no private key found! Path:[" << keyPath << "]" << std::endl;
			return false;
		}
		if (stat(caPath.c_str(), &buffer) != 0) {
			std::cout << "\nERROR: no ca-certificate found Path:[" << caPath << "]" << std::endl;
			return false;
		}
		return true;
	}
	SSL_CTX* DtlsContext::CreateServerContext(std::string confDirPath)
	{
		// DTLSコンテキストの作成
        SSL_CTX *ctx;
		ctx = SSL_CTX_new(DTLSv1_2_server_method());
		//ctx = SSL_CTX_new(DTLS_server_method());
		// SSL_MODE_AUTO_RETRY時、プロセスが再起動し、その後、通信が繋がなくなる現象が起きたため、コメントアウト
		//SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
		//SSL_CTX_set_cipher_list(ctx, "ALL:NULL:eNULL:aNULL");
		//SSL_CTX_set_cipher_list(ctx, "HIGH");
		//SSL_CTX_set_cipher_list(ctx, "TLSv1.2");
		//SSL_CTX_set_cipher_list(ctx, "ALL:!ECDHE-RSA-AES256-GCM-SHA384");
		//SSL_CTX_set_cipher_list(ctx, "ECDHE-RSA-AES256-GCM-SHA384");
		SSL_CTX_set_cipher_list(ctx, "ECDHE-ECDSA-AES256-GCM-SHA384");
		//SSL_CTX_set_cipher_list(ctx, "ECDHE-RSA-AES128-SHA");
		SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);
		//SSL_CTX_set_default_passwd_cb(ctx, SSL_PEM_PASS_PHRASE);

		//double start_time = calcTime();
		LOG4CXX_DEBUG(logger, "証明書のロード START");

		const std::string certPath = confDirPath + SSL_CERTIFICATE_FILE_ABS_PATH;
		const std::string keyPath = confDirPath + SSL_PRIVATE_KEY_FILE_ABS_PATH;
		const std::string caPath = confDirPath + SSL_CERTIFICATE_VERIFY_PATH;
		LOG4CXX_DEBUG(logger, certPath);
		// SSLの公開鍵と秘密鍵の読み出し
		if (!SSL_CTX_use_certificate_file(ctx, certPath.c_str(), SSL_FILETYPE_PEM)){
			std::cout << "\nERROR: no certificate found! Path:[" << certPath << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (!SSL_CTX_use_PrivateKey_file(ctx, keyPath.c_str(), SSL_FILETYPE_PEM)){
			std::cout << "\nERROR: no private key found! Path:[" << keyPath << "]" << std::endl;
			exit(EXIT_FAILURE);

		}
		if (!SSL_CTX_check_private_key(ctx)){
			std::cout << "\nERROR: invalid private key!" << std::endl;
			exit(EXIT_FAILURE);
		}

		if (!SSL_CTX_load_verify_locations(ctx, caPath.c_str(), NULL)){
			//LOG4CXX_WARN(logger, ERR_error_string(ERR_get_error(), NULL));
			std::cout << "\nERROR: no ca-certificate found! Path:[" << caPath << "]" << std::endl;
			exit(EXIT_FAILURE);
		}

		// SSL証明書をロード
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, dtls_verify_callback);

		// コールバックをバインド
		SSL_CTX_set_read_ahead(ctx, 1);
		SSL_CTX_set_cookie_generate_cb(ctx, generate_cookie);
		SSL_CTX_set_cookie_verify_cb(ctx, &verify_cookie);

		SSL_CTX_set_client_CA_list(ctx, SSL_load_client_CA_file(caPath.c_str()));

		//double end_time = calcTime();
		//LOG4CXX_DEBUG(logger, "証明書のロード END 経過時間[ms]：" + std::to_string(end_time - start_time));
        LOG4CXX_DEBUG(logger, "証明書のロード END 経過時間[ms]");
		return ctx;
	}


	SSL_CTX* DtlsContext::CreateClientContext(std::string confDirPath)
	{
		SSL_CTX *_ctx;
		// DTLSコンテキストの作成
		_ctx = SSL_CTX_new(DTLSv1_2_client_method());
		// SSL_MODE_AUTO_RETRY時、プロセスが再起動し、その後、通信が繋がなくなる現象が起きたため、コメントアウト
		//SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
		//ctx = SSL_CTX_new(DTLS_client_method());

		//double start_time = calcTime();
		LOG4CXX_DEBUG(logger, "証明書ロード START");

		const std::string certPath = confDirPath + SSL_CERTIFICATE_CLIENT_PATH;
		const std::string keyPath = confDirPath + SSL_PRIVATE_KEY_CLIENT_PATH;
		const std::string caPath = confDirPath + SSL_CERTIFICATE_VERIFY_PATH;
		LOG4CXX_DEBUG(logger, certPath);
		// SSLの公開鍵と秘密鍵の読み出し
		if (!SSL_CTX_use_certificate_file(_ctx, certPath.c_str(), SSL_FILETYPE_PEM)){
			std::cout << "\nERROR: no certificate found! Path:[" << certPath << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (!SSL_CTX_use_PrivateKey_file(_ctx, keyPath.c_str(), SSL_FILETYPE_PEM)){
			std::cout << "\nERROR: no private key found! Path:[" << keyPath << "]" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (!SSL_CTX_check_private_key (_ctx)){
			std::cout << "\nERROR: invalid private key!" << std::endl;
			exit(EXIT_FAILURE);
		}

		// SSL証明書をロード
		SSL_CTX_set_verify(_ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, dtls_verify_callback);

		if (!SSL_CTX_load_verify_locations(_ctx, caPath.c_str(), NULL)){
			//LOG4CXX_WARN(logger, ERR_error_string(ERR_get_error(), NULL));
			std::cout << "\nERROR: no ca-certificate found! Path:[" << caPath << "]" << std::endl;
			exit(EXIT_FAILURE);
		}

		SSL_CTX_set_verify_depth(_ctx, 2);
		SSL_CTX_set_read_ahead(_ctx, 1);
		SSL_CTX_set_cookie_generate_cb(_ctx, generate_cookie);
		SSL_CTX_set_cookie_verify_cb(_ctx, &verify_cookie);

		//double end_time = calcTime();
		//LOG4CXX_DEBUG(logger, "証明書ロード END ：" + std::to_string(end_time - start_time));
        LOG4CXX_DEBUG(logger, "証明書ロード END ：");
		return _ctx;
	}
}
