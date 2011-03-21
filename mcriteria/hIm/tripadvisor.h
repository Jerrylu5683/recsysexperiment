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

#define USER_NUM 14009
#define ITEM_NUM 1850
#define CRI_NUM 8
#define DIR_PATH "../../tripDataset_t/"

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
