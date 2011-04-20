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
 * result: test set,74688 ratings; training set,616823 ratings
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define  SOURCE "./clean-yahoo-user-ratings.txt"
#define  SEPARATOR " "
#define USER_NUM 409399 
#define ITEM_NUM 5628
using namespace std;
void get_rand(set<int> & myset, int low, int high,int length);

struct rateNode
{
	short item;
	short story;
	short acting;
	short direction;
	short visual;
	short overall;
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
    int itemId = 0, userId = 0, story = 0, acting = 0, direction = 0, visual = 0, overall = 0;
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
            else if(2 == i) story = atoi(pch);
            else if(3 == i) acting = atoi(pch);
            else if(4 == i) direction = atoi(pch);
            else if(5 == i) visual = atoi(pch);
            else if(6 == i) overall = atoi(pch);
            else if(i > 6) break;
            ++i;
            pch = strtok (NULL,SEPARATOR);
        }
    	//cout<<userId<<'\t'<<itemId<<'\t'<<rate<<endl;exit(1);
    	if(0 == itemId || 0 == userId ) {
    		cout<<strTemp<<"####	"<<userId<<"	"<<itemId<<"	"<<"#####################"<<endl;
    		exit(1);
    	}		
    	try {
    		rateNode tmpNode;
    		tmpNode.item = (short)itemId;
    		tmpNode.story = (short)story;
    		tmpNode.acting = (short)acting;
    		tmpNode.direction = (short)direction;
    		tmpNode.visual = (short)visual;
    		tmpNode.overall = (short)overall;
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
    
    srand((unsigned)time(0));
    //split the ratings
    ofstream training("tmpData/training.txt");
    ofstream test("tmpData/test.txt");
    for(int i = 1; i < USER_NUM+1; ++i){
    	int vSize = rateMatrix[i].size();
    	
    	//need to get three random num beteen 0 and (vSize-1)
    	myset.clear();
    	//cout<<"vsize:"<<vSize<<endl;
    	get_rand(myset, 0, vSize-1, 3);
    	set<int>::iterator it;
    	set<int>::iterator end = myset.end(); 
    	for (it=myset.begin(); it!= end; ++it ) {
    		rateMatrix[i][*it].flag = 1;
    	}
    	//cout<<"set flag finish!"<<endl;
		for(int j=0; j < vSize; ++j) {
			if(rateMatrix[i][j].flag == 1) { // in test set
				test <<i<<" "<<rateMatrix[i][j].item<<" "<<rateMatrix[i][j].story<<" ";
				test <<rateMatrix[i][j].acting<<" "<<rateMatrix[i][j].direction<<" ";
				test <<rateMatrix[i][j].visual<<" "<<rateMatrix[i][j].overall<<endl;
			}
			else {
				training <<i<<" "<<rateMatrix[i][j].item<<" "<<rateMatrix[i][j].story<<" ";
				training <<rateMatrix[i][j].acting<<" "<<rateMatrix[i][j].direction<<" ";
				training <<rateMatrix[i][j].visual<<" "<<rateMatrix[i][j].overall<<endl;
			}
		}
		if( i % 30000 == 0)cout << "user "<<i<< " deal successfully!"<<endl;
    }
    test.close();
    training.close();
}


/**
 *	
 */
void get_rand(set<int> & myset, int low, int high,int length)
{
	if(low +5 >= high) return;
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
