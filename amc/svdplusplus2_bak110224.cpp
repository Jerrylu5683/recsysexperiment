/**
 *	本程序试验的方法是直接学习item i 和j 的相似度，
 *  
 */
#include "netflix.h"
#include "mdefineplus.cpp"

namespace svd{
    vector<float> bu(USER_NUM+1,0), bi(ITEM_NUM+1,0);    //用户偏置和item偏置
    vector<int> un(USER_NUM+1,0), in(ITEM_NUM+1,0);    //用户偏置和item偏置  //分别记录用户u打分的item数量和item q被打分的次数
    
    vector< vector<double> >  w(ITEM_NUM+1);  //所求相似矩阵
    vector< vector<int> >  userItems(USER_NUM+1);   //用户打过分的items-----数组
    vector< vector <int> > rateMatrix(USER_NUM+1);  //打分矩阵
	float mean = 0;
    
    //根据svd++公式预测打分值
    double rating(int user,int item, vector<Rating> &data)
    {  
        //double pui = rating(data[i]); //预测打分，向量点乘
        //预测打分，预测方法为（1）找出u打分所有的item，然后利用w(ij)计算结果
        double sumPui = 0;
        double sumW = 0;
        
        for(int j=0; j< userItems[user].size();++j) {
        	//cout <<user<<'\t'<<item<<'\t'<<rateMatrix[user][userItems[user][j]]<<'\t'<<w[item][userItems[user][j]]<<endl;
        	sumPui += w[item][userItems[user][j]] * rateMatrix[user][userItems[user][j]]; //weight 矩阵算是正确的
        	sumW += w[item][userItems[user][j]];
        }
        
        double ret = sumPui/sumW;
           
        
    	if(ret < 1) ret = 1;
    	if(ret > 5) ret = 5;
    	
    	//cout<<sumPui<<'\t'<<sumW<<'\t'<<endl;
    	return ret;
    }

	//检查测试集情况，导入测试集，利用训练集得到的结果进行计算。
    void RMSEProbe(vector<Rating> &data){
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
        		predict = rating(atoi(rateDetail[0].c_str()), atoi(rateDetail[1].c_str()), data);
        		realValue = atoi(rateDetail[2].c_str());
        		rmse += (predict-realValue) * (predict-realValue);
        		++num;
        		//cout <<predict<<'\t'<< realValue<<endl;
        	}
        }
        cout << "test set rmse:"<<sqrt(rmse/num) << endl;     
    }

	/*
    void ratingAll(vector< Rating > & data){
        for(int i = 0; i < data.size(); ++i){
            data[i].setPredict(rating(data[i]));
        }
    }
    */
     
    void model(int dim, double & alpha, double & beta)
    {
        vector<Rating> data;
       
        
        //在初始化之前，必须先对rateMatrix赋值，将所有的打分赋值为0
        for(int i = 1; i<USER_NUM+1;++i) {
        	for(int j = 0; j<ITEM_NUM+1; ++j) {
        		rateMatrix[i].push_back(0);
        	}
        }
        
        loadRating("u1.base",data,rateMatrix);  //初始化，加载数据，将打分数据存储在datavector中
        
        
       
        
       
        //1、首先搞清楚整个过程
        //（1）求平均值
        //2、一步一步实现每个过程
        //3、什么时候弄完，什么时候回去
        
        /*
        本次好像暂时不需要平均值，等改进的时候再添加 2011-2-22 by lvhl
        mean = setMeanRating(data); //求平均值，求bu和bi的值
        */
        
        //中间删除一段代码，是xlvector计算平均打分写关于时间对打分的影响的论文用的
        
        //初始化相似矩阵w(ij)，在0附近random
        for(int i = 1; i < ITEM_NUM+1; ++i){
            w[i].clear();
            setRand(w[i],ITEM_NUM+1,0);
            //if(rateMatrix[1][i])cout << rateMatrix[1][i]<<endl;
        }
        //初始化userItem矩阵，将每个user打过分的item矩阵建立
        for(int ii = 0; ii < data.size(); ++ii){ 
        	userItems[data[ii].user].push_back(data[ii].item);
        }
        
        //迭代更新内容
        cout << "begin : " << endl;
        for(int step = 0; step < 25; ++step){  //只迭代25次
        	
        	//每次迭代的步骤：（1）获取每个现有打分的err（2）更新每个wij
            double rmse = 0;
            double n = 0;
            for(int ii = 0; ii < data.size(); ++ii){  //利用每一个打分调优结果
                int i = ii;  //用于记录是用第几个打分记录来优化
                
                int user = data[i].user;      //userId
                int item = data[i].item;      //itemId
                double rui = data[i].value(); //rate
                //double pui = rating(data[i]); //预测打分，向量点乘
                //预测打分，预测方法为（1）找出u打分所有的item，然后利用w(ij)计算结果
                double sumPui = 0;
                double sumW = 0;
                for(int j=0; j< userItems[user].size();++j) {
                	//cout <<rateMatrix[user][item]<<'\t'<<w[item][userItems[user][j]]<<endl;return;
                	sumPui += w[item][userItems[user][j]] * rateMatrix[user][userItems[user][j]]; //weight 矩阵算是正确的
                	sumW += w[item][userItems[user][j]];
                }
                
                double pui = sumPui/sumW;
                double err = rui - pui;
                //cout <<sumPui<<'\t'<<sumW<< '\t' <<rui<<'\t'<<pui<<'\t'<<err << endl;
                rmse += err * err; ++n;
                data[i].error = err;
                data[i].pRate = pui;
                data[i].sumWij = sumW;
            }
            cout << step << "\t" << sqrt(rmse / n) << endl;
            
            //更新wij耗费的时间实在太长，能不能减少一点呢，因为每次迭代时间复杂度为ITEM_NUM*USER_NUM*KnowRateNum
            //1682X943X80000 = 1269,6552,0000 每一次迭代需要计算1269亿次，计算机的计算速度2亿次，则需要634.5秒，实际上可以用乘法来测试一下，估计每秒计算乘法的速度只有2000万次，至少6345秒，即2个小时。
            //应该重新考虑
            
            for(int i=1; i< ITEM_NUM+1; ++i) {
            	for(int j=1; j< ITEM_NUM+1; ++j) {
            		//针对每个wij，都需要利用所有的打分
            		double sumTemp = 0;
            		for(int ii = 0; ii < data.size(); ++ii){  //利用每一个打分调优结果
		                sumTemp += (data[ii].error*rateMatrix[data[ii].user][j])/data[ii].sumWij;
		            }
                	//每次按照公式更新wij
                	w[i][j] += alpha * ( sumTemp - beta * w[i][j]);
                	//cout << w[i][j]<< endl;
                }
            }
            alpha *= 0.92;    //逐步减小学习速率
        }
   		//为什么预测结果和实际差别这么小，这个是什么原因？？过拟合了？到底是什么原因呢？很可能是过拟合了，拿测试集数据试试就知道了
        RMSEProbe(data);  // 检查训练集情况
        return ;
        /*
        ratingAll(data);  //预测所有的结果
        
        
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
	    */
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