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
* 
* 给所有酒店编号，给所有的用户编号，编号的规则存入两个文件 hotelIds 和 userIds
*
*/
$dirBase = '../../tripDataset_o'; 


//(1)给酒店编号
$hotelIds = array();
$userIds = array();
$num = 1;
$userNum = 1;
foreach( glob($dirBase.'/*.txt') as $filename)
{
	//针对每一个文件，提取出酒店的id
	$nameArray = explode('/',$filename);
	$fileSub = $nameArray[count($nameArray)-1];
	$posStart = strpos($fileSub,'_');
	$posEnd = strpos($fileSub,'_',$posStart+1);
	$hotelId = substr($fileSub,$posStart+1,$posEnd - $posStart-1);
	$hotelIds[$hotelId] = $num;
	++$num;
	//下面给每一个用户编号，先读取文件内容，然后用正则表达式匹配
	$rateContent = file_get_contents($filename);
	preg_match_all('/<Author>(.+)/i',$rateContent,$usersArray);
	foreach($usersArray[1] as $user)		{

		$user = trim($user);
		if('A TripAdvisor Member' == $user)continue;
		if(!isset($userIds[$user]))
		{
			$userIds[$user] = $userNum; //给用户编号
			++$userNum;
		}
	}
}
$hotelIdStr = '';
foreach($hotelIds as $key=>$hotelId) {  //将hotelId储存成c容易读取的形式
	$hotelIdStr .= $key."\t".($hotelId)."\n";
}
$userIdStr = '';
foreach($userIds as $key=>$userId) {  //将hotelId储存成c容易读取的形式
	$userIdStr .= $key."\t".($userId)."\n";
}
//$hotelIdStr = serialize($hotelIds);
//$userIdStr  = serialize($userIds);
file_put_contents('tmpData/hotelIds',$hotelIdStr);
file_put_contents('tmpData/userIds',$userIdStr);
file_put_contents('tmpData/hotelIdArrays',serialize($hotelIds));
file_put_contents('tmpData/userIdArrays',serialize($userIds));
echo "get userId Map and hotelId map successfully!\n";
?>