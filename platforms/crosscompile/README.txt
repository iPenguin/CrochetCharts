To setup cross compile for mac follow a combination of the 2 instructions sets in this directory.
run the script to automatically setup and install the needed files.

If it fails to finish building odcctools in the folder do the following:
LDFLAGS="-m32" CFLAGS="-m32" ./configure --target=i686-apple-darwin8 --prefix=/home/crosscompile/darwin --with-sysroot=/home/crosscompile/darwin/SDKs/MacOSX10.4u.sdk CPPFLAGS=-D_FORTIFY_SOURCE=0 --enable-ld64
Edit Makefile: remove otool from COMPONENTS line
make
make install

restart the script so it can pick up where it left off.
