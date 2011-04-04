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
/**
	本文件的作用是挑选出一批少量的数据，便于测试算法，等算法都调整好了之后然后再使用大规模数据测试
	主要操作有：
	1、挑选1-ITEM_NUM中间的文件
	2、将itemId和userId进行归一化存在关联数组中
	3、将文件内容修改存在单一的文件中
*/
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <errno.h>
//userNum:438212       itemNUm:1000
//userNum:479025       itemNUm:10000
#define ITEM_NUM 1000  
#define DIR_PATH "../../../nf_dataset/transfer_set/"
using namespace std;
void explode(const char * probe,  const char * data ,vector<string> &result);
int main()
{
	DIR *dp;
	char rateStr[256];
	map<int,int> itemMap;
	map<int,int> userMap;
	struct dirent *dirp;
    vector<string> rateDetail(4);
    
    if((dp  = opendir(DIR_PATH)) == NULL) {
        cout << "Error(" << errno << ") opening " << DIR_PATH << endl;
   		return 0;
    }
    int fileNum = 0;
    int itemCount = 1;
    int userCount = 1;
    long int dirBegin  = telldir(dp);
    while ((dirp = readdir(dp)) != NULL) {
        if(string(dirp->d_name).length() < 3)continue;
        string fileName = DIR_PATH + string(dirp->d_name);
        //cout <<fileName<<endl;
        std::ifstream from (fileName.c_str());
        string strTemp(dirp->d_name);
	    int pos = strTemp.find(".");
	    int itemId = atoi(strTemp.substr(0,pos).c_str());
		if(itemMap.find(itemId) == itemMap.end()) {
			itemMap[itemId] = itemCount;
			++itemCount;
		}
        while(from.getline(rateStr,256)){
        	explode(",",rateStr,rateDetail);
        	int userId = atoi(rateDetail[0].c_str());
        	if(userId > 0) {
        		if(userMap.find(userId) == userMap.end()) {
					userMap[userId] = userCount;
					++userCount;
				}
        	}
	    }
	    from.close();
	    if(fileNum %100 ==0)cout<<"select file "<<fileNum<<" sucessfully!"<<endl;
	    ++fileNum;	 
	    if(fileNum >= ITEM_NUM)break;
    }
    
    //重新计算Id完毕，下面的工作是存储成一个文件
    seekdir(dp,dirBegin); //重置目录指针头
    fileNum = 0;          //fileNum置位
    
    std:ofstream result("smalldata.txt");
    while ((dirp = readdir(dp)) != NULL) {
        if(string(dirp->d_name).length() < 3)continue;
        string fileName = DIR_PATH + string(dirp->d_name);
        //cout <<fileName<<endl;
        std::ifstream from (fileName.c_str());
        string strTemp(dirp->d_name);
	    int pos = strTemp.find(".");
	    int itemId = atoi(strTemp.substr(0,pos).c_str());
	    result<<itemMap[itemId]<<":"<<'\n';
        while(from.getline(rateStr,256)){
        	explode(",",rateStr,rateDetail);
        	int userId = atoi(rateDetail[0].c_str());
        	result<<userMap[userId]<<','<<rateDetail[1]<<','<<rateDetail[2]<<'\n';
        	//cout<<rateStr<<'\n';
	    }
	    from.close();
	    ++fileNum;	 
	    if(fileNum >= ITEM_NUM)break;
	    if(fileNum %100 ==0)cout<<"merge file "<<fileNum<<" sucessfully!"<<endl;
    }
    result.close();
    cout<<"select successful! userNum:"<<userMap.size()<<"	itemNUm:"<<itemMap.size()<<endl;
    return 0;
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
