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
#include <cstring>
#include <vector>
#include <math.h>
#include <map>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <ctime>

#define USER_NUM 4652 
#define ITEM_NUM 4244
#define CRI_NUM 5
#define DIR_PATH "../../tripDataset_t/"
#define RATING_FILE_NAME "../../yahooMovie/selectRatings.maped"

class Rating
{
    public:
        Rating(int u_item,int u_user,int u_criteria,float u_rate);
        int item;
        int user;
        int criteria;
        float rate;
        int test;
        float real;
        double predict();
        int yearValue;
        int monthValue;
        void setPredict(float rating);
        float value();
        int   year();
        int   month();
    private:
        ;
};
