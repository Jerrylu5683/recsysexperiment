float test_level = 0.004;
using namespace std;
void explode(const char * probe,  const char * data ,vector<string> &result);
void qsort(vector<float>& array, int start, int end);
int  partition(vector<float>& array, int start, int end);
bool cmp(float a, float b);
Rating::Rating(short u_item,int u_user,short u_rate)//float u_rate,int year=1970,int month=1,int day=1)
{
    item = u_item;
    user = u_user;
    rate = u_rate;
    //yearValue = year;
    //monthValue = month;
    //dayValue = day;
}


void Rating::setPredict(int rating)
{
    rate = rating;
}

short Rating::value()
{
    return rate;
}
/*
float Rating::predict()
{
    return 0.0;
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
*/

float dot(float p[], vector<float> &q)
{
    float result = 0.0;
    for (int i=1; i<K_NUM+1; ++i){
        result += p[i]*q[i];
    }
    return result;
}

/**
 * load filePath中的数据到data这个vector中和 rateMatrix中
 * 
 */
void loadRating(char * dirPath, map<int,int>* rateMatrixLocal)
{
    char rateStr[256];    
    vector<string> rateDetail(10);
    int fileNum = 0;
    std::ifstream from ("data.txt");
    std::ofstream to("readlog.txt");
    int itemId = 0;
    while(from.getline(rateStr,256)){
    	string strTemp(rateStr);
	    int pos = strTemp.find(":");
	    if(-1 != pos) {
	    	itemId = atoi(strTemp.substr(0,pos).c_str());
	    	if(0 == itemId ) {
	    		cout<<strTemp<<"#####################"<<pos<<"####"<<strTemp.substr(0,pos).c_str()<<endl;
	    		exit(1);
	    	}		
	    	 ++fileNum;	 
	    	//cout<<itemId<<endl;
	    	//exit(1);
	    	if(fileNum %100 ==0)cout<<"read file "<<fileNum<<" sucessfully! max map:"<<endl;
	    	continue;
	    }
    	explode(",",rateStr,rateDetail);
        if(rateDetail.size()>=3){
        	
        	int userId = atoi(rateDetail[0].c_str());
        	if(0 == itemId ) {
	    		cout<<userId<<"#####################"<<endl;
	    		exit(1);
	    	}		
        	//初始化rateMatrix
        	rateMatrixLocal[userId][itemId] = atoi(rateDetail[1].c_str());
        	//to<<userId<<'\t'<< itemId<<'\t'<<rateMatrixLocal[userId].size()<<endl;
        }
                
    }
    from.close();
    to.close();
   	cout<<"read file sucessfully!"<<endl;
    return;
}

void explode(const char * probe,  const char * data ,vector<string> &result)
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
float setMeanRating(map<int,int> rateMatrixLocal[])
{
    //计算平均值;
    double sum = 0;
    int num = 0;
    cout<<"beging mean:"<<endl;
    map<int, int>::iterator  iter;
    for(int i = 1; i < USER_NUM+1; ++i){
		for(iter = rateMatrixLocal[i].begin(); iter != rateMatrixLocal[i].end(); ++iter) {
			sum += iter->second;
			++num;
		}
    }
    cout<<"edn mean!"<<endl;
    return sum/num;
}

float get_rand()
{
    return (rand()%1000-500)/5000.0;
}

/**
 * setRand的值
 */
float setRand(float  p[], int num, float base)
{
	srand((unsigned)time(0));
    for(int i=1;i<num;++i){
    	float temp = base+get_rand();
        p[i] = temp;
        //p.push_back(base+0.05); //全部初始化为base，看看影响如何
        //cout << i <<"	"<< temp <<"	"<< endl;
    }
}




void ratingProbe(const char * filePath, vector<Rating*> &data)
{

}

void saveRating(const char * filePath, vector<Rating*> &data)
{
}

void ratingQuiz(const char * filePath, vector<Rating*> &data)
{
}


//下面的这个函数用来利用最小堆找出第k大的元素
float getKmax(vector<float>& array, int K)
{
	vector<float> heapTmp;
	for(int i=0; i < array.size(); ++i)
	{
		heapTmp.push_back(array[i]);
		if(i == K-1) {
			make_heap(heapTmp.begin(),heapTmp.end(),cmp);
		}
		else if(i >=K) {
			push_heap(heapTmp.begin(),heapTmp.end(),cmp);
			pop_heap(heapTmp.begin(),heapTmp.end(),cmp);
			heapTmp.pop_back();
		}
		//cout << i<<'\t'<<heapTmp.size()<<endl;
	}
	return heapTmp.front();
}

bool cmp(float a, float b)
{
	return a > b;
}

//下面的几个函数用来实现快速排序
void qsort(vector<float>& array, int start, int end)
{
    if(start >= end)return;
    //cout<<"begin partition:"<<endl;
    int q = partition(array,start,end);
    //cout<< q<<endl;
    qsort(array,start,q-1);
    qsort(array,q+1,end);
}

int partition(vector<float>& array, int start,int end)
{
    if(start >= end) return start;
    if(start < 0) start = 0;
    int original  = start;
    float tmp = array[start];
    ++start;
    while(1) {
        while( start<=end && array[start] >= tmp)++start;
        while( start<=end && array[end] < tmp)--end;
        if(start < end) {
        	swap(array[start],array[end]);
        }
        else break;
    }
    swap(array[start-1],array[original]);
    return start-1;
}

void swap(float& a, float& b)
{
    float tmp = a;
    a = b;
    b = tmp;
}