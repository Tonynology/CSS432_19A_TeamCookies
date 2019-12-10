welcome to Sea Battle, a game by Team Cookies

run:
>make one ssh connection into uw1-320-01
curl https://transfer.sh/6Foft/Sea_Battle.tar.gz -o Sea_Battle.tar.gz
tar -xzvf Sea_Battle.tar.gz
cd Sea_Battle
./server.out
>make many ssh connections into any Linux Lab machine
curl https://transfer.sh/6Foft/Sea_Battle.tar.gz -o Sea_Battle.tar.gz
tar -xzvf Sea_Battle.tar.gz
cd Sea_Battle
./client.out

build:
>Download src from GitHub
./buildscript.sh

