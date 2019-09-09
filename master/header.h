#ifndef HEADER_H
#define HEADER_H

/*****Includes*****/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstdlib>
#include <stdlib.h>
#include <cstdio>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>

using namespace std;

/*****Constants*****/
#define READ 0
#define WRITE 1
#define MAX_BUF_LEN 255

/*****Function declaration*****/
void pipedCommand(vector<string>, vector<int>, bool *, vector<int> &);
void redirection(char**, vector<int> &);






////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int ALPHABET_SIZE = 128;//26; 

struct trieNode 
{ 
    struct trieNode *children[ALPHABET_SIZE]; 
    bool isEndOfWord; 
};

struct trieNode *getNode() 
{ 
    struct trieNode *pNode =  new trieNode; 
  
    pNode->isEndOfWord = false; 
  
    for (int i = 0; i < ALPHABET_SIZE; i++) 
        pNode->children[i] = NULL; 
  
    return pNode; 
} 

void insert(struct trieNode *root, string key) 
{ 
    struct trieNode *pWalk = root; 
  
    for (int i = 0; i < key.length(); i++) 
    { 
        int index = key[i];// - 'a'; 
        if (!pWalk->children[index]) 
            pWalk->children[index] = getNode(); 
  
        pWalk = pWalk->children[index]; 
    } 

    pWalk->isEndOfWord = true; 
}

bool isLastNode(struct trieNode* root) 
{ 
    for (int i = 0; i < ALPHABET_SIZE; i++) 
        if (root->children[i]) 
            return 0; 
    return 1; 
} 
void suggestionsRec(struct trieNode* root, string currPrefix) 
{ 
    if (root->isEndOfWord) 
    { 
        cout << currPrefix; 
        cout << "\t"; 
    } 
  
    if (isLastNode(root))
    {
        //cout << currPrefix; 
        //cout << "\t";
        return; 
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) 
    { 
        if (root->children[i]) 
        { 
            currPrefix.push_back(i); 
            suggestionsRec(root->children[i], currPrefix);
            currPrefix.pop_back();
        } 
    } 
} 

int printAutoSuggestions(trieNode*** root, const string query, int numOfTrees) 
{
	for(int i=0; i<numOfTrees; ++i)
	{
    	struct trieNode* ptrTrie = (*root)[i]; 
		bool flag = true;    
    	int len = query.length(); 
    	for (int depth = 0; depth < len; depth++) 
    	{ 
    	    int index = query[depth]; 
	
    	    if (!(ptrTrie->children[index])) 
    	    {
				flag = false;
				break;     // if not present in current tree
			} 
	
    	    ptrTrie = ptrTrie->children[index]; 
    	} 
		if(!flag)
			continue;
    	bool isWord = (ptrTrie->isEndOfWord == true); 
	
    	bool isLast = isLastNode(ptrTrie); 
	
    	if (isWord && isLast) 
    	{ 
    	    cout << query << endl; 
    	    //return -1; 
    	} 
	
    	if (!isLast) 
    	{ 
    	    string prefix = query; 
    	    suggestionsRec(ptrTrie, prefix); 
    	    //return 1; 
    	}
	}
    return 1;
}


void trie_init(trieNode ***root)
{
	char *path = getenv("PATH");
	int numOfTrees;
	//puts(path);
	//createTrie(path, &root, numOfTrees);

	vector<string> pathValue;
	int ii=0;
	int len = strlen(path);
	while(ii < len)
	{
		string str;
		while((path[ii] != ':') && (path[ii] != '\0'))
		{
			str+=path[ii];
			ii++;
		}
		ii++;
		pathValue.push_back(str);
	}
	numOfTrees = pathValue.size();
	*root = new trieNode* [pathValue.size()];
	//cout<<&root<<endl;
	for(int j=0; j< pathValue.size(); ++j)
	{
		char *dirPath = new char [pathValue[j].length()+1];
		strcpy(dirPath,pathValue[j].c_str());
		struct stat statbuf;
		(*root)[j] = getNode();
		//cout<<root[j]<<endl;
		if (stat(dirPath, &statbuf) != 0)
		{
			cerr<<"Error in retrieving file/directory info for trie."<<endl;
			exit(-1);
		}
	
		DIR *argdir = opendir(dirPath); // pointer to the directory

		struct dirent *entries = NULL;
		
		if(argdir != NULL)
		{
			while ((entries = readdir(argdir)) != NULL)
			{
				string tmpstr(entries->d_name);
				//cout<<entries->d_name<<endl;
				if((strcmp(entries->d_name, ".") == 0) || (strcmp(entries->d_name, "..") == 0))
					continue;
				insert((*root)[j], tmpstr);
			}
		}
		delete[] dirPath;
	}
}


