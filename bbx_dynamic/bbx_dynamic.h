#pragma once
#include <string>
#include <set>
#include <map>
#include"linkList.h"
using namespace std;
#ifndef BBX_CONSTRUCTION_H
#define BBX_CONSTRUCTION_H


/*
乘客ID	下单时间	预约时间	订单类型	人数/数量	线路ID	出发地详细地址	出发地坐标经纬度	目的地详细地址	目的地坐标经纬度
派车时间	服务司机id	派车时司机坐标经纬度	车型	上车时间	下车时间	订单金额	订单状态  位置类型	订单类型
18个
*/
const int SMALL_MAX = 10;
const int MAX = 5000;
const int NEIGH_NUM = 5;
const double max_capacity = 4.0;
double NO_COMPUTER = -100000;
//NO无效客户		BOOK包车客户	Carpooling_Instant拼车即时客户	
const int FLAG_NO = 0;
const int FLAG_BOOK = 1;
const int FLAG_CARPOOLING_INSTANT = 2;
const int FLAG_CARPOOLING_ORDER = 3;
//区域表示	0：海沧；1：岛内；2：杏林
const int LOCAL_HAICANG = 0;
const int LOCAL_INLANDS = 1;
const int LOCAL_XINGLIN = 2;
const int VEHICLE_CAPACITY = 4;
const int minTD = 45;

//118.112533,24.555763（杏林大桥）				到					118.143434,24.427385（白石炮台遗址）
//118.082457,24.500666（海沧大桥）				到					118.20628,24.50037（观音山）
const double MAX_DIS = (sqrt((118.112533 - 118.143434) * (118.112533 - 118.143434) + (24.555763 - 24.427385) * (24.555763 - 24.427385)) +
	sqrt((118.082457 - 118.20628) * (118.082457 - 118.20628) + (24.500666 - 24.50037) * (24.500666 - 24.50037))) / 2.0 / 2.0 * 1000;
const int inf = 999999;
const int pop_num = 3;
const int FUNC_NUM = 2;
double BOARD_WEIGHT = 0.9;
double dis_metric[MAX][MAX];
double *dis_point;
vector<double> dis_center;
bool U[MAX];											//订单是否使用的标记

double alpha = 1.0;

vector<double> dis_array;
vector<double> time_array;
vector<int> index_array;
vector<double> arcValues;
vector<double> dis_to_exports;
vector<int> inds;
vector<int> dis_inds;
double	dis_export_customers[MAX];

double total_wait[MAX];                     //the total delay time before i

double weights[pop_num][FUNC_NUM] = { 1.0,0,0.5,0.5,0,1.0 };
int startT = 480, overT = 540;


//传统调度的路径索引	与	路径中第1个客户点预约时间
vector<int> index_tras;
vector<double> time_order_firsts;
pQueue pq;
int cust_num;							//客户点数目

double a_time[MAX];                         //arrival time arrays
double w_time[MAX];                         //wait time	arrays
double l_time[MAX];                         //leave time arrays
double max_wait[MAX];						//the max wait time最大合法延迟时间
static struct Customer {
	string id;								//订单id
	string receiving_time;					//下单时间
	double receiving_seconds;
	double order_time;						//预约时间
	string order_time_format;				//24时形式的预约时间
	string order_date;						//预约日期
	string order_type;						//预约类型
	double demand;							//人数
	string road_id;							//线路id 
	string src_address;						//出发地详细地址
	string src_coordinate;					//出发地经纬度
	string dest_address;					//目的地详细地址
	string dest_coordinate;					//目的地经纬度
	string delivery_time;					//派车时间
	string driver_id;						//司机id
	string driver_coordinate;				//司机经纬度
	string car_type;						//车辆类型
	string boarding_time;					//上车时间
	string getoff_time;						//下车时间
	double cost;							//花销
	int state;								//订单完成情况
	string scheduling_id;
	int location_type;
	int cust_type;
	double wait_time;
}customer[MAX];				//客户点数组


							/*
							海沧大桥
							24.494350, 118.076230
							海沧出口
							24.552766, 117.912872
							生成两个虚假客户点一个是	海沧大桥，一个是	海沧出口

							*/
							//海沧区域
string bridge_coordinates = "24.494350,118.076230";
string export_coordinates = "24.552766,117.912872";
//杏林区域
string bridge_coordinates_almond = "24.549322, 118.106789";
string export_coordinates_almond = "24.590472, 118.004321";

double virtual_point[][2] = { { 24.494350,118.076230 },{ 24.552766,117.912872 } };
double virtual_point_almond[][2] = { { 24.549322 ,118.106789 },{ 24.590472 ,118.004321 } };
//海沧两个虚拟点的欧式距离
double o1 = sqrt((virtual_point[0][0] - virtual_point[1][0]) *(virtual_point[0][0] - virtual_point[1][0]) + (virtual_point[0][1] - virtual_point[1][1]) * (virtual_point[0][1] - virtual_point[1][1]));
//杏林两个虚拟点的欧式距离
double o_almond = sqrt((virtual_point_almond[0][0] - virtual_point_almond[1][0]) *(virtual_point_almond[0][0] - virtual_point_almond[1][0]) + (virtual_point_almond[0][1] - virtual_point_almond[1][1]) * (virtual_point_almond[0][1] - virtual_point_almond[1][1]));


struct Route {							//结构体：路径
	vector<int> customers_on;				//路径中含有的客户点标号
	vector<int> customers_off;			    //下客顺序
	string export_coordinates;				//出口坐标
	string center_coordinates;				//出口坐标
	double demand;
	int T_early;
	double dis_off;
	bool modify;
	double f[FUNC_NUM];											//0	代表	上车行驶距离，	1	代表下车行驶距离
	double sp;													//根据车辆的最早预约时间与当前时间的差值，计算它的紧急程度，值越大，越紧急
	Route() {
		modify = true;
		sp = 0.001;
	}
};

struct Chromosome {						//解
	vector<Route> routes;				//路径集合
	double f[FUNC_NUM];
	int demand;
};
Chromosome EP[pop_num];
vector<Route> fin_routes;
vector<Route> out_routes;

#endif#pragma once
