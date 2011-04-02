<?php

//替换movieId，user不需要动
define('DS',DIRECTORY_SEPARATOR);

//var_dump(unserialize(file_get_contents(userIds));
//die;
$filename = "../ml10M_data/ratings.dat";
$dstFilename = "../ml10M_data/ratings_t.dat";
$movieIds = unserialize(file_get_contents("movieIdArrays"));

//下面给每一个用户编号，先读取文件内容，然后用正则表达式匹配
$rateContent = file_get_contents($filename);
$rateArray = explode("\n",$rateContent);
$ret = '';
foreach($rateArray as $rate)
{
	$rateDetail = explode("::",$rate);
	if(count($rateDetail) < 3) continue;
	$user = $rateDetail[0];
	$item = $movieIds[$rateDetail[1]];
	$rate = $rateDetail[2];
	$timestamp = $rateDetail[3];
	$ret .= $user."::".$item."::".$rate."::".$timestamp."\n";
}
file_put_contents($dstFilename,$ret);
?>