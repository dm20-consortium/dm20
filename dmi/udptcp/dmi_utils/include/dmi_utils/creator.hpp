/**
 * @file creater.hpp
 */
#ifndef DMI_UTILS_CREATOR_HPP_
#define DMI_UTILS_CREATOR_HPP_

#include <cool4_api_dataclass/header.hpp>
#include <cool4_api_dataclass/dataclasses_with_header.hpp>

#include <glog/logging.h>


namespace dmi_utils {

/**
 * @brief Construct Frame with header and dataclass array
 *
 * @tparam T Information type
 */
template<class T>
class Creator {
 public:
  using SharedPtr = std::shared_ptr<Creator>;
  using CallbackType = std::function<void(const std::vector<cool4_api_dataclass::ByteArray>&)>;

  /**
   * @brief Frame config
   */
  struct Config {
    /**
     * @brief Initialize with specified parameters
     *
     * @param in_version_major Major version
     * @param in_version_minor Minor version
     * @param in_version_patch Patch version
     * @param in_data_num_per_frame Contained data num in a frame
     */
    Config(
      const uint8_t in_version_major,
      const uint8_t in_version_minor,
      const uint8_t in_version_patch,
      const uint32_t in_data_num_per_frame)
      : version_major(in_version_major),
        version_minor(in_version_minor),
        version_patch(in_version_patch),
        data_num_per_frame(in_data_num_per_frame) {}

    const uint8_t version_major;
    const uint8_t version_minor;
    const uint8_t version_patch;
    const uint32_t data_num_per_frame;
  };

  /**
   * @brief Initialize members
   *
   * @param conf Creator config object
   * @param user_callback User callback function called when process has done
   */
  Creator(
      const Config& conf,
      CallbackType user_callback)
    : seq_(0),
      version_major_(conf.version_major),
      version_minor_(conf.version_minor),
      version_patch_(conf.version_patch),
      data_num_per_frame_(conf.data_num_per_frame),
      user_callback_(user_callback) {}

  /**
   * @brief Convert Data array to list of byte array
   *
   * @param data_array Data array
   */
  void ToByteArrayList(const typename T::Array& data_array) {
    if (data_array.empty()) return;

    const int remain_num = data_array.size() % data_num_per_frame_;
    // フレーム数
    int frame_num = remain_num == 0 ? int(data_array.size() / data_num_per_frame_) : int(data_array.size() / data_num_per_frame_) + 1;
    // 最後のフレームに含まれるデータ数
    int last_frame_data_num = remain_num == 0 ? data_num_per_frame_ : remain_num;

    std::vector<cool4_api_dataclass::DataClassesWithHeader<T>> data_to_send(frame_num);
    for (int i = 0; i < frame_num; ++i) {
      const int push_num = i == frame_num - 1 ? last_frame_data_num : data_num_per_frame_;
      data_to_send[i].header = {
        version_major_,
        version_minor_,
        version_patch_,
        seq_,
        static_cast<uint8_t>(i + 1),
        static_cast<uint8_t>(frame_num),
        static_cast<uint8_t>(push_num)};
      for (int j = 0; j < push_num; ++j) {
        const int idx = i * data_num_per_frame_ + j;
        data_to_send[i].dataclasses.push_back(data_array[idx]);
      }
    }
    ++seq_;

    if (VLOG_IS_ON(3)) {
      for (const auto& e : data_to_send) {
        VLOG(3) << e;
      }
    }

    std::vector<cool4_api_dataclass::ByteArray> bytes_to_send;
    for (auto& array : data_to_send) {
      const auto bytes_opt = array.ToByteArray();
      if (bytes_opt == boost::none) continue;
      bytes_to_send.push_back(bytes_opt.get());
    }

    user_callback_(bytes_to_send);
  }

 private:
  /**
   * @brief Number of sequence that increment every frame set
   */
  uint8_t seq_;

  /**
   * @brief Major version number
   */
  const uint8_t version_major_;

  /**
   * @brief Minor version number
   */
  const uint8_t version_minor_;

  /**
   * @brief Patch version number
   */
  const uint8_t version_patch_;

  /**
   * @brief Contained data num in a frame
   */
  const uint32_t data_num_per_frame_;

  /**
   * @brief User callback function
   */
  CallbackType user_callback_;
};

}
#endif
