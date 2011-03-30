float test_level = 0.004;
using namespace std;
void explode(const char * probe, char * data,vector<string> &result);
Rating::Rating(int u_item, int u_user, int u_criteria, float u_rate)
{
    item = u_item;
    user = u_user;
    rate = u_rate;
    criteria = u_criteria;
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

/**
 * load filePath中的数据到data这个vector中和 rateMatrix中
 * 这里要循环目录
 */
void loadRating(char * dirPath,  map<int,int> rateMatrixLocal[USER_NUM+1][CRI_NUM+1])
{
    char rateStr[256];
    DIR *dp;
 	struct dirent *dirp;
    vector<string> rateDetail(10);
	
	if((dp  = opendir(dirPath)) == NULL) {
        cout << "Error(" << errno << ") opening " << dirPath << endl;
   		return;
    }

    while ((dirp = readdir(dp)) != NULL) {
        
        string fileName = dirPath + string(dirp->d_name);
        //cout <<fileName<<endl;
        std::ifstream from (fileName.c_str());
        int itemId =  atoi(string(dirp->d_name).c_str());
        while(from.getline(rateStr,256)){
			//strTemp: 
			//971:5   5       5       5       5       4       4       4
			//1271:3  3       2       3       4       4       4       3

	    	string strTemp(rateStr);
	    	int pos = strTemp.find(":");
	    	int userId = atoi(strTemp.substr(0,pos-1).c_str());

	    	strcpy(rateStr,strTemp.substr(pos+1).c_str());
	        explode("	",rateStr,rateDetail);
	        
	        if(rateDetail.size()>=3){
	        	for(int c = 0; c<rateDetail.size() && c < 8; ++c) {
	        		//初始化rateMatrix
	        		double rate = atof(rateDetail[c].c_str());
					if(-1 == rate) continue;
					rateMatrixLocal[userId][c+1][itemId] = rate;
					//cout<<userId<<"	"<<itemId<<"	"<<c+1<<"	"<<rateMatrixLocal[userId][c+1][itemId]<<endl;
	        	}
	        }
	        
	    }
	    from.close();
    }
    return;
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
        //if(i>=3)break;
        pos1=pos2+1;
    }
    result[i] = (dataStr.substr(pos1));
}

/**
 * 计算全局的平均值
 */
int setMeanRating(map<int,int> rateMatrixLocal[USER_NUM+1][CRI_NUM+1])
{
    //计算平均值;
    long double sum = 0;
    int num = 0;
    int tmp = 0;
    for(int i = 1; i < USER_NUM+1; ++i) {
		for(int j = 1; j < CRI_NUM+1; ++j) {
			//iteration the map
			map<int,int>::iterator it;
			map<int,int>::iterator end = rateMatrixLocal[i][j].end();
			for ( it=rateMatrixLocal[i][j].begin() ; it != end; ++it) {
				sum +=  it->second;
				++num;
			}
    	}  			
    }
    return sum/num;
}

double get_rand()
{
    return (rand()%1000-500)/5000.0;
}

float setRand(double  p[], int num, float base)
{
	srand((unsigned)time(0));
    for(int i=1;i<num;++i){
    	float temp = base+get_rand();
        p[i] = temp;
    }
}
