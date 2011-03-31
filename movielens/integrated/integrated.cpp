/**
 * 本程序的目的是实现koren在SIGKDD'08论文中的方法，the integrated model of global neighborhood based model 
 * and svd++
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
    
    float p[USER_NUM+1][K_NUM+1] = {0};   //用于存储用户的属性描述p
    float puTemp[USER_NUM+1][K_NUM+1] = {0};   //用于存储用户的属性描述整体pu
    
    float q[ITEM_NUM+1][K_NUM+1] = {0};   //用于item的属性描述q
    float y[ITEM_NUM+1][K_NUM+1] = {0};   //implicit weight y
    
    float w[ITEM_NUM+1][ITEM_NUM+1] = {0};   //weight of explicit
    float c[ITEM_NUM+1][ITEM_NUM+1] = {0};   //weight of implicit
    
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);           //使用一个map数组存储稀疏的打分矩阵                     
	float mean = 0;                         //全局的平均值
    
    //函数声明
    void RMSEProbe(void);
    
    float predictRate(int user,int item);
    
    void model(int dim, float  alpha1, float  alpha2, float  alpha3, 
                        float  beta6, float  beta7, float  beta8)
    {
        cout << "begin initialization: " << endl;
        
        loadRating(DIR_PATH,rateMatrix);  //初始化完成
        
        mean = setMeanRating(rateMatrix); //求平均值，求bu和bi的值
        
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
        //对w ,c进行初始化，初始化的方法是随机函数，不知道这种方法是否好，是否会影响结果？？？？？？？
        for(int i = 1; i < ITEM_NUM+1; ++i){
            setRand(w[i],ITEM_NUM+1,0);    //初始化q[i]
            setRand(c[i],ITEM_NUM+1,0);    //初始化x[i]
        }
       
        for(int i = 1; i < ITEM_NUM+1; ++i){
            setRand(q[i],K_NUM+1,0);    //初始化q[i]
            setRand(y[i],K_NUM+1,0);    //初始化p[i]
        }
        
        for(int i = 1; i < USER_NUM+1; ++i){
            setRand(p[i],K_NUM+1,0);    //初始化p[i]
        }
        
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        float pui = 0.0 ; // 预测的u对i的打分
        double preRmse = 1000000000000.0; //用于记录上一个rmse，作为终止条件的一种，如果rmse上升了，则停止
        double nowRmse = 0.0;
        
        cout <<"begin testRMSEProbe(): " << endl;
        RMSEProbe();
        
        //main loop
        for(int step = 0; step < 70; ++step){  //只迭代35次
            long double rmse = 0.0;
            int n = 0;
            for( u = 1; u < USER_NUM+1; ++u) {   //循环处理每一个用户 
        		
               	int RuNum = rateMatrix[u].size(); //用户u打过分的item数目
               	float sqrtRuNum = 0.0;
               	if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
               	
               	//cacluate puTemp
               	for( k=1; k<K_NUM+1; ++k) {
               		double sumy = 0.0;
               		for(i=0; i < RuNum; ++i) {// 循环处理u打分过的每一个item
               			int itemI = rateMatrix[u][i].item;
               			sumy += y[itemI][k];
            		}
            		puTemp[u][k] = p[u][k]+ sqrtRuNum * sumy;
               	}
               	
               	double sumQE[K_NUM+1] = {0.0};
               	
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
                		exit(1);
                	}
                	
                	rmse += eui * eui; ++n;
                	if(n % 10000000 == 0)cout<<"step:"<<step<<"	n:"<<n<<" dealed!"<<endl;
                	
                	bu[u] += alpha1 * (eui - beta6 * bu[u]);
                	bi[itemI] += alpha1 * (eui - beta6 * bi[itemI]);
                	
                	for( k=1; k< K_NUM+1; ++k) {
	               		p[u][k] += alpha2 * (eui*q[itemI][k] - beta7*p[u][k]);
	            		q[itemI][k] += alpha2 * (eui*puTemp[u][k] - beta7*q[itemI][k]);
	            		sumQE[K_NUM+1] = eui * q[itemI][k];
	               	}
                	
                	
                	for( j=0; j< RuNum; ++j) {
	               		int itemJ = rateMatrix[u][j].item;
	               		double ruj = (double)rateMatrix[u][j].rate;
	               		w[itemI][itemJ] +=  alpha3 * (sqrtRuNum*eui*(ruj - mean -bu[u]-bi[itemJ]) - beta8*w[itemI][itemJ]);
	               		c[itemI][itemJ] +=  alpha3 * (sqrtRuNum*eui - beta8*c[itemI][itemJ]);
	               	}
                }
                
                for(j=0; j < RuNum; ++j) {// 循环处理u打分过的每一个item
               		int itemJ = rateMatrix[u][j].item;
	            	for( k=1; k< K_NUM+1; ++k) {
	            		y[itemJ][k] += alpha2 * (sqrtRuNum * sumQE[k] - beta7*y[itemJ][k]);
	            	}
            	}
            }
            nowRmse =  sqrt( rmse / n);
            
            if( nowRmse >= preRmse && step >= 3) break; //如果rmse已经开始上升了，则跳出循环
            else
            	preRmse = nowRmse;
            cout << step << "\t" << nowRmse <<'\t'<< preRmse<<" 	n:"<<n<<endl;
            //alpha1 *= 0.9;
            //alpha2 *= 0.9;
            //alpha3 *= 0.9;
            RMSEProbe();  // 检查训练集情况
            
            //alpha *= 0.992;    //逐步减小学习速率
            //RMSEProbe(); 
        }
        //ratingAll(data);  //预测所有的结果
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
       	
       	float ret; 
    	if(RuNum > 1)
    	{
    		double sumEx = 0.0, sumIm = 0.0;
    		float sqrtRuNum = 1/sqrt(RuNum);
    		for(int i=0; i< RuNum; ++i) {
    			int itemI = rateMatrix[user][i].item;
    			double rate = (double)rateMatrix[user][i].rate;
    			sumEx = (rate - mean - bu[user] - bi[itemI]) * w[item][itemI];
    			sumIm = c[item][itemI];
    		}
    		
    		ret = mean + bu[user] + bi[item] + dot(puTemp[user],q[item]) + sqrtRuNum * (sumEx+sumIm);//这里先不对k进行变化，先取k=无穷大
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
    float alpha1 = 0.007, alpha2 = 0.007, alpha3 = 0.001;  //parameters comes from the paper of koren SIGKDD,page 7
    float beta6 = 0.005, beta7 = 0.015, beta8 = 0.015;
    
    int dim = 100;   
    //ofstream outputfile("parameter.txt");
    
    //for(int i=0; i < 10; i++)
    {
    //	beta = i*0.001 + 0.002;
    //	cout << beta << endl;
    	svd::model(dim,alpha1,alpha2,alpha3,beta6,beta7,beta8);
    	
    }
    //outputfile.close();
    end = clock();
    duration = (end-start)/CLOCKS_PER_SEC;
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
