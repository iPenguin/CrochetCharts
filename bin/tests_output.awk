#!/bin/awk
#
# (c) 2011 Stitch Works Software
# Brian C. Milco <brian@stitchworkssoftware.com>
#
#This awk script is designed to parse the output of QtTestLib
#test runs. It will condense the output and show only the important
#lines. It will color-ize failed, and skipped tests as well as any
#misc output.
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
}
