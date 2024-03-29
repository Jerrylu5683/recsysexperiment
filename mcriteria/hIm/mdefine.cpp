/*
* This file is Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
* please maitain the copyright information completely when you redistribute the code.
* 
* Please contact me via email honglianglv@gmail.com
* my blog: http://lifecrunch.biz
* my twitter: http://twitter.com/honglianglv
*
* It is free software; you can redistribute it and/or modify it under 
* the GNU General Public License as published by the Free Software
* Foundation; either version 1, or (at your option) any later version.
*/
float test_level = 0.004;
using namespace std;
void explode(const char * probe, char * data,vector<string> &result);
Rating::Rating(int u_item, int u_user, int u_criteria, float u_rate)
{
    item = u_item;
    user = u_user;
    rate = u_rate;
    criteria = u_criteria;
}

double Rating::predict()
{
    return 0.0;
}

void Rating::setPredict(float rating)
{
    rate = rating;
}

float Rating::value()
{
    return rate;
}
int Rating::year()
{
    return yearValue;
}

int Rating::month()
{
    return monthValue;
    int good =10;
}

float dot(vector<float> &p, vector<float> &q)
{
    float result = 0.0;
    for (int i=0; i<p.size(); ++i){
        result += p[i]*q[i];
    }
    return result;
}

void explode(const char * probe,  char * data ,vector<string> &result)
{
    string dataStr(data);
    int pos1 = 0;
    int pos2 = 0;
    int i=0;
    while((pos2=dataStr.find(probe,pos1)) != string::npos){
        result[i] = (dataStr.substr(pos1,pos2-pos1));
        ++i;
        //if(i>=3)break;
        pos1=pos2+1;
    }
    result[i] = (dataStr.substr(pos1));
}

/**
 * 计算全局的平均值
 */
double setMeanRating(map<int,int> rateMatrixLocal[USER_NUM+1][CRI_NUM+1])
{
    //计算平均值;
    long double sum = 0;
    int num = 0;
    int tmp = 0;
    for(int i = 1; i < USER_NUM+1; ++i) {
		for(int j = 1; j < CRI_NUM+1; ++j) {
			//iteration the map
			map<int,int>::iterator it;
			map<int,int>::iterator end = rateMatrixLocal[i][j].end();
			for ( it=rateMatrixLocal[i][j].begin() ; it != end; ++it) {
				sum +=  it->second;
				++num;
			}
    	}  			
    }
    return sum/num;
}

double get_rand()
{
    return (rand()%1000-500)/5000.0;
}

float setRand(double  p[], int num, float base)
{
	srand((unsigned)time(0));
    for(int i=1;i<num;++i){
    	float temp = base+get_rand();
        p[i] = temp;
    }
}
