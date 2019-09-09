#include "header.h"

//extern char **environ;

/******Global variables******/
int fd1[2], fd2[2];

int main()
{
	string ip_comm;
	pid_t pid;
	int status;
	unordered_map<string, string> alias_map;
	/* Trie variables */
	trieNode **root = NULL;
	int numOfTrees;
	//trie_init(&root);

	/* Termios variables */
	FILE *fp;
	struct termios ots;
	//termios_init(&fp, &ots);
	
	/* Environment initialisation */
	unordered_map<string, string> rc_map;
	env_init(rc_map);

	cout<<getenv("USER")<<":"<<getenv("PWD")<<getenv("PS1")<<" ";
	
	
	while(getline(cin, ip_comm)) //
	{
		/*
		char buf[MAX_BUF_LEN];
        char *ptr;
        ptr = buf;
		int c;
		bool firsttab = false;
		time_t time1;
		double seconds;
		int fd = open("/dev/pts/3", O_CREAT | O_RDONLY, 644);
		dup2(fd, STDIN_FILENO);
		close(fd);
		while ((c = getchar()) != EOF && (c != '\n'))
		{
			if((c == 9) && !firsttab)
			{
				time(&time1);
				fseek(fp, -1, SEEK_END);
				firsttab = true;
			}
			else if((c == 9) && firsttab)
			{
				time_t time2;
				time(&time2);
				fseek(fp, -1, SEEK_END);
				seconds = difftime(time2,time1);
				cerr<<seconds;
				if(seconds < 1.0)
				{
					*ptr = 0;
        			string searchstr(buf);
        			int comp = printAutoSuggestions(&root, searchstr, numOfTrees);
					cout<<getenv("PS1")<<" "<<buf;
				}
				firsttab = false;
			}
			else if (ptr < &buf[MAX_BUF_LEN])
                *ptr++ = c;
		}
        *ptr = 0;
		putc('\n', fp);
		ip_comm = buf;
		*/
		vector<string> ip_args;
		vector<int> pipePos;
		vector<int> redirectPos;
		bool pipeflag = false;
		bool redirectflag[3] = {false, false,false};
		bool aliasflag = false;
		bool cdflag = false;
		bool openflag = false;
		string alias_str;
		istringstream ss(ip_comm);
		
		while(!ss.eof())
		{
			string tmpstr;
			ss>>tmpstr;
			size_t pos;// = tmpstr.find('$');
			if((pos = tmpstr.find('$')) != string::npos)
			{
				if(pos == 0)
				{
					char var[100];
					size_t len = tmpstr.copy(var,(tmpstr.length()-pos-1),pos+1);
					var[len] = '\0';
					char *val = getenv(var);
					if(val != NULL)
						tmpstr.replace(pos,string::npos, val);
				}
				else if(tmpstr[pos-1] != '\\')
				{
					char var[100];
					size_t len = tmpstr.copy(var,(tmpstr.length()-pos-1),pos+1);
					var[len] = '\0';
					char *val = getenv(var);
					if(val != NULL)
						tmpstr.replace(pos, string::npos, val);
				}	
				else
					tmpstr.erase(pos-1, 1);
					
			}
			else if(tmpstr.compare("|") == 0)
			{
				pipeflag = true;
				pipePos.push_back(ip_args.size()); // Position of pipe w.r.t 0
			}
			else if((tmpstr.compare(">") == 0) || (tmpstr.compare("<") == 0) || (tmpstr.compare(">>") == 0))
			{
				if((tmpstr.compare(">") == 0))
					redirectflag[0] = true;
				if((tmpstr.compare("<") == 0))
					redirectflag[1] = true;
				if((tmpstr.compare(">>") == 0))
					redirectflag[2] = true;
				if(pipePos.empty())
					redirectPos.push_back(ip_args.size());
				else
					redirectPos.push_back(ip_args.size() - pipePos[pipePos.size()-1] - 1);
			}
			else if((tmpstr.compare("alias") == 0))
			{
				aliasflag = true;
				char *ptr=&ip_comm[0];
				while(*ptr != ' ')
					ptr++;
				ptr++;
				alias_str = ptr;
				alias_str.pop_back();
				ip_args.push_back(tmpstr);
				ip_args.push_back(alias_str);
				break;
			}
			else if((tmpstr.compare("cd") == 0))
			{
				cdflag = true;
			}
			else if((tmpstr.compare("open") == 0))
			{
				openflag = true;
			}
			//cout<<tmpstr<<endl;
			ip_args.push_back(tmpstr); 
		}
		
		if(pipeflag)
		{
			pipedCommand(ip_args, pipePos, redirectflag, redirectPos);
		}
		else if(aliasflag)
		{
			alias(alias_map, ip_args);
			aliasflag=false;
			continue;
		}
		else if(cdflag)
		{
			cdflag = false;
			changedir(ip_args, rc_map);
		}
		else if(openflag)
		{
			openflag = false;
			open_comm(ip_args,rc_map);
		}
		else
		{
			char **args;
			unordered_map<string, string>::iterator itr = alias_map.find(ip_args[0]);
			if(itr != alias_map.end())
			{
				string tmpstr;
				vector<string> alias_comm;
				istringstream alias_ss(itr->second);
				while(!alias_ss.eof())
				{
					alias_ss>>tmpstr;
					alias_comm.push_back(tmpstr);
				}
				args = new char* [alias_comm.size() + (ip_args.size()-1)+1];
				int j=0;
				for(int i=0; i<alias_comm.size(); ++i)
				{
					args[i] = new char [alias_comm[i].size()+1];
					strcpy(args[i],alias_comm[i].c_str());
					j++;
					//args[i][alias_comm[i].size()] = '\0';
				}
				for(int i=1; i<ip_args.size(); ++i)
				{
					args[i+j] = new char [ip_args[i].size()+1];
					strcpy(args[i+j],ip_args[i].c_str());
					
					//args[i][ip_args[i].size()] = '\0';
				}
				args[alias_comm.size() + (ip_args.size()-1)] = NULL;
			}
			else
			{
				args = new char* [ip_args.size()+1];
				for(int i=0; i<ip_args.size(); ++i)
				{
					args[i] = new char [ip_args[i].size()+1];
					strcpy(args[i],ip_args[i].c_str());
					//lsargs[i][ip_args[i].size()] = '\0';
				}
				args[ip_args.size()] = NULL;
			}
			if((pid = fork()) < 0)
			{
				cerr<<"fork() error...exiting.\n";
				exit(-1);
			}
			else if(pid == 0)
			{
				if(redirectflag[0] || redirectflag[1] || redirectflag[2])
				{
					redirection(args, redirectPos);
				}
				else
				{
					execvp(args[0], args);
					cerr<<"Command "<<args[0]<<" couldn't be executed!\n";
				}
			}
			if((pid = waitpid(pid, &status, 0) < 0))
			{
				cerr<<"waitpid() error...exiting.\n";
				exit(-1);
			}
		
			for(int i=0; i<ip_args.size(); ++i)
			{
				delete[] args[i];
			}
			delete[] args;
		}
		cout<<getenv("USER")<<":"<<getenv("PWD")<<getenv("PS1")<<" ";
	}
	tcsetattr(fileno(fp), TCSAFLUSH, &ots);
    fclose(fp);
	return 0;
}

