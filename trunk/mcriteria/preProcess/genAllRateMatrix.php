<?php
/**
   这个文件的作用是将打分的php对象存储成矩阵，方便matlib程序读取
   经过计算，用户一共有148480个
   hotel一共有1850个
*/
define('USERNUM',148480);
define('ITEMNUM',1850);

$dirBase = 'F:\dataset\tripadvisor\Texts';

//读取存储的打分矩阵
$totalMatrix = unserialize(file_get_contents('totalMatrix'));
$valueMatrix = unserialize(file_get_contents('valueMatrix'));
$roomsMatrix = unserialize(file_get_contents('roomsMatrix'));
$locationMatrix = unserialize(file_get_contents('locationMatrix'));
$cleanlinessMatrix = unserialize(file_get_contents('cleanlinessMatrix'));
$service1Matrix = unserialize(file_get_contents('service1Matrix'));
$service2Matrix = unserialize(file_get_contents('service2Matrix'));
$businessMatrix = unserialize(file_get_contents('businessMatrix'));

//先实现，最多转置一下就变了，
$totalStr = '';
$valueStr = '';
$roomsStr = '';
$locationStr = '';
$cleanlinessStr = '';
$service1Str = '';
$service2Str = '';
$businessStr = '';

$totalfp = fopen('matlib/totalStr.txt','w');
$valuefp = fopen('matlib/valueStr.txt','w');
$roomsfp = fopen('matlib/roomsStr.txt','w');
$locationfp = fopen('matlib/locationStr.txt','w');
$cleanlinessfp = fopen('matlib/cleanlinessStr.txt','w');
$service1fp = fopen('matlib/service1Str.txt','w');
$service2fp = fopen('matlib/service2Str.txt','w');
$businessfp = fopen('matlib/businessStr.txt','w');

for($i=0;$i<USERNUM;$i++)
{
	for($j=0;$j<ITEMNUM;$j++)
	{
		if(isset($totalMatrix[$i][$j]))$totalStr .= $totalMatrix[$i][$j]."\t";
		else $totalStr .= '-1'."\t";
		
		if(isset($valueMatrix[$i][$j]))$valueStr .= $valueMatrix[$i][$j]."\t";
		else $valueStr .= '-1'."\t";
		if(isset($roomsMatrix[$i][$j]))$roomsStr .= $roomsMatrix[$i][$j]."\t";
		else $roomsStr .= '-1'."\t";
		if(isset($locationMatrix[$i][$j]))$locationStr .= $locationMatrix[$i][$j]."\t";
		else $locationStr .= '-1'."\t";
		if(isset($cleanlinessMatrix[$i][$j]))$cleanlinessStr .= $cleanlinessMatrix[$i][$j]."\t";
		else $cleanlinessStr .= '-1'."\t";
		if(isset($service1Matrix[$i][$j]))$service1Str .= $service1Matrix[$i][$j]."\t";
		else $service1Str .= '-1'."\t";
		if(isset($service2Matrix[$i][$j]))$service2Str .= $service2Matrix[$i][$j]."\t";
		else $service2Str .= '-1'."\t";
		if(isset($businessMatrix[$i][$j]))$businessStr .= $businessMatrix[$i][$j]."\t";
		else $businessStr .= '-1'."\t";	
	}
	fwrite($totalfp,$totalStr."\n");
	$totalStr = "";
	fwrite($valuefp,$valueStr."\n");
	$valueStr = "";
	fwrite($roomsfp,$roomsStr."\n");
	$roomsStr = "";
	fwrite($locationfp,$locationStr."\n");
	$locationStr = "";
	fwrite($cleanlinessfp,$cleanlinessStr."\n");
	$cleanlinessStr = "";
	fwrite($service1fp,$service1Str."\n");
	$service1Str = "";
	fwrite($service2fp,$service2Str."\n");
	$service2Str = "";
	fwrite($businessfp,$businessStr."\n");
	$businessStr = "";
	
	$j = 0;
}
fclose($totalfp);
fclose($valuefp);
fclose($roomsfp );
fclose($locationfp);
fclose($cleanlinessfp);
fclose($service1fp);
fclose($service2fp);
fclose($businessfp);
?>