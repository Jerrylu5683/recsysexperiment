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
这里应该解释一下svd和svd++的区别，应该解释这个情况，svd++考虑到了隐形反馈，p(u)不是一个简单的项量，
不仅仅包括了向量，还包括了评分item数量的一些反馈。
*/
#include "netflix.h"
#include "mdefineplus.cpp"

namespace svd{
    vector<float> bu(USER_NUM+1,0), bi(ITEM_NUM+1,0);    //用户偏置和item偏置
    vector<int> un(USER_NUM+1,0), in(ITEM_NUM+1,0);    //用户偏置和item偏置  //分别记录用户u打分的item数量和item q被打分的次数
    vector< vector<float> > p(USER_NUM+1), q(ITEM_NUM+1);  //所求的偏置矩阵
	float mean = 0;
    
    //根据svd++公式预测打分值
    double rating(Rating  record){
        int user = record.user;
        int item = record.item;
        double ret = mean + bu[user] + bi[item] + dot(p[user], q[item]);
        if(ret < 1) ret = 1;
        if(ret > 5) ret = 5;
        return ret;
    }
    
    double rating(int user,int item)
    {
    	double ret = mean + bu[user] + bi[item] + dot(p[user], q[item]);
    	if(ret < 1) ret = 1;
    	if(ret > 5) ret = 5;
    	return ret;
    }
    
    int setMeanRating(vector<Rating> &data)
	{
	    //计算平均值;
	    float sum = 0;
	    int num = 0;
	    int tmp = 0;
	    for(int i = 0; i < data.size(); ++i){
	    	tmp = data[i].value();
	    	if(data[i].value() > 0){
	    		sum += data[i].value();
	    		++num;
	    		
	    		++un[data[i].user];
	    		++in[data[i].item];
	    		bu[data[i].user] += data[i].rate;
	    		bi[data[i].item] += data[i].rate;
	    	}		
	    }
	    float totalMean = sum/num;
	    for(int i = 1; i < USER_NUM+1; ++i){
	    	cout<< bu[i]<<"\t"<<un[i]<<"\t";
	    	bu[i] = (bu[i]/un[i]) - totalMean;
	    	cout << bu[i] << endl;
	    }
	    for(int i = 1; i < ITEM_NUM+1; ++i){
	    	cout<< bi[i]<<"\t"<<in[i]<<"\t";
	    	bi[i] = (bi[i]/in[i]) - totalMean;
	    	cout << bi[i] << endl;
	    }
	    
	    return totalMean;
	}

	//检查测试集情况
    void RMSEProbe(void){
    	
        /*	1、load test数据集，
        	2、针对每一条数据，计算预测值，然后计算误差的平方和，计算总数量
        	3、输出rmse
        */
        std::ifstream from("u1.test");
        char rateStr[100];
        vector<string> rateDetail(4);
        int predict = 0;
        double rmse = 0.0;
        int num = 0;
        int realValue = 0;
        while(from.getline(rateStr,100)){
        	explode("	",rateStr,rateDetail);
        	if(rateDetail.size()>=3){
        		predict = rating(atoi(rateDetail[0].c_str()), atoi(rateDetail[1].c_str()));
        		realValue = atoi(rateDetail[2].c_str());
        		rmse += (predict-realValue) * (predict-realValue);
        		++num;
        	}
        }
        cout << "test set rmse:"<<sqrt(rmse/num) << endl;
    }

    void ratingAll(vector< Rating > & data){
        for(int i = 0; i < data.size(); ++i){
            data[i].setPredict(rating(data[i]));
        }
    }
     
    void model(int dim, double & alpha, double & beta)
    {
        vector<Rating> data;
        loadRating("u1.base",data);  //初始化
        
        //1、首先搞清楚整个过程
        //（1）求平均值
        //2、一步一步实现每个过程
        //3、什么时候弄完，什么时候回去
        
        mean = setMeanRating(data); //求平均值，求bu和bi的值
        
        //中间删除一段代码，是xlvector计算平均打分写关于时间对打分的影响的论文用的
        
        double cc = 1 / (double)(dim);
        //cout << cc << endl;exit(1);
        
        //初始化bu，就是user的bias（偏置）
        for(int i = 1; i < USER_NUM+1; ++i){
            //cout<< bu[i] << endl;
            //if(bu[i] < 0) continue;  //这里bu[i]小于0永远不成立，这个语句永远不会执行
            p[i].clear();            //清空所有的p[i]
            setRand(p[i],dim,cc);    //初始化p[i]，不会因为初始化就成为svd++吧？结果不是这个样子的
        }
        cout << bi.size() << endl;
        for(int i = 1; i < ITEM_NUM+1; ++i){
            //if(bi[i] < 0) continue;  
            //cout << i << endl;
            q[i].clear();
            setRand(q[i],dim,cc);
        }
        cout << "begin : " << endl;
        for(int step = 0; step < 25; ++step){  //只迭代25次
            double rmse = 0;
            double n = 0;
            for(int ii = 0; ii < data.size(); ++ii){  //利用每一个打分调优结果
                int i = ii;
                
                //下面两句不知道做什么用？
                //if(data[i].value() > 3) continue;
                //if(data[i].test > test_level) continue;
                
                int user = data[i].user;
                int item = data[i].item;
                double rui = data[i].value();
                double pui = rating(data[i]); //预测打分，向量点乘
                double err = rui - pui;
                rmse += err * err; ++n;
                //更新数据
                bu[user] += alpha * (err - beta * bu[user]);
                bi[item] += alpha * (err - beta * bi[item]);
                for(int k = 0; k < dim; ++k){
                    p[user][k] += alpha * (err * q[item][k] - beta * p[user][k]);
                    q[item][k] += alpha * (err * p[user][k] - beta * q[item][k]);
                }
            }
            cout << step << "\t" << sqrt(rmse / n) << endl;
            alpha *= 0.92;    //逐步减小学习速率
        }
        ratingAll(data);  //预测所有的结果
        RMSEProbe();  // 检查训练集情况
        
        //输出bu的值
        ofstream outputbu("svdbu.txt");
	    for(int i=1; i < USER_NUM+1; i++)
	    {
	    	outputbu << i << "\t" << bu[i] << endl;
	    }
	    outputbu.close();
	    
	    //输出bi的值
        ofstream outputbi("svdbi.txt");
	    for(int i=1; i < ITEM_NUM+1; i++)
	    {
	    	outputbi << i << "\t"<< bi[i] << endl;
	    }
	    outputbu.close();
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
    svd::model(dim,alpha,beta);
    outputfile.close();
    end = clock();
    duration = (end-start)/CLOCKS_PER_SEC;
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}