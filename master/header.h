#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

#define READ 0
#define WRITE 1

void pipedCommand(vector<string>, vector<int>);
#endif
