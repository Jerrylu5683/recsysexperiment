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
 
 × 现在的问题是要对算法的时间复杂度进一步优化，并通过改变k观察rmse的变化，目前的k取的值是无穷大
 × 我们取k个最相邻的邻居，相邻的度量方式有很多种，比如：
 	（1）采用pearson相关系数进行度量，这个比较稳定，只需要计算一次（本程序采用的方法）
 	（2）采用一个自然的与item set相关的层次（a natural hierarchy associated with the item set)
 	（3）使用本程序计算过程中使用的wij，通过对wij进行排序，从而获得这个值，这个可能要动态更新，不过基本几次计算之后
 	     邻居就比较稳定了。
 ×
 
 版本保存说明：本程序使用的方法是global Neighborhood with implicit feeback的方法，具体的公式见Recommender 
 system handbook中的P171页，5.35。
 本程序中k是变化的，邻居的取法是比较相似度实现的，相似度用计算pearson相关系数的方法事先算好，存储在sim.txt中，
 本程序直接从这个文件中读取相似度
 
 */
#include "movielens.h"
#include "mdefine.cpp"

namespace svd{
	//使用一些全局变量，存储需要估计的参数，bu，bi,wij
    vector<double> bu(USER_NUM+1,0), bi(ITEM_NUM+1,0);       //baseline预测器中的用户偏置和item偏置
    vector<int> buNum(USER_NUM+1,0);	//用户u打分的item总数，
    vector<int> biNum(ITEM_NUM+1,0);   //打过item i分的用户总数
    
    vector< vector<double> > sim(ITEM_NUM+1);//用以存储从文件中读取的相似度
    vector< vector<double> > w(ITEM_NUM+1);  //需要学习的item-item相似矩阵
    vector< vector<double> > c(ITEM_NUM+1);  //item-item 隐性反馈的相似性关系
    vector< vector<int> >  userItems(USER_NUM+1);   //用户打过分的items-----数组
    //vector< vector <int> > rateMatrix(USER_NUM+1);  //打分矩阵,这个矩阵耗费了大量的内存，实际系统中必须优化，如何
                                                    //优化呢？？？？？？---->已经通过了下面的一个map数组解决了
    map<int,int> rateMatrix[USER_NUM+1];           //使用一个map数组存储稀疏的打分矩阵                     
	float mean = 0;                         //全局的平均值
    
    //函数声明
    void RMSEProbe(void);
    void  loadSim(char *fileName);
    void ratingAll(vector< Rating > & data);
    double rating(Rating  record);
    double predictRate(int user,int item);
    
