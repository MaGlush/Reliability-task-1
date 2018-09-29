#/!bin/sh


# complile
echo "Compiling..."
g++ -O2 -Wall 2018_422_glushchenko.cpp -o t -std=c++11

# run
echo "Run..."
./t 9 8 7 6 -file states.txt -count
