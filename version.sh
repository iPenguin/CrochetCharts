#!/bin/bash
echo "#define GIT_VERSION" \"`git describe --tags --dirty`\" > build/version.h
