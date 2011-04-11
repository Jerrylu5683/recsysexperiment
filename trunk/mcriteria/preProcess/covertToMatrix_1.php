<?php
$dirBase = 'F:\dataset\tripadvisor\Texts';
$fileNum = 0;

$dh = opendir($dirBase);
while( $file = readdir($dh))
{
	echo $file."\r\n";continue;
	//针对每一个文件，提取出酒店的id
	$posStart = strpos($filename,'_');
	$posEnd = strpos($filename,'_',$posStart+1);
	$hotelId = substr($filename,$posStart+1,$posEnd - $posStart-1);
	echo $hotelId;die;
	$fileNum++;
}
echo $fileNum."\r\n";
die;


//将tripadvisor的评分数据变成评分矩阵

//遍历目录，处理每一个文件

foreach( glob($dirBase.'\*.txt') as $filename)
{
	//针对每一个文件，提取出每个打分指标的打分矩阵，存储在二维数组中
	
	echo $filename."\r\n";
	$fileNum++;
}
echo $fileNum."\r\n";
?>
