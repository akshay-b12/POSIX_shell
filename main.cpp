#include "header.h"

extern char **environ;

int main()
{
	string ip_comm;
	pid_t pid;
	int status;
	//cout<<"Path: "<<getenv("PWD");
	//cout<<environ[1];
	
	for (char **env = environ; *env != 0; env++)
	{
    	char *thisEnv = *env;
    	cout<<thisEnv<<endl; 
	}
	cout<<"% ";
	
	while(getline(cin, ip_comm))
	{
		//ip_comm.pop_back();
		//if(ip_comm[ip_comm.length()-1] == '\n')
		//	ip_comm[ip_comm.length()-1] = '\0';
			
		vector<string> ip_args;
		
		istringstream ss(ip_comm);
		while(!ss.eof())
		{
			string tmpstr;
			ss>>tmpstr;
			size_t pos = tmpstr.find('$');
			if(pos != string::npos)
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
			//cout<<tmpstr<<endl;
			ip_args.push_back(tmpstr); 
		}
		//cout<<ip_args.size()<<endl;
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
			cout<<"fork() error...exiting.\n";
			exit(-1);
		}
		else if(pid == 0)
		{
			execvp(args[0], args);
			cout<<"Command "<<command<<" couldn't be executed!\n";
		}
		
		if((pid = waitpid(pid, &status, 0) < 0))
		{
			cout<<"waitpid() error...exiting.\n";
			exit(-1);
		}
		cout<<"% ";
	}
	return 0;
}
