#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <vector>
#include <math.h>

#include <stdlib.h>
#include <sys/stat.h>
//************* ADD START *************//
#include <iomanip>
#include <limits>

using namespace std;

int strhex2i (const std::string &s);
vector<int> pocs2wgs84(double x, double y, int system);
vector<double> wgs842pocs(double lonitude, double latitude, int system);
string toHex(int n);

//int fusion(void)
//************* MOD START *************//
//int main(void)
extern "C" vector<vector<string>> UF_DATAFUSION3(vector<vector<string>> argList)
//************* MOD  END  *************//
{
	//************* ADD START *************//
	//printf("UF_DATAFUSION called!\n");
	vector<vector<string>> retList;
	//************* ADD  END  *************//
  //変数の登録
  int team[argList.size()];  // object_infoがどちらの所属かを記録する変数
  vector<vector<vector<string>>> candidates(2);

  int unit1_cnt = 0;
  int unit2_cnt = 0;
  for(int i=0; i<argList.size(); i++)
  {
/*****   grouping   *****/
    //object_IDを取得
    std::string object_id = argList.at(i).at(0);

/*
    cout << "[UF_DATAFUSION3] No." << i << "-id:" << argList.at(i).at(0) << endl;
    cout << "[UF_DATAFUSION3] No." << i << "-time:" << argList.at(i).at(1) << endl;
    cout << "[UF_DATAFUSION3] No." << i << "-latitude:" << argList.at(i).at(2) << endl;
    cout << "[UF_DATAFUSION3] No." << i << "-longitude:" << argList.at(i).at(3) << endl;
    cout << "[UF_DATAFUSION3] No." << i << "-altitude:" << argList.at(i).at(4) << endl;
    cout << "[UF_DATAFUSION3] No." << i << "-speed_value:" << argList.at(i).at(5) << endl;
    cout << "[UF_DATAFUSION3] No." << i << "-orientation_value:" << argList.at(i).at(6) << endl;
    cout << "[UF_DATAFUSION3] No." << i << "-length_value:" << argList.at(i).at(7) << endl;
    cout << "[UF_DATAFUSION3] No." << i << "-width_value:" << argList.at(i).at(8) << endl;
    cout << "[UF_DATAFUSION3] No." << i << "-height_value:" << argList.at(i).at(9) << endl;
*/    
    //unit_IDを取得する
    int unit_id = strhex2i(object_id.substr(8));
    //cout << "[UF_DATAFUSION3]unit_id:" << unit_id << endl;

    if(unit_id==1){
//unit1のオブジェクトIDに詰め込む
      candidates.at(0).push_back(argList.at(i));
      unit1_cnt++;
    }else{
//unit2のオブジェクトIDに詰め込む
      candidates.at(1).push_back(argList.at(i));
      unit2_cnt++;
    }
  }
/******/
    //retList.push_back(argList.at(i));
/******/
  
  //cout << "[UF_DATAFUSION3]candidates.at(0).size():" << candidates.at(0).size() << endl;
  //cout << "[UF_DATAFUSION3]candidates.at(1).size():" << candidates.at(1).size() << endl;

/**** data association && data fusion ****/
    bool flag_list[candidates.at(1).size()] = {false};
    for(int i = 0;i<candidates.at(0).size();i++){
      vector<double> position1 = wgs842pocs(std::stod(candidates.at(0).at(i).at(2)),std::stod(candidates.at(0).at(i).at(3)),9);
      //cout << "lon=" << candidates.at(0).at(i).at(2) << ", lat=" <<  candidates.at(0).at(i).at(3) << endl;
      //cout << "x=" << position1.at(0) << ", y=" <<  position1.at(1) << endl;
      double length_value = 1000;
      int length_row   = 1000;
      for(int j = 0;j<candidates.at(1).size();j++){
        vector<double> position2 = wgs842pocs(std::stod(candidates.at(1).at(j).at(2)),std::stod(candidates.at(1).at(j).at(3)),9);
        //重心点の長さを求め最小値を保存する。
        double length_value_temp = sqrt( pow(position1.at(0)-position2.at(0),2)+pow(position1.at(1)-position2.at(1),2) );
        int length_row_temp   = j;
        if(flag_list[j] == false){
          if(length_value_temp < length_value){
            length_value = length_value_temp;
            length_row = length_row_temp;
          }
        }
      }
 /**** data fusion ****/
      if(length_value<3){
        vector<string> fsnList;
    //id
        std::string object_id1 = candidates.at(0).at(i).at(0);
        std::string object_id2 = candidates.at(1).at(length_row).at(0);

        //cout << "object_id1:" << object_id1 << ", object_id2:" <<  object_id2 << endl;

    //tracking_IDを取得する
        int tracking_id1 = strhex2i(object_id1.substr(1,7));
        int tracking_id2 = strhex2i(object_id2.substr(1,7));    

        //cout << "[UF_DATAFUSION3]object_id1:" << object_id1 << ", object_id2" << object_id2 << endl;
    
    //cleating object_id
        std::string object_id3 = "8";
        object_id3 += toHex(int((tracking_id1+tracking_id2)/2));
        object_id3 += "00000003";
        //cout << "object_id3_string:" << object_id3 << endl;
        fsnList.push_back(object_id3);
    //time
        //cout << "time1_string:" << candidates.at(0).at(i).at(1) <<  ", time2_string:" << candidates.at(0).at(length_row).at(1) << endl;
        long time1 = std::stol(candidates.at(0).at(i).at(1));
        long time2 = std::stol(candidates.at(1).at(length_row).at(1));
        long time3 = (time1+time2)/2;
        fsnList.push_back(std::to_string(time3));     
          
    //position
        vector<double> position2 = wgs842pocs(std::stod(candidates.at(1).at(length_row).at(2)),std::stod(candidates.at(1).at(length_row).at(3)),9);
        double x = (position1.at(0) + position2.at(0))/2;
        double y = (position1.at(1) + position2.at(1))/2;
        double alt1 = std::stod(candidates.at(0).at(i).at(4));
        double alt2 = std::stod(candidates.at(1).at(length_row).at(4));
        double z = (alt1+alt2)/2;
        vector<int> position3 = pocs2wgs84(y,x,9);
/*
        cout << "lat=" << candidates.at(1).at(length_row).at(2) << ", lon=" <<  candidates.at(1).at(length_row).at(3) << endl;
        cout << "x1=" << position1.at(0) << ", y1=" <<  position1.at(1) << endl;
        cout << "x2=" << position2.at(0) << ", y2=" <<  position2.at(1) << endl;
        cout << "lat=" << position3.at(0) << ", lon=" <<  position3.at(1) << endl;
*/
        //cout << "position3.at(0):" << position3.at(0) << "position3.at(1):" << position3.at(1) << endl;
        fsnList.push_back(std::to_string(position3.at(0)));
        fsnList.push_back(std::to_string(position3.at(1)));
        fsnList.push_back(std::to_string(z));
        
        //speed
        double speed1 = std::stod(candidates.at(0).at(i).at(5));
        double speed2 = std::stod(candidates.at(1).at(length_row).at(5));
        double speed3 = (speed1+speed2)/2;
        fsnList.push_back(std::to_string(speed3));

        //orientation & size
        double length1 = std::stod(candidates.at(0).at(i).at(7));
        double length2 = std::stod(candidates.at(1).at(length_row).at(7));
        double width1 = std::stod(candidates.at(0).at(i).at(8));
        double width2 = std::stod(candidates.at(1).at(length_row).at(8));
        double height1 = std::stod(candidates.at(0).at(i).at(9));
        double height2 = std::stod(candidates.at(1).at(length_row).at(9)); 

        double area1 = length1*width1;
        double area2 = length2*width2;

        if(area1 > area2){
          fsnList.push_back(candidates.at(0).at(i).at(6));
          fsnList.push_back(candidates.at(0).at(i).at(7));
          fsnList.push_back(candidates.at(0).at(i).at(8));
          fsnList.push_back(candidates.at(0).at(i).at(9));
        }else{
          fsnList.push_back(candidates.at(1).at(length_row).at(6));
          fsnList.push_back(candidates.at(1).at(length_row).at(7));
          fsnList.push_back(candidates.at(1).at(length_row).at(8));
          fsnList.push_back(candidates.at(1).at(length_row).at(9));
        }
        retList.push_back(fsnList);
        flag_list[length_row] = true;
      }else{
        retList.push_back(candidates.at(0).at(i));
      }
      for(int j=0;j<candidates.at(1).size();j++){
        if(flag_list[j] == false){
          retList.push_back(candidates.at(1).at(length_row));
        }
      }
    }
  /*
  //cout << "[UF_DATAFUSION3]unit1_cnt:" << unit1_cnt << ",unit2_cnt:" << unit2_cnt << endl;
  if (retList.size() == 0) {
    cout << "[UF_DATAFUSION3]Data is not combined." << endl;
    vector<string> ret;
    //cout << "[UF_DATAFUSION3]argList.at(0).size():" << argList.at(0).size() << endl;
    for (int i = 0;i<argList.at(0).size(); i++) {
      ret.push_back("0");
    }
    retList.push_back(ret);
  } else {
    cout << "[UF_DATAFUSION3]Data is combined. cnt:" << retList.size() << endl;
  }
  */
/*
    for(int i=0; i<retList.size(); i++) {
      cout << "[UF_DATAFUSION3] combined-No." << i << "-id:" << retList.at(i).at(0) << endl;
      cout << "[UF_DATAFUSION3] combined-No." << i << "-time:" << retList.at(i).at(1) << endl;
      cout << "[UF_DATAFUSION3] combined-No." << i << "-latitude:" << retList.at(i).at(2) << endl;
      cout << "[UF_DATAFUSION3] combined-No." << i << "-longitude:" << retList.at(i).at(3) << endl;
      cout << "[UF_DATAFUSION3] combined-No." << i << "-altitude:" << retList.at(i).at(4) << endl;
      cout << "[UF_DATAFUSION3] combined-No." << i << "-speed_value:" << retList.at(i).at(5) << endl;
      cout << "[UF_DATAFUSION3] combined-No." << i << "-orientation_value:" << retList.at(i).at(6) << endl;
      cout << "[UF_DATAFUSION3] combined-No." << i << "-length_value:" << retList.at(i).at(7) << endl;
      cout << "[UF_DATAFUSION3] combined-No." << i << "-width_value:" << retList.at(i).at(8) << endl;
      cout << "[UF_DATAFUSION3] combined-No." << i << "-height_value:" << retList.at(i).at(9) << endl;
    }
*/
//  }
  //cout << "[UF_DATAFUSION3]retList.size():" << retList.size() << endl;

	//************* MOD  END  *************//
	//printf("UF_DATAFUSION end!\n");
	return retList;
	//************* MOD  END  *************//
}

