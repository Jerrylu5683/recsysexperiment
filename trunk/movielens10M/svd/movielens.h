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

//#define USER_NUM 438212 
//#define ITEM_NUM 1000
#define USER_NUM 71567  //10K:943 1M:6040 10M:71567 
#define ITEM_NUM 10677 //10K:1682 1M:3900 10M:10677 
#define K_NUM  20
#define DIR_PATH "../../nf_dataset/transfer_set/"
#define PROBE_SET "../../ml10M_data/ra.test"
#define TRAINING_SET "../../ml10M_data/ra.train"

struct rateNode
{
	short item;
	float rate;
};
