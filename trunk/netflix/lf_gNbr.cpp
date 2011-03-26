/**
 * 本程序的目的是实现koren在TKDD'09论文中的方法，learnFactorizedNeighborhoodModel的方法
 */
#include "netflix.h"
#include "mdefine.cpp"

namespace svd{
	//使用一些全局变量，存储需要估计的参数，bu，bi,wij
    double bu[USER_NUM+1] = {0};
    double bi[ITEM_NUM+1] = {0};       //baseline预测器中的用户偏置和item偏置
    int buNum[USER_NUM+1] = {0};	//用户u打分的item总数，
    int biNum[ITEM_NUM+1] = {0};   //打过item i分的用户总数
    
    float q[ITEM_NUM+1][K_NUM+1]; //Item向量
    float x[ITEM_NUM+1][K_NUM+1]; //
    float y[ITEM_NUM+1][K_NUM+1]; //
    
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
        
        
        mean = setMeanRating(rateMatrix); //求平均值，求bu和bi的值
        
        
        float pu[K_NUM+1] = {0};   //用于存储中间变量puk
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
        }
        
        for(i = 1; i < ITEM_NUM+1; ++i) {
        	if(biNum[i] >=1)bi[i] = (bi[i]/biNum[i]) - mean;
        	else bi[i] = 0.0;
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
        
       
        for(int step = 0; step < 35; ++step){  //只迭代35次
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
               			sumx += (rui-bui) * x[itemI][k];
               			sumy += y[itemI][k];
               		}
               		pu[k] = sqrtRuNum*(sumx + sumy);
               	}
                //迭代处理
                for(i=0; i < RuNum; ++i) {// 循环处理u打分过的每一个item
                	int itemI = rateMatrix[u][i].item;
                	short rui = rateMatrix[u][i].rate; //实际的打分
                	double bui = mean + bu[u] + bi[itemI];
                	if(RuNum >1) {
                		pui = bui + dot(q[itemI],pu);
                	}
                	else pui = bui;
                		
                	if(pui < 1) pui = 1;
                	if(pui > 5) pui = 5;
                	
                	//cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<endl;
                	
                	float eui = rui - pui;
                	if( fabs(eui) >= 3 && step > 1) {
                		cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<"	"<<bu[u]<<"	"<<bi[itemI]<<"	"<<mean<<endl;
                	}
                	rmse += eui * eui; ++n;
                	if(n % 1000000 == 0)cout<<"step:"<<step<<"	n:"<<n<<" dealed!"<<endl;
                	bu[u] += alpha * (eui - beta * bu[u]);
                	bi[itemI] += alpha * (eui - beta * bi[itemI]);
                	
	               	for( k=1; k< K_NUM+1; ++k) {
	               		q[itemI][k] += alpha * (eui*pu[k] - beta*q[itemI][k]);
	               		x[itemI][k] += alpha * (eui*sqrtRuNum*q[itemI][k]*(rui-bui) - beta*x[itemI][k]);
	               		y[itemI][k] += alpha * (eui*sqrtRuNum*q[itemI][k] - beta*y[itemI][k]);
	               	}
                } 
            }
            nowRmse =  sqrt( rmse / n);
            if( nowRmse >= preRmse && step > 5) break; //如果rmse已经开始上升了，则跳出循环
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse <<'\t'<< rmse<<endl;
            //alpha *= 0.992;    //逐步减小学习速率
            //RMSEProbe(); 
        }
        //ratingAll(data);  //预测所有的结果
        log.close();
        logbi.close();
        //RMSEProbe();  // 检查训练集情况
        return;
	    
    }
    
    //根据svd++公式预测打分值
    float rating(Rating  record){
        /*
        int user = record.user;
        int item = record.item;
        float ret = mean + bu[user] + bi[item] + dot(p[user], q[item]);
        if(ret < 1) ret = 1;
        if(ret > 5) ret = 5;
        return ret;
        */
        return  0.0;
    } 
    
    float predictRate(int user,int item)
    {
    	return 0.0;
    	/*
    	int u = user;
    	int i =  item;
    	float sumBias = 0.0;
    	int ruK = 0;
    	for(int j=0; j < userItems[u].size(); ++j) {
    		if((sim[i][userItems[u][j]]) < simLevel[i])continue;
    		++ruK;
    		float buj = mean + bu[u] + bi[userItems[u][j]];
    		float ruj = rateMatrix[u][userItems[u][j]];// u 对 j的打分如何快速获得呢？
    		sumBias += (ruj - buj) * w[i][userItems[u][j]] + c[i][userItems[u][j]];
    	}
    	float ret = 0.0;
    	if(ruK>1)
    	{
    		ret = mean + bu[u] + bi[i] + (1.0/sqrt(ruK)) * sumBias;//这里先不对k进行变化，先取k=无穷大
    	}
    	else ret  = mean;
    	if(ret < 1.0) ret = 1;
        if(ret > 5.0) ret = 5;
        //cout <<u<<"	"<<i<<"	"<<mean <<"	"<<bu[u]<<"	"<<bi[i]<<"	"<<userItems[u].size()<<"	"<<sumBias<<"	"<<ret<<endl;
        return ret;
        
        */
    }
    
    //检查测试集情况
    void RMSEProbe(void){
    	
        /*	1、load test数据集，
        	2、针对每一条数据，计算预测值，然后计算误差的平方和，计算总数量
        	3、输出rmse
        */
        /*
        std::ifstream from("../../mldataset/u1.test");
        char rateStr[100];
        vector<string> rateDetail(4);
        float predict = 0.0;
        float rmseSum = 0.0;
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
        */
    }
	
    void ratingAll(vector< Rating*> & data){
        /*
        for(int i = 0; i < data.size(); ++i){
            data[i].setPredict(rating(data[i]));
        }
        */
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
