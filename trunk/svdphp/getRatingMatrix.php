<?php
$preg_str = '/\[(.+)\]/i';
define("USER_NUM",943);
define("ITEM_NUM",1682);

//initial rating matrix with zero 
$ratingMatrix = array();
for($i=1;$i<USER_NUM+1;++$i)
{
	$userArray = array();
	for($j=1;$j<ITEM_NUM+1;++$j)
	{
		$userArray[$j] = 0;
	}
	$ratingMatrix[$i] = $userArray; 
}

//initial average array with zero
$userAvg = array();
$itemAvg = array();
for($i=1;$i<USER_NUM+1;++$i)
{	
	$userAvg[$i] = array('avg'=>0,'total'=>0,'ratingNum'=>0); 
}
for($i=1;$i<ITEM_NUM+1;++$i)
{	
	$itemAvg[$i] = array('avg'=>0,'total'=>0,'ratingNum'=>0); 
}

//填充打分矩阵，没有打分的内容用0填充
$ratingPath = 'E:\netflix_simulation\ml-data_0\u1.base';
$ratingHandler = fopen($ratingPath,'r');
$count = 0;
while(!feof($ratingHandler))
{
	$line = fgets($ratingHandler);
	if(!empty($line))
	{
		$ratingDetail = explode('	',$line);
		$ratingMatrix[$ratingDetail[0]][$ratingDetail[1]] = (int)$ratingDetail[2];//构造rating matrix
		
		//用于计算用户的平均打分
		++$userAvg[$ratingDetail[0]]["ratingNum"]; 
		if(!isset($userAvg[$ratingDetail[0]]['ratingNum']))
		{
			var_dump($userAvg[$ratingDetail[0]]);die;
		}
		$userAvg[$ratingDetail[0]]["total"] += $ratingDetail[2];
		
		//用于item的平均打分
		++$itemAvg[$ratingDetail[1]]["ratingNum"];
		if(!isset($itemAvg[$ratingDetail[1]]["ratingNum"]))
		{
			var_dump($ratingDetail);
			var_dump($itemAvg[$ratingDetail[1]]);die;
		} 
		$itemAvg[$ratingDetail[1]]["total"] += $ratingDetail[2];
		
		++$count;	
	}
}
fclose($ratingHandler);
$userAvgStr = '';
//计算平均打分
for($i=1;$i<USER_NUM+1;++$i)
{	
	if($userAvg[$i]['ratingNum'] > 0)
		$userAvg[$i]['avg'] = (float)$userAvg[$i]['total']/$userAvg[$i]['ratingNum'];
	$userAvgStr .= $i.'	'.$userAvg[$i]['avg']."\n";
}
file_put_contents('userAvg.txt',$userAvgStr);

$itemAvgStr = '';
for($i=1;$i<ITEM_NUM+1;++$i)
{	
	if($itemAvg[$i]['ratingNum'] > 0)
		$itemAvg[$i]['avg'] = (float)$itemAvg[$i]['total']/$itemAvg[$i]['ratingNum'];
	$itemAvgStr .= $i.'	'.$itemAvg[$i]['avg']."\n";
}
file_put_contents('itemAvg.txt',$itemAvgStr);

//计算相似度，pearson相关系数计算方法：
//（1）首先找出两者共同打分的项目，如果共同打分项目少于N，则相似度为0
//（2）得出一个mXm的相似度矩阵
//（3）使用knn预测打分，先进行排序找出最相似的n个邻居
//（4）预测打分
//（5）计算RMSE和MAE

