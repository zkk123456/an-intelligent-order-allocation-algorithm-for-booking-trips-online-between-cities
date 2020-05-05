
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include<algorithm>
#include "bbx_dynamic.h"
#include "WininetHttp.h"
#include "k-means1.h"
#include <stack> 


using namespace std;
#define pi 3.1415926

double max_wait_time = 0;
double max_dist = 0;

string Trim(string& str) {
	//str.find_first_not_of(" \t\r\n"),在字符串str中从索引0开始，返回首次不匹配"\t\r\n"的位置
	str.erase(0, str.find_first_not_of(" \t\r\n"));
	str.erase(str.find_last_not_of(" \t\r\n") + 1);
	return str;
}

bool cmp0(int a, int b) {
	return a < b;
}


bool cmp(int a, int b) {
	return customer[a].order_time < customer[b].order_time;
}

bool cmp1(int a, int b) {
	return dis_point[a] < dis_point[b];
}

bool cmp2(int a, int b) {										//降序排列
	return customer[a].wait_time > customer[b].wait_time;
}

bool cmp3(int a, int b) {
	return customer[a].receiving_seconds < customer[b].receiving_seconds;
}

//升序排序不同路径中心的距离
bool cmp4(int a, int b) {										//升序排序
	return dis_center[a] < dis_center[b];
}

vector<string> splitStr(string s, string substr) {
	char buf[100];
	vector<string> strlist;
	string str(s);
	strcpy(buf, str.c_str());
	char *ptr;
	ptr = strtok(buf, substr.c_str());
	while (ptr != NULL) {
		strlist.push_back(ptr);
		ptr = strtok(NULL, substr.c_str());
	}
	return strlist;
}
//以分钟为单位
double converted_to_number(string strtime) {
	vector<string> arr = splitStr(strtime, ":");
	double time = 0;
	time = (atof(arr[0].c_str())) * 60 + atof(arr[1].c_str()) + atof(arr[2].c_str()) / 60.0;
	return time;
}


bool comp_angle1(int a, int b)//升序结果 
{
	//return earliest_time[a] < earliest_time[b];
	return customer[a].order_time < customer[b].order_time;
}

bool comp_angle3(int a, int b)//升序结果 
{
	//return earliest_time[a] < earliest_time[b];
	return arcValues[a] < arcValues[b];
}

bool comp_angle4(int a, int b)//升序结果 
{
	//return earliest_time[a] < earliest_time[b];
	return dis_to_exports[a] > dis_to_exports[b];
}

bool comp_angle5(int a, int b)//升序结果 
{
	//return earliest_time[a] < earliest_time[b];
	return time_order_firsts[a] < time_order_firsts[b];
}

bool comp_angle2(int a, int b)//升序结果 
{
	//return earliest_time[a] < earliest_time[b];
	return dis_array[a] < dis_array[b] || (dis_array[a] == dis_array[b] && time_array[a] < time_array[b]);
}

/*以分钟为单位
只转换"2018-09-01  01：00：00"以后到"2018-09-02  01：00：00"之前的24h的订单，其他时间为-100000

*/
double time_to_number(string string_time) {
	double tmptime = 0;
	vector<string> date_time = splitStr(string_time, " ");
	if (date_time[0] == "2018-09-01")
		tmptime = converted_to_number(date_time[1]);																//将24制的时间转化为double型的
	else if (date_time[0] == "2018-09-02") {
		tmptime = 24 * 60;
		tmptime += converted_to_number(date_time[1]);
	}
	else
		tmptime = NO_COMPUTER++;
	return tmptime;
}

void getData(string spec) {
	ifstream fin(spec);												 //打开文件流操作
	string line;
	int count = 1;
	getline(fin, line);
	//spec	:	种群，id,demand,arrival_time
	while (getline(fin, line))   //整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取
	{
		istringstream sin(line);
		vector<string> fields;
		string field;
		while (getline(sin, field, ','))
		{
			fields.push_back(field);
		}
		Customer cust;

		string id, receiving_time, order_time, order_type, demand, road_id, src_address, src_coordinate, dest_address, dest_coordinate,
			delivery_time, driver_id, driver_coordinate, car_type, boarding_time, getoff_time, cost, state, scheduling_id, location_type;

		id = Trim(fields[0]); receiving_time = Trim(fields[1]); order_time = Trim(fields[2]); order_type = Trim(fields[3]); demand = Trim(fields[4]);
		road_id = Trim(fields[5]); src_address = Trim(fields[6]); src_coordinate = Trim(fields[7]); dest_address = Trim(fields[8]); dest_coordinate = Trim(fields[9]);
		delivery_time = Trim(fields[10]); driver_id = Trim(fields[11]); driver_coordinate = Trim(fields[12]); car_type = Trim(fields[13]); boarding_time = Trim(fields[14]);
		getoff_time = Trim(fields[15]);	cost = Trim(fields[16]); scheduling_id = Trim(fields[17]);  state = Trim(fields[18]);
		if (receiving_time.size() < 6)
			receiving_time = order_time;
		location_type = Trim(fields[19]);

		vector<string> date_time = splitStr(src_coordinate, ";");
		//else
		//date_time = splitStr(customer[custs[i]].dest_coordinate, ";");
		string str1 = date_time[1] + "," + date_time[0];
		date_time = splitStr(dest_coordinate, ";");
		string str2 = date_time[1] + "," + date_time[0];

		cust.id = id;
		//cust.receiving_time = receiving_time;
		cust.order_type = order_type; cust.demand = atof(demand.c_str());
		cust.road_id = road_id; cust.src_address = src_address; cust.src_coordinate = str1; cust.dest_address = dest_address;
		cust.dest_coordinate = str2; cust.delivery_time = delivery_time; cust.driver_id = driver_id; cust.driver_coordinate = driver_coordinate;
		cust.car_type = car_type; cust.cost = atof(cost.c_str()); cust.scheduling_id = scheduling_id; cust.state = atof(state.c_str());

		cust.location_type = atof(location_type.c_str()) - 1;							//0表示海沧 1表示岛内 2表示杏林
																						//转化时间
																						/*
																						2018 - 08 - 31 22:18 : 23	2018 - 09 - 01 00:00 : 00
																						2018 - 09 - 01 01:14 : 26	2018 - 09 - 01 00:00 : 14
																						2018 - 08 - 31 23:55 : 21	2018 - 09 - 01 00:02 : 03
																						*/
																						//将time to number 		
		date_time = splitStr(order_time, " ");
		cust.order_time = ceil(time_to_number(order_time));
		//cust.order_time_format = date_time[1].substr(0, 5);
		cust.order_time_format = date_time[1];
		cust.order_date = date_time[0];
		cust.wait_time = 0;

		cust.receiving_seconds = time_to_number(receiving_time);
		vector<string> date_time0 = splitStr(receiving_time, " ");
		cust.receiving_time = date_time0[0];
		double rn = rand() % 1000 / 1000.0;
		if (rn < 0.5)
			cust.receiving_seconds = NO_COMPUTER++;
		else {
			if (abs(cust.order_time - cust.receiving_seconds) < 40) {
				double rn2 = rand() % 1000 / 1000.0;
				if (cust.order_time >= 60 && cust.order_time < 300)				//如果是包车，得至少提前60min
					cust.receiving_seconds = cust.order_time - (rn2 * 30 + 60);
				else
					cust.receiving_seconds = cust.order_time - (rn2 * 20 + 40);
			}
		}

		//cust.receiving_time = time_to_number(receiving_time);


		//包车订单的预约时间为01：00：00	~	06:00:00
		if (cust.order_time < 0)
			cust.cust_type = FLAG_NO;
		else if (cust.order_time < 300)
			cust.cust_type = FLAG_BOOK;
		else  //if (diff > 30)
			cust.cust_type = FLAG_CARPOOLING_ORDER;
		//else if (diff >= 0 && diff <= 30)
		//cust.cust_type = FLAG_CARPOOLING_INSTANT;

		//不用处理的用户
		customer[count++] = cust;
	}
	cust_num = count;
	fin.close();
}