int strhex2i (const std::string &s) {
    int out = 0;
    std::string temp;
    for (int i = 0, size = s.size() ; i < size ; ++i ) {
      if      (s[i]==49){ out+= 1*pow(16,size-1-i); }
      else if (s[i]==50){ out+= 2*pow(16,size-1-i); }
      else if (s[i]==51){ out+= 3*pow(16,size-1-i); }
      else if (s[i]==52){ out+= 4*pow(16,size-1-i); }
      else if (s[i]==53){ out+= 5*pow(16,size-1-i); }
      else if (s[i]==54){ out+= 6*pow(16,size-1-i); }
      else if (s[i]==55){ out+= 7*pow(16,size-1-i); }
      else if (s[i]==56){ out+= 8*pow(16,size-1-i); }
      else if (s[i]==57){ out+= 9*pow(16,size-1-i); }
      else if (s[i]==65){ out+= 10*pow(16,size-1-i); }
      else if (s[i]==66){ out+= 11*pow(16,size-1-i); }
      else if (s[i]==67){ out+= 12*pow(16,size-1-i); }
      else if (s[i]==66){ out+= 13*pow(16,size-1-i); }
      else if (s[i]==67){ out+= 14*pow(16,size-1-i); }      
      else if (s[i]==68){ out+= 15*pow(16,size-1-i); }
    }
    return out;
}

