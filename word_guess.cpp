/******************************************************************

word_guess.c
This program is an implementation of a word guessing game.

Authors: Daniel Dukeshire, Thomas Durkin, Chris Allen
Date: 10.5.2020

******************************************************************/

#include <vector>
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>

#define MAXBUFFER 512
#define ADDRBUFFER 128

using namespace std;

int main(int argc, char* argv[])
{
  if(argc != 5){
    cerr << "Incorrect number of arguments.\n";
  }

  string filename;
  int seed, port, longest_length;

  seed = atoi(argv[1]);
  port = atoi(argv[2]);
  filename = argv[3];
  longest_length = atoi(argv[4]);

  cout << seed << " " << port << " " << longest_length << " " << filename << " \n" ;




}