void getData1(string spec, vector<vector<int>> &tra_arranges) {
	//打开文件读取文件内容到文件流对象 in中
	//1.建三个文件流对象，和文件关联	
	ifstream fin(spec); //打开文件流操作
	string line;
	int count = 0;
	vector<vector<int>> routes_cuts;
	//spec	:	种群，id,demand,arrival_time
	while (getline(fin, line))   //整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取
	{
		istringstream sin(line);
		vector<string> fields;
		string field;
		while (getline(sin, field, ','))
		{
			fields.push_back(field);
		}
		vector<int> route_custs;
		for (int i = 0; i < fields.size(); i++)
			route_custs.push_back(atof(Trim(fields[i]).c_str()) + 1);						//因为传统分类从0开始计数

		routes_cuts.push_back(route_custs);
		index_tras.push_back(count++);
		int ind = atof(Trim(fields[0]).c_str());
		time_order_firsts.push_back(customer[ind].order_time);
	}
	sort(index_tras.begin(), index_tras.end(), comp_angle5);

	for (int i = 0; i < routes_cuts.size(); i++) {
		vector<int> custs;
		custs.insert(custs.begin(), routes_cuts[index_tras[i]].begin(), routes_cuts[index_tras[i]].end());
		tra_arranges.push_back(custs);
	}

	fin.close();

}


void print_result(vector<Route> carpooling_routes, string path) {
	ofstream output;
	int vehicle_num = 0, count = 0;
	//double dis_on = 0, dis_off = 0, demand = 0, wait_time = 0;
	double dis_on = 0, demand = 0, wait_time = 0;
	//path = "A.csv";
	output.open(path, ios::out);
	for (int i = 0; i < carpooling_routes.size(); i++) {
		bool flag = false;
		Route r = carpooling_routes[i];
		//求每条路径的最大相邻客户点的最大间隔时间
		double maxV = DBL_MIN;
		Route desc_r;
		for (int j = 0; j < r.customers_on.size(); j++) {
			vector<string> coordinate = splitStr(customer[r.customers_on[j]].src_coordinate, ",");
			//output << i  << ",\t\t\t\t" << coordinate[0] << ",\t\t\t\t" << coordinate[1] << ",\t\t\t\t" << customer[r.customers_on[j]].order_date << ",\t" << customer[r.customers_on[j]].demand << ",\t" << r.customers_on[j] << endl;
			if (customer[r.customers_on[j]].order_time >= startT && customer[r.customers_on[j]].order_time <= overT) {
				//output << NUM_ROUTE << "," << coordinate[1] << "," << coordinate[0] << "," << customer[r.customers_on[j]].order_time_format << endl;					//<< "," << customer[r.customers_on[j]].order_time << "," << customer[r.customers_on[j]].demand << endl;
				flag = true;
				desc_r.customers_on.push_back(r.customers_on[j]);
			}
			if (j < r.customers_on.size() - 1) {
				double value = customer[r.customers_on[j + 1]].order_time - customer[r.customers_on[j]].order_time;
				if (value > maxV)
					maxV = value;
			}
			count++;
		}
		cout << "---------------------" << maxV << endl;
		if (r.demand > VEHICLE_CAPACITY)
			r.demand = VEHICLE_CAPACITY;
		demand += abs(r.demand / VEHICLE_CAPACITY);
		if (flag) 
			out_routes.push_back(desc_r);
			

		dis_on += r.f[0];
		//dis_off += r.dis_off;
		wait_time += r.f[1];
	}
	//output << "\n\n\n" << "demand :" << +demand * 1.0 / carpooling_routes.size() << "\t" << "dis_on : " << dis_on * 1.0 / carpooling_routes.size() << "\t" << "dis_off : " << dis_off * 1.0 / carpooling_routes.size() << "wait_time:" << wait_time * 1.0 / carpooling_routes.size() << endl;
	//output << "\n\n\n" << "demand :" << +demand * 1.0 / carpooling_routes.size() << "\t" << "dis_on : " << dis_on * 1.0 / carpooling_routes.size() << "wait_time:" << wait_time * 1.0 / carpooling_routes.size() << endl;
	//output << "服务的客户点数：" << count << endl;
	output.close();
}



//此处1000是为了方便计算
double calculateDis(string src, string desc) {
	vector<double> a, b;
	vector<string> date_time = splitStr(src, ",");
	a.push_back(atof(date_time[0].c_str()) * 1000);
	a.push_back(atof(date_time[1].c_str()) * 1000);

	date_time = splitStr(desc, ",");
	b.push_back(atof(date_time[0].c_str()) * 1000);
	b.push_back(atof(date_time[1].c_str()) * 1000);
	return sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
}


/*
后期得重写，让其重新计算久岛外元素
//-1表示海沧 0表示岛内 1表示杏林
*/
double getEuclideanDis(Customer src, Customer desc) {
	//double getEuclideanDis(string src,string desc) {
	//两点都在岛内；										0		0
	//一个点在岛内，一个在岛外								0		1（-1）
	//  两个都在岛外（相同的岛外，不同的岛外）		1		1	||	1		-1
	int v1 = src.location_type, v2 = desc.location_type;
	double dis = 0;
	string bridge = bridge_coordinates, export1 = export_coordinates;
	if (v1 == v2)												//都在岛内，或在岛外的相同区
		dis = calculateDis(src.src_coordinate, desc.src_coordinate);
	else if (v1 * v2 == 0) {
		//找出岛外的点
		if (v1 + v2 == 1) {
			bridge = bridge_coordinates_almond;
			export1 = export_coordinates_almond;
		}
		string pos = v1 == 0 ? desc.src_coordinate : src.src_coordinate;
		dis = calculateDis(bridge, pos) + calculateDis(pos, export1) - calculateDis(bridge, export1);
	}
	else
		dis = inf;

	return dis;
}

void computeCapacity(Route &r) {
	vector<int> custs = r.customers_on;
	double demand = 0;
	for (int i = 0; i < custs.size(); i++) {
		demand += customer[custs[i]].demand;
	}
	r.demand = demand;

}

void arrange_customer_on_dist(Route &r) {
	vector<double> longitudes, latitudes;
	vector<int> custs = r.customers_on;
	int len = custs.size();
	double lo, la, sum_lo = 0, sum_la = 0;											//对于下车点的顺序	：	求这几个点的中间经纬度	
	vector<string> date_time;
	for (int i = 0; i < len; i++) {
		date_time = splitStr(customer[custs[i]].src_coordinate, ",");
		lo = atof(date_time[0].c_str());
		la = atof(date_time[1].c_str());
		longitudes.push_back(lo);
		latitudes.push_back(la);
		sum_lo += lo;
		sum_la += la;
	}
	sum_lo /= len;
	sum_la /= len;
	r.center_coordinates =  to_string(sum_lo) + ";" + to_string(sum_la);

	inds.clear();
	arcValues.clear();
	for (int i = 0; i < len; i++) {
		//上车点排序,计算几个点的极角
		lo = longitudes[i] - sum_lo;
		la = latitudes[i] - sum_la;
		double t = la * 1.0 / lo;
		double arc = atan(t) *(double)180 / pi;
		inds.push_back(i);
		arcValues.push_back(arc);
	}
	//根据极角，对下客点进行排序
	sort(inds.begin(), inds.end(), comp_angle3);
	r.customers_on.clear();
	for (int i = 0; i < len; i++)
		r.customers_on.push_back(custs[inds[i]]);
}


