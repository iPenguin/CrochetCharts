#!/bin/awk
#
# (c) 2011 Stitch Works Software
# Brian C. Milco <brian@stitchworkssoftware.com>
#
#This is an awk script designed to parse the output of QtTestLib
#test runs. It will condense the output to the following format:
#TestName: XX passed, XX failed, XX skipped
#The output will be displayed with color for easy reading.
{
    if ($1 == "*********") { 
        printf "\033[1;29m%25s: \033[0m", $5; 
    } else {
        if($2 > 0)
            pass = "\033[1;32m%2s %s\033[0m ";
        else
            pass = "%2s %s ";

        if($4 > 0)
            fail = "\033[1;31m%2s %s\033[0m ";
        else
            fail = "%2s %s ";

        if($6 > 0) 
            skipped = "\033[1;33m%2s %s\033[0m\n";
        else
            skipped = "%2s %s\n"; 

        printf pass, $2, $3; 
        printf fail, $4, $5;
        printf skipped, $6, $7;
    }    
}
