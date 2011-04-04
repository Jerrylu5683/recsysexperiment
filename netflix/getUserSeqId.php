<?php
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
$dirBase = '/home/lvhl/nf_dataset/training_set/';
$dstBase = '/home/lvhl/nf_dataset/transfer_set/';
//第一步给所有的用户编成一个连续的号码，然后将文件重新保存

$userIds = array();
$num = 0;
$userNum = 1;
//文件名example：mv_0000023.txt
$posStart = 0;
$posEnd = 0;
$movieId = 0;
$rateContent = $ret = '';
foreach( glob($dirBase.'*.txt') as $filename)
{
	//针对每一个文件，提取出movie的id
	
	$posStart = strrpos($filename,'_');
	$posEnd = strrpos($filename,'.');
	$movieId = substr($filename,$posStart+1);
	//echo $movieId;die;
	//下面给每一个用户编号，先读取文件内容，然后用正则表达式匹配
	//$rateContent = file_get_contents($filename);
	//$rateArray = explode("\n",$rateContent);
	$handler = fopen($dstBase.$movieId,'w');
	$rateHandler = fopen($filename,'r');
	while (!feof($rateHandler)) 
	{
		$buffer = fgets($rateHandler, 4096);
		$details = explode(',',$buffer);
		$userId = '';
		if(count($details)< 2)continue;
		if(!isset($userIds[$details[0]]))
		{
			$userIds[$details[0]] = $userNum; //给用户编号
			$userId = $userNum;
			++$userNum;
		}
		else
		{
			$userId = $userIds[$details[0]];
		}
		fwrite($handler, $userId.','.$details[1].','.$details[2]);
	}
	fclose($handler);
	fclose($rateHandler);
	echo $filename." is dealed!\n";
}
$userIdStr  = '';
foreach($userIds as $key=>$userId)
{
	$userIdStr .= $key."\t".$userId."\n";
}
file_put_contents('userIds',$userIdStr);
?>