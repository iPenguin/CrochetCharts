execute_process(COMMAND git describe --tags --dirty
    OUTPUT_VARIABLE GIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
file(WRITE custom.h "#define COMPILE_TIME_VALUE \"${GIT_VERSION}\"")
