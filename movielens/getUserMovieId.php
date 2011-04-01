<?php
//userid 不需要转换，只转换itemId
//var_dump(unserialize(file_get_contents(movieIds)));
//var_dump(unserialize(file_get_contents(userIds));
//die;
$filename = "../ml1M_data/ratings.dat";

//第一步是不是应该给所有的酒店和所有的用户编上号？恩，是应该这样。
//给所有酒店编号，给所有的用户编号，编号的规则存入两个文件 movieIds 和 userIds
//(1)给酒店编号
$movieIds = array();
$userIds = array();

$itemNum = 1;
//下面给每一个用户编号，先读取文件内容，然后用正则表达式匹配
$rateContent = file_get_contents($filename);
$rateArray = explode("\n",$rateContent);
foreach($rateArray as $rate)
{
	$rateDetail = explode("::",$rate);
	if(count($rateDetail) < 3) continue;
	$user = $rateDetail[0];
	$item = $rateDetail[1];
	if(!isset($movieIds[$item]))
	{
		$movieIds[$item] = $itemNum; //给用户编号
		++$itemNum;
	}
}

$hotelIdStr = '';
foreach($movieIds as $key=>$movieId) {  //将hotelId储存成c容易读取的形式
	$hotelIdStr .= $key."\t".($movieId)."\n";
}
//$hotelIdStr = serialize($movieIds);
//$userIdStr  = serialize($userIds);
file_put_contents('movieIds',$hotelIdStr);
file_put_contents('movieIdArrays',serialize($movieIds));
?>