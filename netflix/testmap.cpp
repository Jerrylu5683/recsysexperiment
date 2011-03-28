#include<map>
#include<iostream>
#include<vector>
#include <malloc.h>
#define USER_NUM 480189
#define ITEM_NUM 17770
using namespace std;

int main (void)
{
	double bu[USER_NUM+1] = {0};
    double bi[ITEM_NUM+1] = {0};       //baseline预测器中的用户偏置和item偏置
    int buNum[USER_NUM+1] = {0};	//用户u打分的item总数，
    int biNum[ITEM_NUM+1] = {0};   //打过item i分的用户总数
    
	vector <map<int,int> > tmp(500000); 
	for(int i=0;i< 500000;++i) {
		for(int j=0;j<200;++j) {
			tmp[i][j] = 1;
			cout<< sizeof(tmp[i])<<endl;
			exit(1);
		}
		if(i % 3000 == 0) {
			malloc_stats();  
	    	//mallinfo();
		}
	}
	cout<<"sucessful!"<<endl;
}
