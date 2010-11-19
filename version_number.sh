#!/bin/bash
echo "#define GIT_VERSION" \"`git describe`\" > VERSION
