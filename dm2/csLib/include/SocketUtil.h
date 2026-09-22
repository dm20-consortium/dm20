#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
#include <ctime>

#include "Struct.h"
#include "UnorderedMap.h"

namespace SocketUtil {
    bool combineFragment(CS::send_message& buf_, CS::send_message_vector& vectorBuf_, CS::UnorderedMap<std::string, time_t>& flagment_data_receive_time_map_, CS::UnorderedMap<std::string, std::vector<std::string>>& flagment_data_combination_map_);
    void ClearUnorderedMap(int& combination_map_clear_time_th, CS::UnorderedMap<std::string, std::vector<std::string>>& flagment_data_combination_map_th, CS::UnorderedMap<std::string, time_t>& flagment_data_receive_time_map_th);
    void print_send_message_header(const CS::send_message_header &header);
}
#endif