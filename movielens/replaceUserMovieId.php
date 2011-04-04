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

//替换movieId，user不需要动
define('DS',DIRECTORY_SEPARATOR);

//var_dump(unserialize(file_get_contents(userIds));
//die;
$filename = "../ml1M_data/ratings.dat";
$dstFilename = "../ml1M_data/ratings_t.dat";
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