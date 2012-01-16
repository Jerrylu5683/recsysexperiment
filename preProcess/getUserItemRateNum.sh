#!/bin/sh
#usage: getUserItemRateNum.sh ratingFileName separator

if [ $# -gt 0 ];
then
    filename=$1
else
    filename="../yahooMovie/clean-yahoo-user-ratings.txt"
fi
if [ $# -gt 1 ];
then
    seperator=$2
else
    seperator=" "
fi

awk -F"${seperator}" '
BEGIN {
}
{
    if (userArray[$2]) {
        userArray[$2]++;
    }
    else {
        userArray[$2] = 1;
    }
    if (itemArray[$1]) {
        itemArray[$1]++;
    }
    else {
        itemArray[$1] = 1;
    }
}
END {
    print '' >  "userIdStat.txt";
    print '' >  "itemIdStat.txt";
    for (userId in userArray) {
        print userId,userArray[userId] >> "userIdStat.txt";
        userNum++;
    }
    for (itemId in itemArray) {
        print itemId,itemArray[itemId] >> "itemIdStat.txt";
        itemNum++;
    }
    print "userNum:",userNum >  "StatInfo.txt";
    print "itemNum:",itemNum >> "StatInfo.txt";
}' $filename
sort -k2nr userIdStat.txt > userIdStat.sortbynum
sort -k1n userIdStat.txt > userIdStat.sort
sort -k2nr itemIdStat.txt > itemIdStat.sortbynum
sort -k1n itemIdStat.txt > itemIdStat.sort
