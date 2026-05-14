#include <iostream>

#include <google/protobuf/text_format.h>

#include "sensor_io.pb.h"


int main() {
  SensingMessage msg;

  // はじめはデバグ出力しても何も出ない状態
  std::cout << "msg is empty like -> " << msg.DebugString() << std::endl;

  // 組み込み型は普通にset/getすれば良い
  // 消すこともできる
  msg.set_message_id(0x12345678);
  std::cout << "message id is set -> " << msg.message_id() << std::endl;
  msg.clear_message_id();
  std::cout << "message id is cleared -> " << msg.DebugString() << std::endl;

  // 配列の場合add_xxxにより参照を得る形で要素を追加/設定できる
  std::cout << "sensor info num -> " << msg.sensor_info_size() << std::endl;
  SensorInformation* si = msg.add_sensor_info();
  si->set_type(SensorType::ST_UNKNOWN);
  si->set_latitude(350000000);
  si->set_longitude(1390000000);
  si->set_altitude(0);
  si->set_sensor_status(1);
  DetectCapability* dc = si->add_detect_capabilities();
  dc->set_detectable_classes(1);
//SensorInformation si;
//si.set_latitude(350000000);
//si.set_longitude(1390000000);
//si.set_altitude(0);
//DetectCapability dc;
//dc.set_detactable_classes(1);
//OffsetPointXY xy;
//xy.dx = 100;
//xy.dy = 200;



  // 独自の方はmutableであることを明確にした上で設定する必要がある

  std::cout << msg.DebugString() << std::endl;

  return EXIT_SUCCESS;
}
