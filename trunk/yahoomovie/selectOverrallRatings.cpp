/*
 * Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * Please contact me via email honglianglv@gmail.com
 * my blog: http://lifecrunch.biz
 * my twitter: http://twitter.com/honglianglv
 *
 * It is free software; you can redistribute it and/or modify it under GPLV3.
 *
 * This file select the overrall ratings and store in a separate
 * files. 
 * 1.read the files
 * 2.store the overalling ratings, including the test set and training set
 *
 */
#define USER_NUM 409399
#define ITEM_NUM 5628
#define CRI_NUM 5
#define K_NUM 1
#define TRAINING_SET "tmpData/training.txt"
#define TEST_SET "tmpData/test.txt"
#define RATE_SP " "
#include "gNbrHim/yahoo.h"
#include "common.cpp"
#include "gNbrHim/yahooBase.cpp"
#include "gNbrHim/gNbrHimBase.cpp"
vector<rateNode> rateMatrix[USER_NUM+1][CRI_NUM+1];   // store training set
vector<testSetNode> probeRow;                            // store test set

//int main(int argc, char *argv[]) {
int main() {

    loadRating(TRAINING_SET, rateMatrix, RATE_SP);  // load training set
    loadProbe(TEST_SET, probeRow, RATE_SP);   // load test set

    // select the overall ratings and save in new files!
    int u, i, j, c, k;
    ofstream trainingOut("tmpData/training_overall.txt");
    for (u = 1; u < USER_NUM+1; ++u) {
        c = 5;  // overall ratings
        int vSize = rateMatrix[u][c].size();
        for (i = 0; i < vSize; ++i) {
            int itemI = rateMatrix[u][c][i].item;
            int rate = rateMatrix[u][c][i].rate;
            if (rate == -1) continue;
            trainingOut << u << '\t' << itemI << '\t';
            trainingOut << rate << '\t' << 0 << endl;
        }
    }
    trainingOut.close();
    ofstream testOut("tmpData/test_overall.txt");
    int testSize = probeRow.size();
    for (k = 0; k < testSize; ++k) {
        int rate = probeRow[k].rate;
        int cri = probeRow[k].cri;
        if (5 != cri || -1 == rate) continue;
        testOut << probeRow[k].user << '\t' << probeRow[k].item << '\t';
        testOut << rate << '\t' << 0 << endl;
    }
    testOut.close();
    return 0;
}
