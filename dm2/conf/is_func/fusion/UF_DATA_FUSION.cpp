#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>

#include <stdlib.h>
#include <sys/stat.h>
//************* ADD START *************//
#include <iomanip>
#include <limits>
//************* ADD  END  *************//
#include "GridMap.h"

using namespace std;
using namespace std::chrono;

void my_copy(const char *, const char *);
int err(const char *);
void file_exist(const char *);

//int fusion(void)
//************* MOD START *************//
//int main(void)
extern "C" vector<vector<string>> UF_DATAFUSION(vector<vector<string>> argList)
//************* MOD  END  *************//
{
	//************* ADD START *************//
	printf("UF_DATAFUSION called!\n");
	vector<vector<string>> retList;
	//************* ADD  END  *************//

	// グリッドマップの全領域 矩形領域4点(3次元)
	double points[4][3] = {
		{136.965625000, 35.156242, 1.5},
		{136.965625000, 35.155217, 1.5},
		{136.967187500, 35.156242, 1.5},
		{136.967187500, 35.155217, 1.5}};

	//************* DEL START *************//
	// KMLファイルのファイル名(複数指定可能)
	//char *files[] = {
	//	(char *)"stereo_1.kml", (char *)"stereo_2.kml"};
	//************* DEL  END  *************//

//	my_copy("stereo_1.kml", "../stereo_1.kml");
//	my_copy("stereo_2.kml", "../stereo_2.kml");

	// KMLファイルのファイル数
	// int numFiles = 2;
	//************* DEL START *************//
	//int numFiles = sizeof(files) / sizeof(files[0]);
	//************* DEL  END  *************//

	// 最初に1回だけ必ずinitialize()を呼ぶこと
	initialize();

	// ファイル名の配列から，グリッドマップの配列を返す関数
	//************* MOD START *************//
	//GridMap *g = getGridMap(points, files, numFiles);
	// エッジの基準点を設定
	double point[1][3] = {
		{ 136.96640625, 35.1557295, 1.5 }
		//{ 136.9667114257813, 35.1557295, 1.5 } //+30m
	};
	std::cout << std::fixed;
	cout << std::setprecision(std::numeric_limits<float >::max_digits10);
	//cout << "point[0][0] : " << point[0][0] << endl;
	//cout << "point[0][1] : " << point[0][1] << endl;
	//cout << "point[0][2] : " << point[0][2] << endl;

	int gridNumX, gridNumY, num;
	double areaPoints[4][3];
	
	// 基準点から指定した大きさの領域を生成する
	createAreaPoints(point, 153.6, 153.6, gridNumX, gridNumY, areaPoints);
	GridMap *g = getGridMap(areaPoints, argList, num, gridNumX, gridNumY);
	//************* MOD  END  *************//

	// 多数決の結果を入れる配列作成
	//************* MOD START *************//
	//int numRows = DEFAULT_NUM_ROWS;
	//int numCols = DEFAULT_NUM_COLUMNS;
	int numRows = gridNumX;
	int numCols = gridNumY;

	//GridMap *n = createGridMap(points, numRows, numCols);
	GridMap *n = createGridMap(areaPoints, numRows, numCols);
	//************* MOD  END  *************//

	// 同じ位置の配列ごとで,セルの属性を決める
//--------------------------------------------------------------------O
	int i, j, k;
	int counti, countj;
	int ai = 0;
	int aj = 0;		
//--------------------------------------------------------------------D
	
	for (i = 0; i < numRows; i++)
	{
		for (j = 0; j < numCols; j++)
		{
//--------------------------------------------------------------------O
			int obj = 0;
			int emp = 0;
			int t_obj = 0;
			counti = 0;
			countj = 0;			
//--------------------------------------------------------------------D
			// OBJECTかEMPTYか多数決で判定
			//************* MOD START *************//
			//for (k = 0; k < numFiles; k++)
			for (k = 0; k < num; k++)
			//************* MOD  END  *************//
			{
				if (g[k].cells[i][j].state == OBJECT)
				{
					obj += 1;
				}
				else if (g[k].cells[i][j].state == EMPTY)
				{
					emp += 1;
				}
				else if (g[k].cells[i][j].state == TEMP_OBJECT)
				{
					t_obj += 1;
				}
			}
			if(t_obj == 0)
			{
				if(obj < emp)
				{
					n->cells[i][j].state = EMPTY;
				} 
				else if (0 < obj && emp <= obj)
				{
					n->cells[i][j].state = OBJECT;
				}
				else if (obj == 0 && emp == 0)
				{
					n->cells[i][j].state = UNKNOWN;
				}
			}
			else if(t_obj == 1)
			{
				if(0 < emp && obj < emp)
				{
					n->cells[i][j].state = EMPTY;
				} 
				else if (0 < obj && emp <= obj)
				{
					n->cells[i][j].state = OBJECT;
				}
				else if (obj == 0 && emp == 0)
				{
					n->cells[i][j].state = UNKNOWN;
				}
			}
			else if(1 < t_obj)
			{
				if(t_obj <= emp && obj < emp)
				{
					n->cells[i][j].state = EMPTY;
				}
				else if(emp < t_obj && obj < emp)
				{
					n->cells[i][j].state = TEMP_OBJECT;
				} 
				else if (t_obj <= obj && emp <= obj)
				{
					n->cells[i][j].state = OBJECT;
				}
				else if (obj < t_obj && emp <= obj)
				{
					n->cells[i][j].state = OBJECT;
				}
				else if (obj == 0 && emp == 0)
				{
					n->cells[i][j].state = TEMP_OBJECT;
				}
			}
			/*			
			}
			if (obj < emp)
			{
				n->cells[i][j].state = EMPTY;
			}
			else if (obj > 0 && obj >= emp)
			{
				n->cells[i][j].state = OBJECT;
			}
			else if (obj == 0 && emp == 0)
			{
				n->cells[i][j].state = UNKNOWN;
			}
			*/
//--------------------------------------------------------------------O
						
			if(1 < i && 1 < j)
			{
				//if(n->cells[i-1][j-1].state = EMPTY || n->cells[i-1][j-1].state = UNKNOWN){
				if(n->cells[i - 1][j - 1].state != OBJECT) // && n->cells[i - 1][j - 1].state != EMPTY)
				{
					if(n->cells[i - 1][j - 2].state == OBJECT || n->cells[i - 1][j].state == OBJECT)
					{
						countj += 1;					
					}
					else if(n->cells[i - 2][j - 2].state == OBJECT || n->cells[i - 2][j].state == OBJECT)
					{
						countj += 1;					
					}
					else if(n->cells[i - 2][j - 1].state == OBJECT || n->cells[i][j - 1].state == OBJECT)
					{
						counti += 1;
					}
					else if(n->cells[i][j - 2].state == OBJECT || n->cells[i][j].state == OBJECT)
					{
						counti += 1;
					}
					//if(1 <= counti && 1 <= countj){
						if(1 <= counti + countj)
						{
							n->cells[i - 1][j - 1].state = TEMP_OBJECT;
						}
					//}
				}
			}
			
			/*
			if((4 + aj) <= j){
				if(n->cells[i][j - (2 + aj)].state != OBJECT){
										
					if(n->cells[i][j - (4 + aj)].state == OBJECT){
						countj += 1;					
					}
					if(n->cells[i][j].state == OBJECT){
						countj += 1;					
					}
					
					
					if(n->cells[i][j - (3 + aj)].state == OBJECT){
						countj += 1;					
					}
					if(n->cells[i][j - (1 + aj)].state == OBJECT){
						countj += 1;
					}
					
					if(2 <= countj){
						n->cells[i][j - (2 + aj)].state = TEMP_OBJECT;
					}					
				}
			}			
		}
		
			if((4 + ai) <= i){
				if(n->cells[i - (2 + ai)][j].state != OBJECT){
										
					if(n->cells[i - (4 + ai)][j].state == OBJECT){
						counti += 1;					
					}
					if(n->cells[i][j].state == OBJECT){
						counti += 1;					
					}
					
				
					if(n->cells[i - (3 + ai)][j].state == OBJECT){
						counti += 1;					
					}
					if(n->cells[i - (1 + ai)][j].state == OBJECT){
						counti += 1;
					}
				
					if(2<= counti){
						n->cells[i - (2 + ai)][j].state = TEMP_OBJECT;
					}
				}
			}*/
			
		}
//--------------------------------------------------------------------D				
	}
	//************* ADD START *************//
	/* for debug
	cout << "---------- INPUT  GridMap Info ----------" << endl;
	for (int i = 0; i<num; i++) {
		cout << "INPUT  GridMap Info [" << i << "] :" << endl;
		convertGridMapToVector(*n, retList);
		retList.clear();
	}*/
	//************* ADD  END  *************//

	// printf("HWW");
	// for (int i = 0; i < 1; i++)
	// {
	//************* MOD START *************//
	//writeToKMLFile((char *)"fusion.kml", *n, true);		// unknownセルを出力しない
	//writeToKMLFile((char *)"stereo_90.kml", g[0], false);
	//writeToKMLFile((char *)"stereo_92.kml", g[1], false);	
	//cout << "---------- OUTPUT GridMap Info ----------" << endl;
	convertGridMapToVector(*n, retList);
	//************* MOD  END  *************//
	// }
	//************* DEL START *************//
	//my_copy("stereo_90.kml", "../stereo_90.kml");
	//my_copy("stereo_92.kml", "../stereo_92.kml");
	//my_copy("fusion.kml", "../fusion.kml");
	//************* DEL  END  *************//

	//************* MOD START *************//
	//for(int i=0; i < numFiles; i++)
	for(int i=0; i < num; i++)
	//************* MOD  END  *************//
		clearGridMap(&g[i]);
	clearGridMap(n);

	delete g;
	delete n;

	// 最後に1回だけ必ずfinish()を呼ぶこと
//	finish();
	//************* MOD START *************//
	//return 0;
	printf("UF_DATAFUSION end!\n");
	return retList;
	//************* MOD  END  *************//
}

void my_copy(const char* from, const char* to)
{
	FILE* f1, *f2;
	int c;

	f1 = fopen(from, "r");
	if (!f1)
		err(from);

	file_exist(to);
	f2 = fopen(to, "w");
	if (!f2)
		err(to);

	while ((c = fgetc(f1)) != EOF)
	{
		if (fputc(c, f2) == EOF)
			err(to);
	}

	fclose(f2);
	fclose(f1);
}

int err(const char *s)
{
	perror(s);
	exit(1);
}

//
void file_exist(const char *fname)
{
	struct stat st; //#include <sys/stat.h>

	if (!stat(fname, &st))
	{
		printf("File %s already exists.\n", fname);
		exit(1);
	}
}

