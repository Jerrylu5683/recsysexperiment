<?php
/*
 * Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * Please contact me via email honglianglv@gmail.com
 * my blog: http://lifecrunch.biz
 * my twitter: http://twitter.com/honglianglv
 *
 * It is free software; you can redistribute it and/or modify it under GPLV3.
 * 
 * This file stat how many item in the rating each num.
 */
include "./common.php";
 
$filename = "./tmpData/userNum_netflix";

$nums = array();
$rows = explode("\n",file_get_contents($filename));

foreach($rows as $key=>$row) {
    if(strlen($row) < 2)continue;
    $detail = explode("\t",trim($row));
    if(count($detail) < 2)continue;
    if(!isset($nums[$detail[1]])) {
        $nums[$detail[1]] = 1;
    }
    else ++$nums[$detail[1]]; 
    //if($key == 10){var_dump($nums);die}
}
ksort($nums);

saveArrayToFile($nums,$filename.".stat");
?>