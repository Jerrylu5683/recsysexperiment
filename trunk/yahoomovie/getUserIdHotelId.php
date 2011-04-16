<?php
/*
* Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
* please maitain the copyright information completely when you redistribute the code.
* 
* Please contact me via email honglianglv@gmail.com
* my blog: http://lifecrunch.biz
* my twitter: http://twitter.com/honglianglv
*
* It is free software; you can redistribute it and/or modify it under GPLV3.
*/
include "./common.php";

$filename = './clean-yahoo-user-ratings.txt'; 

$itemIds = array();
$userIds = array();
$userStat = array(); //stat the rate num of every user
$itemStat = array(); //stat the rate num of every item
$itemNum = 1;
$userNum = 1;

$rateContent = file_get_contents($filename);
$rateArray = explode("\n",$rateContent);

foreach($rateArray as $rate){
    if(strlen($rate) < 5)continue;
    $rateDetail = explode(" ",trim($rate));
    //itemId, UserId, StoryRating, ActingRating, DirectionRating, VisualRating, OverallRating
    $userId = trim($rateDetail[1]);
    $itemId = trim($rateDetail[0]);
    
    if(!isset($userIds[$userId]))
    {
        $userIds[$userId] = $userNum; //给用户编号
        $userStat[$userNum] = 1;
        ++$userNum;
        
    }
    else ++$userStat[$userIds[$userId]];
    
    if(!isset($itemIds[$itemId]))
    {
        $itemIds[$itemId] = $itemNum; //给用户编号
        $itemStat[$itemNum] = 1;
        ++$itemNum;
    }
    else ++$itemStat[$itemIds[$itemId]];
}
$itemIdStr = '';
foreach($itemIds as $key=>$itemId) {  //将itemId储存成c容易读取的形式
    $itemIdStr .= $key."\t".($itemId)."\n";
}
$userIdStr = '';
foreach($userIds as $key=>$userId) {  //将itemId储存成c容易读取的形式
    $userIdStr .= $key."\t".($userId)."\n";
}
asort($userStat); //对user打分数量进行排序
asort($itemStat); //对item打分数量进行排序

file_put_contents('tmpData/itemIds',$itemIdStr);
file_put_contents('tmpData/userIds',$userIdStr);
saveArrayToFile($userStat,'tmpData/userStat');
saveArrayToFile($itemStat,'tmpData/itemStat');
file_put_contents('tmpData/itemIdArrays',serialize($itemIds));
file_put_contents('tmpData/userIdArrays',serialize($userIds));
echo "get userId Map and itemId map successfully!\n";
?>