//(1)计算相似性矩阵
/*
$sim = array();
$simResult = '';
for ($i=1; $i<USER_NUM+1; $i++) //计算相似度，每两个用户计算一遍mXmXK
{
	$rowResult = '';
	for ($j = 1; $j < USER_NUM+1; $j++)
	{
		if( $i == $j )
		{
			$sim[$i][$j] = 1;
		}
		else 
		{
			//对于每一个$i $j，计算相似度，先找出共同打分的集合，然后再计算
			$common_item = array();
			for($k = 1; $k< ITEM_NUM+1; $k++)
			{
				//@TODO  这个地方可以优化,找到共同打分的集合可以直接计算
				if($ratingMatrix[$i][$k] != 0 && $ratingMatrix[$i][$k] != 0)
				{
					$common_item[] = $k;		
				}
			}
			if( count($common_item) <2) //对于公共评分项目少于2个的用户，相似度为0
			{
				$sim[$i][$j] = 0;
			}
			else 
			{
				$numerator = 0;
				$firstSquare= 0;
				$secondSquare= 0;
				foreach ($common_item as $k) //针对每一个common_item做处理
				{
					$numerator += ($ratingMatrix[$i][$k] - $userAvg[$i]['avg'])*($ratingMatrix[$j][$k] - $userAvg[$j]['avg']);
					$firstSquare += pow($ratingMatrix[$i][$k] - $userAvg[$i]['avg'],2);
					$secondSquare += pow($ratingMatrix[$j][$k] - $userAvg[$j]['avg'],2);
				}
				$sim[$i][$j] = $numerator/(sqrt($firstSquare)*sqrt($secondSquare));
			}
			
		}
		
		$rowResult .= $sim[$i][$j]."	";
	}
	$simResult .=  trim($rowResult)."\r\n";
}
file_put_contents("simResult.txt",$simResult);
echo "finish sim computate!\r\n";die;
*/
//对于已经计算过的结果，直接读取内容
$sim = array();
$simPath = 'simResult.txt';
$simHandler = fopen($simPath,'r');
$i = 1;
while(!feof($simHandler))
{
	$line = fgets($simHandler);
	if(!empty($line))
	{
		$simDetail = explode('	',$line);
		//$ratingDetail[0]:userId;$ratingDetail[1]:itemId;$ratingDetail[2]:$ratings;
//		if($i == 3)
//		{
//			var_dump($simDetail);
//		}
		$simRow = array();
		foreach ($simDetail as $key=>$simInfo)
		{
			$simRow[$key+1] = (float)$simInfo;
		}
		$sim[$i] = $simRow;
		//if($i == 3){echo count($simRow);var_dump($simRow[365]);die;}
		++$i;
	}
}



//（2）先利用所有的邻居，预测测试集中数据的打分，并计算MAE和RMSE
//填充打分矩阵，没有打分的内容用0填充
$testPath = 'E:\netflix_simulation\ml-data_0\u1.test';
$testHandler = fopen($testPath,'r');
$testCount = 0;
$predictResult = '';
$ae = 0;
$se = 0;
while(!feof($testHandler))
{
	$line = fgets($testHandler);
	if(!empty($line))
	{
		$ratingDetail = explode('	',$line);
		$ratingDetail[0] = (int)$ratingDetail[0];//将字符型的数据转化为int型
		$ratingDetail[1] = (int)$ratingDetail[1];
		$ratingDetail[2] = (int)$ratingDetail[2];
		//$ratingDetail[0]:userId;$ratingDetail[1]:itemId;$ratingDetail[2]:$ratings;
		$sumSim = 0.0;
		$sumRate = 0.0;
		$noRatingNum = 0;
		for($i=1;$i<USER_NUM;$i++)
		{
			if($ratingMatrix[$i][$ratingDetail[1]] == 0)
			{
				++$noRatingNum;
				continue; //如果没有打分，则跳过
			}
			$sumSim += abs($sim[$ratingDetail[0]][$i]);
			$sumRate += $sim[$ratingDetail[0]][$i] * ($ratingMatrix[$i][$ratingDetail[1]] - $userAvg[$i]['avg']);
		}
		if($sumSim == 0 )
		{
			var_dump($ratingDetail[0],$ratingDetail[1],$noRatingNum);
			if(!empty($ratingMatrix[$ratingDetail[0]][$ratingDetail[1]]))$predictRate =$ratingMatrix[$ratingDetail[0]][$ratingDetail[1]];
			else $predictRate = $userAvg[$ratingDetail[0]]['avg'];
		}
		else
			$predictRate = $userAvg[$ratingDetail[0]]['avg'] + (1/$sumSim) * $sumRate;//预测打分
		//else $predictRate = 0;
		$ae += abs($predictRate - $ratingDetail[2]);
		$se += pow(($predictRate - $ratingDetail[2]),2);
		$predictResult .= $ratingDetail[0]."	".$ratingDetail[1]."	".$predictRate."	".$ratingDetail[2]."\n";
		++$testCount;
	}
	//if($testCount > 10)break;
}
$mae = $ae/$testCount;
$rmse = sqrt((1/$testCount) * $se);
echo $mae."	".$rmse;
file_put_contents("predictU1.txt",$predictResult);