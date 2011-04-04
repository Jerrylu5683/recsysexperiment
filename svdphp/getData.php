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
$length = 9;
$result = array();
$result[0] = 0.845;//$int_array[0];
$speed = 0.07;
for($i = 1;$i<$length;$i++)
{
	$result[$i] = $result[$i-1] - getIncrement($speed/sqrt($i));
	$result[$i] = number_format($result[$i],3);
	echo getIncrement($speed/$i)."\r\n";
}
foreach ($result as $item)
{
	echo $item.",";
}

function getIncrement($base)
{
	
	$rand = rand(0.05,-0.05);
	return $base+$rand;
}