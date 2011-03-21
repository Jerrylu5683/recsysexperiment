<?php
/**
 * 本程序的作用是通过预测和实际的结果，计算RMSE和MAE
 */
$resultPath = 'E:\workspace\php\movielens\predictU1.txt';
$resultHandler = fopen($resultPath,'r');
$resultCount = 0;
$ae = 0;
$se = 0;
while(!feof($resultHandler))
{
	$line = fgets($resultHandler);
	if(!empty($line))
	{
		$ratingDetail = explode('	',$line);
		$ratingDetail[0] = (int)$ratingDetail[0];//将字符型的数据转化为int型
		$ratingDetail[1] = (int)$ratingDetail[1];
		$ratingDetail[2] = (float)$ratingDetail[2];
		$ratingDetail[3] = (float)$ratingDetail[3];
		$ae += abs($ratingDetail[2] - $ratingDetail[3]);
		$se += pow(($ratingDetail[2] - $ratingDetail[3]),2);
		++$resultCount;
	}
	//if($testCount > 10)break;
}
$mae = $ae/$resultCount;
$rmse = sqrt((1/$resultCount) * $se);
echo $mae."	".$rmse;
?>