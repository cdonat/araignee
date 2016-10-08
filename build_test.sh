#! /usr/bin/env sh

tail -2 $0 | head -1

g++ --std=c++17 -Iexternal/Beast/include/ -Iinclude -lboost_system -lboost_filesystem test.cpp -o test