void arrange_customer_on(Route &r) {
	//1.判断出城点在哪个区
	vector<double> longitudes, latitudes;
	vector<int> custs = r.customers_on;
	int len = custs.size();
	int sum = 0;
	for (int i = 0; i < len; i++)
		sum += customer[custs[i]].location_type;
	string end_coordinates;
	if (sum < 0)							//海沧
		end_coordinates = export_coordinates;
	else									//杏林
		end_coordinates = export_coordinates_almond;
	//加出城口
	r.export_coordinates = end_coordinates;
	//用longitudes和latitudes记录下每个朋友的经纬度		 

	//根据		上车点到出站口的距离进行排序
	r.customers_on.clear();
	int ind = 0;
	for (int i = 0; i < custs.size(); i++)
		dis_metric[ind][custs[i]] = atof(test(customer[custs[i]].src_coordinate, end_coordinates, false).c_str());
	dis_point = dis_metric[ind];
	sort(custs.begin(), custs.end(), cmp1);
	reverse(custs.begin(), custs.end());
	r.customers_on.insert(r.customers_on.end(),custs.begin(),custs.end());
	
}


void arrange_customer_off(Route &r) {
	//1.判断出城点在哪个区
	vector<double> longitudes, latitudes;
	vector<int> custs = r.customers_on;
	int len = custs.size();
	double lo, la, sum_lo = 0, sum_la = 0;											//对于下车点的顺序	：	求这几个点的中间经纬度	
	vector<string> date_time;
	for (int i = 0; i < len; i++) {
		date_time = splitStr(customer[custs[i]].dest_coordinate, ",");
		lo = atof(date_time[0].c_str());
		la = atof(date_time[1].c_str());
		longitudes.push_back(lo);
		latitudes.push_back(la);
		sum_lo += lo;
		sum_la += la;
	}
	sum_lo /= len;
	sum_la /= len;

	inds.clear();
	arcValues.clear();
	for (int i = 0; i < len; i++) {
		//下车点排序,计算几个点的极角
		lo = longitudes[i] - sum_lo;
		la = latitudes[i] - sum_la;
		double t = la * 1.0 / lo;
		double arc = atan(t) *(double)180 / pi;
		inds.push_back(i);
		arcValues.push_back(arc);
	}
	//根据极角，对下客点进行排序
	sort(inds.begin(), inds.end(), comp_angle3);
	r.customers_off.clear();
	for (int i = 0; i < len; i++)
		r.customers_off.push_back(custs[inds[i]]);
}

void arrange_customer(Route &r) {
	arrange_customer_on(r);
	arrange_customer_off(r);
}

/*
计算了路径的最早预约时间,接客行驶距离,路径是否被评价
*/
void compute_route(Route &r) {
	vector<int> custs = r.customers_on;
	double  travel_time = customer[custs[0]].order_time;
	r.f[1] = 0;
	int len = custs.size();
	int T_early = customer[custs[0]].order_time;

	//要更新的量：最早的预约时间、上车行驶的距离
	//计算上车距离
	double dis_sum = 0;
	vector<string> res;
	for (int i = 1; i < len; i++) {
		res = test2(customer[custs[i - 1]].src_coordinate, customer[custs[i]].src_coordinate);
		dis_sum += atof(res[0].c_str());
		if (T_early > customer[custs[i]].order_time)
			T_early = customer[custs[i]].order_time;

		double wait_time = 0;
		travel_time += atof(res[1].c_str());
		if (customer[custs[i]].order_time > travel_time)
			travel_time = customer[custs[i]].order_time;
		else
			wait_time = (travel_time - customer[custs[i]].order_time);

		r.f[1] += wait_time;
		customer[custs[i]].wait_time = wait_time;
	}	
	r.T_early = T_early;

	dis_sum += atof(test(customer[custs[len - 1]].src_coordinate, r.export_coordinates, false).c_str());
	r.f[0] = dis_sum;
}

void compute_route_dist(Route &r) {
	vector<int> custs = r.customers_on;
	int len = custs.size();
	int T_early = customer[custs[0]].order_time;
	//计算上车距离
	double dis_sum = 0;
	//按顺序计算路径的行驶距离
	for (int i = 1; i < len; i++) {
		dis_sum += getEuclideanDis(customer[custs[i - 1]], customer[custs[i]]);
		if (T_early >  customer[custs[i]].order_time)
			T_early = customer[custs[i]].order_time;
	}
	r.T_early = T_early;
	r.f[0] = dis_sum;
	r.modify = false;

}


void compute_time(Route &r) {
	vector<int> custs = r.customers_on;
	double  travel_time = customer[custs[0]].order_time;
	total_wait[custs[0]] = 0;
	r.f[1] = 0;
	for (int i = 1; i < custs.size(); i++) {
		double wait_time = 0;
		travel_time += atof(test(customer[custs[i - 1]].src_coordinate, customer[custs[i]].src_coordinate, true).c_str());

		if (customer[custs[i]].order_time > travel_time)
			travel_time = customer[custs[i]].order_time;
		else
			wait_time = (travel_time - customer[custs[i]].order_time);

		r.f[1] += wait_time;
		customer[custs[i]].wait_time = wait_time;
		total_wait[custs[i]] = total_wait[custs[i - 1]] + wait_time;
	}
}

void compute_route_capacity(Route &route) {
	double demand = 0;
	vector<int> custs = route.customers_on;
	for (int i = 0; i < custs.size(); i++)
		demand += customer[custs[i]].demand;
	route.demand = demand;
}

void compute_r(Route &r) {
	compute_route(r);
	compute_route_capacity(r);
	compute_time(r);
}

void compute_r_dist(Route &r) {
	if (r.customers_on.size() == 1) {
		r.center_coordinates = customer[r.customers_on[0]].src_coordinate;
		r.f[0] = 0;
		r.T_early = customer[r.customers_on[0]].order_time;
		r.modify = false;
		compute_route_capacity(r);
		return;
	}
	else {
		arrange_customer_on_dist(r);
		compute_route_dist(r);
		compute_route_capacity(r);
		return;
	}

}

void compute_r1(Route &r) {
	if (r.customers_on.size() == 1) {
		r.f[0] = 0;
		r.f[1] = 0;
		r.T_early = customer[r.customers_on[0]].order_time;
		r.modify = false;
		compute_route_capacity(r);
		return;
	}
	else {
		arrange_customer_on(r);
		compute_route(r);
		compute_route_capacity(r);
		return;
	}

}

void print_f(vector<Route> carpooling_routes, string path) {
	ofstream output;
	output.open(path, ios::app);
	double dis_on = 0, demand = 0, wait_time = 0;
	for (int i = 0; i < carpooling_routes.size(); i++) {
		Route r = carpooling_routes[i];
		compute_r1(r);

		for (int j = 0; j < r.customers_on.size(); j++) {
			vector<string> coordinate = splitStr(customer[r.customers_on[j]].src_coordinate, ",");
			output << i << "," << coordinate[1] << "," << coordinate[0] << "," << customer[r.customers_on[j]].order_time_format << "," << r.customers_on[j] << endl;
		}

		demand += abs(r.demand / VEHICLE_CAPACITY);
		dis_on += r.f[0];
		wait_time += r.f[1];
	}

	output << "\n\n\n" << "demand :" << +demand * 1.0 / carpooling_routes.size() << "\t" << "dis_on : " << dis_on * 1.0 / carpooling_routes.size() << "wait_time:" << wait_time * 1.0 / carpooling_routes.size() << endl;
	output.close();
}




