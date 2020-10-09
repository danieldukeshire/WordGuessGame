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
#include <sys/select.h>
using namespace std;

vector<string> client_names;
vector<int> client_sockets;
int number_clients = 0;
int current_clients;


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

findMaxSd()
With the given fd_table, determines the sockets in use, as well
as the largest socket in use

******************************************************************/
int findMaxSd(fd_set& fd_table)
{
  int max_fd = -1;                                                                // The variable to store the maxium value
  for(int i=0; i<5; i++)                                                          // Looping across the client_sockets vector
  {
    int temp = client_sockets[i];                                                 // Adds each socket in use
    if(temp!=-1) FD_SET(temp, &fd_table);
    if(temp>max_fd) max_fd = temp;                                                // Sets the max_fd if necessary
  }
  return max_fd;
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
  while(filename>>temp_word) dictionary.push_back(temp_word);                     // Reading the file into the vector of strings

  string random_word = getRandomWord(seed, dictionary, longest_length);           // With the given word and length, finds a word of given length

  for( int i = 0; i < 5; i++)                                                     // Initializing the client_names array
  {                                                                               // Initializing the client_sockets array
    client_names.push_back("");
    client_sockets.push_back(-1);
  }

  int socket_fd, socket_option = 1;                                               // Setting up the socket

  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0)
  {
    cerr << "ERROR: create socket failed.\n";
    return EXIT_FAILURE;
  }

  if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&socket_option, sizeof(socket_option)) < 0 )
  {
      cerr << "ERROR: setsockopt failed.\n";
      return EXIT_FAILURE;
  }

  struct sockaddr_in server;                                                      // Setting up the server struct
  int server_len = sizeof(server);
  server.sin_family = AF_INET;                                                    // Setting the sin_family value
  server.sin_addr.s_addr = INADDR_ANY;                                            // Allows the connection of any IP address
  server.sin_port = htons(0);                                                     //

  int bind_val = bind(socket_fd, (struct sockaddr *)&server, server_len);         // Binding the server to the socket
  if(bind_val < 0)                                                                // Checking to see if the bind was successful
  {
    cerr << "ERROR: create socket failed.\n" ;
    return EXIT_FAILURE;
  }

  // Now, we set the socket to actively listen
  int is_listen = listen(socket_fd, 5);
  if(is_listen < 0)                                                               // Error checking on listen
  {
    cerr << "ERROR: listen failed\n";
  }

  fd_set fd_table;                                                                // The set of socket descriptors
  int fd_new;
  current_clients = 0;                                                            // The current number of clients in the server

  cout<<"SERVER STARTED\n";

  // ------------------------------------------ Beginning the application ------------------------------------------
  while(1)
  {
    FD_ZERO(&fd_table);                                                           // Zeros out the socket set
    FD_SET(socket_fd, &fd_table);                                                 // Assigning the master socket to the table

    int max_fd = findMaxSd(fd_table);
    int is_select = select(max_fd+1, &fd_table, NULL, NULL, NULL);                // Wait indefinitely for an activity on one of the sockets

    if(is_select == -1)
    {
      cerr << "ERROR: NO connection\n";                                           // An error check for select
      return EXIT_FAILURE;
    }

    if(FD_ISSET(socket_fd, &fd_table))                                            // If we accept client ...
    {
      fd_new = accept(socket_fd, (struct sockaddr*)& server, (socklen_t *)& server_len);
      if(fd_new == -1)                                                            // Now, check to see if the accept() call passes
      {
        cerr << "ERROR: accepr() failed\n";
        return EXIT_FAILURE;
      }
      else if(current_clients == 5)                                               // Otherwise, check to see if there is room for more clients
      {
        cout << "ERROR: Maximum number of clients meeted\n";                      // Print to the console
        continue;
      }

      cout << "successful client connected\n";
    }

  }

  return(EXIT_SUCCESS);
}
