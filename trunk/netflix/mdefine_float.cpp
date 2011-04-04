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
void explode(const char * probe,  const char * data ,vector<string> &result);
void qsort(vector<double>& array, int start, int end);
int  partition(vector<double>& array, int start, int end);
bool cmp(double a, double b);
Rating::Rating(short u_item,int u_user,short u_rate)//float u_rate,int year=1970,int month=1,int day=1)
{
    item = u_item;
    user = u_user;
    rate = u_rate;
    //yearValue = year;
    //monthValue = month;
    //dayValue = day;
}


void Rating::setPredict(int rating)
{
    rate = rating;
}

short Rating::value()
{
    return rate;
}
/*
double Rating::predict()
{
    return 0.0;
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
*/

float dot(vector<double> &p, vector<double> &q)
{
	if(p.size() != q.size())return 0.0;
    float result = 0.0;
    for (int i=1; i<p.size(); ++i){
        result += p[i]*q[i];
    }
    return result;
}

/**
 * load filePath中的数据到data这个vector中和 rateMatrix中
 * 
 */
void loadRating(char * dirPath, vector<Rating> &data, map<int,int> rateMatrixLocal[USER_NUM+1])
{
    char rateStr[256];
    DIR *dp;
 	struct dirent *dirp;
    vector<string> rateDetail(10);
    vector<string> dayDetail(4);
    if((dp  = opendir(dirPath)) == NULL) {
        cout << "Error(" << errno << ") opening " << dirPath << endl;
   		return;
    } 
    int fileNum = 0;
    while ((dirp = readdir(dp)) != NULL) {
        if(string(dirp->d_name).length() < 3)continue;
        string fileName = dirPath + string(dirp->d_name);
        //cout <<fileName<<endl;
        std::ifstream from (fileName.c_str());
        string strTemp(dirp->d_name);
	    int pos = strTemp.find(".");
	    int itemId = atoi(strTemp.substr(0,pos).c_str());
	    //cout<< itemId<<'\t'<<strTemp<<"   end"<<endl;
	      
        while(from.getline(rateStr,256)){
	    	explode(",",rateStr,rateDetail);
	        if(rateDetail.size()>=3){
	        	int userId = atoi(rateDetail[0].c_str());
	        	Rating aRate(itemId,userId,atoi(rateDetail[1].c_str()));
	        	data.push_back(aRate);
	        	//初始化rateMatrix
	        	//rateMatrixLocal[userId][itemId] = atoi(rateDetail[1].c_str());
	        }	        
	    }
	    from.close();
	    ++fileNum;	 
	    if(fileNum %100 ==0)cout<<"read file "<<fileNum<<" sucessfully!"<<endl;
    }
    //cout<<fileNum<<endl;
    //exit(1);
    return;
}

void explode(const char * probe,  const char * data ,vector<string> &result)
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
double setMeanRating(vector<Rating> &data)
{
    //计算平均值;
    double sum = 0;
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




void ratingProbe(const char * filePath, vector<Rating> &data)
{

}

void saveRating(const char * filePath, vector<Rating> &data)
{
}

void ratingQuiz(const char * filePath, vector<Rating> &data)
{
}


//下面的这个函数用来利用最小堆找出第k大的元素
double getKmax(vector<double>& array, int K)
{
	vector<double> heapTmp;
	for(int i=0; i < array.size(); ++i)
	{
		heapTmp.push_back(array[i]);
		if(i == K-1) {
			make_heap(heapTmp.begin(),heapTmp.end(),cmp);
		}
		else if(i >=K) {
			push_heap(heapTmp.begin(),heapTmp.end(),cmp);
			pop_heap(heapTmp.begin(),heapTmp.end(),cmp);
			heapTmp.pop_back();
		}
		//cout << i<<'\t'<<heapTmp.size()<<endl;
	}
	return heapTmp.front();
}

bool cmp(double a, double b)
{
	return a > b;
}

//下面的几个函数用来实现快速排序
void qsort(vector<double>& array, int start, int end)
{
    if(start >= end)return;
    //cout<<"begin partition:"<<endl;
    int q = partition(array,start,end);
    //cout<< q<<endl;
    qsort(array,start,q-1);
    qsort(array,q+1,end);
}

int partition(vector<double>& array, int start,int end)
{
    if(start >= end) return start;
    if(start < 0) start = 0;
    int original  = start;
    double tmp = array[start];
    ++start;
    while(1) {
        while( start<=end && array[start] >= tmp)++start;
        while( start<=end && array[end] < tmp)--end;
        if(start < end) {
        	swap(array[start],array[end]);
        }
        else break;
    }
    swap(array[start-1],array[original]);
    return start-1;
}

void swap(double& a, double& b)
{
    double tmp = a;
    a = b;
    b = tmp;
}
