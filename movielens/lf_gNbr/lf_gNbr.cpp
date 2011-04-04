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
 * 本程序的目的是实现koren在TKDD'09论文中的方法，learnFactorizedNeighborhoodModel的方法
 */
#include "movielens.h"
#include "mdefine.cpp"

namespace svd{
	//使用一些全局变量，存储需要估计的参数，bu，bi,wij
    double bu[USER_NUM+1] = {0};
    double bi[ITEM_NUM+1] = {0};       //baseline预测器中的用户偏置和item偏置
    float buBase[USER_NUM+1] = {0};
    float biBase[ITEM_NUM+1] = {0};       //baseline预测器中的用户偏置和item偏置
    int buNum[USER_NUM+1] = {0};	//用户u打分的item总数，
    int biNum[ITEM_NUM+1] = {0};   //打过item i分的用户总数
    
    float pu[USER_NUM+1][K_NUM+1] = {0};   //用于存储中间变量puk
    float q[ITEM_NUM+1][K_NUM+1]; //Item向量
    float x[ITEM_NUM+1][K_NUM+1]; //
    float y[ITEM_NUM+1][K_NUM+1]; //
    void getProbeReal();          //填充probe的实际打分
    
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);           //使用一个map数组存储稀疏的打分矩阵                     
	float mean = 0;                         //全局的平均值
    
    //函数声明
    void RMSEProbe(void);
    
    float predictRate(int user,int item);
    
    void model(int dim, float  alpha, float  beta)
    {
        cout << "begin initialization: " << endl;
        ofstream log("log.txt");
        ofstream logbi("log_bi.txt");
        
        loadRating(DIR_PATH,rateMatrix);  //初始化完成
        //getProbeReal();
        //exit(0);
        
        mean = setMeanRating(rateMatrix); //求平均值，求bu和bi的值
        
        loadArray(buBase,"bu.txt",USER_NUM+1);
        loadArray(buBase,"bu.txt",ITEM_NUM+1);
        
        int i,u,j,k;
        
        //对bu，bi进行初始化,bu,bi的初始化的方法是求平均值，然后减去mean，
        //在计算的过程中必须要记得所有的值，包括所有的打分总数和分数总和
        int tmpIndex = 0;
	    for(i = 1; i < USER_NUM+1; ++i){
	    	int vSize = rateMatrix[i].size();
			for(j=0; j < vSize; ++j) {
				bu[i] += rateMatrix[i][j].rate;
				buNum[i] += 1;
				bi[rateMatrix[i][j].item] += rateMatrix[i][j].rate;
				biNum[rateMatrix[i][j].item] += 1;
        		
			}			
	    }
        
        //以下过程求平均值
        for(i = 1; i < USER_NUM+1; ++i) {
        	if(buNum[i]>=1)bu[i] = (bu[i]/buNum[i]) - mean;
        	else bu[i] = 0.0;
        	//log<<i<<'\t'<<bu[i]<<endl;
        	//buBase[i] = bu[i];
        }
        
        for(i = 1; i < ITEM_NUM+1; ++i) {
        	if(biNum[i] >=1)bi[i] = (bi[i]/biNum[i]) - mean;
        	else bi[i] = 0.0;
        	//biBase[i] = bi[i];
        	//logbi<<i<<'\t'<<bi[i]<<endl;
        }
        //@todo 不知道是否能针对初始化的过程做一些优化
        //对w进行初始化，初始化的方法是随机函数，不知道这种方法是否好，是否会影响结果？？？？？？？
        for(int i = 1; i < ITEM_NUM+1; ++i){
            setRand(q[i],K_NUM+1,0);    //初始化q[i]
            setRand(x[i],K_NUM+1,0);    //初始化x[i]
            setRand(y[i],K_NUM+1,0);    //初始化y[i]
        }
       
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        float pui = 0.0 ; // 预测的u对i的打分
        double preRmse = 1000000000000.0; //用于记录上一个rmse，作为终止条件的一种，如果rmse上升了，则停止
        double nowRmse = 0.0;
        cout <<"begin compute first pu: " << endl;
        for( u = 1; u < USER_NUM+1; ++u) {   //循环处理每一个用户 
           	int RuNum = rateMatrix[u].size(); //用户u打过分的item数目
           	float sqrtRuNum = 0.0;
           	if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
           	
           	//求出pu
           	for( k=1; k< K_NUM+1; ++k) {
           		float sumx = 0.0;
           		float sumy = 0.0;
           		
           		for(j=0; j < RuNum; ++j) {
           			int itemI = rateMatrix[u][j].item;
           			short rui =  rateMatrix[u][j].rate;
           			float bui = mean + bu[u] + bi[itemI];
           			sumx += (rui-mean-buBase[u]-biBase[itemI]) * x[itemI][k];
           			sumy += y[itemI][k];
           		}
           		pu[u][k] = sqrtRuNum*(sumx + sumy);
           	}
        }
        cout <<"end compute first pu! " << endl;
        cout <<"begin testRMSEProbe(): " << endl;
        RMSEProbe();
        for(int step = 0; step < 60; ++step){  //只迭代35次
            long double rmse = 0.0;
            int n = 0;
            for( u = 1; u < USER_NUM+1; ++u) {   //循环处理每一个用户 
        		
               	int RuNum = rateMatrix[u].size(); //用户u打过分的item数目
               	float sqrtRuNum = 0.0;
               	if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
               	
               	//求出pu
               	for( k=1; k< K_NUM+1; ++k) {
               		float sumx = 0.0;
               		float sumy = 0.0;
               		
               		for(j=0; j < RuNum; ++j) {
               			int itemI = rateMatrix[u][j].item;
               			short rui =  rateMatrix[u][j].rate;
               			float bui = mean + bu[u] + bi[itemI];
               			sumx += (rui-mean-buBase[u]-biBase[itemI]) * x[itemI][k];
               			sumy += y[itemI][k];
               		}
               		pu[u][k] = sqrtRuNum*(sumx + sumy);
               		/*
               		if(pu[k]>10) {
               			//cout<<pu[k]<<endl;
               			//cout<<pu[k]<<endl;
               		}
               		*/
               	}
               	float sum[K_NUM+1] = {0};   //用于存储中间变量sum
               	
                //迭代处理
                for(i=0; i < RuNum; ++i) {// 循环处理u打分过的每一个item
                	int itemI = rateMatrix[u][i].item;
                	short rui = rateMatrix[u][i].rate; //实际的打分
                	double bui = mean + bu[u] + bi[itemI];
                	pui = predictRate(u,itemI);
                	//cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<endl;
                	
                	float eui = rui - pui;
                	if( isnan(eui) ) {// fabs(eui) >= 4.2 || 
                		cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<"	"<<bu[u]<<"	"<<bi[itemI]<<"	"<<mean<<endl;
                		printArray(q[itemI],pu[u],K_NUM+1);
                		exit(1);
                	}
                	
                	rmse += eui * eui; ++n;
                	if(n % 10000000 == 0)cout<<"step:"<<step<<"	n:"<<n<<" dealed!"<<endl;
                	
                	bu[u] += alpha * (eui - beta * bu[u]);
                	bi[itemI] += alpha * (eui - beta * bi[itemI]);
                	
                	for( k=1; k< K_NUM+1; ++k) {
	               		sum[k] = sum[k]+ eui*q[itemI][k];
	               		q[itemI][k] += alpha * (eui*pu[u][k] - beta*q[itemI][k]);
	               	}
                }
                
                for(i=0; i < RuNum; ++i) {// 循环处理u打分过的每一个item
	                int itemI = rateMatrix[u][i].item;
                	short rui = rateMatrix[u][i].rate; //实际的打分
	               	for( k=1; k< K_NUM+1; ++k) {
	               		
						x[itemI][k] += alpha * (sqrtRuNum*sum[k]*(rui-mean-buBase[u]-biBase[itemI]) - beta*x[itemI][k]);
	               		y[itemI][k] += alpha * (sqrtRuNum*sum[k] - beta*y[itemI][k]);
						/*
						x[itemI][k] += alpha * (eui*sqrtRuNum*q[itemI][k]*(rui-bui) - beta*x[itemI][k]);
	               		y[itemI][k] += alpha * (eui*sqrtRuNum*q[itemI][k] - beta*y[itemI][k]);
						*/
	               	}
                } 
            }
            nowRmse =  sqrt( rmse / n);
            
            if( nowRmse >= preRmse && step >= 3) break; //如果rmse已经开始上升了，则跳出循环
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse <<'\t'<< preRmse<<" 	n:"<<n<<endl;
            RMSEProbe();  // 检查训练集情况
            
            //alpha *= 0.992;    //逐步减小学习速率
            //RMSEProbe(); 
        }
        //ratingAll(data);  //预测所有的结果
        log.close();
        logbi.close();
        RMSEProbe();  // 检查训练集情况
        return;
	    
    }
    
    //根据svd++公式预测打分值
    float rating(Rating  record){
        return  0.0;
    } 
    
    float predictRate(int user,int item)
    {
    	int RuNum = rateMatrix[user].size(); //用户u打过分的item数目
       	float sqrtRuNum = 0.0;
       	float ret; 
    	if(RuNum > 1)
    	{
    		ret = mean + bu[user] + bi[item] +  dot(pu[user],q[item]);//这里先不对k进行变化，先取k=无穷大
    	}
    	else ret  = mean+bu[user] + bi[item];
    	if(ret < 1.0) ret = 1;
        if(ret > 5.0) ret = 5;
        return ret;
    }
    
    //检查测试集情况
    void RMSEProbe(void){
    	
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
    }
	
    void ratingAll(vector< Rating*> & data){
        /*
        for(int i = 0; i < data.size(); ++i){
            data[i].setPredict(rating(data[i]));
        }
        */
    }
    
    void getProbeReal()
	{
		//首先读取probe_t.txt 文件，对于每一个rate，找出对应的实际打分
		ifstream in("probe_t.txt");
		ofstream out("probe_real.txt");
		ofstream logNP("n_p.txt");
		char rateStr[256];
		int pos1,pos2;
		string strTemp;
		int rateValue,itemId,userId;
		//load userIdMap
		while(in.getline(rateStr,256)){
	    	strTemp = rateStr;
		    pos1 = strTemp.find(",");
	    	itemId = atoi(strTemp.substr(0,pos1).c_str());
	    	userId = atoi(strTemp.substr(pos1+1).c_str());
	    	//cout<<itemId<<'\t'<<userId<<'\t'<<endl;	    	exit(0);
	    	rateValue = getRate(rateMatrix[userId],itemId);
	    	if(rateValue == 0) {
	    		logNP<<itemId<<','<<userId<<','<<rateValue<<endl;
	    	}
	    	out<<itemId<<','<<userId<<','<<rateValue<<endl;
	    }
		in.close(); //load userId map完毕
		out.close();
		logNP.close();
	}
};

int main(int argc, char ** argv)
{
	float start,end,duration; 
	start = clock();
    float alpha = 0.005;  //经测试这个值比较好
    float beta = 0.002;   //经过测试这个也还行
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
