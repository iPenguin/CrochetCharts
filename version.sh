#!/bin/bash
echo "#define GIT_VERSION" \"`git describe --tags --dirty`\" > version.h
