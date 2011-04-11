<?php
$dirBase = 'F:\dataset\tripadvisor\Texts';

//第一步是不是应该给所有的酒店和所有的用户编上号？恩，是应该这样。
//给所有酒店编号，给所有的用户编号，编号的规则存入两个文件 hotelIds 和 userIds
//(1)给酒店编号
$hotelIds = array();
$userIds = array();
$num = 0;
$userNum = 0;
foreach( glob($dirBase.'\*.txt') as $filename)
{
	//针对每一个文件，提取出酒店的id
	$posStart = strpos($filename,'_');
	$posEnd = strpos($filename,'_',$posStart+1);
	$hotelId = substr($filename,$posStart+1,$posEnd - $posStart-1);
	$hotelIds[$hotelId] = $num;
	++$num;
	//下面给每一个用户编号，先读取文件内容，然后用正则表达式匹配
	$rateContent = file_get_contents($filename);
	preg_match_all('/<Author>(.+)/i',$rateContent,$usersArray);
	foreach($usersArray[1] as $user)
	{
		if('A TripAdvisor Member' == $user)continue;
		if(!isset($userIds[$user]))
		{
			$userIds[$user] = $userNum; //给用户编号
			++$userNum;
		}
	}
}
$hotelIdStr = serialize($hotelIds);
$userIdStr  = serialize($userIds);
file_put_contents('hotelIds',$hotelIdStr);
file_put_contents('userIds',$userIdStr);
?>