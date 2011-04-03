/* This file is Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
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
#ifndef NETFLIX_SVD_MOVIELENS_H_
#define NETFLIX_SVD_MOVIELENS_H_
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <map>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <ctime>
#include <dirent.h>
#include <malloc.h>
#include <errno.h>
#include <time.h>
using namespace std;

//#define USER_NUM 438212 
//#define ITEM_NUM 1000
#define USER_NUM 480189 
#define ITEM_NUM 17770
#define K_NUM  50
#define DIR_PATH "../../nf_dataset/transfer_set/"
#define PROBE_SET "../probe_real.txt"
#define TRAINING_SET "../data_without_prob.txt"


struct rateNode
{
	int user;
	short rate;
};
#endif // NETFLIX_SVD_MOVIELENS_H_
