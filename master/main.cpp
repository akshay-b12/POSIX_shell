#include "header.h"

//extern char **environ;

/******Global variables******/
int fd1[2], fd2[2];

int main()
{
	string ip_comm;
	pid_t pid;
	int status;
	
	/*for (char **env = environ; *env != 0; env++)
	{
    	char *thisEnv = *env;
    	cout<<thisEnv<<endl; 
	}*/
	cout<<"% ";
	
	while(getline(cin, ip_comm))
	{
		//cout<<ip_comm<<endl;
		//ip_comm.pop_back();
		//if(ip_comm[ip_comm.length()-1] == '\n')
		//	ip_comm[ip_comm.length()-1] = '\0';
			
		vector<string> ip_args;
		vector<int> pipePos;
		vector<int> redirectPos;
		bool pipeflag = false;
		bool redirectflag = false;
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
				redirectflag = true;
				if(pipePos.empty())
					redirectPos.push_back(ip_args.size());
				else
					redirectPos.push_back(ip_args.size() - pipePos[pipePos.size()-1] - 1);
			}
			//cout<<tmpstr<<endl;
			ip_args.push_back(tmpstr); 
		}
		
		if(pipeflag)
		{
			pipedCommand(ip_args, pipePos, redirectflag, redirectPos);
		}
		else
		{
			char **args;
			args = new char* [ip_args.size()+1];
			char command[ip_args[0].length()+1];
			ip_args[0].copy(command, ip_args[0].length()+1);
			command[ip_args[0].length()] = '\0';
			for(int i=0; i<ip_args.size(); ++i)
			{
				args[i] = new char [ip_args[i].size()+1];
				strcpy(args[i],ip_args[i].c_str());
				//args[i][ip_args[i].size()] = '\0';
			}
			args[ip_args.size()] = NULL;

			if((pid = fork()) < 0)
			{
				cerr<<"fork() error...exiting.\n";
				exit(-1);
			}
			else if(pid == 0)
			{
				if(redirectflag)
				{
					redirection(args, redirectPos);
				}
				else
				{
					execvp(args[0], args);
					cerr<<"Command "<<command<<" couldn't be executed!\n";
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
		cout<<"% ";
	}
	return 0;
}

void pipedCommand(vector<string> ip_args, vector<int> pipePos, bool redirectflag, vector<int> redirectPos)
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
				}
				close(fd1[1]);
				execvp(args[comm_num][0], args[comm_num]);
				cerr<<"Command "<<args[comm_num][0]<<" couldn't be executed!\n";
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
						close(fd1[0]);
					}
					if(redirectflag)
						redirection(args[comm_num], redirectPos);
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
						close(fd2[0]);
					}
					if(redirectflag)
						redirection(args[comm_num], redirectPos);
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
						close(fd1[0]);
					}
					close(STDOUT_FILENO);
					if(fd2[1] != STDOUT_FILENO)
					{
						if(dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
						{
							cerr<<"dup2() error to stdout";
							exit(-1);
						}
						close(fd2[1]);
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
						close(fd2[0]);
					}
					close(STDOUT_FILENO);
					if(fd1[1] != STDOUT_FILENO)
					{
						if(dup2(fd1[1], STDOUT_FILENO) != STDOUT_FILENO)
						{
							cerr<<"dup2() error to stdout";
							exit(-1);
						}
						close(fd1[1]);
					}
					execvp(args[comm_num][0], args[comm_num]);
					cerr<<"Command "<<args[comm_num][0]<<" couldn't be executed!\n";
				}
			}	
		}
		comm_num++;
		count--;
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

void redirection(char** ip_comm, vector<int> redirectPos)
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
			int fd = open(ip_comm[redirectPos[0]+1], O_CREAT | O_APPEND, 644);
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
	//}
}