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
OFMT="%6s";
    if ($1 == "FAIL!") { 
        printf "\033[1;31mFAIL\033[0m", $1;
        tail=$0; for (i=1;i<2;i++) sub($i,"",tail); sub(/ */,"",tail)
        print tail;
    } else if ($1 == "SKIP") {
        printf "\033[1;33m%s\033[0m", $1;
        tail=$0; for (i=1;i<2;i++) sub($i,"",tail); sub(/ */,"",tail)
        print tail;
    } else if ($1 != "PASS" && $1 != "*********" && $1 != "Config:" && $1 != "Totals:") {
        printf "     \033[1;29m%s\033[0m", $1;
        tail=$0; for (i=1;i<2;i++) sub($i,"",tail); sub(/ */,"",tail)
        print tail;
    }
#            pass = "\033[1;32m%2s %s\033[0m ";
#            fail = "\033[1;31m%2s %s\033[0m ";
#            skipped = "\033[1;33m%2s %s\033[0m\n";
}
