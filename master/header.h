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

/*****Function declaration*****/
void pipedCommand(vector<string>, vector<int>, bool, vector<int>);
void redirection(char**, vector<int>);






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
#endif