vector<int> pocs2wgs84(double x, double y, int system){
	vector<int> position;
	double a = 6378137; //楕円体の長半径
	double F=298.257222101; //楕円体の逆扁平率
	double m0=0.9999; //平面直角座標系のX軸上における縮尺係数 (0.9999)

	//展開パラメータを事前に入力する
	double n = 1/(2*F-1);

	double A0 =  1 + pow(n,2)*1/4 + pow(n,4)*1/64;
	double A1 = -(n - pow(n,3)*1/8 - pow(n,5)*1/64)*3/2;
	double A2 =  (pow(n,2) - pow(n,4)*1/4)*15/16;
	double A3 = -(pow(n,3) - 5/16*pow(n,5))*35/48;
	double A4 =  pow(n,4)*315/512;
	double A5 = -pow(n,5)*693/1280;

	double B1 = n*1/2 - pow(n,2)*2/3 + pow(n,3)*37/96 - pow(n,4)*1/360 -pow(n,5)*81/512;
	double B2 = pow(n,2)*1/48 + pow(n,3)*1/15 - pow(n,4)*437/1440 + pow(n,5)*46/105;
	double B3 = pow(n,3)*17/480 -pow(n,4)*37/840 - pow(n,5)*209/4480;
	double B4 = pow(n,4)*4397/161280 - pow(n,5)*11/504;
	double B5 = pow(n,5)*4583/161280;

	double D1 = n*2 -pow(n,2)*2/3 - pow(n,3)*2 + pow(n,4)*116/45 + pow(n,5)*26/45 -pow(n,6)*2854/675;
	double D2 = pow(n,2)*7/3 -pow(n,3)*8/5 -pow(n,4)*227/45 + pow(n,5)*2704/315 + pow(n,6)*2323/945;
	double D3 = pow(n,3)*56/15 -pow(n,4)*136/35  -pow(n,5)*1262/105 +pow(n,6)*73814/2835;
	double D4 = pow(n,4)*4279/630 -pow(n,5)*332/35 -pow(n,6)*399572/14175;
	double D5 = pow(n,5)*4174/315 - pow(n,6)*144838/6237;
	double D6 = pow(n,6)*601676/22275;

	double phi0[20]= {0,33,33,36,33,36,36,36,36,36,40,44,44,44,26,26,26,26,20,26};
	double lmbd0[20]={0,7770,7860,7930,8010,8060,8160,8230,8310,8390,8450,8415,8535,8655,8520,7650,7440,7860,8160,9240};

	double phi0rad  = phi0[system]/(180/M_PI);
	double lmbd0rad = lmbd0[system]/(60*180/M_PI);

	// 計算する
	double A_ = (m0*a)/(1+n)*A0;
	double S_ = (m0*a)/(1+n)*(
		+A0*phi0rad
		+A1*sin(2*1*phi0rad)
		+A2*sin(2*2*phi0rad)
		+A3*sin(2*3*phi0rad)
		+A4*sin(2*4*phi0rad)
		+A5*sin(2*5*phi0rad));

	double xi  = (x + S_)/A_;
	double eta = y/A_;

	double xi_ = xi-(
        +B1 * sin(2*1*xi) * cosh(2*1*eta)
        +B2 * sin(2*2*xi) * cosh(2*2*eta)
        +B3 * sin(2*3*xi) * cosh(2*3*eta)
        +B4 * sin(2*4*xi) * cosh(2*4*eta)
        +B5 * sin(2*5*xi) * cosh(2*5*eta));
        
	double eta_ = eta-(
        +B1 * cos(2*1*xi) * sinh(2*1*eta)
        +B2 * cos(2*2*xi) * sinh(2*2*eta)
        +B3 * cos(2*3*xi) * sinh(2*3*eta)
        +B4 * cos(2*4*xi) * sinh(2*4*eta)
        +B5 * cos(2*5*xi) * sinh(2*5*eta));

	double chi = asin(sin(xi_)/cosh(eta_));

	double latitude = 
		(chi + (
            +D1*sin(2*1*chi)
            +D2*sin(2*2*chi)
            +D3*sin(2*3*chi)
            +D4*sin(2*4*chi)
            +D5*sin(2*5*chi)
            +D6*sin(2*6*chi)))*180/M_PI*1000*1000*10;
	position.push_back(int(latitude));
		
            
	double longitude = (lmbd0rad + atan(sinh(eta_)/cos(xi_)))*180/M_PI*1000*1000*10;
	position.push_back(int(longitude));
	
	return position;
}

