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
#include "netflix.h"
#include "mdefine.cpp"

namespace svdtime{    //这里增加了time的影响，我应该改成svd
    vector<float> bu(2750000,-1), bi(18000,-1);
    vector< vector<float> > p(2700000), q(18000);  //所求的偏置矩阵
    
    double rating(Rating  record){
        int user = record.user;
        int item = record.item;
        double ret = record.predict() + bu[user] + bi[item] + dot(p[user], q[item]);
        if(ret < 1) ret = 1;
        if(ret > 5) ret = 5;
        return ret;
    }

	//检查测试集情况
    void RMSEProbe(vector< Rating > & data){
        double ret = 0;
        double n = 0;
        vector<double> rmse(5, 0);
        vector<double> nn(5,0);
        for(int i = 0; i < data.size(); ++i){
            if(data[i].test != 1) continue;  //利用测试集来检查结果
            double rui = data[i].value();
            double pui = rating(data[i]);
            ret += (rui - pui) * (rui - pui);
            n++;

            rmse[(int)(rui) - 1] += (rui - pui) * (rui - pui);
            nn[(int)(rui) - 1]++;
        }
        cout << sqrt(ret / n) << endl;
        for(int k = 0; k < 5;++k){
            cout << k + 1 << "\t" << sqrt(rmse[k] / nn[k]) << endl;
        }
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
        
        /*  检查初始化是否正确
        for(int i=0; i<data.size();i++){
            cout << data[i].user<<"	"<<data[i].item<<"	"<<data[i].rate<<"	"<<endl;
        }
        cout<<data.size()<<endl;
        exit(1);
        */
        //1、首先搞清楚整个过程
        //（1）求平均值
        //2、一步一步实现每个过程
        //3、什么时候弄完，什么时候回去
        
        setMeanRating(data); //求平均值
        map<int, int> ni;    //？
        map<int, map<string, int> > itr, itn;  //一个存储某部电影每月的积分总和，一个存储某部电影当月打分的总数量
        
        for(int i = 0; i < data.size(); ++i){
            int user = data[i].user;
            int item = data[i].item;
            bu[user] = 0;    //svd++，用户偏差
            bi[item] = 0;    //item偏差
            if(data[i].test == 0){    //test应该是将一批数据随机赋值为1，
                if(i % 1000000 == 0) cout << i << endl;  //每一百万个数据打印一下，以前共100M的打分
                ostringstream oss;
                oss << data[i].year() << "-" << data[i].month() << "-" << 1;//data[i].mday();
                itr[item][oss.str()] += data[i].real;  //存储每月的打分总和值
                itn[item][oss.str()] += 1;             //存储每月的数量
                ni[item]++;                            //该电影打分总次数
            }
        }
        
        //out主要记录月份对item打分的总的影响
        ofstream out("netflix-bit.csv");
        out << endl;
        for(map<int, map<string, int> >::iterator i = itr.begin(); i != itr.end(); ++i){
            for(map<string, int>::iterator t = i->second.begin(); t != i->second.end(); ++t){
                out << i->first << "," << t->first << "," ;
                out << (double)(t->second) / (double)(itn[i->first][t->first]) << "," ;
                out << itn[i->first][t->first] << "," << ni[i->first] << endl;
                //itemId,2002-7-1,itemId在2002-7的平均打分，itemId在2002-7的打分总数，该电影打分总次数
            }
        }
        out.close();
        
        //return;  这个地方是原来有的，估计是xlvector测试用的，可以不管
        
        double cc = 1 / (double)(dim);
        
        //初始化bu，就是user的bias（偏置）
        for(int i = 0; i < bu.size(); ++i){
            if(bu[i] < 0) continue;  //这里bu[i]小于0永远不成立，这个语句永远不会执行
            p[i].clear();            //清空所有的p[i]
            setRand(p[i],dim,cc);    //初始话p[i]
        }
        for(int i = 0; i < bi.size(); ++i){
            if(bi[i] < 0) continue;  
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
                if(data[i].value() > 3) continue;
                if(data[i].test > test_level) continue;
                
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
            cout << step << "\t" << sqrt(rmse / n) << "\t";
            RMSEProbe(data);  // 检查训练集情况
            alpha *= 0.92;    //逐步减小学习速率
        }
        ratingAll(data);  //预测所有的结果
        if(test_level == 0){
            ostringstream oss;
            oss << "pb-svd-" << dim << ".txt";
            ratingProbe(oss.str().c_str(), data);
            saveRating("../data/xlvector-svd20.data0", data);
        }
        if(test_level == 1){
            ostringstream oss1;
            oss1 << "qf-svd-" << dim << ".txt";
            ratingQuiz(oss1.str().c_str(), data);
        }
    }
};


int main(int argc, char ** argv)
{
    double alpha = 0.008;
    double beta = 0.01;
    int dim = 100;//atoi(argv[1]);
    test_level = atoi(argv[2]);
    svdtime::model(dim,alpha,beta);  
    return 0;
}
//