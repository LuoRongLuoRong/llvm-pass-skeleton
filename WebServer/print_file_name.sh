!/bin/bash
cpp_file='WebServer/*.cpp'
for file in ${cpp_file}
do
    printf ${file}
    printf " "
done

h_file='WebServer/base/*.cpp'
for file in ${h_file}
do
    printf ${file}
    printf " "
done
