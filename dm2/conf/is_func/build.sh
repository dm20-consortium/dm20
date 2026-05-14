#!/bin/bash
build_dir_list=`ls -l | grep ^d | awk '{print $9}'`

for build_dir in ${build_dir_list}; do
    cd ${build_dir}
    if [ -f "makefile" ]; then
        echo "****** `pwd`内のmakefileを使ってビルドします ******"
        make
    fi
    cd ..
done
