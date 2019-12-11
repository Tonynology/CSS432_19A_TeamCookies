welcome to Sea Battle, a game by Team Cookies

upload:
tar -czvf src.tar.gz src
curl --upload-file ./src.tar.gz https://transfer.sh/src.tar.gz

download:
curl htts://transfer.sh/ThEXh/src.tar.gz -o src.tar.gz
tar -xzvf src.tar.gz
cd src

compile:
./buildscript.sh

run:
./server.out
./client.out
./peer.out
