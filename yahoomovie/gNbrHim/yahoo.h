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
 * This file gives some define of the yahoo movie dataset
 *
 */
#ifndef YAHOO_H_
#define YAHOO_H_

#include <iostream>
#include <string>
#include <string.h>
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

struct rateNode
{
	short item;
	short story;
	short acting;
	short direction;
	short visual;
	short overall;
};


//use different struct between test set and training set to save memory space
//when the training set is very large( eg netflix dataset), the saving space is significant
struct testSetNode
{
    int user;
    short item;
    short story;
	short acting;
	short direction;
	short visual;
	short overall;
};
float predictRate(int user,int item,int dim);
#endif // YAHOO_H_
