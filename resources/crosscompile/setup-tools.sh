#!/bin/bash
#http://www.gossamer-threads.com/lists/mythtv/dev/473623

# Select an installation folder 
export PREFIX=/home/crosscompile/darwin
sudo mkdir -p $PREFIX 
sudo chown $USER:$USER $PREFIX 
# Create a build folder 
mkdir -p ~/xtools && cd ~/xtools 
# This build script automates the rest 
wget http://www.softsystem.co.uk/download/mythtv/mkodcctools 
chmod +x mkodcctools 
./mkodcctools 
# Add the cross tools to your path: 
PATH=$PATH:$PREFIX/bin 
