#ifndef UDPDMICONTROLLER_H
#define UDPDMICONTROLLER_H

#include "UDPDMIUploaderWrapper.h"
#include "UDPDMIDownloaderWrapper.h"

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>

namespace IS {
    class UDPDMIController {
    public:
        UDPDMIController();
        ~UDPDMIController();
        void init();
        void start();
        void stop();
        void uploaderStart();
        void downloaderStart();
        void wrapSendTo(IS::TupleSet &tupleset, string &tableName);
        UDPDMIUploaderWrapper uploader;
        UDPDMIDownloaderWrapper &downloader = UDPDMIDownloaderWrapper::get_instance();
    private:

    };
}
#endif