#!/bin/bash

#welcome to Sea Battle, a game by Team Cookies

# upload:
tar -czvf src.tar.gz src && url=$(curl --upload-file ./src.tar.gz https://transfer.sh/src.tar.gz) && rm src.tar.gz && echo $url

# download:
echo $url && curl $url -o src.tar.gz && tar -xzvf src.tar.gz && rm src.tar.gz && cd src

# compile:
./buildscript.sh

# run:
./server.out
./client.out
./peer.out
