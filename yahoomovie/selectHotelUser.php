<?php

//选择hotel非常容易，因为hotel的打分数据都在一个文件中，选择hotel完毕，再选择user

//（1）先将评价个数存在一个数组中，然后排序，最后打印出来，人工看需要多少个
$dirBase = 'F:\dataset\tripadvisor\Texts';

/*
$hotelCountArray = array();

foreach( glob($dirBase.'\*.txt') as $filename)
{
	$userNum = 0;
	//针对每一个文件，提取出酒店的id
	$posStart = strpos($filename,'_');
	$posEnd = strpos($filename,'_',$posStart+1);
	$hotelId = substr($filename,$posStart+1,$posEnd - $posStart-1);
	
	//下面给每一个用户编号，先读取文件内容，然后用正则表达式匹配
	$rateContent = file_get_contents($filename);
	preg_match_all('/<Author>(.+)/i',$rateContent,$usersArray);
	$userNum = count($usersArray[1]);
	$hotelCountArray[$hotelId] = $userNum;
}
asort($hotelCountArray);
file_put_contents('select/hotelCountArray',serialize($hotelCountArray));
saveArrayToFile($hotelCountArray,'select/hotelCount');
*/
$hotelCountArray = unserialize(file_get_contents('select/hotelCountArray'));
//遍历其中的每一个文件，找出其中评分数目大于10个的用户，然后删除它们的评价
$userCountArray =  array();
foreach( glob($dirBase.'\*.txt') as $filename)
{
	$userNum = 0;
	
	//通过文件名获得hotelId，
	$posStart = strpos($filename,'_');
	$posEnd = strpos($filename,'_',$posStart+1);
	$hotelId = substr($filename,$posStart+1,$posEnd - $posStart-1);//得到hotelId
	
	if($hotelCountArray[$hotelId] < 50)continue; //忽略评价用户少于50个的酒店
	
	//正则表达式获得用户的名，然后获得userId，正则表达式同时获得打分详情
	$rateContent = file_get_contents($filename);
	preg_match_all('/<Author>(.+)\n.+\n.+\n<Rating>(.+)\n/i',$rateContent,$usersArray);
	foreach($usersArray[1] as $key=>$user)
	{
		if(isset($userCountArray[$user])) ++$userCountArray[$user];
		else $userCountArray[$user] = 1;
	}
}

asort($userCountArray);
file_put_contents('select/userCountArray',serialize($userCountArray));
saveArrayToFile($userCountArray,'select/userCount');  //观察发现，大于4个评价的用户只有1967个，取这些用户作为实验对象
//重新构造实验数据集合，按照筛选的条件重新构造。构造出来的数据集合放在select/data中，重复利用以上的函数，这样得到的数据
//就可以放入内存中了8M的数据处理起来还是比较轻松的。

//将二维数组存储在文件中
function saveArrayToFile($array,$filename)
{
	$str = '';
	foreach($array as $key=>$item)
	{
		$str .= $key."\t".$item."\n";
	}
	file_put_contents($filename,$str);
}
?>