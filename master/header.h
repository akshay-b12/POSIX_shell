#ifndef HEADER_H
#define HEADER_H

/*****Includes*****/
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

/*****Constants*****/
#define READ 0
#define WRITE 1

/*****Function declaration*****/
void pipedCommand(vector<string>, vector<int>, bool, vector<int>);
void redirection(char**, vector<int>);
#endif
