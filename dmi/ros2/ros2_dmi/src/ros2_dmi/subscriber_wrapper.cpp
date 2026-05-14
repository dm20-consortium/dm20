#include <memory>
#include <vector>
#include <map>
#include "ros2_dmi/subscriber.hpp"


/**
 * @brief ノードの作成とspin、各subscriberの作成を行うクラス。
 */
class SubscriberWrapper{
public:
    /**
     * @brief コンストラクタ
     * @brief ノードの作成、各subscriberの作成を行う。
     *
     * @param user_callback
     */
    SubscriberWrapper(std::map<std::string, std::map<std::string, std::string>>& configMap, void (*user_callback)(std::vector<IS::Tuple>&)) 
    {
        // TODO : 多重init問題.. publisher_wrapperと一緒に使うと下のコードでエラー。要対応
        try {
          if(!rclcpp::ok()) {
              rclcpp::init(0, nullptr);
          }
        } catch (const rclcpp::ContextAlreadyInitialized& e) {
          // Nothing to do
        }

        if (node_ == nullptr) {
          node_.reset(new rclcpp::Node("my_uploader"));
        }

        for (auto& subMap : configMap) {
            subsc_ = ros2_dmi::SubscriberFactory::Create(
                subMap.second["topicName"],
                subMap.second["topicType"],
                node_.get());
            if (subsc_ == nullptr) {
                throw std::runtime_error("Failed to create subscriber");
            }
            subsc_->RegisterUserCallback(
                std::bind(&SubscriberWrapper::Callback,
                  this,
                  std::placeholders::_1));
            subscVector.push_back(subsc_);
        }
        rcv_cb_func = user_callback;
    }

    SubscriberWrapper(const SubscriberWrapper&) = delete;
    SubscriberWrapper& operator=(const SubscriberWrapper&) = delete;

    /**
     * @brief デストラクタ
     * @brief ノードのshutdownを行う。
     */
    ~SubscriberWrapper() {
        if(rclcpp::ok()) {
            rclcpp::shutdown();
        }
    }

    /**
     * @brief ノードのspinを行う。
     */
    void spin() {
        rclcpp::spin(node_);
    }

private:
    std::shared_ptr<rclcpp::Node> node_;
    ros2_dmi::SubscriberBase::SharedPtr subsc_;
    std::vector<ros2_dmi::SubscriberBase::SharedPtr> subscVector;
    using CallbackType = std::function<void (std::vector<IS::Tuple>&)>;
    CallbackType rcv_cb_func;

    /**
     * @brief subscribe時のCallback関数。
     * @brief ユーザ定義のcallback関数をコールする。
     *
     * @param tuples
     */
    void Callback(const std::vector<IS::Tuple>& tuplesConst) {
        std::vector<IS::Tuple> tuples = tuplesConst;
        rcv_cb_func(tuples);
    }
};

extern "C" {

    /**
     * @brief SubscriberWrapperクラスのインスタンスを生成する。
     * @brief エントリーポイント
     *
     * @param callback
     * 
     * @return instance
     */
    intptr_t create(std::map<std::string, std::map<std::string, std::string>>& confMap, void (*callback)(std::vector<IS::Tuple>&)) {
        return reinterpret_cast<intptr_t>(new SubscriberWrapper(confMap, callback));
    }

    /**
     * @brief SubscriberWrapperクラスのspinメソッドをコールする。
     * @brief エントリーポイント
     *
     * @param ptr
     * 
     */
    void spin(intptr_t ptr) {
        reinterpret_cast<SubscriberWrapper*>(ptr)->spin();
    }

    /**
     * @brief SubscriberWrapperクラスのインスタンスを破棄する。
     * @brief エントリーポイント
     *
     * @param ptr
     * 
     */
    void destroy(intptr_t ptr) {
        delete reinterpret_cast<SubscriberWrapper*>(ptr);
    }
}
