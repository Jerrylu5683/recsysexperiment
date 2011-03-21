<?php
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