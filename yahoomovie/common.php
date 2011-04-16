<?php
/*
* Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
* please maitain the copyright information completely when you redistribute the code.
* 
* Please contact me via email honglianglv@gmail.com
* my blog: http://lifecrunch.biz
* my twitter: http://twitter.com/honglianglv
* sina weibo: http://t.sina.com/lvhl
*
* It is free software; you can redistribute it and/or modify it under GPLV3.
*
* this file provides some common php functions!
*/

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