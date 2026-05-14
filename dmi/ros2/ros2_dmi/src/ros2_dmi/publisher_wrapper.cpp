
#define SINGLE_SUB      0

#include <vector>
#include <map>
#include "ros2_dmi/publisher.hpp"

#define INFO_NUMBER     4

class PublisherWrapper{
public:
    PublisherWrapper(std::map<std::string, std::map<std::string, std::string>>& configMap) 
    {
        try {
          if(!rclcpp::ok()) {
              rclcpp::init(0, nullptr);
          }
        } catch (const rclcpp::ContextAlreadyInitialized& e) {
          // Nothing to do
        }
        if (node_ == nullptr) {
          node_.reset(new rclcpp::Node("my_downloader"));
        }

        for (auto& subMap : configMap) {
            pub_ = ros2_dmi::PublisherFactory::Create(
                subMap.second["topicName"],
                subMap.second["topicType"],
                node_.get());
            if (pub_ == nullptr) {
                throw std::runtime_error("Failed to create publisher.");
            }
            pubPair_.insert(std::make_pair(subMap.second["targetTable"], pub_));
        }

    }

    PublisherWrapper(const PublisherWrapper&) = delete;
    PublisherWrapper& operator=(const PublisherWrapper&) = delete;

    ~PublisherWrapper() {
        if(rclcpp::ok()) {
            rclcpp::shutdown();
        }
    }

    void publishOnDmEvent(std::string &tableName, IS::TupleSet &tupleSet) {
        pubPair_[tableName]->Publish(tupleSet);
    }

private:
    std::shared_ptr<rclcpp::Node> node_;
    ros2_dmi::IPublisher::SharedPtr pub_;
    std::map<std::string, ros2_dmi::IPublisher::SharedPtr> pubPair_;

};


extern "C" {

    intptr_t create(std::map<std::string, std::map<std::string, std::string>>& confMap) {
        return reinterpret_cast<intptr_t>(new PublisherWrapper(confMap));
    }

    void pub_on_dm_event(intptr_t ptr, std::string &tableName, IS::TupleSet &ts) {
        reinterpret_cast<PublisherWrapper*>(ptr)->publishOnDmEvent(tableName, ts);
    }

    void destroy(intptr_t ptr) {
        delete reinterpret_cast<PublisherWrapper*>(ptr);
    }
}