void termios_init(FILE **fp, struct termios *ots)
{
    int fd;
    struct termios ts; //, ots;
    
    if ((*fp = fopen(/*ctermid(NULL)*/"/dev/pts/3", "r+")) == NULL)
    {
        cerr<<"Cannot open terminal file... Exiting"<<ctermid(NULL);
        exit(-1);
    }
    setbuf(*fp, NULL);

    tcgetattr(fileno(*fp), &ts);
    *ots = ts;
    ts.c_lflag &= ~ICANON | ECHO | ECHOE | ECHOCTL;// | VWERASE | VERASE | VEOF;
    tcsetattr(fileno(*fp), TCSAFLUSH, &ts);
}

void env_init(unordered_map<string, string> &rc_map)
{
	ifstream fin("myrc.txt", std::ifstream::in);
	string line;
	while (std::getline(fin, line))
	{
		string substr1, substr2;
		size_t pos = 0;
    	pos = line.find("=");
    	substr1 = line.substr(0, pos);
		substr2 = line.substr(pos+1, line.length()- pos - 1);
		rc_map.insert({substr1, substr2});
		char *var = new char [line.size()+1];
		strcpy(var,line.c_str());
		putenv(var);
	}
	fin.close();
}

void alias(unordered_map<string, string> &alias_map, vector<string> ip_args)
{
	if((ip_args[0].compare("alias") == 0))
	{
		string substr1, substr2;
		size_t pos = 0;
    	pos = ip_args[1].find("=");
    	substr1 = ip_args[1].substr(0, pos);
		substr2 = ip_args[1].substr(pos+2, ip_args[1].size()-pos-2);  // skip double-quotes
		alias_map.insert({substr1, substr2});
	}
	else
	{
		cerr<<"alias command error.\n";
	}
	
}

void changedir(vector<string> ip_args, unordered_map<string, string> &rc_map)
{
	if((ip_args[0].compare("cd") == 0))
	{
		char *var = new char [ip_args[1].size()+1];
		strcpy(var, ip_args[1].c_str());
		if((ip_args[1].compare("~") == 0))
		{
			//auto itr = rc_map.find("HOME");
			char *home = getenv("HOME");//new char [itr->second.size()+1];
			//strcpy(home, itr->second.c_str());
			string str1("PWD=");
			str1+=home;
			char *ptr = new char [str1.length()+1];
			strcpy(ptr, str1.c_str());
			putenv(ptr);
			string str2(home);
			rc_map["PWD"] = str2;
		}
		else
		{
			chdir(var);
			char cwd[255];
			getcwd(cwd, 255);
			string str1("PWD=");
			str1+=cwd;
			char *ptr = new char [str1.length()+1];
			strcpy(ptr, str1.c_str());
			putenv(ptr);
			string str2(cwd);
			rc_map["PWD"] = str2;
		}
	}
	else
		cerr<<"cd command error.\n";
}

void open_comm(vector<string> ip_args, unordered_map<string, string> &rc_map)
{
	char **args = new char* [ip_args.size()+1];
	args[1] = new char [100];
	size_t pos = ip_args[1].find('.');
	pid_t pid;
	int status;
	if(pos != string::npos)
	{
		string tmpstr(ip_args[1], pos+1);
		auto itr = rc_map.find(tmpstr);
		args[1] = new char [ip_args[1].length()+1];
		strcpy(args[1],ip_args[1].c_str());
		args[0] = new char [itr->second.size()+1];
		strcpy(args[0],itr->second.c_str());
		args[2] = NULL;
		if((pid = fork()) < 0)
		{
			cerr<<"fork() error...exiting.\n";
			exit(-1);
		}
		else if(pid == 0)
		{
			execvp(args[0], args);
			cerr<<"Command "<<args[0]<<" couldn't be executed!\n";
		}
		if((pid = waitpid(pid, &status, 0) < 0))
		{
			cerr<<"waitpid() error...exiting.\n";
			exit(-1);
		}
	}
	else
		cerr<<"File extension missing\n";
	
}
#endif