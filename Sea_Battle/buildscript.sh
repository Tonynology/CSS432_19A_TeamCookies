#!/bin/bash

g++ -o client.out Client.cpp -pthread -std=c++11
g++ -o server.out Server.cpp -pthread -std=c++11
g++ -o player.out Player.cpp -pthread -std=c++11
