#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

void command(vector<string> tokens);
void outputRed(vector<string> tokens, int size);
void inputRed(vector<string> tokens, int size);
void pipe(vector<string> tokens, int size);
void background(vector<string> tokens, int size);

//Main
int main(){
	//Variables
	int size = 0; //Used to track the number of words for first process
	bool gotSize = false; //keep increasing size until a special character
	char c;
	string arg;
	
	do{
		//Command input	
		cout << "<";
		string buf;
		getline(cin,arg); 
		stringstream ss(arg);
		vector<string> tokens;
		gotSize = false;
		size = 0;

		while(ss >> buf){
			if(buf == "<" || buf == ">" || buf == "|" || buf == "&"){ //Special character found, we know the exact number of words in command before function character
				c = buf[0];
				gotSize = true;
			}else if(gotSize == false){ 	//Keep incrementing size until a special character was found
				size++;
			}
			tokens.push_back(buf);		//Push each word into a vector
		}

		switch(c){
			//Output redirection
			case '>':
				outputRed(tokens, size);
				break;
			//Input redirection
			case '<':
				inputRed(tokens, size);
				break;
			//Pipe
			case '|':
				pipe(tokens, size);
				break;
			//Background
			case '&':
				background(tokens,size);
				break;
			default: 
				command(tokens);
				break;
		}
	}while(arg != "exit");
	
	return 0;
}

//No function character, reuglar command function
void command(vector<string> tokens){
	//Local Variables
	pid_t pid;
	const char *cmd = tokens[0].c_str(); //arg[0] 
	const char* args[tokens.size() + 1];
	
	//Fork
	if((pid = fork()) == 0){
	//Child Process
		for(int i = 0; i < tokens.size(); i++) {
			args[i] = tokens[i].c_str();
			if(i == tokens.size()-1){
				args[tokens.size()] = NULL;
			}			
		}
		execvp(cmd, (char* const*) args);
	}else{
		//Parent Process
		wait(NULL);
	}
}	


//Output redirection
void outputRed(vector<string> tokens, int size){
	//Local Variables
	pid_t pid;
	const char *cmd = tokens[0].c_str();
	const char* args[size + 1];
	
	//Fork
	if((pid = fork()) == 0){
		//Child Process
		int file = open(tokens[size+1].c_str(), O_WRONLY | O_CREAT, S_IRWXU);
		dup2(file, 1);
		
		for(int i = 0; i < size; i++) {
			args[i] = tokens[i].c_str();
			if(i == size-1){	
				args[size] = NULL;
			}			
		}
		close(file);
		execvp(cmd, (char* const*) args);
	}else{
		//Parent Process
		wait(NULL);
	}
}

//Input redirection
void inputRed(vector<string> tokens, int size){
	//Local Variables
	pid_t pid;
	const char *cmd = tokens[0].c_str();
	const char* args[size + 1];	
	int file = open(tokens[size+1].c_str(),O_RDONLY);
	
	if(file == -1){
		cout << tokens[size+1] << ": No such file or directory" << endl;
		return;
	}
	
	//Fork
	if((pid = fork()) == 0){
		//Child Process
		dup2(file, 0);
		
		for(int i = 0; i < size; i++) {
			args[i] = tokens[i].c_str();
			if(i == size-1){	
				args[size] = NULL;
			}
		}
		
		close(file);
		execvp(cmd, (char* const*) args);
	}else{
		//Parent Process
		wait(NULL);
	}
}

//Pipe 
void pipe(vector<string> tokens, int size){
	//Variables
	int pipefd[2], ret;
	pid_t pid;
	const char *cmd = tokens[0].c_str();
	const char* args[size + 1];	
	const char *cmd2 = tokens[size+1].c_str();
	const char* args2[tokens.size() - size];	

	//Pipe
	ret = pipe(pipefd);
	
	if(ret == -1){
		cout << "Error" << endl;
	}
	
	//Fork
	if((pid = fork()) == 0){
		//Child process
		dup2(pipefd[1], 1);
		close(pipefd[0]);

		for(int i = 0; i < size; i++) {
			args[i] = tokens[i].c_str();
			if(i == size-1){	
				args[size] = NULL;
			}
			cout << args[i] << endl;
		}
		execvp(cmd, (char* const*) args);
	}else{
		//Parent process
		if((pid = fork()) == 0){
			dup2(pipefd[0], 0);
			close(pipefd[1]);
		
			for(int i = 0; i < tokens.size() - size - 1; i++) {
				args2[i] = tokens[size + 1 + i].c_str();
				if(i == tokens.size() - size - 2){	
					args2[tokens.size() - size - 1] = NULL;
				}
			}	
			execvp(cmd2, (char* const*) args2);	
		}else{
			wait(NULL);
			
		}
	}
}

//Background
void background(vector<string> tokens, int size){
	//Local Variables
	pid_t pid;
	const char *cmd = tokens[0].c_str(); //arg[0] 
	const char* args[tokens.size()];
	
	//Fork
	if((pid = fork()) == 0){
		//Child Process
		return;
	}else{
		//Parent Process
		for(int i = 0; i < tokens.size() - 1; i++) {
			args[i] = tokens[i].c_str();
			if(i == tokens.size()-2){
				args[tokens.size()-1] = NULL;
				cout << "&&" << endl;
			}			
		}
		execvp(cmd, (char* const*) args);
		return;
	}
}




	