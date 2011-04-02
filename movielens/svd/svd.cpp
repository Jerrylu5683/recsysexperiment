/* This file is Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * Please contact me via email honglianglv@gmail.com
 * my blog: http://lifecrunch.biz
 * my twitter: http://twitter.com/honglianglv
 *
 * It is free software; you can redistribute it and/or modify it under 
 * the GNU General Public License as published by the Free Software
 * Foundation; either version 1, or (at your option) any later version.
 *
 * 本程序的目的是实现koren在SIGKDD'08论文中的方法，svd model
 */
#include "movielens.h"
#include "mdefine.cpp"

namespace svd{
	//使用一些全局变量，存储需要估计的参数，bu，bi,p,q
    double bu[USER_NUM+1] = {0};
    double bi[ITEM_NUM+1] = {0};       //baseline预测器中的用户偏置和item偏置
    
    int buNum[USER_NUM+1] = {0};	//用户u打分的item总数，
    int biNum[ITEM_NUM+1] = {0};   //打过item i分的用户总数
    
    double p[USER_NUM+1][K_NUM+1] = {0};   //用于存储用户的属性描述p
    double q[ITEM_NUM+1][K_NUM+1] = {0};   //用于item的属性描述q
    
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);           //使用一个vector数组存储稀疏的打分矩阵                     
	float mean = 0;                         //全局的平均值
    
    //函数声明
    double RMSEProbe(void);
    
    float predictRate(int user,int item);
    
    void model(int dim, float  alpha, float  beta)
    {
        cout << "begin initialization: " << endl;
        
        loadRating(DIR_PATH,rateMatrix);  //初始化完成

        mean = setMeanRating(rateMatrix); //求平均值，求bu和bi的值
        
        int i,u,j,k;
        
       
        /*
        //对bu，bi进行初始化,bu,bi的初始化的方法是求平均值，然后减去mean，
        //在计算的过程中必须要记得所有的值，包括所有的打分总数和分数总和
        int tmpIndex = 0;
	    for(i = 1; i < USER_NUM+1; ++i){
	    	int vSize = rateMatrix[i].size();
			for(j=0; j < vSize; ++j) {
				bi[rateMatrix[i][j].item] += (rateMatrix[i][j].rate - mean);
				biNum[rateMatrix[i][j].item] += 1;
			}			
	    }
        
       
        //ofstream logbi("bi.txt");
        for(i = 1; i < ITEM_NUM+1; ++i) {
        	if(biNum[i] >=1)bi[i] = bi[i]/(biNum[i]+25);
        	else bi[i] = 0.0;
        	
        }
       
         for(i = 1; i < USER_NUM+1; ++i){
	    	int vSize = rateMatrix[i].size();
			for(j=0; j < vSize; ++j) {
				bu[i] += (rateMatrix[i][j].rate - mean - bi[rateMatrix[i][j].item]);
				buNum[i] += 1;
			}			
	    }        
        
         //以下过程求平均值
        for(i = 1; i < USER_NUM+1; ++i) {
        	if(buNum[i]>=1)bu[i] = bu[i]/(buNum[i]+10);
        	else bu[i] = 0.0;
        	//logbi<<i<<"	"<<buNum[i]<<"	"<<bu[i]<<endl;
        }
        //logbi.close();
        */

        //@todo 不知道是否能针对初始化的过程做一些优化
        //对w进行初始化，初始化的方法是随机函数，不知道这种方法是否好，是否会影响结果？？？？？？？
        for(int i = 1; i < ITEM_NUM+1; ++i){
           setRand(q[i],K_NUM,0);    //初始化q[i]
        }
        
        for(int i = 1; i < USER_NUM+1; ++i){
           setRand(p[i],K_NUM,0);    //初始化p[i]
        }
       
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        float pui = 0.0 ; // 预测的u对i的打分
        double preRmse = 100000000.0;
        double preProbRmse = RMSEProbe();//用于记录上一个rmse，作为终止条件的一种，如果rmse上升了，则停止u 
        double nowRmse = 0.0;
        double nowProbRmse = 0.0;
        
        cout <<"begin testRMSEProbe(): " << endl;
        
        //main loop
        for(int step = 0; step < 290; ++step){  //只迭代60次
            long double rmse = 0.0;
            int n = 0;
            for( u = 1; u < USER_NUM+1; ++u) {   //循环处理每一个用户 
            	
               	int RuNum = rateMatrix[u].size(); //用户u打过分的item数目
               	float sqrtRuNum = 0.0;
               	if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
               	
                //迭代处理
                for(i=0; i < RuNum; ++i) {// 循环处理u打分过的每一个item
                	int itemI = rateMatrix[u][i].item;
                	short rui = rateMatrix[u][i].rate; //实际的打分
                	pui = predictRate(u,itemI);
                	
                	float eui = rui - pui;
                	
                	if( isnan(eui) ) {// fabs(eui) >= 4.2 || 
                		cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<"	"<<bu[u]<<"	"<<bi[itemI]<<"	"<<mean<<endl;
                		//printArray(q[itemI],p[u],K_NUM+1);
                		exit(1);
                	}
                	rmse += eui * eui; ++n;
                	if(n % 10000000 == 0)cout<<"step:"<<step<<"	n:"<<n<<" dealed!"<<endl;
                	
                	//bu[u] += alpha * (eui - beta * bu[u]);
                	//bi[itemI] += alpha * (eui - beta * bi[itemI]);
                	
                	for( k=1; k< K_NUM+1; ++k) {
	               		double tempPu = p[u][k];
	               		p[u][k] += alpha * (eui*q[itemI][k] - beta*p[u][k]);
	               		q[itemI][k] += alpha * (eui*tempPu - beta*q[itemI][k]);
	               	}
                } 
            }
            nowRmse =  sqrt( rmse / n);
            
            cout << step << "\t" << nowRmse <<'\t'<< preRmse<<" 	n:"<<n<<endl;
            
            nowProbRmse = RMSEProbe(); // 检查训练集情况
            if( nowRmse >= preRmse && step >= 3) break; //如果rmse已经开始上升了，则跳出循环
            else
            	preRmse = nowRmse;
            
            if(alpha > 0.01)alpha *= 0.9;    //逐步减小学习速率
            else if( alpha > 0.003) alpha *= 0.999;
            //RMSEProbe(); 
        }
        RMSEProbe();  // 检查训练集情况
        return;    
    }
    
    //根据svd公式预测打分值
    float predictRate(int user,int item)
    {
    	int RuNum = rateMatrix[user].size(); //用户u打过分的item数目
       	float sqrtRuNum = 0.0;
       	double ret; 
    	if(RuNum > 1)
    	{
    		ret = mean + bu[user] + bi[item] +  dot(p[user],q[item]);//这里先不对k进行变化，先取k=无穷大
    	}
    	else ret  = mean+bu[user] + bi[item];
    	if(ret < 1.0) ret = 1;
        if(ret > 5.0) ret = 5;
        //cout<<ret<<"    "<<mean<<"    "<<bu[user]<<"     "<<bi[item]<<endl;
        return ret;
    }
    
    //检查测试集情况
    double RMSEProbe(void){
    	
        /*	1、load test数据集，
        	2、针对每一条数据，计算预测值，然后计算误差的平方和，计算总数量
        	3、输出rmse
        
        */
         //首先读取probe_t.txt 文件，对于每一个rate，算出预测的打分，根据对应的实际打分计算RMSE
		ifstream in(PROBE_SET);
		char rateStr[256];
		int pos1,pos2,pos3;
		string strTemp;
		int rateValue,itemId,userId,probeNum=0;
		float pRate,err;
		long double rmse = 0;
		
		while(in.getline(rateStr,256)){
	    	strTemp = rateStr;
	    	if(strTemp.length() < 4) continue;
		    pos1 = strTemp.find("	");
		    pos2 = strTemp.find("	",pos1+1);
		    pos3 = strTemp.find("	",pos2+1);
	    	userId = atoi(strTemp.substr(0,pos1).c_str());
	    	itemId = atoi(strTemp.substr(pos1+1,pos2-pos1-1).c_str());
	    	rateValue = atoi(strTemp.substr(pos2+1,pos3-pos2-1).c_str());
	    	
	    	pRate = predictRate(userId,itemId);
	    	err = pRate-rateValue;
	    	
	    	rmse += err*err;++probeNum;
	    }
	    rmse = sqrt( rmse / probeNum);
	    cout<<"RMSE:"<<rmse<<" probeNum:"<<probeNum<<endl;
		in.close(); //load userId map完毕
        return rmse;
    }
};

int main(int argc, char ** argv)
{
	time_t start,end;
    struct tm * timeStartInfo;
    struct tm * timeEndInfo;
    double duration; 
	start = time(NULL);
    float alpha = 0.01;  //0.0045according to the paper of "a guide to SVD for CF"
    float beta = 0.05;   //0.015 according to the paper of "a guide to SVD for CF"
    					 //0.0005 according the experiment
    
    //for(int i=0; i < 10; i++)
    {
    	//alpha = i*0.0005 + 0.0025;
    	//cout << alpha << endl;
    	svd::model(K_NUM,alpha,beta);	
    }
    end = time(NULL);
    duration = (end-start);
    timeStartInfo = localtime(&start);
    timeEndInfo = localtime(&end);
    cout << "Start at "<<asctime(timeStartInfo)<<", and end at "<< asctime(timeEndInfo)<<"!"<<endl;
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
