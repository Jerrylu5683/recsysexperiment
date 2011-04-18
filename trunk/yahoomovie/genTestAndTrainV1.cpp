/*
 * Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * Please contact me via email honglianglv@gmail.com
 * my blog: http://lifecrunch.biz
 * my twitter: http://twitter.com/honglianglv
 *
 * It is free software; you can redistribute it and/or modify it under GPLV3.
 *
 * this file split the ratings to test set and train set
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <math.h>
#include <stdlib.h>
#define  SOURCE "./clean-yahoo-user-ratings.txt"
#define  SEPARATOR " "
#define USER_NUM 409399 
#define ITEM_NUM 5628
using namespace std;
void get_rand(set<int> & myset, int low, int high,int length);

struct rateNode
{
	short item;
	short rate;
	short flag;
};


int main()
{
    //(1)load the data in a rateMatrix 
    //(2)random select the test rate for every
    //(3)save the rate in different files.
    ifstream in(SOURCE);
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);           //使用一个vector数组存储稀疏的打分矩阵
    char rateStr[200];
    set<int> myset;
    int itemId = 0, userId = 0, rate = 0;
    char* pch;
    int i = 0;
    //load all the ratings to the rateMatrix
    while(in.getline(rateStr,200)) {
        string strTemp(rateStr);
    	if(strTemp.length() < 3)continue;
    	//cout<<rateStr<<endl;
    	i = 0;
    	pch = strtok (rateStr,SEPARATOR);
        while (pch != NULL) {
            if(0 == i) itemId = atoi(pch);
            else if(1 == i) userId = atoi(pch);
            else if(2 == i) rate = atoi(pch);
            else if(i > 2) break;
            ++i;
            pch = strtok (NULL,SEPARATOR);
        }
    	//cout<<userId<<'\t'<<itemId<<'\t'<<rate<<endl;exit(1);
    	if(0 == itemId || 0 == userId ) {
    		cout<<strTemp<<"####	"<<userId<<"	"<<itemId<<"	"<<rate<<"#####################"<<endl;
    		exit(1);
    	}		
    	try {
    		rateNode tmpNode;
    		tmpNode.item = itemId;
    		tmpNode.rate = (short)rate;
    		tmpNode.flag = 0;
    		rateMatrix[userId].push_back(tmpNode);
    	}
    	catch (bad_alloc& ba)
    	{
    		cerr << "bad_alloc caught: " << ba.what() << endl;
    	}
    }
    in.close();
    cout<<"read file successfully!"<<endl;
    
    //split the ratings
    ofstream training("training.txt");
    ofstream test("test.txt");
    for(int i = 1; i < USER_NUM+1; ++i){
    	int vSize = rateMatrix[i].size();
    	
    	//need to get three random num beteen 0 and (vSize-1)
    	
    	myset.clear();
    	get_rand(myset, 0, vSize-1, 3);
    	set<int>::iterator it;
    	set<int>::iterator end = myset.end(); 
    	for (it=myset.begin(); it!= end; ++it ) {
    		rateMatrix[i][*it].flag = 1;
    	}
		for(int j=0; j < vSize; ++j) {
			if(rateMatrix[i][j].flag == 1) { // in test set
				test << i<<"	"<<rateMatrix[i][j].item<<"	"<<rateMatrix[i][j].rate<<"	"<<0<<endl;
			}
			else {
				training << i<<"	"<<rateMatrix[i][j].item<<"	"<<rateMatrix[i][j].rate<<"	"<<0<<endl;
			}
		}
		cout << "user "<<i<< " deal successfully!"<<endl;
    }
    test.close();
    training.close();
}


/**
 *	
 */
void get_rand(set<int> & myset, int low, int high,int length)
{
	srand((unsigned)time(0));
	int num = 0;
	set<int>::iterator end =  myset.end();
	int good = high-low+1;
	while (num < length) {
		int tmp = rand()%good+low;
		if(myset.find(tmp) == end) {
			myset.insert(tmp);
			++num;
		}
	}
}
