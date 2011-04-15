<?php
/**将选择好的hotel和user的打分数据存储到“select/data”文件夹中，这样
 * 大部分代码都不用改，只用该一个dirBase变量即可
*/

//首先从文件中读取存储的hotelCountArray 和 userCountArray
$dirBase = 'F:\dataset\tripadvisor\Texts';

$hotelCountArray = unserialize(file_get_contents('select/hotelCountArray'));
$userCountArray = unserialize(file_get_contents('select/userCountArray'));

//遍历每一个文件，将符合条件的文件内容重新组合存储到“select/data”文件夹中去

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
	$newContent = '';
	foreach($usersArray[1] as $key=>$user)
	{
		if($userCountArray[$user] < 4)continue;
		else 
		{
			$newContent .= $usersArray[0][$key]."\n";
		}
	}
	$newfilename = str_replace('Texts','select\data',$filename);
	file_put_contents($newfilename,$newContent);
}
echo "done\n";
?> 