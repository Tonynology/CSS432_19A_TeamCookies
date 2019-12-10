#!/bin/bash

g++ -o client.out Client.cpp -pthread -std=c++11
g++ -o server.out Server.cpp -pthread -std=c++11
g++ -o peer.out Peer.cpp -pthread -std=c++11
