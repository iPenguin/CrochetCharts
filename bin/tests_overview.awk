#!/bin/awk
#
# (c) 2011 Stitch Works Software
# Brian C. Milco <brian@stitchworkssoftware.com>
#
#This is an awk script designed to parse the output of QtTestLib
#test runs. It will condense the output to the following format:
#TestName: XX passed, XX failed, XX skipped
#The output will be displayed with color for easy reading.
BEGIN {
    padTest = 50;
    print "+-------------------------------+--------------------------+";
    print "|                               | \033[1;32mPassed\033[0m  \033[1;31mFailed\033[0m  \033[1;33mSkipped\033[0m  |";
    print "+-------------------------------+--------------------------+";
}
{
    passFill = "%7s";
    failFill = "%8s";
    skipFill = "%9s";

    if ($1 == "*********") { 
        printf "|\033[1;29m%30s\033[0m |", $5;
    } else {
        if($2 > 0)
            pass = "\033[1;32m" passFill "\033[0m";
        else
            pass = passFill;

        if($4 > 0)
            fail = "\033[1;31m" failFill "\033[0m";
        else
            fail = failFill;

        if($6 > 0) 
            skip = "\033[1;33m" skipFill "\033[0m  |\n";
        else
            skip= skipFill "  |\n";

        printf pass, $2; 
        printf fail, $4;
        printf skip, $6;
        passCnt += $2;
        failCnt += $4;
        skipCnt += $6;
    }    
}
END {
    if(passCnt > 0)
        pass = "\033[1;32m" passFill "\033[0m";
    else
        pass = passFill;

    if(failCnt > 0)
        fail = "\033[1;31m" failFill "\033[0m";
    else
        fail = failFill;

    if(skipCnt > 0)
        skip = "\033[1;33m" skipFill "\033[0m  |\n";
    else
        skip = skipFill "  |\n";
    totalsStr = "TOTALS";

    print "+-------------------------------+--------------------------+";
    printf "|\033[1;29m%30s\033[0m |", totalsStr;
    printf pass, passCnt;
    printf fail, failCnt;
    printf skip, skipCnt;
    print "+-------------------------------+--------------------------+";
}