void pipedCommand(vector<string> ip_args, vector<int> pipePos, bool *redirectflag, vector<int> &redirectPos)
{
	pid_t pid;
	int status;
	int comm_count = pipePos.size() + 1;
	pipePos.push_back(ip_args.size());
	char ***args;
	args = new char** [comm_count];
	int argc=0;
	for(int i=0; i<comm_count; ++i)
	{
		args[i] = new char* [pipePos[i] - argc +1];
		args[i][pipePos[i] - argc] = NULL;
		for(int j=argc, k=0; j<pipePos[i]; ++j, k++)
		{
			args[i][k] = new char [ip_args[argc].size()+1];
			strcpy(args[i][k],ip_args[argc].c_str());
			//cout<<args[i][k]<<endl;
			argc++;
		}
		argc++; // For the pipe '|'
	}
	
	if((pipe(fd1) < 0) || (pipe(fd2) < 0))
	{
		cerr<<"pipe() error...exiting.\n";
		exit(-1);
	}
	int comm_num = 0;
	int count = comm_count;
	while(count)
	{
		if((comm_num > 1) && (comm_num%2 != 0))
		{
			close(fd2[0]);
			close(fd2[1]);
			if((pipe(fd2) < 0))
			{
				cerr<<"pipe() error...exiting.\n";
				exit(-1);
			}

		}
		else if((comm_num > 1) && (comm_num%2 == 0))
		{
			close(fd1[0]);
			close(fd1[1]);
			if((pipe(fd1) < 0))
			{
				cerr<<"pipe() error...exiting.\n";
				exit(-1);
			}
		}
		if((pid = fork()) < 0)
		{
			cerr<<"fork() error...exiting.\n";
			exit(-1);
		}
		
		else if(pid == 0)
		{
			if(comm_num == 0) // if it is first command
			{
				close(fd1[0]);
				close(fd2[0]);
				close(fd2[1]);
				close(STDOUT_FILENO);
				if(fd1[1] != STDOUT_FILENO)
				{
					if(dup2(fd1[1], STDOUT_FILENO) != STDOUT_FILENO)
					{
						cerr<<"dup2() error to stdout";
						exit(-1);
					}
					//close(fd1[1]);
				}
				if(redirectflag[1])
				{
					redirectflag[1] = false;
					redirection(args[comm_num], redirectPos);
				}
				else
				{
					execvp(args[comm_num][0], args[comm_num]);
					cerr<<"Command "<<args[comm_num][0]<<" couldn't be executed!\n";
				}
			}	
			else if(comm_num == (comm_count - 1))  // if it is last command
			{
				if(comm_num%2 != 0)
				{
					close(fd1[1]);
					close(fd2[0]);
					close(fd2[1]);
					close(STDIN_FILENO);
					if(fd1[0] != STDIN_FILENO)
					{
						if(dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
						{
							cerr<<"dup2() error to stdin";
							exit(-1);
						}
						//close(fd1[0]);
					}
					if(redirectflag[0])
					{
						redirectflag[0] = false;
						redirection(args[comm_num], redirectPos);
					}
					else if(redirectflag[2])
					{
						redirectflag[2] = false;
						redirection(args[comm_num], redirectPos);
					}
					else
					{
						execvp(args[comm_num][0], args[comm_num]);
						cerr<<"Command "<<args[comm_num][0]<<" couldn't be executed!\n";
					}
				}
				else
				{
					close(fd1[1]);
					close(fd1[0]);
					close(fd2[1]);
					close(STDIN_FILENO);
					if(fd2[0] != STDIN_FILENO)
					{
						if(dup2(fd2[0], STDIN_FILENO) != STDIN_FILENO)
						{
							cerr<<"dup2() error to stdin";
							exit(-1);
						}
						//close(fd2[0]);
					}
					if(redirectflag[0])
					{
						redirectflag[0] = false;
						redirection(args[comm_num], redirectPos);
					}
					else if(redirectflag[2])
					{
						redirectflag[2] = false;
						redirection(args[comm_num], redirectPos);
					}
					else
					{					
						execvp(args[comm_num][0], args[comm_num]);
						cerr<<"Command "<<args[comm_num][0]<<" couldn't be executed!\n";
					}
				}
			}
			else
			{
				if(comm_num%2 != 0)
				{
					close(fd1[1]);
					close(fd2[0]);
					close(STDIN_FILENO);
					if(fd1[0] != STDIN_FILENO)
					{
						if(dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
						{
							cerr<<"dup2() error to stdin";
							exit(-1);
						}
						//close(fd1[0]);
					}
					close(STDOUT_FILENO);
					if(fd2[1] != STDOUT_FILENO)
					{
						if(dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
						{
							cerr<<"dup2() error to stdout";
							exit(-1);
						}
						//close(fd2[1]);
					}
					execvp(args[comm_num][0], args[comm_num]);
					cerr<<"Command "<<args[comm_num][0]<<" couldn't be executed!\n";
				}
				else
				{
					close(fd1[0]);
					close(fd2[1]);
					close(STDIN_FILENO);
					if(fd2[0] != STDIN_FILENO)
					{
						if(dup2(fd2[0], STDIN_FILENO) != STDIN_FILENO)
						{
							cerr<<"dup2() error to stdin";
							exit(-1);
						}
						//close(fd2[0]);
					}
					close(STDOUT_FILENO);
					if(fd1[1] != STDOUT_FILENO)
					{
						if(dup2(fd1[1], STDOUT_FILENO) != STDOUT_FILENO)
						{
							cerr<<"dup2() error to stdout";
							exit(-1);
						}
						//close(fd1[1]);
					}
					execvp(args[comm_num][0], args[comm_num]);
					cerr<<"Command "<<args[comm_num][0]<<" couldn't be executed!\n";
				}
			}	
		}
		if(comm_num == 0)
		{
			close(fd1[1]);
		}
		else if(comm_num == (comm_count - 1))
		{
			if(comm_num%2 != 0)
				close(fd2[1]);
			else
				close(fd1[1]);

		}
		else
		{
			if(comm_num%2 != 0)
				close(fd2[1]);
			else
				close(fd1[1]);
		}
		comm_num++;
		count--;
		
		if((pid = waitpid(pid, &status, 0) < 0))
		{
			cerr<<"waitpid() error...exiting.\n";
			exit(-1);
		}
	}
	close(fd1[0]);
	close(fd1[1]);
	close(fd2[0]);
	close(fd2[1]);
	argc=0;
	for(int i=0; i<comm_count; ++i)
	{
		for(int j=argc, k=0; j<=pipePos[i]; ++j, k++)
		{
			delete[] args[i][k];
			argc++;
		}
		argc++;
		delete[] args[i];
	}
	delete[] args;

}

void redirection(char** ip_comm, vector<int> &redirectPos)
{
	pid_t pid;
	int k=0;
	char **args = new char* [redirectPos[0]+1];
	for(int k=0; k<redirectPos[0]; ++k)
	{
		args[k] = new char [strlen(ip_comm[k])];
		strcpy(args[k], ip_comm[k]);
	}
	args[redirectPos[0]] = NULL;
	//if((pid = fork()) < 0)
	//{
	//	cerr<<"fork() error...exiting.\n";
	//	exit(-1);
	//} 
	//else if(pid == 0)
	//{
		if(strcmp(ip_comm[redirectPos[0]], ">") == 0)
		{
			//cout<<ip_comm[0]<<ip_comm[1]<<ip_comm[2]<<ip_comm[3]<<endl;
			int fd = open(ip_comm[redirectPos[0]+1], O_CREAT | O_WRONLY, 644);
			if(fd < 0)
			{
				cerr<<"Could not open file. Redirection failed";
				exit(-1);
			}
			if(dup2(fd, STDOUT_FILENO) != STDOUT_FILENO)
			{
				cerr<<"dup2() error to stdout";
				exit(-1);
			}
			close(fd);
			
			execvp(args[0], args);
			cerr<<"Command "<<args[0]<<" couldn't be executed!\n";
		}
		else if(strcmp(ip_comm[redirectPos[0]], "<") == 0)
		{
			int fd = open(ip_comm[redirectPos[0]+1], O_CREAT | O_RDONLY, 644);
			if(fd < 0)
			{
				cerr<<"Could not open file. Redirection failed";
				exit(-1);
			}
			if(dup2(fd, STDIN_FILENO) != STDIN_FILENO)
			{
				cerr<<"dup2() error to stdout";
				exit(-1);
			}
			close(fd);
			execvp(args[0], args);
			cerr<<"Command "<<args[0]<<" couldn't be executed!\n";
		}
		else if(strcmp(ip_comm[redirectPos[0]], ">>") == 0)
		{
			int fd = open(ip_comm[redirectPos[0]+1], O_CREAT | O_WRONLY | O_APPEND, 644);
			if(fd < 0)
			{
				cerr<<"Could not open file. Redirection failed";
				exit(-1);
			}
			if(dup2(fd, STDOUT_FILENO) != STDOUT_FILENO)
			{
				cerr<<"dup2() error to stdout";
				exit(-1);
			}
			close(fd);
			execvp(args[0], args);
			cerr<<"Command "<<args[0]<<" couldn't be executed!\n";
		}
		redirectPos.erase(redirectPos.begin());
	//}
}