#!/bin/bash
cd build
tests/crochet_tests > tests.log
cat tests.log | grep -e Start -e Totals | awk -f ../tests/tests_parse.awk

