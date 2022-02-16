#!/usr/bin/sh

CLEAR_AFTER_BUILD=false

while getopts ":c" option; do
   case $option in
      c) # clear output
         CLEAR_AFTER_BUILD=true;;
     \?) # Invalid option
         echo "Error: Invalid option"
         exit;;
   esac
done

mkdir build
cd build

if ! cmake ..; then
    echo "cmake failed, exiting run.sh"
    exit
fi

if ! make; then
    echo "make failed, exiting run.sh"
    exit
fi

if $CLEAR_AFTER_BUILD
then
    clear
fi

./SSG
