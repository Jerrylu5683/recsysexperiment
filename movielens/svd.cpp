#include "netflix.h"
#include "mdefine.cpp"

namespace svd{
    vector< vector<float> > p(USER_NUM+1), q(ITEM_NUM+1);  //所求的特性矩阵
    float mean = 0;
    
    //根据svd++公式预测打分值
    double rating(Rating  record){
        int user = record.user;
        int item = record.item;
        //double ret = record.predict() + bu[user] + bi[item] + dot(p[user], q[item]);
        double ret = mean + dot(p[user], q[item]);
        if(ret < 1) ret = 1;
        if(ret > 5) ret = 5;
        return ret;
    }
    
    double rating(int user,int item)
    {
    	double ret = mean + dot(p[user], q[item]);
    	if(ret < 1) ret = 1;
    	if(ret > 5) ret = 5;
    	return ret;
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
        float rmse = 0.0;
        int num = 0;
        int realValue = 0;
        while(from.getline(rateStr,100)){
        	explode("	",rateStr,rateDetail);
        	if(rateDetail.size()>=3){
        		predict = rating(atoi(rateDetail[0].c_str()), atoi(rateDetail[1].c_str()));
        		realValue = atoi(rateDetail[2].c_str());
        		rmse += pow(predict-realValue,2);
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
        if(data.size() == 0)loadRating("u1.base",data);  //初始化
        
        //1、首先搞清楚整个过程
        //（1）求平均值
        //2、一步一步实现每个过程
        //3、什么时候弄完，什么时候回去
        
        if(mean == 0)mean = setMeanRating(data); //求平均值，求bu和bi的值
        
        //中间删除一段代码，是xlvector计算平均打分写论文用的
        
        double cc = 1 / (double)(dim);
        //cout << cc << endl;exit(1);
        
        //初始化bu，就是user的bias（偏置）
        for(int i = 1; i < USER_NUM+1; ++i){
            //if(bu[i] < 0) continue;  //这里bu[i]小于0永远不成立，这个语句永远不会执行
            p[i].clear();            //清空所有的p[i]
            setRand(p[i],dim,cc);    //初始化p[i]
        }
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
                for(int k = 0; k < dim; ++k){
                    p[user][k] += alpha * (err * q[item][k] - beta * p[user][k]);
                    q[item][k] += alpha * (err * p[user][k] - beta * q[item][k]);
                }
            }
            if(step == 24)cout << alpha <<"	"<<step << ":\t" << sqrt(rmse / n) << endl;  
            alpha *= 0.98;    //逐步减小学习速率
        }
        ratingAll(data);  //预测所有的结果
        RMSEProbe();  // 检查训练集情况
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
    cout << "duration:"<<duration <<" s!" << CLOCKS_PER_SEC<<endl;
    return 0;
} 