#include "header.h"

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
        cout << endl; 
    } 
  
    if (isLastNode(root)) 
        return; 
  
    for (int i = 0; i < ALPHABET_SIZE; i++) 
    { 
        if (root->children[i]) 
        { 
            currPrefix.push_back(i); 
            suggestionsRec(root->children[i], currPrefix); 
        } 
    } 
} 

int printAutoSuggestions(trieNode** root, const string query, int numOfTrees) 
{
	for(int i=0; i<numOfTrees; ++i)
	{
    	struct trieNode* ptrTrie = root[i]; 
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
    	    return -1; 
    	} 
	
    	if (!isLast) 
    	{ 
    	    string prefix = query; 
    	    suggestionsRec(ptrTrie, prefix); 
    	    return 1; 
    	}
	}
}

int main()
{
	trieNode **root = NULL;
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
	root = new trieNode* [pathValue.size()];
	//cout<<&root<<endl;
	for(int j=0; j< pathValue.size(); ++j)
	{
		char *dirPath = new char [pathValue[j].length()+1];
		strcpy(dirPath,pathValue[j].c_str());
		struct stat statbuf;
		root[j] = getNode();
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
				insert(root[j], tmpstr);
			}
		}
		delete[] dirPath;
	}

	//cout<<"*********"<<&root<<endl;
	//for(int i=0; i<numOfTrees; ++i)
	//	cout<<root[i]<<endl;
	int comp = printAutoSuggestions(root, "grub", numOfTrees); 
  
    if (comp == -1) 
        cout << "No other strings found with this prefix\n"; 
  
    else if (comp == 0) 
        cout << "No string found with this prefix\n"; 
  
    return 0; 
}

/*bool search(struct trieNode **root, string key, int numOfTrees) 
{ 
	for(int i=0; i<numOfTrees; ++i)
	{
    	struct trieNode *pWalk = root[i]; 
		string tmpstr;
		bool flag;
    	for (int j = 0; j < key.length(); j++) 
    	{ 
    	    int index = key[j];// - 'a';
			tmpstr.push_back(index);
    	    if (!pWalk->children[index])
				flag = false;
    	    pWalk = pWalk->children[index]; 
    	}
		if(!flag)
		{
			
		}
	}
    	//return (pWalk != NULL && pWalk->isEndOfWord); 
}*/

/*void createTrie (char *path, trieNode*** root, int &numOfTrees)
{
	puts(path);
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
	cout<<&root<<endl;
	for(int j=0; j< pathValue.size(); ++j)
	{
		char *dirPath = new char [pathValue[j].length()+1];
		strcpy(dirPath,pathValue[j].c_str());
		struct stat statbuf;
		(*root)[j] = getNode();
		cout<<*root[j]<<endl;
		if (stat(dirPath, &statbuf) != 0)
		{
			cerr<<"Error in retrieving file/directory info for trie."<<endl;
			exit(-1);
		}
	
		//if((statbuf.st_mode & S_IFMT) == S_IFDIR) //If argument is a directory
		//{
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
				insert(*root[j], tmpstr);
			}
		}
		//}
		delete[] dirPath;
	}
}*/