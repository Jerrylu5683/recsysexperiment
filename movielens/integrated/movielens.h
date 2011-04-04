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
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <map>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <ctime>
#include <dirent.h>
#include <malloc.h>
#include <errno.h>

//#define USER_NUM 438212 
//#define ITEM_NUM 1000
#define USER_NUM 943 
#define ITEM_NUM 1682
#define K_NUM  50
#define DIR_PATH "../../nf_dataset/transfer_set/"
#define PROBE_SET "../../ml_data/u1.test"
#define TRAINING_SET "../../ml_data/u1.base"

class Rating
{
    public:
        Rating(short u_item,int u_user,short u_rate);//,int year,int month,int day);
        short item;
        int user;
        short rate;
        //int test;
        //float real;
        //double predict();
        //int yearValue;
        //int monthValue;
        //int dayValue;
        void setPredict(int rating);
        short value();
        //int   year();
        //int   month();
    private:
        ;
};


struct rateNode
{
	short item;
	short rate;
};