bool check_best_pos(int cust_index, Route& route, int &c_pos, double &min_delta, int obj, double now_dist, double now_wait) {
	bool feasible = false;
	min_delta = 99999999; double delta_dist = 0;
	double capacity = 0, delta = 0;
	//容量约束检查
	vector<int>::iterator c_iter;
	for (c_iter = route.customers_on.begin(); c_iter != route.customers_on.end(); ++c_iter)
		capacity += customer[*c_iter].demand;

	capacity += customer[cust_index].demand;
	if (capacity > max_capacity)					//容量约束没通过
		return false;
	//容量约束检查通过，检查时间约束
	compute_time(route);
	double travel_time = 300;
	c_iter = route.customers_on.begin() + 1;
	total_wait[*(c_iter - 1)] = 0;
	for (; c_iter != route.customers_on.end(); ++c_iter) {
		double wait_time = total_wait[*(c_iter - 1)];

		//插入到c_iter之前，求到达cust_index的时间
		a_time[cust_index] = travel_time +
			atof(test(customer[*(c_iter - 1)].src_coordinate, customer[cust_index].src_coordinate, true).c_str());
		//提前到cust_index客户点
		if (a_time[cust_index] < customer[cust_index].order_time)
			a_time[cust_index] = customer[cust_index].order_time;
		//迟到cust_index客户点，但迟到时间不超过max_delay_time
		if (a_time[cust_index] > customer[cust_index].order_time)
			wait_time += a_time[cust_index] - customer[cust_index].order_time;
		travel_time = a_time[cust_index];

		//在c_iter的距离增量
		if (dis_metric[*c_iter][*(c_iter - 1)] != inf)
			delta_dist = -dis_metric[*c_iter][*(c_iter - 1)];
		else
			delta_dist = -atof(test(customer[*(c_iter - 1)].src_coordinate, customer[*c_iter].src_coordinate, false).c_str());

		delta_dist += atof(test(customer[*(c_iter - 1)].src_coordinate, customer[cust_index].src_coordinate, false).c_str())
			+ atof(test(customer[cust_index].src_coordinate, customer[*c_iter].src_coordinate, false).c_str());

		//到c_iter的行驶时间
		travel_time = travel_time +
			atof(test(customer[cust_index].src_coordinate, customer[*c_iter].src_coordinate, true).c_str());
		//早到c_iter
		if (travel_time < customer[*c_iter].order_time)
			travel_time = customer[*c_iter].order_time;
		//迟到c_iter
		if (travel_time > customer[*c_iter].order_time)
			//delay_time += customer[*c_iter].e_time - travel_time;						//原来是这样的，这样可以算出延迟时间吗
			wait_time += travel_time - customer[*c_iter].order_time;

		//之后客户点
		vector<int>::iterator c_ind = c_iter;
		for (; c_ind != route.customers_on.end() - 1; ++c_ind) {
			travel_time += atof(test(customer[*c_iter].src_coordinate, customer[*(c_iter + 1)].src_coordinate, true).c_str());
			if (travel_time < customer[*(c_iter + 1)].order_time)
				travel_time = customer[*(c_iter + 1)].order_time;

			//travel_time表示到达c_iter + 1的行驶时间，
			if (travel_time > customer[*(c_iter + 1)].order_time)
				wait_time += travel_time - customer[*(c_iter + 1)].order_time; //我认为是这样的
		}

		if (obj == 0) {
			delta = now_dist + delta_dist;
		}
		else
			delta = now_wait + wait_time;


		if (min_delta > delta) {
			min_delta = delta;
			c_pos = c_iter - route.customers_on.begin();
			feasible = true;
		}

	}
	return feasible;
}

void insert_customer(Chromosome &chrome, int obj, int cust) {
	//没有插入custermer[cust]客户点时，解的总行驶距离
	double now_dist = 0;
	////没有插入custermer[cust]客户点时，解的总延迟时间
	double now_wait = 0;
	vector<Route>::iterator r_iter, r_pos;
	//求目前总行驶距离
	for (r_iter = chrome.routes.begin(); r_iter != chrome.routes.end(); ++r_iter)
		now_dist += r_iter->f[0];

	int c_pos = 0;
	int min_c_pos = 0;
	double min_delta = 99999999;
	double delta = 0;
	bool feasible = false;
	for (r_iter = chrome.routes.begin(); r_iter != chrome.routes.end(); ++r_iter) {
		now_wait = 0;
		for (vector<Route>::iterator r_index = chrome.routes.begin(); r_index != chrome.routes.end(); ++r_index)
			if (r_iter != r_index)						//除r_iter以外其他路径的延迟时间				
				now_wait += r_index->f[1];

		//此处有问题
		if (check_best_pos(cust, *r_iter, c_pos, delta, obj, now_dist, now_wait)) {  //检测将cust插入r_iter路径中是否是最优
			feasible = true;
			if (min_delta > delta) {
				min_delta = delta;
				min_c_pos = c_pos;
				r_pos = r_iter;
			}
		}
	}

	//没有找到最佳插入位置
	if (!feasible) {
		//新建一个路径，0,cust,0
		Route temp_route;
		temp_route.customers_on.push_back(cust);
		arrange_customer(temp_route);									//安排上下车的顺序
		chrome.routes.push_back(temp_route);
	}
	else { //找到最佳插入位置，把cust插入到最佳路径r_pos的最佳位置min_c_pos中		 
		r_pos->customers_on.insert(r_pos->customers_on.begin() + min_c_pos, cust);
		r_pos->modify = true;
	}
}

//将接收到的订单插入到routes中,此方法得重写
void  insertOrderInRoutes(vector<int> &resCusts, vector<Route> &routes) {
	sort(resCusts.begin(), resCusts.end(), cmp);
	Chromosome some;
	double sum_dis = 0, sum_wait = 0, demand = 0;
	for (int i = 0; i < routes.size(); i++)
		some.routes.push_back(routes[i]);

	//将resCusts中的客户点插入到routes中的路径里
	for (int i = 0; i < resCusts.size(); i++) {
		Customer c = customer[resCusts[i]];
		double now_time = 9999999;
		insert_customer(some, 0, resCusts[i]);											//0	：	表示优化行驶距离
	}
}

/*功能:判断cust客户点是否可以插入到route路径中,并且实现插入
1.route			:	某路径
2.cust			:	路径中的客户点
*/
bool check_feasible_place(Route &route, int cust) {
	double time = 0, capacity = 0;
	//计算路径route的容量
	compute_route_capacity(route);
	capacity = route.demand;
	capacity += customer[cust].demand;
	if (capacity > max_capacity)							//没通过容量约束
		return false;

	route.customers_on.push_back(cust);
	//route.customers_off.push_back(cust);
	route.modify = true;
	return true;
}

bool route_number(Chromosome &chrome, int route_ind) {
	vector<int>::iterator c_iter;
	vector<Route>::iterator r_iter;
	//对route_ind路径中的客户点进行遍历
	for (c_iter = chrome.routes[route_ind].customers_on.begin(); c_iter != chrome.routes[route_ind].customers_on.end();) {
		for (r_iter = chrome.routes.begin(); r_iter != chrome.routes.end(); ++r_iter) {
			if (r_iter - chrome.routes.begin() != route_ind && check_feasible_place(*r_iter, *c_iter)) {								//*c_iter客户点可以插入到*r_iter路径
				c_iter = chrome.routes[route_ind].customers_on.erase(c_iter);//因为c_iter客户点被移除，c_iter = chrome.routes[route_ind].customers.begin()+1是容器中其它客户点
																			 //chrome.routes[route_ind].customers_off.erase(c_iter);
				r_iter->modify = true;
				break;
			}
		}
		if (r_iter == chrome.routes.end())
			return false;

		if (r_iter == chrome.routes.end())																		//c_iter都没插入，到最后一条路径 ，不问它了，插入下一个
			++c_iter;																												//循环步长
	}
	if (chrome.routes[route_ind].customers_on.size() == 0) {
		chrome.routes.erase(chrome.routes.begin() + route_ind);																		//删除route_ind路径
		return true;
	}
	return false;
}

void neigh_operator_romove_route(Chromosome &chrome) {
	//选择要删除的路径
	while (true) {
		vector<int>::iterator c_iter;
		vector<Route>::iterator r_iter;
		int route_ind;																			//记录第几条路径
																								//从解中找到订单数量最少的路径（即：简单的轮盘赌选择策略），记录其下标，尝试删除路径。
		int min_num = 999999;
		for (r_iter = chrome.routes.begin(); r_iter != chrome.routes.end(); ++r_iter) {
			if (min_num > r_iter->customers_on.size()) {
				min_num = r_iter->customers_on.size();
				route_ind = r_iter - chrome.routes.begin();
			}
		}
		if (!route_number(chrome, route_ind))														//删除route_ind路径失败，跳出while循环
			break;
	}

	for (int i = 0; i < chrome.routes.size(); i++)
		if (chrome.routes[i].modify)
			compute_r(chrome.routes[i]);
}

