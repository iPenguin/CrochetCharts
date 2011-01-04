#!/bin/bash
echo "#include \"../src/version.h\"" > build/version.cpp
echo "#define GIT_VERSION" \"`git describe --tags --dirty=-working`\" >> build/version.cpp
echo "const QString gGIT_VERSION = GIT_VERSION;" >> build/version.cpp
