#include "header.h"


using namespace std;
#define MAX_BUF_LEN 255

main()
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
        cout<<entries->d_name<<endl;
			}
		}
		delete[] dirPath;
	}

	//cout<<"*********"<<&root<<endl;
	//for(int i=0; i<numOfTrees; ++i)
	//	cout<<root[i]<<endl;










  /////////////////////////////////////////////////////////////////////////////////////////////////
    int fd,c, res;
    struct termios ts, ots;
    FILE *fp;
    
    if ((fp = fopen(ctermid(NULL), "r+")) == NULL)
    {
      cerr<<"Cannot open terminal file... Exiting"<<ctermid(NULL);
      return -1 ;
    }
    setbuf(fp, NULL);

    tcgetattr(fileno(fp), &ts);
    ots = ts;
    ts.c_lflag &= ICANON | ECHO | ECHOE | VERASE | ECHOCTL;// | VWERASE | VERASE | VEOF;
    tcsetattr(fileno(fp), TCSAFLUSH, &ts);
          ///////   fputs(prompt, fp); /////Print PS1 here
       
    while(1)
    {
        char buf[MAX_BUF_LEN];
        char *ptr;
        ptr = buf;
        cout<<"%";
        while ((c = getc(fp)) != EOF && (c != '\n'))
            if (ptr < &buf[MAX_BUF_LEN])
                *ptr++ = c;
        *ptr = 0;
        string searchstr(buf);
        int comp = printAutoSuggestions(&root, searchstr, numOfTrees); 
  
        if (comp == -1) 
          cout << "No other strings found with this prefix\n"; 
        else if (comp == 0) 
          cout << "No string found with this prefix\n"; 
        
        putc('\n', fp);
        //cout<<buf;
    }
    tcsetattr(fileno(fp), TCSAFLUSH, &ots);
    fclose(fp);
    return 0;
}  