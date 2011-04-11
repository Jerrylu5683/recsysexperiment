<?php
/*
 * convert the ratings in the files to 8 matrixs
 * 
 */
$dirBase = 'F:\dataset\tripadvisor\Texts';

//第一步是不是应该给所有的酒店和所有的用户编上号？恩，是应该这样。
//给所有酒店编号，给所有的用户编号，编号的规则存入两个文件 hotelIds 和 userIds
//(1)给酒店编号
$hotelIds = unserialize(file_get_contents('hotelIds'));
$userIds  = unserialize(file_get_contents('userIds'));

//将tripadvisor的评分数据变成评分矩阵
//遍历目录，处理每一个文件
$fileNum = 0;
$totalMatrix = array();
$valueMatrix = array();
$roomsMatrix = array();
$locationMatrix = array();
$cleanlinessMatrix = array();
$service1Matrix = array();
$service2Matrix = array();
$businessMatrix = array();
foreach( glob($dirBase.'\*.txt') as $filename)
{
	//针对每一个文件，提取出每个打分指标的打分矩阵，存储在二维数组中
	
	$fileContent = file_get_contents($filename);
	//通过文件名获得hotelId，
	$posStart = strpos($filename,'_');
	$posEnd = strpos($filename,'_',$posStart+1);
	$hotelId = $hotelIds[substr($filename,$posStart+1,$posEnd - $posStart-1)]; //得到hotelId
	
	//正则表达式获得用户的名，然后获得userId，正则表达式同时获得打分详情
	$rateContent = file_get_contents($filename);
	preg_match_all('/<Author>(.+)\n.+\n.+\n<Rating>(.+)\n/i',$rateContent,$usersArray);
	foreach($usersArray[1] as $key=>$user)
	{
		$userId = $userIds[$user];
		$ratingArray = explode('	',$usersArray[2][$key]);
		$totalMatrix[$userId][$hotelId] = $ratingArray[0];
		$valueMatrix[$userId][$hotelId] = $ratingArray[1];
		$roomsMatrix[$userId][$hotelId] = $ratingArray[2];
		$locationMatrix[$userId][$hotelId] = $ratingArray[3];
		$cleanlinessMatrix[$userId][$hotelId] = $ratingArray[4];
		$service1Matrix[$userId][$hotelId] = $ratingArray[5];
		$service2Matrix[$userId][$hotelId] = $ratingArray[6];
		$businessMatrix[$userId][$hotelId] = $ratingArray[7];
	}
	$fileNum++;
}
file_put_contents('totalMatrix',serialize($totalMatrix));
file_put_contents('valueMatrix',serialize($valueMatrix));
file_put_contents('roomsMatrix',serialize($roomsMatrix));
file_put_contents('locationMatrix',serialize($locationMatrix));
file_put_contents('cleanlinessMatrix',serialize($cleanlinessMatrix));
file_put_contents('service1Matrix',serialize($service1Matrix));
file_put_contents('service2Matrix',serialize($service2Matrix));
file_put_contents('businessMatrix',serialize($businessMatrix));
echo $fileNum."\r\n";
?>