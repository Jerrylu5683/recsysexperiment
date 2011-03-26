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

#define USER_NUM 438212 
#define ITEM_NUM 1000
#define K_NUM  30
#define DIR_PATH "../../nf_dataset/transfer_set/"

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
	int item;
	short rate;
};
