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
#define USER_NUM 6040  //10K:943 1M:6040
#define ITEM_NUM 3706 //10K:1682 1M:3900
#define K_NUM  50
#define DIR_PATH "../../nf_dataset/transfer_set/"
#define PROBE_SET "../../ml_data/test.txt"
#define TRAINING_SET "../../ml_data/training.txt"

struct rateNode
{
	short item;
	short rate;
};
