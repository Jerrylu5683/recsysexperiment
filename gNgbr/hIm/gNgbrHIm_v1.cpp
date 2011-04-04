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
 * 本程序的目的是测试 global Neighborhood 方法的准确性，使用的公式是Recommender system handbook中的P171页，5.35的
 *  公式，不去掉Cij（cij表示的就是隐形反馈），加上隐性反馈看看效果
 */
#include "movielens.h"
#include "mdefine.cpp"

namespace svd{
	//使用一些全局变量，存储需要估计的参数，bu，bi,wij
    vector<double> bu(USER_NUM+1,0), bi(ITEM_NUM+1,0);       //baseline预测器中的用户偏置和item偏置
    vector<int> buNum(USER_NUM+1,0);	//用户u打分的item总数，
    vector<int> biNum(ITEM_NUM+1,0);   //打过item i分的用户总数
    vector< vector<double> > w(ITEM_NUM+1);  //item-item相似矩阵
    vector< vector<double> > c(ITEM_NUM+1);  //item-item 隐性反馈的相似性关系
    vector< vector<int> >  userItems(USER_NUM+1);   //用户打过分的items-----数组
    vector< vector <int> > rateMatrix(USER_NUM+1);  //打分矩阵,这个矩阵耗费了大量的内存，实际系统中必须优化，如何
                                                    //优化呢？？？？？？
	float mean = 0;                         //全局的平均值
    
    //函数声明
    void RMSEProbe(void);
    void ratingAll(vector< Rating > & data);
    double rating(Rating  record);
    double predictRate(int user,int item);
    
    void model(int dim, double & alpha, double & beta)
    {
        vector<Rating> data;
        cout << "begin initialization: " << endl;
        
        //对rateMatix进行初始化，在初始化之前，必须先对rateMatrix赋值，将所有的打分赋值为0
        for(int i = 1; i<USER_NUM+1;++i) {
        	for(int j = 0; j<ITEM_NUM+1; ++j) {
        		rateMatrix[i].push_back(0);
        	}
        }
        
        loadRating("../../mldataset/u1.base",data,rateMatrix);  //初始化，将打分数据load 入data中
        
        mean = setMeanRating(data); //求平均值，求bu和bi的值
        int i,u,j,k;
        
        //对bu，bi进行初始化,bu,bi的初始化的方法是求平均值，然后减去mean，
        //在计算的过程中必须要记得所有的值，包括所有的打分总数和分数总和
        for(i = 0; i < data.size(); ++i){  //利用每一个打分调优结果
           	bu[data[i].user] += data[i].rate;
           	++buNum[data[i].user];
        	bi[data[i].item] += data[i].rate;
        	++biNum[data[i].item];
        	
        	userItems[data[i].user].push_back(data[i].item);//初始化userItems数组
        } //以上过程相加求和（sigma）
        
        //以下过程求平均值
        for(i = 1; i < USER_NUM+1; ++i) {
        	if(buNum[i]>1)bu[i] = bu[i]/buNum[i] - mean;
        	else bu[i] = 0.05;
        }
        
        for(i = 1; i < ITEM_NUM+1; ++i) {
        	if(biNum[i] > 1)bi[i] = bi[i]/biNum[i] - mean;
        	else bi[i] = 0.05;
        }
        
        //@todo 不知道是否能针对初始化的过程做一些优化
        //对w进行初始化，初始化的方法是随机函数，不知道这种方法是否好，是否会影响结果？？？？？？？
        for(int i = 1; i < ITEM_NUM+1; ++i){
            //cout<< bu[i] << endl;
            w[i].clear();            //清空所有的w[i]
            setRand(w[i],ITEM_NUM+1,0);    //初始化w[i]
        }
        
        //@todo 不知道是否能针对初始化的过程做一些优化
        //对c进行初始化，初始化的方法是随机函数，不知道这种方法是否好，是否会影响结果？？？？？？？
        for(int i = 1; i < ITEM_NUM+1; ++i){
            //cout<< bu[i] << endl;
            c[i].clear();            //清空所有的c[i]
            setRand(c[i],ITEM_NUM+1,0);    //初始化c[i]
        }
       
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        double pui = 0.0 ; // 预测的u对i的打分
        double preRmse = 1000000000000.0; //用于记录上一个rmse，作为终止条件的一种，如果rmse上升了，则停止
        double nowRmse = 0.0;
        for(int step = 0; step < 35; ++step){  //只迭代25次
            double rmse = 0;
            double n = 0;
            
            for( u = 1; u < USER_NUM+1; ++u) {   //循环处理每一个用户 
                for(i = 0; i < userItems[u].size(); ++i) {
                	//为预测u对i的打分做一些准备
                	double sumBias = 0;
                	for(j=0; j < userItems[u].size(); ++j) {
                		double buj = mean + bu[u] + bi[userItems[u][j]];
                		double ruj = rateMatrix[u][userItems[u][j]];// u 对 j的打分如何快速获得呢？
                		sumBias += (ruj - buj) * w[userItems[u][i]][userItems[u][j]] + c[userItems[u][i]][userItems[u][j]];
                	}
                	if( userItems[u].size() > 1)pui = mean + bu[u] + bi[userItems[u][i]] + (1.0/sqrt(userItems[u].size()))*sumBias;//这里先不对k进行变化，先取k=无穷大
                	else pui = mean;
                	if(pui < 1) pui = 1;
                	if(pui > 5) pui = 5;
                	//cout <<pui<<"	"<< rateMatrix[u][userItems[u][i]]<<endl;
                	double eui = rateMatrix[u][userItems[u][i]] - pui;
                	rmse += eui * eui; ++n;
                	
                	//更新bu，bi
                	bu[u] += alpha * (eui - beta * bu[u]);
                	bi[userItems[u][i]] += alpha * (eui - beta * bi[userItems[u][i]]);
                	
                	//更新wij
                	for(j=0; j < userItems[u].size(); ++j) {
                		double buj = mean + bu[u] + bi[userItems[u][j]];
                		w[userItems[u][i]][userItems[u][j]] += alpha * ( (1/sqrt(userItems[u].size())) * eui * (rateMatrix[u][userItems[u][j]] - buj) -  beta * w[userItems[u][i]][userItems[u][j]]);
                		c[userItems[u][i]][userItems[u][j]] += alpha * ( (1/sqrt(userItems[u].size())) * eui - beta * c[userItems[u][i]][userItems[u][j]]);
                	}	
                } 
            }
            nowRmse =  sqrt(rmse / n);
            if( nowRmse >= preRmse && step > 5) break; //如果rmse已经开始上升了，则跳出循环
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse << endl;
            alpha *= 0.992;    //逐步减小学习速率
            RMSEProbe(); 
        }
        //ratingAll(data);  //预测所有的结果
        
        RMSEProbe();  // 检查训练集情况
        return;
        
        //输出bu的值
        ofstream outputbu("bu.txt");
	    for(int i=1; i < USER_NUM+1; i++)
	    {
	    	outputbu << i << "\t" << bu[i] << endl;
	    }
	    outputbu.close();
	    
	    //输出bi的值
        ofstream outputbi("bi.txt");
	    for(int i=1; i < ITEM_NUM+1; i++)
	    {
	    	outputbi << i << "\t"<< bi[i] << endl;
	    }
	    outputbu.close();
	    
    }
    
