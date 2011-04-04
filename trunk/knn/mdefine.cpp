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
Rating::Rating(int u_item,int u_user,float u_rate)
{
    item = u_item;
    user = u_user;
    rate = u_rate;
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

/**
 * load filePath中的数据到data这个vector中和 rateMatrix中
 */
void loadRating(char * filePath, vector<Rating> &data, map<int,int> rateMatrixLocal[])
{
    std::ifstream from (filePath);
    char rateStr[256];
    vector<string> rateDetail(4);
   
    while(from.getline(rateStr,256)){
        explode("	",rateStr,rateDetail);
        if(rateDetail.size()>=3){
        	Rating aRate(atoi(rateDetail[1].c_str()),atoi(rateDetail[0].c_str()),atof(rateDetail[2].c_str()));
			aRate.test = 0; //这里所有的数据都是训练数据，测试数据另外存储        	
        	data.push_back(aRate);
        	
        	//初始话rateMatrix
        	rateMatrixLocal[atoi(rateDetail[1].c_str())][atoi(rateDetail[0].c_str())] = atoi(rateDetail[2].c_str());
        }
    }
    from.close();
    return;
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
        if(i>=3)break;
        pos1=pos2+1;
    }
    result[3] = (dataStr.substr(pos1));
}

double get_rand()
{
    return (rand()%1000-500)/5000.0;
}

/**
 * setRand的值
 */
float setRand(vector<double> &p, int num, float base)
{
	srand((unsigned)time(0));
    for(int i=0;i<num;++i){
    	double temp = base+get_rand();
        p.push_back(temp);
        //p.push_back(base+0.05); //全部初始化为base，看看影响如何
        //cout << i <<"	"<< temp <<"	"<< endl;
    }
}

/**
 * 计算全局的平均值
 */
int setMeanRating(vector<Rating> &data)
{
    //计算平均值;
    float sum = 0;
    int num = 0;
    int tmp = 0;
    for(int i = 0; i < data.size(); ++i){
    	tmp = data[i].value();
    	if(data[i].value() > 0){
    		sum += data[i].value();
    		++num;
    	}  			
    }
    return sum/num;
}


void ratingProbe(const char * filePath, vector<Rating> &data)
{

}

void saveRating(const char * filePath, vector<Rating> &data)
{
}

void ratingQuiz(const char * filePath, vector<Rating> &data)
{
}
