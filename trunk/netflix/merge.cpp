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
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string>
#include <errno.h>
#define DIR_PATH "../../nf_dataset/transfer_set/"
using namespace std;


int main()
{
	DIR *dp;
	char rateStr[256];
	struct dirent *dirp;
   
    if((dp  = opendir(DIR_PATH)) == NULL) {
        cout << "Error(" << errno << ") opening " << DIR_PATH << endl;
   		return 0;
    }
    int fileNum = 0;
    std:ofstream result("data.txt");
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
