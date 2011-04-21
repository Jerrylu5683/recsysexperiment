/* Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * If there are some bugs, please contact me via email honglianglv@gmail.com or submit the bugs 
 * in the google code project http://code.google.com/p/recsyscode/issues/list
 * 
 * my blog: http://lifecrunch.biz
 * my twitter: http://twitter.com/honglianglv
 * my google profile:https://profiles.google.com/honglianglv/about?hl=en
 *
 * It is free software; you can redistribute it and/or modify it under 
 * the license GPLV3.
 *
 * This file contains some common operations of netflix dataset, for example reading the test set
 * and reading the test set
 *
 * the ratings of  training set store in the array (USER X ITEM, user is the row ,and item is the column)
 */
#ifndef YAHOOBASE_CPP_
#define YAHOOBASE_CPP_

/**
 * load the training set of netflix dataset
 * 
 */
void loadRating(const char * fileName, vector<rateNode> rateMatrixLocal[USER_NUM+1][CRI_NUM+1], const char* separator)
{
    char rateStr[256];
    char* pch;    
    vector<string> rateDetail;
    int fileNum = 0;
    std::ifstream from (fileName);
    if (!from.is_open()) {
        cout << "can't open  operation failed!\n";
        exit(1);
    }
    
    int itemId = 0, userId = 0, story = 0, acting = 0, direction = 0, visual = 0, overall = 0;
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
            if(fileNum %3000 ==0) {
                cout<<"read file "<<fileNum<<" sucessfully!"<<endl;
            }
            continue;
        }
        if(strTemp.length() < 3)continue;
        int i = 0;
        pch = strtok (rateStr,separator);
        while (pch != NULL) {
            if(0 == i) itemId = atoi(pch);
            else if(1 == i) userId = atoi(pch);
            else if(2 == i) story = atoi(pch);
            else if(3 == i) acting = atoi(pch);
            else if(4 == i) direction = atoi(pch);
            else if(5 == i) visual = atoi(pch);
            else if(6 == i) overall = atoi(pch);
            else if(i > 6) break;
            ++i;
            pch = strtok (NULL,separator);
        }
        if(0 == itemId || 0 == userId) {
            cout<<strTemp<<"#####################"<<endl;
            exit(1);
        }        
        //initialization rateMatrix
        try {
            rateNode tmpNode1;
    		tmpNode1.item = (short)itemId;
    		tmpNode1.rate = (0 == story) ? -1 : (short)story;
            rateMatrixLocal[userId][1].push_back(tmpNode1);
            rateNode tmpNode2;
    		tmpNode1.item = (short)itemId;
    		tmpNode1.rate = (0 == acting) ? -1 : (short)acting;
            rateMatrixLocal[userId][2].push_back(tmpNode2);
            rateNode tmpNode3;
    		tmpNode1.item = (short)itemId;
    		tmpNode1.rate = (0 == direction) ? -1 : (short)direction;
            rateMatrixLocal[userId][3].push_back(tmpNode3);
            rateNode tmpNode4;
    		tmpNode1.item = (short)itemId;
    		tmpNode1.rate = (0 == visual) ? -1 : (short)visual;
            rateMatrixLocal[userId][4].push_back(tmpNode4);
            rateNode tmpNode5;
    		tmpNode1.item = (short)itemId;
    		tmpNode1.rate = (0 == overall) ? -1 : (short)overall;
            rateMatrixLocal[userId][5].push_back(tmpNode5);
        }
        catch (bad_alloc& ba) {
            cerr << "bad_alloc caught: " << ba.what() << endl;
            cout << "Can't allocate the momery!" << endl; exit(1);
        }
    }
    from.close();
       cout<<"read file sucessfully!"<<endl;
    return;
}

//load test set of netflix dataset
/*
void loadProbe(char * fileName,vector<testSetNode>& probeSet, const char* separator)
{
    ifstream in(fileName);
    if (!in.is_open()) {
        cout << "can't open test set file!\n";
        exit(1);
    }
    char rateStr[256];
    char* pch ; // store a token of a string

    string strTemp;
    int rateValue = 0, itemId = 0, userId = 0, story = 0, acting = 0, direction = 0, visual = 0, overall = 0;
    while(in.getline(rateStr,256)){
        strTemp = rateStr;
        if(strTemp.length() < 4) continue;
        int i = 0;
        pch = strtok (rateStr,separator);
        while (pch != NULL) {
            if(0 == i) itemId = atoi(pch);
            else if(1 == i) userId = atoi(pch);
            else if(2 == i) story = atoi(pch);
            else if(3 == i) acting = atoi(pch);
            else if(4 == i) direction = atoi(pch);
            else if(5 == i) visual = atoi(pch);
            else if(6 == i) overall = atoi(pch);
            else if(i > 6) break;
            ++i;
            pch = strtok (NULL,separator);
        }
        try {
            testSetNode tmpNode;
            tmpNode.user = (short)userId;
            tmpNode.item = (short)itemId;
    		tmpNode.story = (short)story;
    		tmpNode.acting = (short)acting;
    		tmpNode.direction = (short)direction;
    		tmpNode.visual = (short)visual;
    		tmpNode.overall = (short)overall;
    		tmpNode.flag = 0;
            probeSet.push_back(tmpNode);
            ++probeNum;
        }
        catch (bad_alloc& ba) {
            cerr << "bad_alloc caught: " << ba.what() << endl;
            cout << "Can't allocate the momery!" << endl; exit(1);
        }
    }
    cout << "Load " << probeNum << " test ratings successfully!"<<endl;
    in.close(); 
}
*/
#endif // YAHOOBASE_CPP_