void calculate_solution_length(Chromosome &some) {
	double sum_dis = 0;
	for (int i = 0; i < some.routes.size(); i++)
		sum_dis += BOARD_WEIGHT * some.routes[i].f[0] + (1 - BOARD_WEIGHT) * some.routes[i].dis_off;
	some.f[0] = sum_dis;
	return;
}

void calculate_solution_time(Chromosome &some) {
	double sum_wait = 0;
	for (int i = 0; i < some.routes.size(); i++)
		sum_wait += some.routes[i].f[1];

	some.f[1] = sum_wait;
}

void calculate_solution_capacity(Chromosome &some) {
	double demand = 0;
	for (int i = 0; i < some.routes.size(); i++)
		demand += some.routes[i].demand;
	some.demand = demand;
}

void calculate_solution(Chromosome &some) {
	calculate_solution_length(some);
	calculate_solution_time(some);
	calculate_solution_capacity(some);
}


// exchange two customers，距离的变化将路径r1中的客户点k1与路径r2中的客户点k2进行交换
void ls_exchange(Chromosome & s, int r1, int obj, int &count) {
	count++;
	int r2;
	int V = s.routes.size();
	r2 = rand() % V;
	int nv1 = s.routes[r1].customers_on.size(), nv2 = s.routes[r2].customers_on.size();

	if (nv1 > 1 && nv2 > 1) {										//客户点数目都大于1
		int k1 = rand() % (nv1 - 1) + 1;
		int k2 = rand() % (nv2 - 1) + 1;
		//int k1 = rand() % nv1;
		//int k2 = rand() % nv2;
		int x = s.routes[r1].customers_on[k1];
		int y = s.routes[r2].customers_on[k2];
		string str1;
		string str2;

		if (k1 + 1 >= nv1)
			str1 = s.routes[r1].export_coordinates;
		else
			str1 = customer[s.routes[r1].customers_on[k1 + 1]].src_coordinate;
		if (k2 + 1 >= nv2)
			str2 = s.routes[r2].export_coordinates;
		else
			str2 = customer[s.routes[r2].customers_on[k2 + 1]].src_coordinate;

		if (x == y) {												//有存在的可能性，因为当r1 == r2 ，且k1 == k2时
			return;
		}

		if (r1 == r2 ||
			(s.routes[r1].demand - customer[x].demand + customer[y].demand <= max_capacity + 1e-5 &&
				s.routes[r2].demand - customer[y].demand + customer[x].demand <= max_capacity + 1e-5)) {  //当r1 == r2,且x与y进行交换后，容量约束满足
			double dt1, dt2;

			//距离的变化
			//计算 路径r1中k1 - 1 到r2中k2的距离				
			if (r1 == r2 && k2 == k1 + 1) {
				dt1 = atof(test(customer[s.routes[r1].customers_on[k1 - 1]].src_coordinate, customer[y].src_coordinate, false).c_str()) -
					atof(test(customer[s.routes[r1].customers_on[k1 - 1]].src_coordinate, customer[x].src_coordinate, false).c_str());

				dt2 = atof(test(customer[x].src_coordinate, str2, false).c_str()) -
					atof(test(customer[y].src_coordinate, str2, false).c_str());

			}
			else if (r1 == r2 && k1 == k2 + 1) {
				dt1 = atof(test(customer[s.routes[r2].customers_on[k2 - 1]].src_coordinate, customer[x].src_coordinate, false).c_str()) -
					atof(test(customer[s.routes[r2].customers_on[k2 - 1]].src_coordinate, customer[y].src_coordinate, false).c_str());

				dt2 = atof(test(customer[y].src_coordinate, str1, false).c_str()) -
					atof(test(customer[x].src_coordinate, str1, false).c_str());

			}
			else {
				dt1 = atof(test(customer[s.routes[r1].customers_on[k1 - 1]].src_coordinate, customer[y].src_coordinate, false).c_str())
					+ atof(test(customer[y].src_coordinate, str1, false).c_str())
					- atof(test(customer[s.routes[r1].customers_on[k1 - 1]].src_coordinate, customer[x].src_coordinate, false).c_str())
					- atof(test(customer[x].src_coordinate, str1, false).c_str());

				dt2 = atof(test(customer[s.routes[r2].customers_on[k2 - 1]].src_coordinate, customer[x].src_coordinate, false).c_str())
					+ atof(test(customer[x].src_coordinate, str2, false).c_str())
					- atof(test(customer[s.routes[r2].customers_on[k2 - 1]].src_coordinate, customer[y].src_coordinate, false).c_str())
					- atof(test(customer[y].src_coordinate, str2, false).c_str());
			}

			Chromosome ss;
			for (int i = 0; i < V; i++) {
				Route r;
				ss.routes.push_back(r);
				ss.routes[i].demand = s.routes[i].demand;
				ss.routes[i].f[0] = s.routes[i].f[0];
				ss.routes[i].dis_off = s.routes[i].dis_off;
				ss.routes[i].f[1] = s.routes[i].f[1];
			}

			ss.routes[r1].demand += customer[y].demand - customer[x].demand;
			ss.routes[r2].demand += customer[x].demand - customer[y].demand;

			ss.routes[r1].f[obj] += dt1;
			ss.routes[r2].f[obj] += dt2;

			//对比ss与s点的第obj个目标
			calculate_solution_length(ss);
			calculate_solution_time(ss);
			calculate_solution_capacity(ss);
			if (ss.f[obj] < s.f[obj]) {
				s.f[obj] = ss.f[obj];
				s.routes[r1].demand = ss.routes[r1].demand;
				s.routes[r2].demand = ss.routes[r2].demand;
				s.routes[r1].f[obj] = ss.routes[r1].f[obj];
				s.routes[r2].f[obj] = ss.routes[r2].f[obj];
				s.routes[r1].customers_on[k1] = y;
				s.routes[r2].customers_on[k2] = x;
				s.routes[r1].modify = true;
				s.routes[r2].modify = true;
				count = 11;
			}
		}
	}

}





void ls_N1(Chromosome &tmp_some, int r_ind, int obj) {
	//找到r_ind中等待时间最大的客户点
	inds.clear();
	Route r = tmp_some.routes[r_ind];
	for (int i = 0; i < r.customers_on.size(); i++)
		inds.push_back(r.customers_on[i]);

	sort(inds.begin(), inds.end(), cmp2);
	//将等待时间最大的客户点从删除
	vector<int>::iterator it = tmp_some.routes[r_ind].customers_on.begin();
	while (it != tmp_some.routes[r_ind].customers_on.end())
		if (*it == inds[0]) {
			it = tmp_some.routes[r_ind].customers_on.erase(it);    //删除元素，返回值指向已删除元素的下一个位置    
			break;
		}
		else
			++it;    //指向下一个位置
					 //将客户点	inds[0]		插入到其他路径中，并获得更小的等待时间
	insert_customer(tmp_some, obj, inds[0]);
}

void local_search(Chromosome &tmp_some, vector<int> weight) {
	//确定优化的目标
	int obj = weight[0] == 1 ? 0 : 1;
	int r_ind = 0;
	double max = DBL_MIN;
	for (int i = 0; i < tmp_some.routes.size(); i++)
		if (max < tmp_some.routes[i].f[obj]) {
			max = tmp_some.routes[i].f[obj];
			r_ind = i;
		}
	int count = 0;
	if (obj == 0)
		while (count < 10)
			ls_exchange(tmp_some, r_ind, obj, count);
	else
		ls_N1(tmp_some, r_ind, obj);

}

