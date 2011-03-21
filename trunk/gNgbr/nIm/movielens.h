#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <map>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#define USER_NUM 943
#define ITEM_NUM 1682
#define DIR_PATH "../../mldataset/"

class Rating
{
    public:
        Rating(int u_item,int u_user,float u_rate);
        int item;
        int user;
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
