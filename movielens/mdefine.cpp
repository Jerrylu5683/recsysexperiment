float test_level = 0.004;
using namespace std;
void explode(const char * probe, char * data,vector<string> &result);
Rating::Rating(int u_item,int u_user,float u_rate)
{
    item = u_item;
    user = u_user;
    rate = u_rate;
}

double Rating::predict()
{
    return 0.0;
}

void Rating::setPredict(float rating)
{
    rate = rating;
}

float Rating::value()
{
    return rate;
}
int Rating::year()
{
    return yearValue;
}

int Rating::month()
{
    return monthValue;
    int good =10;
}

float dot(vector<float> &p, vector<float> &q)
{
    float result = 0.0;
    for (int i=0; i<p.size(); ++i){
        result += p[i]*q[i];
    }
    return result;
}

void loadRating(char * filePath, vector<Rating> &data)
{
    std::ifstream from (filePath);
    char rateStr[256];
    vector<string> rateDetail(4);
   
    while(from.getline(rateStr,256)){
        //cout << rateStr;
        explode("	",rateStr,rateDetail);
        if(rateDetail.size()>=3){
        	Rating aRate(atoi(rateDetail[1].c_str()),atoi(rateDetail[0].c_str()),atof(rateDetail[2].c_str()));
			aRate.test = 0; //这里所有的数据都是训练数据，测试数据另外存储        	
        	data.push_back(aRate);
        }
        /**/
       // exit(1);
    }
    from.close();
    return;
}

void ratingProbe(const char * filePath, vector<Rating> &data)
{

}

void saveRating(const char * filePath, vector<Rating> &data)
{
}

void ratingQuiz(const char * filePath, vector<Rating> &data)
{
}

void explode(const char * probe,  char * data ,vector<string> &result)
{
    string dataStr(data);
    int pos1 = 0;
    int pos2 = 0;
    int i=0;
    while((pos2=dataStr.find(probe,pos1)) != string::npos){
        result[i] = (dataStr.substr(pos1,pos2-pos1));
        ++i;
        if(i>=3)break;
        pos1=pos2+1;
    }
    result[3] = (dataStr.substr(pos1));
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
    	}
    			
    }
    
    return sum/num;
}

float get_rand()
{
    return (rand()%100+1)/1000;
}

float setRand(vector<float> &p, int num, float base)
{
    for(int i=0;i<num;++i){
        p.push_back(base+get_rand());
        //cout << i <<"	"<< num <<"	"<< endl;
    }
}