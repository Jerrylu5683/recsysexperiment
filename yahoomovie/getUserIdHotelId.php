<?php
/*
* Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
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
* 
*
*/
$filename = './clean-yahoo-user-ratings.txt'; 

$movieIds = array();
$userIds = array();
$num = 1;
$userNum = 1;

$rateContent = file_get_contents($filename);
$rateArray = explode("\n",$rateContent);
echo count($rateArray);die;
foreach($usersArray[1] as $user)		{

	$user = trim($user);
	if('A TripAdvisor Member' == $user)continue;
	if(!isset($userIds[$user]))
	{
		$userIds[$user] = $userNum; //给用户编号
		++$userNum;
	}
}
$movieIdStr = '';
foreach($movieIds as $key=>$movieId) {  //将movieId储存成c容易读取的形式
	$movieIdStr .= $key."\t".($movieId)."\n";
}
$userIdStr = '';
foreach($userIds as $key=>$userId) {  //将movieId储存成c容易读取的形式
	$userIdStr .= $key."\t".($userId)."\n";
}
//$movieIdStr = serialize($movieIds);
//$userIdStr  = serialize($userIds);
file_put_contents('tmpData/movieIds',$movieIdStr);
file_put_contents('tmpData/userIds',$userIdStr);
file_put_contents('tmpData/movieIdArrays',serialize($movieIds));
file_put_contents('tmpData/userIdArrays',serialize($userIds));
echo "get userId Map and movieId map successfully!\n";
?>