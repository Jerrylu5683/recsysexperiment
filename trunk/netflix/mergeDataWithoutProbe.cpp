#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string>
#include <map>
#include <errno.h>
#define USER_NUM 480189 
#define ITEM_NUM 17770
#define DIR_PATH "../../nf_dataset/transfer_set/"
using namespace std;

int main()
{
	DIR *dp;
	char rateStr[256];
	struct dirent *dirp;
	map<int,short> probeMatrix[USER_NUM+1];   
	
	//先load probe数据
	//首先读取probe_t.txt 文件，对于每一个rate，算出预测的打分，根据对应的实际打分计算RMSE
	ifstream in("probe_real.txt");
	int pos1,pos2;
	string strTemp;
	int rateValue,itemId,userId,probeNum;
	float pRate,err;
	
	while(in.getline(rateStr,256)){
    	strTemp = rateStr;
	    pos1 = strTemp.find(",");
	    pos2 = strTemp.find(",",pos1+1);
    	itemId = atoi(strTemp.substr(0,pos1).c_str());
    	userId = atoi(strTemp.substr(pos1+1,pos2-pos1-1).c_str());
    	rateValue = atoi(strTemp.substr(pos2+1).c_str());
    	probeMatrix[userId][itemId] = rateValue;
    }
	in.close(); //load userId map完毕
   
    if((dp  = opendir(DIR_PATH)) == NULL) {
        cout << "Error(" << errno << ") opening " << DIR_PATH << endl;
   		return 0;
    }
    int fileNum = 0;
    std:ofstream result("data_without_prob.txt");
    while ((dirp = readdir(dp)) != NULL) {
        if(string(dirp->d_name).length() < 3)continue;
        string fileName = DIR_PATH + string(dirp->d_name);
        //cout <<fileName<<endl;
        std::ifstream from (fileName.c_str());
        string strTemp(dirp->d_name);
	    int pos = strTemp.find(".");
	    int itemId = atoi(strTemp.substr(0,pos).c_str());
	   
	    result<<itemId<<":"<<'\n';
        while(from.getline(rateStr,256)){
        	strTemp = rateStr;
		    pos1 = strTemp.find(",");
	    	userId = atoi(strTemp.substr(0,pos1).c_str());
	    	if(probeMatrix[userId].find(itemId) != probeMatrix[userId].end()) continue;
        	result<<rateStr<<'\n';
        	//cout<<rateStr<<'\n';
	    }
	    from.close();
	    ++fileNum;	 
	    //if(fileNum == 1)break;
	    if(fileNum %100 ==0)cout<<"merge file "<<fileNum<<" sucessfully!"<<endl;
    }
    result.close();
    return 0;
}
