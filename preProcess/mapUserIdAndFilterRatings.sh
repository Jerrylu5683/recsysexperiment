#!/bin/sh

#this script map the old userId to new userId to keep the sequential character of the userIds
#and at the same time select
rm "../yahooMovie/selectRatings.txt"
awk '
NR==FNR{
    { #first use the dict file to get the select User dict
        selectUser[$1] = $3;
    }
}
NR>FNR{
    { #select and map the ratings
        if (selectUser[$2]) { #if is select
            print $1, selectUser[$2], $3, $4, $5, $6, $7
#            print $1,$2,$3,$4,$5,$6,$7 >> "../yahooMovie/selectRatings.txt"
            print $0 >> "../yahooMovie/selectRatings.txt"


        }
    }
}
END {
    for (user in selectUser) {
#    print selectUser[user];
    }
}
' '../yahooMovie/userId_old_new.map' '../yahooMovie/clean-yahoo-user-ratings.txt' > "../yahooMovie/selectRatings.maped"
