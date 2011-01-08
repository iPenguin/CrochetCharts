#!/bin/bash
cd build
tests/tests > tests.log
cat tests.log | grep -e Start -e Totals | awk -f ../tests/tests_parse.awk

