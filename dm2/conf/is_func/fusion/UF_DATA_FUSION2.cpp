#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>

#include <stdlib.h>
#include <sys/stat.h>
#include <vector>

#include <is/StringUtil.h>
#include <is/DmException.h>
using namespace std;
using namespace std::chrono;

extern "C" vector<vector<string>> UF_DATAFUSION2(vector<vector<string>> argList)
{
  vector< vector<string> > retList;
  cout << argList.size() << endl;
  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string id;
    long time;
    int latitude;
    int longitude;
    int altitude;
    int speed_value;
    int orientation_value;
    int length_value;
    int width_value;
    int height_value;
    try {      
      id = argList.at(i).at(0);
      time = std::stol(argList.at(i).at(1));
      latitude = std::stoi(argList.at(i).at(2));
      longitude = std::stoi(argList.at(i).at(3));
      altitude = std::stoi(argList.at(i).at(4));
      speed_value = std::stoi(argList.at(i).at(5));
      orientation_value = std::stoi(argList.at(i).at(6));
      length_value = std::stoi(argList.at(i).at(7));
      width_value = std::stoi(argList.at(i).at(8));
      height_value = std::stoi(argList.at(i).at(9));
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }
/*
    cout << id << endl;
    cout << time << endl;
    cout << latitude << endl;
    cout << longitude << endl;
    cout << altitude << endl;
    cout << speed_value << endl;
    cout << orientation_value << endl;
    cout << length_value << endl;
    cout << width_value << endl;
    cout << height_value << endl;
*/
    ret.push_back(id);
    ret.push_back(std::to_string(time));
    ret.push_back(std::to_string(latitude));
    ret.push_back(std::to_string(longitude));
    ret.push_back(std::to_string(altitude));
    ret.push_back(std::to_string(speed_value));
    ret.push_back(std::to_string(orientation_value));
    ret.push_back(std::to_string(length_value));
    ret.push_back(std::to_string(width_value));
    ret.push_back(std::to_string(height_value));
    retList.push_back(ret);
    //わざと引数と戻り値で行列不一致させるため、breakする。
    break;
  }
  cout << "Fusion2:" << retList.size() << endl;
  return retList;
}


