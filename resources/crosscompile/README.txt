To setup cross compile for mac follow a combination of the 2 instructions sets in this directory.
run the script to automatically setup and install the needed files.

If it fails to finish building odcctools in the folder do the following:
LDFLAGS="-m32" CFLAGS="-m32" ./configure --target=i686-apple-darwin8 --prefix=/home/crosscompile/darwin --with-sysroot=/home/crosscompile/darwin/SDKs/MacOSX10.4u.sdk CPPFLAGS=-D_FORTIFY_SOURCE=0 --enable-ld64
Edit Makefile: remove otool from COMPONENTS line
make
make install

restart the script so it can pick up where it left off.

------

    #TODO: use /home/crosscompile/darwin/bin/i686-apple-darwin8-lipo to join the different archs for the mac into 1 file.
    #command: i686-apple-darwin8-lipo -create a.out.i386 a.out.x86_64 a.out.ppc a.out.ppc64 -output a.out
    #Q: would you need to do the same thing for the libraries?
    # execute_process(. COMMAND i686-apple-darwin8-lipo -create a.out.i386 a.out.ppc -output a.out) and add_to_distclean(a.out)