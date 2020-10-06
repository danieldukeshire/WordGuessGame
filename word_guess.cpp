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
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
using namespace std;


/******************************************************************

getRandomWord()
With the given seed, dictionary of words, and longest word length,
finds a random word from the dictionary that is of max sixe
longest_length.

******************************************************************/
string getRandomWord(int seed, vector<string>dictionary, int longest_length)
{
  srand(seed);                                                                    // Using the seed value for srand()
  string word;

  while(true)
  {
    int random_index = rand()%dictionary.size();                                  // Determines a random index for the dictionary
    word = dictionary[random_index];
    if(word.size() <= longest_length) break;
  }

  return word;
}

/******************************************************************

main()
Takes command line input:
< seed > < port > < dictionary_file > < longest_word_length >

******************************************************************/

int main(int argc, char* argv[])
{
  if(argc != 5)                                                                   // Error Checking for number of arguments
  {
    cerr << "Incorrect number of arguments, Usage: < seed > < port > < dictionary_file > < longest_word_length >\n";
    return EXIT_FAILURE;
  }
  // Variable Declaration
  int seed, port, longest_length;
  if(!(seed=atoi(argv[1])))                                                       // Error Checking for the input types
  {
    cerr << "Incorrect seed type, Usage: Int.\n";
    return EXIT_FAILURE;
  }
  if(!(port = atoi(argv[2])))                                                     // Error Checking for the input types
  {
    cerr << "Incorrect seed type, Usage: Int.\n";
    return EXIT_FAILURE;
  }

  ifstream filename(argv[3]);                                                     // The filename inputted by the command line
  longest_length = atoi(argv[4]);                                                 // The longest length inputted by the command line

  vector<string> dictionary;                                                      // A vector of strings to represent the dictionary
  string temp_word;                                                               // A temp word to add each iteration of the stream
  while(filename>>temp_word) dictionary.push_back(temp_word);                                              // Reading the file into the vector of strings

  string random_word = getRandomWord(seed, dictionary, longest_length);           // With the given word and length, finds a word of given length




}
