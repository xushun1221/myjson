#!/bin/bash

set -e

g++ test_main.cc -o alltest -std=c++14 -lpthread -lgtest

./alltest

rm -rf ./alltest