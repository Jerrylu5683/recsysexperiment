<?php
define('DS',DIRECTORY_SEPARATOR);
$dirBase = '../tripDataset';
$newDirBase = '../tripDataset_t';
$hotelIds = unserialize(file_get_contents('hotelIdArrays'));
$userIds  = unserialize(file_get_contents('userIdArrays'));

//针对每一个文件进行替换,然后重新组成一个新的文件
foreach( glob($dirBase.'/*.txt') as $filename)
{
	//针对每一个文件，提取出酒店的id
	$posStart = strpos($filename,'_');
	$posEnd = strpos($filename,'_',$posStart+1);
	$hotelId = substr($filename,$posStart+1,$posEnd - $posStart-1);
	$hotelNumId = $hotelIds[$hotelId];
	
	//下面给每一个用户编号，先读取文件内容，然后构造新的文件，暂时忽略时间因素影响
	$newFileContent = '';
	$rateContent = file_get_contents($filename);
	preg_match_all('/<Author>(.+)\n.+\n.+\n<Rating>(.+)\n/i',$rateContent,$usersArray);
	foreach($usersArray[1] as $key=>$user)
	{
		if('A TripAdvisor Member' == $user)continue;
		$userNumId = $userIds[$user];
		//var_dump($user,$userNumId);
		$newFileContent .= $userNumId.':'.$usersArray[2][$key]."\n";
	}
	//echo $newFileContent;die;
	file_put_contents($newDirBase.DS.$hotelNumId, $newFileContent);
}
?>