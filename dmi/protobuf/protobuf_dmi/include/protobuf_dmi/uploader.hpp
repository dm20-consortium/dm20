#ifndef PROTOBUF_DMI_UPLOADER_HPP_
#define PROTOBUF_DMI_UPLOADER_HPP_
#include <string>
#include <memory>
#include <vector>


class DatagramSocket;

namespace IS
{
class Tuple;
}

namespace protobuf_dmi
{

class Uploader
{
public:
  struct Config {
    Config(
      const std::string& in_ipaddr,
      const int in_port,
      const std::string& in_user,
      const std::string& in_pass,
      const uint32_t in_connection_try_count)
      : ipaddr(in_ipaddr),
        port(in_port),
        user(in_user),
        pass(in_pass),
        connection_try_count(in_connection_try_count) {}

    std::string ipaddr;
    int port;
    std::string user;
    std::string pass;
    uint32_t connection_try_count;
  };

  using SharedPtr = std::shared_ptr<Uploader>;

  explicit Uploader(const Config& conf);
  ~Uploader() = default;

  void Upload(const std::vector<IS::Tuple>& tuples);

private:
  DatagramSocket* socket_;
};

}  // namespace protobuf_dmi
#endif  // PROTOBUF_DMI_UPLOADER_HPP_