    void model(int dim, double & alpha, double & beta)
    {
        vector<Rating> data;
        cout << "begin initialization: " << endl;
        
        loadRating("../../mldataset/u1.base",data,rateMatrix);  //初始化，将打分数据load 入data中
        loadSim("sim.txt");                                 //将事先计算好的相似度load进入内存
        
        mean = setMeanRating(data); //求平均值，求bu和bi的值
        int i,u,j,k;
        
        
        for(i = 1; i < ITEM_NUM+1; ++i) {
        	for(j = 1; j < ITEM_NUM+1; ++j) {
        		if(i == 554 && j==17500)cout<< i <<'\t'<< j << sim[i][j]<<endl;        		
        	}
    	}
        
        //对bu，bi进行初始化,bu,bi的初始化的方法是求平均值，然后减去mean，
        //在计算的过程中必须要记得所有的值，包括所有的打分总数和分数总和
        for(i = 0; i < data.size(); ++i){  //利用每一个打分调优结果
           	bu[data[i].user] += data[i].rate;
           	++buNum[data[i].user];
        	bi[data[i].item] += data[i].rate;
        	++biNum[data[i].item];
        	
        	userItems[data[i].user].push_back(data[i].item);//初始化userItems数组
        	if(data[i].item > ITEM_NUM )cout<<"*********"<<data[i].item <<"*****"<<endl;
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
                for(i = 0; i < userItems[u].size(); ++i) {  // 循环处理u打分过的每一个item
                	//为预测u对i的打分做一些准备
                	double sumBias = 0;
                	
                	//这里就是取前k个与userItems[u][i]最相似的地方，这个地方应该修改
                	//这里要取出u打分的项目J中与userItems[u][i]最相似的k个项目，采用的方法是什么呢？取出J中每个元素与i的相似度，
                	//然后排序
                	//（1)排序，所用的复杂度是|userItems[u]|lg|userItems[u]|
                	//（2）实时计算，需要为每个u对应的每个i实时计算，其负责度应该为sigma（|userItems[u]|lg|userItems[u]|)
                	//(3) 设定一个阀值，
                	int ruK = 0;
                	for(j=0; j < userItems[u].size(); ++j) {  
                		if(userItems[u][j] > ITEM_NUM )
                		{
                				cout<<"*********"<<data[i].item <<"*****in main cycler!"<<endl;
                				exit(0);
                		}
                		//cout <<userItems[u][i]<<'\t'<<userItems[u][j]<<endl;
                		
                		if((sim[userItems[u][i]][userItems[u][j]]) < -100)
                		{
                			//cout <<userItems[u][i]<<'\t'<<userItems[u][j]<<"	j	"<<sizeof(userItems[u][j])<<'\t'<<sim[userItems[u][i]][userItems[u][j]];
                			//cout <<'\t'<<(sim[userItems[u][i]][userItems[u][j]])<<'\t'<<"end"<<endl;
                			continue;
                		}
                		
                		++ruK;
                		double buj = mean + bu[u] + bi[userItems[u][j]];
                		double ruj = rateMatrix[u][userItems[u][j]];// u 对 j的打分如何快速获得呢？
                		sumBias += (ruj - buj) * w[userItems[u][i]][userItems[u][j]] + c[userItems[u][i]][userItems[u][j]];
                	}
                	if( ruK > 1)pui = mean + bu[u] + bi[userItems[u][i]] + (1.0/sqrt(ruK))*sumBias;//这里先不对k进行变化，先取k=无穷大
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
                	//这里也是取前k个与i最相似的地方
                	
                	for(j=0; j < userItems[u].size(); ++j) {
                		if((sim[userItems[u][i]][userItems[u][j]]) < -100)continue;
                		double buj = mean + bu[u] + bi[userItems[u][j]];
                		w[userItems[u][i]][userItems[u][j]] += alpha * ( (1/sqrt(ruK)) * eui * (rateMatrix[u][userItems[u][j]] - buj) -  beta * w[userItems[u][i]][userItems[u][j]]);
                		c[userItems[u][i]][userItems[u][j]] += alpha * ( (1/sqrt(ruK)) * eui - beta * c[userItems[u][i]][userItems[u][j]]);
                	}	
                } 
            }
            nowRmse =  sqrt(rmse / n);
            if( nowRmse >= preRmse && step > 5) break; //如果rmse已经开始上升了，则跳出循环
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse << endl;
            //alpha *= 0.992;    //逐步减小学习速率
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
    
    
   	void loadSim(char *fileName)
   	{	
   		std::ifstream from (fileName);
   		//@TODO 这里的数组大小应该随着itemnum的大小变化而变化9*ITEM_NUM
	    char rateStr[15138];
	    vector<string> rateDetail(ITEM_NUM+1);
	    int lineNum  = 1;
	    
	    while(from.getline(rateStr,15138)){
	        sim[lineNum].push_back(0.0);
	        explode("	",rateStr,rateDetail);
	        if(rateDetail.size()>=3){
	        	for(int i=1; i<ITEM_NUM+1; ++i) {
	        		sim[lineNum].push_back(0.0);
	        		sim[lineNum][i] = atof(rateDetail[i-1].c_str());
	        		//if(sim[lineNum][i])cout <<lineNum<<'\t'<<i<<'\t'<<sim[lineNum][i]<<endl;
	        	}
	        }
	        ++lineNum;
	    }
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
    	int ruK = 0;
    	for(int j=0; j < userItems[u].size(); ++j) {
    		if((sim[i][userItems[u][j]]) < -100)continue;
    		++ruK;
    		double buj = mean + bu[u] + bi[userItems[u][j]];
    		double ruj = rateMatrix[u][userItems[u][j]];// u 对 j的打分如何快速获得呢？
    		sumBias += (ruj - buj) * w[i][userItems[u][j]] + c[i][userItems[u][j]];
    	}
    	double ret = 0.0;
    	if(ruK>1)
    	{
    		ret = mean + bu[u] + bi[i] + (1.0/sqrt(ruK)) * sumBias;//这里先不对k进行变化，先取k=无穷大
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