void update_EP(Chromosome other) {
	for (int i = 0; i < pop_num; i++) {
		Chromosome some = EP[i];
		double v1 = 0, v2 = 0;
		for (int j = 0; j < FUNC_NUM; j++) {
			v1 += weights[i][j] * some.f[j];
			v2 += weights[i][j] * other.f[j];
		}
		if (v1 > v2)
			EP[i] = other;
	}

}

void negibor_operater(vector<Route> &routes) {
	//将routes包装为一个个体，然后采用邻域操作进行优化
	Chromosome some;
	double sum_dis = 0, sum_wait = 0, demand = 0;
	for (int i = 0; i < routes.size(); i++) {
		some.routes.push_back(routes[i]);
		sum_dis += BOARD_WEIGHT * routes[i].f[0] + (1 - BOARD_WEIGHT) * routes[i].dis_off;
		sum_wait += routes[i].f[1];
		demand += routes[i].demand;
	}
	some.f[0] = sum_dis;
	some.f[1] = sum_wait;
	some.demand = demand;
	//评价 当前解

	//进行邻域操作,如果使用的车辆数目大于demand / 4.0 + 1,此操作减少了车辆的总行驶距离
	if (some.routes.size() > ceil(some.demand / max_capacity) + 1) {
		neigh_operator_romove_route(some);
		calculate_solution(some);
	}

	for (int i = 0; i < pop_num; i++)
		EP[i] = some;

	for (int i = 0; i < FUNC_NUM; i++) {
		vector<int> weight;
		weight.push_back(0);
		weight.push_back(0);
		weight[i] = 1;
		Chromosome tmp_some = some;
		local_search(tmp_some, weight);
		for (int i = 0; i < tmp_some.routes.size(); i++)
			if (tmp_some.routes[i].modify)
				compute_r(tmp_some.routes[i]);

		calculate_solution(tmp_some);

		update_EP(tmp_some);
	}
	routes.clear();
	for (int i = 0; i < EP[1].routes.size(); i++)
		routes.push_back(EP[1].routes[i]);
}

void print_chrome(vector<Route> &routes, int T) {
	ofstream output;
	output.open("./change/k_means_result.txt", ios::app);
	vector<Route>::iterator r_iter = routes.begin();
	while (r_iter != routes.end()) {
		if (r_iter->T_early - T < 60) {
			Route r = *r_iter;
			fin_routes.push_back(r);
			for (int j = 0; j < r.customers_on.size(); j++)
				output << r.customers_on[j] << "->";
			output << "......";
			for (int j = 0; j < r.customers_off.size(); j++)
				output << r.customers_off[j] << "->";
			output << endl;
			r_iter = routes.erase(r_iter);//删除元素，返回值指向已删除元素的下一个位置
		}
		else {
			++r_iter;
		}
		output.close();
	}
}

void print_chrome3(vector<Route> &routes, int T) {
	if (T < 320)
		return;
	if (routes.size() == 0  ||  routes.begin()->T_early - T > 40)
		return;
	ofstream output;
	output.open("./change/k_means_result0.csv", ios::app);
	output << "优化后" << "," << endl;
	vector<Route>::iterator r_iter = routes.begin();
	int i = 0;
	while (r_iter != routes.end()) {
		Route r = *r_iter;
		vector<string> coordinate;
		for (int j = 0; j < r.customers_on.size(); j++) {
			coordinate = splitStr(customer[r.customers_on[j]].src_coordinate, ",");
			output << i << "," << coordinate[1] << "," << coordinate[0] << "," << customer[r.customers_on[j]].order_time << endl;
		} 
		i++;
		r_iter++;
	}
	output.close();
}

void print_route(Route r) {
	ofstream output;
	output.open("./change/init_solution.csv", ios::app);
	vector<string> coordinate;
	Route desc_r;
	bool flag = false;
	for (int j = 0; j < r.customers_on.size(); j++) 
		if (customer[r.customers_on[j]].order_time >= startT && customer[r.customers_on[j]].order_time <= overT) {
			flag = true;
			desc_r.customers_on.push_back(r.customers_on[j]);
		}
		
	if (flag)
		out_routes.push_back(desc_r);
		
	output.close();
}

void print_chrome2(vector<Route> &routes, int T) {
	if (T < 320)
		return ;
	ofstream output;
	output.open("./change/k_means_result0.csv", ios::out);
	vector<Route>::iterator r_iter = routes.begin();
	int i = 0;
	while (r_iter != routes.end()) {
		Route r = *r_iter;
		vector<string> coordinate;
		for (int j = 0; j < r.customers_on.size(); j++) {
			coordinate = splitStr(customer[r.customers_on[j]].src_coordinate, ",");
			output << i << "," << coordinate[1] << "," << coordinate[0] << "," << customer[r.customers_on[j]].order_time_format << endl;
		}
		i++;
		r_iter++;
	}
	output.close();
}



void print_chrome1(vector<Route> &routes, int T) {
	ofstream output;
	output.open("./change/k_means_result.txt", ios::app);
	vector<Route>::iterator r_iter = routes.begin();
	while (r_iter != routes.end())
		if (r_iter->T_early - T == 30 || T == 1500) {
			Route r = *r_iter;
			fin_routes.push_back(r);
			for (int j = 0; j < r.customers_on.size(); j++) {
				output << r.customers_on[j] << "->";
				DeleteKeyQueue(&pq, r.customers_on[j]);				//从queue中删除已使用的订单
			}
			output << "......";
			for (int j = 0; j < r.customers_off.size(); j++)
				output << r.customers_off[j] << "->";
			output << endl;
			r_iter = routes.erase(r_iter);//删除元素，返回值指向已删除元素的下一个位置
		}
		else
			++r_iter;
	output.close();
}



void mainMethod() {
	int T = 0;
	while (T < 24 * 60) {
		//将T之前及T接收到的订单根据预约时间（升序）插入到链表队列queue中
		//获取T处理时间窗[T + 60,T + 90]范围内的订单集custs
		/*将获得的订单构造路径库routes
		1）创建一条新路径，将获得订单集中第一个元素c0添加到新路径中，且将其从custs中删除
		2) 计算c0与custs中其它订单之间的距离，并将custs根据距离进行升序排序，将custs中的订单依次插入到新路径中，直到距离超过Dmax(最大距离)	或	车辆超容
		3）将2)中使用的订单从custs中删除，将新路径加入到路径集routes中。
		4) 重复1)-3),直到custs为空
		*/



	}


}

int RouletteWheelSelection(vector<double> a)
{
	//将a中的概率归一化
	double sum = 0;
	for (int i = 0; i < a.size(); i++)
		sum += a[i];
	for (int i = 0; i < a.size(); i++)
		a[i] /= sum;
	double fSlice = rand() % 1000 / (double)1001;

	double cfTotal = 0.0; //概率统计

	int SelectedGenomePos = 0; //被选中的个体下标
	int i;
	for (i = 0; i < a.size(); ++i)
	{

		cfTotal += a[i];
		if (cfTotal > fSlice)
		{
			SelectedGenomePos = i;
			break;
		}
	}

	return SelectedGenomePos;
}

void compute_f(vector<Route> routes, Chromosome &some) {
	double sum_dis = 0, sum_wait = 0, demand = 0;
	for (int i = 0; i < routes.size(); i++) {
		some.routes.push_back(routes[i]);
		//sum_dis += BOARD_WEIGHT * routes[i].f[0] + (1 - BOARD_WEIGHT) * routes[i].dis_off;
		sum_dis += routes[i].f[0];
		//sum_wait += routes[i].f[1];
		demand += routes[i].demand;
	}
	some.f[0] = sum_dis;
	//some.f[1] = sum_wait;
	some.demand = demand;
}