    //根据svd++公式预测打分值
    double rating(Rating  record){
        /*
        int user = record.user;
        int item = record.item;
        double ret = mean + bu[user] + bi[item] + dot(p[user], q[item]);
        if(ret < 1) ret = 1;
        if(ret > 5) ret = 5;
        return ret;
        */
        return  0.0;
    } 
    
    double predictRate(int user,int item)
    {
    	int u = user;
    	int i =  item;
    	double sumBias = 0.0;
    	for(int j=0; j < userItems[u].size(); ++j) {
    		double buj = mean + bu[u] + bi[userItems[u][j]];
    		double ruj = rateMatrix[u][userItems[u][j]];// u 对 j的打分如何快速获得呢？
    		sumBias += (ruj - buj) * w[i][userItems[u][j]] + c[i][userItems[u][j]];
    	}
    	double ret = 0.0;
    	if(userItems[u].size()>1)
    	{
    		ret = mean + bu[u] + bi[i] + (1.0/sqrt(userItems[u].size())) * sumBias;//这里先不对k进行变化，先取k=无穷大
    	}
    	else ret  = mean;
    	if(ret < 1.0) ret = 1;
        if(ret > 5.0) ret = 5;
        //cout <<u<<"	"<<i<<"	"<<mean <<"	"<<bu[u]<<"	"<<bi[i]<<"	"<<userItems[u].size()<<"	"<<sumBias<<"	"<<ret<<endl;
        return ret;
    }
    
    //检查测试集情况
    void RMSEProbe(void){
    	
        /*	1、load test数据集，
        	2、针对每一条数据，计算预测值，然后计算误差的平方和，计算总数量
        	3、输出rmse
        */
        std::ifstream from("../../mldataset/u1.test");
        char rateStr[100];
        vector<string> rateDetail(4);
        double predict = 0.0;
        double rmseSum = 0.0;
        int num = 0;
        int realValue = 0;
        while(from.getline(rateStr,100)){
        	explode("	",rateStr,rateDetail);
        	if(rateDetail.size()>=3){
        		predict = predictRate(atoi(rateDetail[0].c_str()), atoi(rateDetail[1].c_str()));
        		realValue = atoi(rateDetail[2].c_str());
        		rmseSum =  rmseSum + pow(predict-realValue,2);
        		//cout << predict <<"	"<<realValue<<"	"<<rmseSum<<endl;
        		++num;
        	}
        }
        cout << "test set rmse:"<<sqrt(rmseSum/num) <<"	"<< rmseSum <<"	"<< num<< endl;
    }

    void ratingAll(vector< Rating > & data){
        for(int i = 0; i < data.size(); ++i){
            data[i].setPredict(rating(data[i]));
        }
    }
};

int main(int argc, char ** argv)
{
	double start,end,duration; 
	start = clock();
    double alpha = 0.013;  //经测试这个值比较好
    double beta = 0.001;   //经过测试这个也还行
    int dim = 100;//atoi(argv[1]);
    test_level = 1;//atoi(argv[2]);
    ofstream outputfile("parameter.txt");
    
    //for(int i=0; i < 10; i++)
    {
    //	beta = i*0.001 + 0.002;
    //	cout << beta << endl;
    	svd::model(dim,alpha,beta);
    	
    }
    outputfile.close();
    end = clock();
    duration = (end-start)/CLOCKS_PER_SEC;
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
