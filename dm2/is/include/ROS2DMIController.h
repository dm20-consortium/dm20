#ifndef ROS2DMICONTROLLER_H
#define ROS2DMICONTROLLER_H
#include <memory>

#include "ROS2DMIUploaderWrapper.h"
#include "ROS2DMIDownloaderWrapper.h"

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>

using namespace log4cxx;

namespace IS {
class ROS2DMIController {
    public:
        ROS2DMIController();
        ~ROS2DMIController();
        void init();
        void start();
        void stop();
        void uploaderStart();
        void uploaderStop();
        void downloaderStop();
        void downloaderStart();
        void wrapPublishTo(IS::TupleSet &tupleset, string &tableName);
        void errorHandling();

    private:
        LoggerPtr logger_ros = Logger::getLogger("ROS2DMIController");
        ROS2DMIUploaderWrapper uploader;
        void watchDogProcess();
        bool uploaderErrorFlag = false;
        bool downloaderErrorFlag = false;
};
}
#endif