void opt_N1(vector<Route> &routes, int ind) {

	Chromosome pre_chrome, chrome;
	vector<Route> pre_routes = routes;
	compute_f(routes, pre_chrome);

	int m = routes.size() > 3 ? 3 : routes.size();
	//从routes中随机选择m - 1个路径（异于ind）
	set<int> ss;
	double p;
	ss.insert(ind);

	while (ss.size() < m) {							//此处再选择m-1个路径
		p = rand() % 1000 / 1000.0;
		int p_ind = p * routes.size();
		if(find(ss.begin(),ss.end(),p_ind) == ss.end())
			ss.insert(p_ind);
	}

	//将选择的路径中的订单加入到dos中，然后将选择的路径从routes中删除
	vector<int> dos;		
	for (set<int>::reverse_iterator it = ss.rbegin(); it != ss.rend(); it++) {
		dos.insert(dos.end(), routes[*it].customers_on.begin(), routes[*it].customers_on.end());
		routes.erase(routes.begin() + *it);
	}

	//将删除的订单，根据预约时间进行排序
	sort(dos.begin(), dos.end(), cmp);

	while (dos.size() > 0) {
		int first_ind = dos[0];
		vector<int> friends;
		vector<int>::iterator c_iter = dos.begin();
		while (c_iter != dos.end()) {
			//找到first_ind的符合时间和距离约束的朋友
			dis_metric[first_ind][*c_iter] = getEuclideanDis(customer[first_ind], customer[*c_iter]);
			if (dis_metric[first_ind][*c_iter] < MAX_DIS)
				friends.push_back(*c_iter);
			U[*c_iter] = true;								//将使用的订单标记
			++c_iter;
		}
		//对满足的朋友择优生成路径，将first_ind到friends朋友的距离进行升序排序
		if (friends.size() > 1) {
			dis_point = dis_metric[first_ind];
			sort(friends.begin(), friends.end(), cmp1);
		}
		//将friends中的元素生成路径
		double demand = 0.0;
		Route r;
		for (int i = 0; i < friends.size(); i++) {
			if (demand + customer[friends[i]].demand > max_capacity)
				break;
			r.customers_on.push_back(friends[i]);
			//遍历dos，删除dos中包含的friedns[i]

			U[friends[i]] = false;								//将使用的订单标记
			demand += customer[friends[i]].demand;
		}
		if (r.customers_on.size() > 0) {
			r.T_early = customer[r.customers_on[0]].order_time;
			compute_r_dist(r);
			routes.push_back(r);
		}
		//遍历dos，并将其中使用的订单进行删除
		c_iter = dos.begin();
		while (c_iter != dos.end())
			if (!U[*c_iter])
				c_iter = dos.erase(c_iter);
			else
				c_iter++;
	}

	compute_f(routes, chrome);

	//if (chrome.f[0] + chrome.f[1] > pre_chrome.f[0] + pre_chrome.f[1])
	
	double w = 1.0;
	if (pre_chrome.routes.size() != 0)
		w = chrome.routes.size() * 1.0 / pre_chrome.routes.size();
	if (w * chrome.f[0] > pre_chrome.f[0])
		routes = pre_routes;
}

string calRouteCenter(Route r) {
	vector<int> custs = r.customers_on;
	int len = custs.size();
	double lo, la, sum_lo = 0, sum_la = 0;											//对于下车点的顺序	：	求这几个点的中间经纬度	
	vector<string> date_time;
	for (int i = 0; i < len; i++) {
		date_time = splitStr(customer[custs[i]].src_coordinate, ",");
		lo = atof(date_time[0].c_str());
		la = atof(date_time[1].c_str());
		sum_lo += lo;
		sum_la += la;
	}
	sum_lo /= len;
	sum_la /= len;

	return to_string(sum_lo) + "," + to_string(sum_la);

}
//一个方法，输入一个经纬度字符串，和一个路径集，返回路径集索引的升序排序（根据一个地点到路径中心的距离）
vector<int> findNearRoute(string first, vector<Route> routes) {
	dis_center.clear();
	vector<int> ri;
	//计算其它路径中心，到这个路径中心的距离
	for (int i = 0; i < routes.size(); i++) {
		string other = calRouteCenter(routes[i]);
		double tmp = calculateDis(first, other);
		ri.push_back(i);
		dis_center.push_back(tmp);
	}
	sort(ri.begin(), ri.end(), cmp4);
	return ri;
}

void opt_N2(vector<Route> &routes, int ind) {
	Chromosome pre_chrome, chrome;
	compute_f(routes, pre_chrome);

	//选择m条路径
	int m = routes.size() > 3 ? 3 : routes.size();
	string first = calRouteCenter(routes[ind]);						//计算路径中心点
	vector<int> ri = findNearRoute(first, routes);

	//从路径rs中删除距离中心点最远的订单
	vector<int> orders;
	for (int i = 0; i < m; i++) {
		string coordinate = calRouteCenter(routes[ri[i]]);
		//string coordinate = routes[ri[i]].center_coordinates;
		//计算路径中心到各个订单的距离，并找出最大的一个
		double max = DBL_MIN;
		int ind = 0;
		for (int j = 0; j < routes[ri[i]].customers_on.size(); j++) {
			double v = calculateDis(coordinate, customer[routes[ri[i]].customers_on[j]].src_coordinate);
			if (max < v) {
				max = v;
				ind = j;
			}
		}
		//从r中删除订单ind
		orders.push_back(routes[ri[i]].customers_on[ind]);
		routes[ri[i]].customers_on.erase(routes[ri[i]].customers_on.begin() + ind);
		routes[ri[i]].modify = true;
	}


	//将orders中的订单重新插入到routes的路径中
	for (int j = 0; j < orders.size(); j++) {
		double min = DBL_MAX;
		string str = customer[orders[j]].src_coordinate;
		double d = customer[orders[j]].demand;
		int ind = -1;
		for (int i = 0; i < routes.size(); i++) {
			double v = calculateDis(str, calRouteCenter(routes[i]));
			computeCapacity(routes[i]);
			if (d + routes[i].demand <= max_capacity && v < min) {
				min = v;
				ind = i;
			}
		}
		//将订单orders[j]插入到路径ind中
		if (ind != -1)
			routes[ind].customers_on.push_back(orders[j]);
		else {
			//没有合适的插入位置,创建新的路径
			Route r;
			r.customers_on.push_back(orders[j]);
			routes.push_back(r);
		}
	}

	//遍历,并将routes中空路径删除
	vector<Route>::iterator r_iter = routes.begin();
	while (r_iter != routes.end())
		if (r_iter->customers_on.size() == 0)
			r_iter = routes.erase(r_iter);					//删除元素，返回值指向已删除元素的下一个位置
		else {
			if (r_iter->modify)
				compute_r_dist(*r_iter);
			++r_iter;
		}

	compute_f(routes, chrome);

	double w = 1.0;
	if (pre_chrome.routes.size() != 0)
		w = chrome.routes.size() * 1.0 / pre_chrome.routes.size();
	if (w * chrome.f[0] > pre_chrome.f[0])
		routes = pre_chrome.routes;
}

//选择一条路径，将其中订单插入到其他路径中
void opt_N3(vector<Route> &routes, int ii) {
	Chromosome pre_chrome, chrome;
	compute_f(routes, pre_chrome);
	if (pre_chrome.demand * 1.0 / max_capacity >= pre_chrome.routes.size())
		return;

	//从路径ind中删除其中所有的订单
	vector<int> orders;
	for (int i = 0; i < routes[ii].customers_on.size(); i++)
		orders.push_back(routes[ii].customers_on[i]);
	routes.erase(routes.begin() + ii);

	//将orders中的订单重新插入到ri所指的路径中
	for (int j = 0; j < orders.size(); j++) {
		double min = DBL_MAX;
		string str = customer[orders[j]].src_coordinate;
		double d = customer[orders[j]].demand;
		int ind = -1;
		for (int i = 0; i < routes.size(); i++) {
			double v = calculateDis(str, calRouteCenter(routes[i]));
			computeCapacity(routes[i]);
			if (d + routes[i].demand <= max_capacity && v < min) {
				min = v;
				ind = i;
			}
		}
		//将订单orders[j]插入到路径ind中
		if (ind != -1) {
			routes[ind].customers_on.push_back(orders[j]);
			routes[ind].modify = true;
		}
		else {
			//没有合适的插入位置,创建新的路径
			Route r;
			r.customers_on.push_back(orders[j]);
			routes.push_back(r);
		}
	}

	//遍历,并将routes中空路径删除
	vector<Route>::iterator r_iter = routes.begin();
	while (r_iter != routes.end())
		if (r_iter->customers_on.size() == 0)
			r_iter = routes.erase(r_iter);					//删除元素，返回值指向已删除元素的下一个位置
		else {
			if (r_iter->modify)
				compute_r_dist(*r_iter);
			++r_iter;
		}

	compute_f(routes, chrome);

	double w = 1.0;
	if (pre_chrome.routes.size() != 0)
		w = chrome.routes.size() * 1.0 / pre_chrome.routes.size();
	//double w = chrome.routes.size() * 1.0 / pre_chrome.routes.size();
	if (w * chrome.f[0] > pre_chrome.f[0])
		routes = pre_chrome.routes;
}

