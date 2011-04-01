#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <math.h>
#include <stdlib.h>
#define  SOURCE "../ml1M_data/ratings.dat"
#define USER_NUM 6040 
#define ITEM_NUM 3592
using namespace std;
void get_rand(set<int> & myset, int low, int high,int length);

struct rateNode
{
	short item;
	short rate;
	short flag;
};


int main()
{
    //(1)load the data in a rateMatrix 
    //(2)random select the test rate for every
    //(3)save the rate in different files.
    ifstream in(SOURCE);
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);           //使用一个vector数组存储稀疏的打分矩阵
    char rateStr[200];
    set<int> myset;
    int itemId = 0, userId = 0, rate = 0;
    int pos1=0,pos2=0,pos3=0;    
    //load all the ratings to the rateMatrix
    while(in.getline(rateStr,200)) {
        string strTemp(rateStr);
    	if(strTemp.length() < 3)continue;
    	pos1 = strTemp.find("::");
    	userId = atoi(strTemp.substr(0,pos1).c_str());
    	pos2 = strTemp.find("::",pos1+2);
    	itemId = atoi(strTemp.substr(pos1+2,pos2-pos1-1).c_str());
    	pos3 = strTemp.find("::",pos2+2);
    	rate = atoi(strTemp.substr(pos2+2,pos3-pos2-1).c_str());
    	//cout<<userId<<'\t'<<itemId<<'\t'<<rate<<endl;exit(1);
    	if(0 == itemId || 0 == userId ||  0 == rate ) {
    		cout<<rateStr<<"	"<<userId<<"	"<<itemId<<"	"<<rate<<"#####################"<<endl;
    		exit(1);
    	}		
    	try {
    		rateNode tmpNode;
    		tmpNode.item = itemId;
    		tmpNode.rate = (short)rate;
    		tmpNode.flag = 0;
    		rateMatrix[userId].push_back(tmpNode);
    	}
    	catch (bad_alloc& ba)
    	{
    		cerr << "bad_alloc caught: " << ba.what() << endl;
    	}
    }
    in.close();
    cout<<"read file successfully!"<<endl;
    
    //split the ratings
    ofstream training("training.txt");
    ofstream test("test.txt");
    for(int i = 1; i < USER_NUM+1; ++i){
    	int vSize = rateMatrix[i].size();
    	//need to get three random num beteen 0 and (vSize-1)
    	get_rand(myset, 0, vSize-1, 3);
    	set<int>::iterator it;
    	set<int>::iterator end = myset.end(); 
    	for (it=myset.begin(); it!= end; ++it ) {
    		rateMatrix[i][*it].flag = 1;
    	}
		for(int j=0; j < vSize; ++j) {
			if(rateMatrix[i][j].flag == 1) { // in test set
				test << i<<"	"<<rateMatrix[i][j].item<<"	"<<rateMatrix[i][j].rate<<"	"<<0<<endl;
			}
			else {
				training << i<<"	"<<rateMatrix[i][j].item<<"	"<<rateMatrix[i][j].rate<<"	"<<0<<endl;
			}
		}
		cout << "user "<<i<< " deal successfully!"<<endl;
    }
    test.close();
    training.close();
}


/**
 *	
 */
void get_rand(set<int> & myset, int low, int high,int length)
{
	srand((unsigned)time(0));
	int num = 0;
	set<int>::iterator end =  myset.end();
	int good = high-low+1;
	while (num < length) {
		int tmp = rand()%good+low;
		if(myset.find(tmp) == end) {
			myset.insert(tmp);
			++num;
		}
	}
}
