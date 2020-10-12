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
#include <arpa/inet.h>
#include <strings.h>
using namespace std;
#define MAXBUFFER 3000

vector<char*> client_names;
vector<int> client_sockets;
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
  // Variable Declaration ----------------------------------------------------------
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

  // Reading in the file into a vector of strings ----------------------------------
  vector<string> dictionary;                                                      // A vector of strings to represent the dictionary
  string temp_word;                                                               // A temp word to add each iteration of the stream
  while(filename>>temp_word) dictionary.push_back(temp_word);                     // Reading the file into the vector of strings
  string random_word = getRandomWord(seed, dictionary, longest_length);           // With the given word and length, finds a word of given length


  // Setting up the clients vector, as the maximum number of clients is 5 ----------
  for( int i = 0; i < 5; i++)                                                     // Initializing the client_names array
  {                                                                               // Initializing the client_sockets array
    char * temp = (char*)calloc(2, sizeof(char));
    temp = (char*)"";
    client_names.push_back(temp);
    client_sockets.push_back(-1);
  }

  // Some variable declaration -----------------------------------------------------
  struct sockaddr_in server;                                                      // Declaring server struct
  struct sockaddr_in client;                                                      // Declaring client struct
  server.sin_family = AF_INET;                                                    // AF_INET ???
  server.sin_addr.s_addr = htonl(INADDR_ANY);                                     // Allows any IP address to connect
  server.sin_port = htons(port);                                                  // Data Marshalling
  int server_len = sizeof(server);                                                // The size of the server
  int client_len = sizeof(client);                                                // The size of the client
  //char buffer[MAXBUFFER];                                                         // Buffer to be read in by UDP
  fd_set fd_table;                                                                // The file descriptor table
  current_clients = 0;                                                            // The current number of clients is 0
  int opt = 1;

  // Setting up the TCP Connection -------------------------------------------------
  int fd_tcp = socket( AF_INET, SOCK_STREAM, 0 );
  if(fd_tcp == -1)                                                                // Creating Socket for the TCP
  {
    perror( "ERROR TCP socket() failed" );
    return EXIT_FAILURE;
  }
  if(setsockopt(fd_tcp, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))< 0)  // Allow multiple connections on the socket
  {
      perror("setsockopt");
      return EXIT_FAILURE;
  }
  if(bind(fd_tcp, (struct sockaddr *)&server, server_len)<0)                    // Attempting to bind
  {
    perror( "ERROR TCP bind() failed" );
    return EXIT_FAILURE;
  }
  if(listen(fd_tcp, 5) < 0 )                                                     // Attempting to listen
  {
    perror( "ERROR TCP listen() failed" );
    return EXIT_FAILURE;
  }
  else printf( "MAIN: Listening for TCP connections on port: %d\n", port);        // Printing to console

  // BEGINNING THE APPLICATION -----------------------------------------------------
  while(1)
  {
    FD_ZERO(&fd_table);                                                           // Zeroing out the socket set
    FD_SET(fd_tcp, &fd_table);                                                    // Setting the tcp socket
    int fd_max = fd_tcp;                                                          // Finding the max
    for(int i=0; i<5; i+=1)                                                       // Loop over all of the clients
    {
      int fd_temp = client_sockets[i];
      if(fd_temp > -1) FD_SET(fd_temp,  &fd_table);                               // If in use, add to table
      if(fd_temp > fd_max) fd_max = fd_temp;                                      // Reset the max if this is larger
    }

    // Calling the select(), determining which fd value to read
    int select_check= select(fd_max+1 , &fd_table , NULL , NULL , NULL);
    if (select_check == 0) { perror("ERROR No connection\n"); continue; }

    if(FD_ISSET(fd_tcp, &fd_table))                                               // On new client connection
    {
        int fd_new = accept(fd_tcp, (struct sockaddr *)& client, (socklen_t *)& client_len);
        if(fd_new == -1)                                                          // Error Check for the accept call
        {
          perror("ERROR: accept() failed\n");
          return EXIT_FAILURE;
        }
        if(current_clients == 5)                                                  // Check to see if there is room to add the next client
        {
          send(fd_new, "ERROR: server is full\n", 21, 0);
          printf("MAIN: SEND ERROR (server is full)\n");
          continue;
        }
        else printf("MAIN: Successful client connected: %s\n", inet_ntoa(client.sin_addr));
        char name[200];                                                           // The name to be recieved from client
        char* user_ask = (char*)"Welcome to Guess the Word, please enter your username.\n";

        // Getting the name from the client, therefore we loop until a valid name is inputted
        if( send(fd_new, user_ask, strlen(user_ask), 0) != strlen(user_ask) ) perror("ERROR on send()\n");
        while(1)
        {
          int flag = 0;
          int n = recv(fd_new, name, 200, 0);
          if(n < 0) { perror("ERROR on name length\n"); continue; }
          else                                                                    // If name is recieved, check if it is in use
          {
            name[n-1] = '\0';
            for(int i=0; i<5; i+=1)                                               // Looping over the client names
            {
              char* temp_name = client_names[i];
              int cmp = strcasecmp(name, temp_name);                              // Compares two case -insensitive strings
              if(cmp == 0) { flag = 1; break; }                                   // If there is a name that is the same ...
            }
            if(flag == 1)                                                         // If username is in use ...
            {
              char* in_use = (char*)calloc(200, sizeof(char));                    // We have to send a message to the client
              strcpy(in_use, "Username ");
              strcat(in_use, name);
              strcat(in_use, " is already taken, please enter a different username.\n");
              name[0] = '\0';
              send(fd_new, in_use, strlen(in_use), 0);                            // Send message, wait for new username
              free(in_use);
              continue;
            }
            else break;                                                           // Otherwise, the name is valid
          }
        }
        // Have valid name, add to vector ----------------------------------------
        char * start = (char*)calloc(200, sizeof(char*));
        strcpy(start, "Let's start playing, ");
        strcat(start, name);
        strcat(start, "\n");
        send(fd_new, start, strlen(start), 0);                                    // Send message, wait for new username

        int i;
        for(i=0; i<5; i+=1)                                                       // Finding a location for the client in the vector
        {
          if(strcmp(client_names[i],"") == 0 && client_sockets[i] == -1) break;   // Spot available in the list...
        }
        current_clients +=1;                                                      // We just added a client, increment accordingly
        client_names[i] = (char*)calloc(strlen(name), sizeof(char));
        strcpy(client_names[i], name);
        client_sockets[i] = fd_new;                                               // Set the socket accordingly
        char current_c_char = '0' + current_clients;                              // int to char* conversion
        const char *c = &current_c_char;
        char rando_length[] = {'0' + random_word.length(), '\0'};                 // int to char* conversion, accounts for >1 digits

        char* current_players = (char*)calloc(200, sizeof(char));                 // Composing the string
        strcpy(current_players, "There are ");
        strcat(current_players, c);
        strcat(current_players, " player(s) playing. The secret word is ");
        strcat(current_players, rando_length);
        strcat(current_players, " letter(s)\n");
        send(fd_new, current_players, strlen(current_players), 0);                // Send message, wait for new username
    }

  }


  return(EXIT_SUCCESS);
}