void localsearch(vector<Route> &routes, int T) {

	//此为领域搜索
	if (routes.size() < 2)
		return;
	vector<double> sps;
	//计算路径集中，各个路径的选择概率
	for (int i = 0; i < routes.size(); i++)
		if (routes[i].sp != 0) {					//此处不解，为什么	!= 0时，才能操作
			routes[i].sp = routes[i].T_early - T < 60 ? 1 : 60.0 / (routes[i].T_early - T);
			sps.push_back(routes[i].sp);
		}

	int ind = 0;
	if (sps.size() > 0)
		ind = RouletteWheelSelection(sps);
	double p = rand() % 1000 / 1000.0;				//生成[0,1]的随机数，这样可以生成精确度到千分位上的[0,1]小数
	if (p < 1.0 / 3)
		opt_N1(routes, ind);
	else if (p < 2.0 / 3)
		opt_N2(routes, ind);
	else
		opt_N3(routes, ind);
}


void main() {
	//读取基本数据
	getData("9_1_data_dynamic02.csv");
	//传统方案的读取
	vector<vector<int>> tra_arranges;									//表示的是原始路径规划方案
	getData1("9_1_result.csv", tra_arranges);

	//将原始数据根据下单时间进行排序，并获得一个时间片内的所有客户点
	int T = 0;															//T从凌晨1点开始记为0
	//int T = startT;
	int preT = T;
	int o = 0;
	fill(dis_export_customers, dis_export_customers + MAX, inf);		//注意这里要是N*N，要不然不是全部 
	fill(dis_metric[0], dis_metric[0] + MAX * MAX, inf);
	fill(U, U + MAX, true);
	pq = CreatedQueue();
	//将customer中的订单记录根据下单时间进行升序排序
	vector<int> custs;
	for (int i = 1; i < cust_num; i++)
		custs.push_back(i);
	sort(custs.begin(), custs.end(), cmp3);								//根据下单时间对订单进行排序
	vector<Route> routes;
	while (T <= 1500) {
	//while (T <= overT) {

		for (; o < custs.size(); )
			if (customer[custs[o]].order_time >= 60 && customer[custs[o]].state == 100) {								//只处理预约时间在1:00以后的
				if (customer[custs[o]].receiving_seconds <= T ) {
					insertNode(pq, custs[o], customer[custs[o]].order_time);
					o++;
				}
				else
					break;
			}
			else
				o++;

		vector<int> dos;
		if (T < 5 * 60) {				//包车订单的处理,可以处理当前时间+30到+60的订单
			dos = FindValuesQueue(&pq, T + 60, T + 61, U);											//不要忘了从queue中删除使用到的			
																									//对包车的订单分配车辆
			for (int i = 0; i < dos.size(); i++) {
				Route r;
				r.customers_on.push_back(dos[i]);
				r.demand = customer[dos[i]].demand;
				r.T_early = customer[dos[i]].order_time;
				r.sp = 0;
				compute_r_dist(r);
				routes.push_back(r);
				U[dos[i]] = false;												//将已使用的订单标记为false
			}
		} 
		else if (T < 1460) {													//处理预约时间起于[6:00,6:30]终于[00:30,1:00]的订单
			double endT = T + 90;
			if (T + 90 >= 1500)
				endT = 1500;
			//dos = FindValuesQueue(&pq, T + 40, T + 90, U);		//因为有些订单下单时间与预约时间相差40min,所以当出现这些订单时，就要处理,没使用
			dos = FindValuesQueue(&pq, T + 40, endT, U);
			int start_pos = 0;
			while (dos.size() > 0) {
				int first_ind = dos[0];
				//找出first_ind符合时间约束的朋友
				vector<int> p;
				endT = customer[first_ind].order_time + 30;
				if (endT >= 1500)
					endT = 1500;
				p = FindValuesQueue(&pq, customer[first_ind].order_time, endT, U);
				vector<int> friends;
				vector<int>::iterator c_iter = p.begin();
				while (c_iter != p.end()) {
					//找到first_ind的符合时间和距离约束的朋友
					dis_metric[first_ind][*c_iter] = getEuclideanDis(customer[first_ind], customer[*c_iter]);
					if (dis_metric[first_ind][*c_iter] < MAX_DIS)
						friends.push_back(*c_iter);
					++c_iter;
				}
				//对满足的朋友择优生成路径，将first_ind到friends朋友的距离进行升序排序
				if (friends.size() > 1) {
					dis_point = dis_metric[first_ind];
					sort(friends.begin(), friends.end(), cmp1);
				}
				//将friends中的元素生成路径
				double demand = 0.0;
				Route r;
				for (int i = 0; i < friends.size(); i++) {
					if (demand + customer[friends[i]].demand > max_capacity)
						break;
					r.customers_on.push_back(friends[i]);
					//遍历dos，删除dos中包含的friedns[i]
					U[friends[i]] = false;								//将使用的订单标记
					demand += customer[friends[i]].demand;
				}

				if (r.customers_on.size() > 0) {
					compute_r_dist(r);
					print_route(r);
					routes.push_back(r);
				}
				//遍历dos，并将其中使用的订单进行删除
				c_iter = dos.begin();
				while (c_iter != dos.end())
					if (!U[*c_iter])
						c_iter = dos.erase(c_iter);
					else
						c_iter++;
			}	//while end
		}

		print_chrome2(routes, T);

		//优化阶段
		if (T > 60 * 5 && T % 5 == 0)
			localsearch(routes, T);
		print_chrome3(routes, T);

		//将最早预约时间为T+30的订单提交，并从rs中删除,从queue中删除已使用的订单
		print_chrome1(routes, T);
		T++;
	}
	print_f(out_routes, "./change/init_solution.csv");
	out_routes.clear();

	//遍历tra_arranges
	vector<Route> temp_rs;
	vector<int> rSets;
	for (int i = 0; i < tra_arranges.size(); i++) {
		Route temp_route;
		for (int j = 0; j < tra_arranges[i].size(); j++)
			temp_route.customers_on.push_back(tra_arranges[i][j]);
		//arrange_customer(temp_route);									//安排上下车的顺序
		compute_r_dist(temp_route);
		temp_rs.push_back(temp_route);
	}

	print_result(temp_rs, "./change/arrange01.csv");
	print_f(out_routes, "./change/arrange01.csv");
	out_routes.clear();
	/*
	rSets.clear();
	for (int i = 0; i < fin_routes.size(); i++)
		for (int j = 0; j < fin_routes[i].customers_on.size(); j++)
			if (startT <= customer[fin_routes[i].customers_on[j]].order_time && customer[fin_routes[i].customers_on[j]].order_time < overT) {
				rSets.push_back(i);
				break;
			}
	temp_rs.clear();
	for (int i = 0; i < rSets.size(); i++)
		temp_rs.push_back(fin_routes[rSets[i]]);*/
	//print_result(temp_rs, "./change/arrange02.csv");
	print_result(fin_routes, "./change/arrange02.csv");
	print_f(out_routes, "./change/arrange02.csv");
	out_routes.clear();
}