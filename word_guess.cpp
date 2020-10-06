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
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;

vector<string> client_names;
vector<int> client_sockets;
int number_clients = 0;


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
    if((int)word.size() <= longest_length) break;
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

  for( int i = 0; i < 5; i++)
  {
    client_names.push_back("");
    client_sockets.push_back(-1);
  }

  int socket_fd, socket_option = 1;

  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0)
  {
    printf("ERROR: create socket failed.\n");
    return EXIT_FAILURE;
  }

  if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&socket_option, sizeof(socket_option)) < 0 )
  {
      perror("ERROR: setsockopt failed.\n");
      return EXIT_FAILURE;
  }

}
