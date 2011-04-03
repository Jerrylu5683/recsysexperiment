/* This file is Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
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
#ifndef NETFLIX_SVD_MDEFINE_CPP_
#define NETFLIX_SVD_MDEFINE_CPP_

float dot(double* p, double* qLocal)
{
    double result = 0.0;
    for (int i=1; i<K_NUM+1; ++i){
        result += p[i]*qLocal[i];
    }
    return result;
}

/**
 *  将fileName中的数据 load 入 array a中
 */
void loadArray(float a[],char* fileName,int length)
{
	char rateStr[256];    
	std::ifstream in(fileName,ifstream::in);
	int i = 1;
	int id,pos1;
	double value;
	while(in.getline(rateStr,256) && i < length) {
		string strTemp(rateStr);
    	if(strTemp.length() < 3)continue;
    	pos1 = strTemp.find("	");
    	id = atoi(strTemp.substr(0,pos1).c_str());
    	value = atof(strTemp.substr(pos1+1).c_str());
		a[id] = value;
		++i;
	}
	in.close();
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
    std::ifstream from (TRAINING_SET);
    if (!from.is_open())
  	{
    	cout << "Output operation failed!\n";
    	exit(1);
  	}
    
    int itemId = 0, userId = 0, rate = 0;
    int pos1=0,pos2=0,pos3=0;
    string strTemp = "";
    int totalM = 0;
    int i = 0;
    while(from.getline(rateStr,256)){
    	string strTemp(rateStr);
		int pos = strTemp.find(":");
	    if(-1 != pos) {
	    	itemId = atoi(strTemp.substr(0,pos).c_str());
	    	if(0 == itemId ) {
	    		cout<<strTemp<<"#####################"<<pos<<"####"<<strTemp.substr(0,pos).c_str()<<endl;
	    		exit(1);
	    	}		
	    	 ++fileNum;	 
	    	if(fileNum %3000 ==0) {
	    		//malloc_stats();  
	    		//mallinfo();
	    		cout<<"read file "<<fileNum<<" sucessfully!"<<endl;
	    	}
	    	continue;
	    }
    	if(strTemp.length() < 3)continue;
    	pos1 = strTemp.find(",");
    	userId = atoi(strTemp.substr(0,pos1).c_str());
    	pos2 = strTemp.find(",",pos1+1);
    	rate = atoi(strTemp.substr(pos1+1,pos2-pos1-1).c_str());
    	//cout<<userId<<'\t'<<itemId<<'\t'<<rate<<endl;exit(1);
    	if(0 == itemId || 0 == userId ||  0 == rate ) {
    		cout<<userId<<"#####################"<<endl;
    		exit(1);
    	}		
    	//初始化rateMatrix
    	try {
    		rateNode tmpNode;
    		tmpNode.user = userId;
    		tmpNode.rate = (short)rate;
    		rateMatrixLocal[itemId].push_back(tmpNode);
    	}
    	catch (bad_alloc& ba)
    	{
    		cerr << "bad_alloc caught: " << ba.what() << endl;
    	}
    }
    from.close();
   	cout<<"read file sucessfully!"<<endl;
    return;
}

/**
 * 计算全局的平均值
 */
double setMeanRating(vector< vector<rateNode> > rateMatrixLocal)
{
    //计算平均值;
    double sum = 0;
    int num = 0;
    cout<<"beging mean:"<<endl;
    for(int i = 1; i < ITEM_NUM+1; ++i){
		for(int j=0; j < rateMatrixLocal[i].size(); ++j) {
			sum += rateMatrixLocal[i][j].rate;
			++num;
		}
    }
    cout<<"end mean!mean:"<<(sum/num)<<endl;
    return sum/num;
}

double get_rand(int dim)
{
    return 0.1 * (rand()%1000/1000.0)/sqrt(dim);
}

/**
 * setRand的值
 */
void setRand(double  p[], int num, float base)
{
    for(int i=1;i<num+1;++i){
    	double temp = base+get_rand(num);
        p[i] = temp;
    }
}

int getRate(vector<rateNode> &v, int user)
{
	int size = v.size();
	for(int i = 0; i < size; ++i) {
		if(user == v[i].user)return v[i].user;
	}
	cout<<"**********************error!!!can't get whate you look for!!***********"<<endl;
	return 0;
}

#endif // NETFLIX_SVD_MDEFINE_CPP_