vector<double> wgs842pocs(double lat, double lon, int system){
	vector<double> position;
	double a = 6378137; //楕円体の長半径
	double F=298.257222101; //楕円体の逆扁平率
	double m0=0.9999; //平面直角座標系のX軸上における縮尺係数 (0.9999)

	//展開パラメータを事前に入力する
	double n = 1/(2*F-1);

	double A0 = 1 + pow(n,2)*1/4 + pow(n,4)*1/64;
	double A1 = -(n - pow(n,3)*1/8 - pow(n,5)*1/64)*3/2;
	double A2 = (pow(n,2) - pow(n,4)*1/4)*15/16;
	double A3 = -(pow(n,3) - pow(n,5)*5/16)*35/48;
	double A4 = pow(n,4)*315/512;
	double A5 = -pow(n,5)*693/1280;
  //printf("A0=%lf,A1=%lf,A2=%lf,A3=%lf,A4=%lf,A5=%lf",A0,A1,A2,A3,A4,A5);

  double a1 = n*1/2 - pow(n,2)*2/3 + pow(n,3)*5/16 + pow(n,4)*41/180 -pow(n,5)*127/288;
  double a2 = pow(n,2)*13/48 - pow(n,3)*3/5 + pow(n,4)*557/1440 + pow(n,5)*281/630;
  double a3 = pow(n,3)*61/240 -pow(n,4)*103/140 - pow(n,5)*15061/26880;
  double a4 = pow(n,4)*49561/161280 - pow(n,5)*179/168;
  double a5 = pow(n,5)*34729/80640;
  //printf("a1=%lf,a2=%lf,a3=%lf,a4=%lf,a5=%lf",a1,a2,a3,a4,a5);

	double phi0[20]= {0,33,33,36,33,36,36,36,36,36,40,44,44,44,26,26,26,26,20,26};
	double lmbd0[20]={0,7770,7860,7930,8010,8060,8160,8230,8310,8390,8450,8415,8535,8655,8520,7650,7440,7860,8160,9240};

	double phi0rad  = phi0[system]/(180/M_PI);
	double lmbd0rad = lmbd0[system]/(60*180/M_PI);
  double phirad  = lat*0.0000001/(180/M_PI);
	double lmbdrad = lon*0.0000001/(180/M_PI);
//

	// 計算する
	double A_ = (m0*a)/(1+n)*A0;
	double S_ = (m0*a)/(1+n)*(A0*phi0rad+A1*sin(2*1*phi0rad)+A2*sin(2*2*phi0rad)+A3*sin(2*3*phi0rad)+A4*sin(2*4*phi0rad)+A5*sin(2*5*phi0rad));

  double t = sinh(atanh(sin(phirad))-2*sqrt(n)/(1+n)*atanh(2*sqrt(n)/(1+n)*sin(phirad)));
  double t_ = sqrt(1+pow(t,2));

  double lmbdc = cos(lmbdrad-lmbd0rad);
  double lmbds = sin(lmbdrad-lmbd0rad);
  
  double xi_ = atan(t/lmbdc);
  double eta_ = atanh(lmbds/t_);

  double x = A_*(xi_+
    a1*sin(2*1*xi_)*cosh(2*1*eta_)+
    a2*sin(2*2*xi_)*cosh(2*2*eta_)+
    a3*sin(2*3*xi_)*cosh(2*3*eta_)+
    a4*sin(2*4*xi_)*cosh(2*4*eta_)+
    a5*sin(2*5*xi_)*cosh(2*5*eta_))-
    S_;
    
  double y = A_*(eta_+
    a1*cos(2*1*xi_)*sinh(2*1*eta_)+
    a2*cos(2*2*xi_)*sinh(2*2*eta_)+
    a3*cos(2*3*xi_)*sinh(2*3*eta_)+
    a4*cos(2*4*xi_)*sinh(2*4*eta_)+
    a5*cos(2*5*xi_)*sinh(2*5*eta_));

	position.push_back(y);
  position.push_back(x);
  
	return position;
}

string toHex(int n)
{
  string r;
	string ans;
  std::ostringstream ss;
  ss << std::hex << n;
  std::string result = ss.str();

  //cout << "result.size():" << result.size()<< endl;
	for(int i = result.size();i<7;i++){
        ans += "0";
    }
	ans+=result;

  return ans;
}
