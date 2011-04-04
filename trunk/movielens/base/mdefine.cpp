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
void qsort(vector<float>& array, int start, int end);
int  partition(vector<float>& array, int start, int end);
bool cmp(float a, float b);
Rating::Rating(short u_item,int u_user,short u_rate)//float u_rate,int year=1970,int month=1,int day=1)
{
    item = u_item;
    user = u_user;
    rate = u_rate;
}


void Rating::setPredict(int rating)
{
    rate = rating;
}

short Rating::value()
{
    return rate;
}

float dot(float* p, float* qLocal)
{
    double result = 0.0;
    for (int i=1; i<K_NUM+1; ++i){
        result += p[i]*qLocal[i];
    }
    return result;
}

void dump(map<int,short>& tmp,int fileNum)
{
	ofstream ret("/var/dump.txt");
	map<int,short>::iterator mapEnd = tmp.end();
	for(map<int,short>::iterator it = tmp.begin(); it != mapEnd; it++) {
		ret<<'['<<it->first<<']'<<" = "<< it->second<<endl;
		if(it->first == fileNum) {
			cout<<'['<<it->first<<']'<<" = "<< it->second<<endl;
		} 
	}
	ret.close();
}


/**
 * load filePath中的数据到data这个vector中和 rateMatrix中
 * 
 */
void loadRating(char * dirPath, vector< vector<rateNode> >& rateMatrixLocal)
{
    char rateStr[256];    
    vector<string> rateDetail(10);
    int fileNum = 0;
    //std::ifstream from ("smalldata_1000.txt");
    std::ifstream from (TRAINING_SET);
    if (!from.is_open())
  	{
    	cout << "Output operation failed!\n";
    	exit(1);
  	}
    
    std::ofstream to("readlog.txt");
    int itemId = 0, userId = 0, rate = 0;
    int pos1=0,pos2=0,pos3=0;
    string strTemp = "";
    int totalM = 0;
    int i = 0;
    while(from.getline(rateStr,256)){
    	string strTemp(rateStr);
    	if(strTemp.length() < 3)continue;
    	pos1 = strTemp.find("	");
    	userId = atoi(strTemp.substr(0,pos1).c_str());
    	pos2 = strTemp.find("	",pos1+1);
    	itemId = atoi(strTemp.substr(pos1+1,pos2-pos1-1).c_str());
    	pos3 = strTemp.find("	",pos2+1);
    	rate = atoi(strTemp.substr(pos2+1,pos3-pos2-1).c_str());
    	//cout<<userId<<'\t'<<itemId<<'\t'<<rate<<endl;exit(1);
    	if(0 == itemId || 0 == userId ||  0 == rate ) {
    		cout<<userId<<"#####################"<<endl;
    		exit(1);
    	}		
    	//初始化rateMatrix
    	try {
    		rateNode tmpNode;
    		tmpNode.item = itemId;
    		tmpNode.rate = (short)rate;
    		rateMatrixLocal[userId].push_back(tmpNode);
    		//cout<<sizeof(rateMatrixLocal[userId])<<endl;
    		//rateMatrixLocal[userId].insert(itemId,(short)atoi(str2.c_str()));
    		//to<<userId<<'\t'<< itemId<<'\t'<<rateMatrixLocal[userId].size()<<endl;
    	}
    	catch (bad_alloc& ba)
    	{
    		cerr << "bad_alloc caught: " << ba.what() << endl;
    	}
    }
    from.close();
    to.close();
   	cout<<"read file sucessfully!"<<endl;
    return;
}

/*
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
*/

/**
 * 计算全局的平均值
 */
float setMeanRating(vector< vector<rateNode> > rateMatrixLocal)
{
    //计算平均值;
    double sum = 0;
    int num = 0;
    cout<<"beging mean:"<<endl;
    for(int i = 1; i < USER_NUM+1; ++i){
		for(int j=0; j < rateMatrixLocal[i].size(); ++j) {
			sum += rateMatrixLocal[i][j].rate;
			++num;
		}
    }
    cout<<"end mean!mean:"<<(sum/num)<<endl;
    return sum/num;
}

float get_rand()
{
    return (rand()%1000-500)/50000.0;
}

/**
 * setRand的值
 */
float setRand(float  p[], int num, float base)
{
	srand((unsigned)time(0));
    for(int i=1;i<num;++i){
    	float temp = base+get_rand();
        p[i] = temp;
        //p.push_back(base+0.05); //全部初始化为base，看看影响如何
        //cout << i <<"	"<< temp <<"	"<< endl;
    }
}




void ratingProbe(const char * filePath, vector<Rating*> &data)
{

}

void saveRating(const char * filePath, vector<Rating*> &data)
{
}

void ratingQuiz(const char * filePath, vector<Rating*> &data)
{
}


//下面的这个函数用来利用最小堆找出第k大的元素
float getKmax(vector<float>& array, int K)
{
	vector<float> heapTmp;
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

bool cmp(float a, float b)
{
	return a > b;
}

//下面的几个函数用来实现快速排序
void qsort(vector<float>& array, int start, int end)
{
    if(start >= end)return;
    //cout<<"begin partition:"<<endl;
    int q = partition(array,start,end);
    //cout<< q<<endl;
    qsort(array,start,q-1);
    qsort(array,q+1,end);
}

int partition(vector<float>& array, int start,int end)
{
    if(start >= end) return start;
    if(start < 0) start = 0;
    int original  = start;
    float tmp = array[start];
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

void swap(float& a, float& b)
{
    float tmp = a;
    a = b;
    b = tmp;
}


int getRate(vector<rateNode> &v, int item)
{
	int size = v.size();
	for(int i = 0; i < size; ++i) {
		if(item == v[i].item)return v[i].rate;
	}
	cout<<"**********************error!!!can't get whate you look for!!***********"<<endl;
	return 0;
}

void printArray(float a[],float b[],int length)
{
	for(int i=1;i<length;++i) {
		cout<<a[i]<<'\t'<<b[i]<<endl;
	}
}
