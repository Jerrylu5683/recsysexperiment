# -*- coding: UTF-8 -*-
# A dictionary of movie critics and their ratings of a small
# set of movies
critics={'Lisa Rose': {'Lady in the Water': 2.5, 'Snakes on a Plane': 3.5,
 'Just My Luck': 3.0, 'Superman Returns': 3.5, 'You, Me and Dupree': 2.5, 
 'The Night Listener': 3.0},
'Gene Seymour': {'Lady in the Water': 3.0, 'Snakes on a Plane': 3.5, 
 'Just My Luck': 1.5, 'Superman Returns': 5.0, 'The Night Listener': 3.0, 
 'You, Me and Dupree': 3.5}, 
'Michael Phillips': {'Lady in the Water': 2.5, 'Snakes on a Plane': 3.0,
 'Superman Returns': 3.5, 'The Night Listener': 4.0},
'Claudia Puig': {'Snakes on a Plane': 3.5, 'Just My Luck': 3.0,
 'The Night Listener': 4.5, 'Superman Returns': 4.0, 
 'You, Me and Dupree': 2.5},
'Mick LaSalle': {'Lady in the Water': 3.0, 'Snakes on a Plane': 4.0, 
 'Just My Luck': 2.0, 'Superman Returns': 3.0, 'The Night Listener': 3.0,
 'You, Me and Dupree': 2.0}, 
'Jack Matthews': {'Lady in the Water': 3.0, 'Snakes on a Plane': 4.0,
 'The Night Listener': 3.0, 'Superman Returns': 5.0, 'You, Me and Dupree': 3.5},
'Toby': {'Snakes on a Plane':4.5,'You, Me and Dupree':1.0,'Superman Returns':4.0}}

from math import sqrt

# getMeans of all the persons
def getMeans(prefs,means):
    for person in prefs:
        mean=sum([prefs[person][it] for it in prefs[person]])/len(prefs[person]) # means of all the preferences
        means.setdefault(person,mean)
    return means

# Returns the Pearson correlation coefficient for p1 and p2
def sim_pearson(prefs,p1,p2,means):
  # Get the list of mutually rated items
  if p1==p2: return 1
  si={}
  for item in prefs[p1]: 
    if item in prefs[p2]: si[item]=1

  # if they are no ratings in common, return 0
  if len(si)==0: return 0

  # Sum calculations
  n=len(si)
  
  # means of all the preferences
  mean1=means[p1]
  mean2=means[p2]

  #
  
  # Sums of the squares
  sum1Sq=sum([pow(prefs[p1][it]-mean1,2) for it in si])
  sum2Sq=sum([pow(prefs[p2][it]-mean2,2) for it in si])	
  
  # Sum of the products
  pSum=sum([(prefs[p1][it]-mean1)*(prefs[p2][it]-mean2) for it in si])
  
  # Calculate r (Pearson score)
  den=sqrt(sum1Sq*sum2Sq)
  if den==0: return 0
  r=pSum/den
  
  return r

#计算出每两个用户之间的相似度矩阵，然后利用这个相似度预测打分
def getSim(prefs,means):
    simMatrix = {}
    for item in prefs:
        itemSim = {}
        for itemSecond in prefs:
            sim = sim_pearson(prefs,item,itemSecond,means)
            itemSim.setdefault(itemSecond,sim)
        simMatrix.setdefault(item,itemSim)
    return simMatrix
        
#给定输入user和item,预测item的打分
def predictRank(prefs,simMatrix,means,person,item):
    if item in prefs[person]: return prefs[person][item] #如果以前打过分，直接返回
    simSum = 0
    total  = 0
    predictResult = 0
    for other in prefs:
        # don't compare me to myself
        if other==person: continue
        sim=simMatrix[person][other]  #get the similarity
        
        # ignore scores of zero or lower
        if sim<=0: continue
        # 仅仅计算对item打过分的用户
        if item in prefs[other]:
            # Similarity * Score
            total += (prefs[other][item]-means[other])*sim
            # Sum of similarities
            simSum+=sim
    # Create the normalized list
    if simSum !=0 :
    	predictResult= means[person]+(1/simSum)*total
    else :
    	predictResult= means[person]
    return predictResult

#给定文件，初始化打分矩阵
def initialRateMatrix(filePath,ratingMatrix):
	fileHandler = open(filePath,'r')
	str = fileHandler.readline()
	while str:
		detail = str.split('	');
		if detail[0] not in ratingMatrix:
			itemInfo = {}
			ratingMatrix[detail[0]]=itemInfo
		ratingMatrix[detail[0]][detail[1]]=float(detail[2])
		str = fileHandler.readline()
	return ratingMatrix

#主函数进程，计算MAE，RMSE
trainingPath = "E:/netflix_simulation/ml-data_0/u1.base"
testFilePath = "E:/netflix_simulation/ml-data_0/u1.test"
testFileHandler = open(testFilePath,'r');
#初始化
ae = 0
se = 0
critics = {}
initialRateMatrix(trainingPath,critics)
#print(critics)
#exit()
means = {}
getMeans(critics,means)
simMatrix = getSim(critics,means)

str  = testFileHandler.readline()
testNum = 0
while str:
    detail = str.split('	')
    predictScore = predictRank(critics,simMatrix,means,detail[0],detail[1])
    print(detail[0],detail[1],predictScore,detail[2])
    ae += abs(predictScore-float(detail[2]))
    se += pow((predictScore-float(detail[2])),2)
    testNum += 1
    str = testFileHandler.readline()
mae = ae/testNum;
rmse = sqrt(se/testNum)
print mae
print rmse
