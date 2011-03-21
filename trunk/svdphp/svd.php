<?php
/**
 * 采用svd的方式预测打分
 * 
 */
define("USER_NUM",943);
define("ITEM_NUM",1682);
define("LP",0.001);//learning speed
define("FACTOR",0.05);//learning speed
$learingSpeed = 0.01;
//初始化矩阵
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

$dim = 100; //特性矩阵的维数
$base = 0.01;
$userMatrix = array();
$itemMatrix = array();
//初始化用户矩阵 USER_NUM X dim
for($i=1; $i<USER_NUM+1; $i++)
{
	for($j=1;$j<$dim+1;$j++)
	{
		$userMatrix[$i][$j] = $base+rand(-0.005,0.005);
	}
}

//初始化Item矩阵 dim X ITEM_NUM，先求item矩阵的逆矩阵
for($i=1; $i<ITEM_NUM+1; $i++)
{
	for($j=1;$j<$dim+1;$j++)
	{
		$itemMatrix[$i][$j] = $base+rand(-0.005,0.005);
	}
}
//开始迭代,迭代的方式为每次修正特性矩阵，修正特性矩阵中的每一个项目
$count = 0;
$firstError = 0.0;   // 用于统计每次预测误差

while(1)
{
	$secondError = 0;
	$ratingPath = 'E:\netflix_simulation\ml-data_0\u1.base';
	$ratingHandler = fopen($ratingPath,'r');
	while(!feof($ratingHandler))
	{
		$line = fgets($ratingHandler);
		if(!empty($line))
		{
			$ratingDetail = explode('	',$line);
			//读取每一个打分，针对每一个打分，来修正对用户特性矩阵和Item特性矩阵的预估，如何读取列向量的取值，
			$predictRate = vectorMultiple($userMatrix[$ratingDetail[0]],$itemMatrix[$ratingDetail[1]],$dim);
			$error = $ratingDetail[2]-$predictRate;
			$secondError += $error*$error;  
			for($j=1; $j<$dim+1; $j++)
			{
//				if(!isset($itemMatrix[$ratingMatrix[1]][$j]))
//				{
//					var_dump($ratingMatrix);
//				}
				$userMatrix[$ratingDetail[0]][$j] = $userMatrix[$ratingDetail[0]][$j] + 
						$learingSpeed * ($error*$itemMatrix[$ratingDetail[1]][$j] - FACTOR * $userMatrix[$ratingDetail[0]][$j]);
				$itemMatrix[$ratingDetail[1]][$j] = $itemMatrix[$ratingDetail[1]][$j] + 
						$learingSpeed * ($error*$userMatrix[$ratingDetail[0]][$j] - FACTOR * $itemMatrix[$ratingDetail[1]][$j] );	
			}
			$ratingMatrix[$ratingDetail[0]][$ratingDetail[1]] = (int)$ratingDetail[2];//构造rating matrix
		}	
	}
	fclose($ratingHandler);
	var_dump($secondError);
	if($firstError == 0)$firstError = $secondError;
	else if($firstError < $secondError)break;
	else if($firstError > $secondError) $secondError = $firstError;
	if($firstError-$secondError < 10000) $learingSpeed = 0.01;
	else if($firstError-$secondError < 1000)$learingSpeed = 0.001;
	else if($firstError-$secondError < 100)$learingSpeed = 0.0001;
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
		$predictRate = vectorMultiple($userMatrix[$ratingDetail[0]],$itemMatrix[$ratingDetail[1]],$dim); //预测打分
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
file_put_contents("predictSVDU1.txt",$predictResult);


function vectorMultiple($vector1,$vector2,$dim)
{
	$result = 0;
	for($i=1;$i<$dim+1;$i++)
	{
		$result += $vector1[$i] * $vector2[$i];
	}
	return $result;
}