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
	        		int rate = atoi(rateDetail[c].c_str());
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
