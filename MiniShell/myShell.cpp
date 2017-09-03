#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <queue>

#define MAXLINE 1000

using namespace std;

queue<char*> dirStack;

struct commandLineStruct{
  int size;
  char** cmdargv;
};

void pushdir(char * dir){
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  if(chdir(dir) != 0){
    printf("pushd Error: Invalid directory.\n");
    return;
  }
  char * st = (char*) malloc(sizeof(char)*(strlen(cwd)+1));
  strcpy(st, cwd);
  dirStack.push(st);
}

void popdir(){
  if(dirStack.empty()){
    printf("popd: directory stack empty\n");
    return;
  }
  if(dirStack.size() > 1){
    for(int i = 0; i < dirStack.size()-1; i++){
      char* dir = dirStack.front();
      dirStack.pop();
      dirStack.push(dir);
    }
  }
  char* dir = dirStack.front();
  dirStack.pop();
  if(chdir(dir) != 0){
    printf("pushd Error: Invalid directory.\n");
  }
  free(dir);
}

void Printdirstack(){
  int sz = (int) dirStack.size();
  if(sz == 0){
    return;
  }
  for(int i = 0; i < sz; i++){
    char* dir = dirStack.front();
    cout<<dir<<endl;
    dirStack.pop();
    dirStack.push(dir);
  }
}

void cleandirstack(){
  int sz = (int) dirStack.size();
  if(sz == 0){
    return;
  }
  for(int i = 0; i < sz; i++){
    char* dir = dirStack.front();
    dirStack.pop();
    free(dir);
  }
}

void parseLine(commandLineStruct & pargv, char s[]){
  int i = 0, pos = 0;
  pargv.size = 0;
  pargv.cmdargv = (char**) malloc(sizeof(char*));
  pargv.cmdargv[0] = NULL;
  while(isspace(s[i]))
    i++;
  if (s[i] == '\0')
    return;
  while (s[i] != '\0') {
    char word[MAXLINE];
    while (s[i] != '\0' && !isspace(s[i])) {
      if(s[i] == '\\') {
	i++;
	if(s[i] == ' '){
	  word[pos] = ' ';
	  pos++;
	  i++;
	  continue;
	  //}
	}else{
	  //  \b===>\b
	  word[pos++] = '\\';
	  word[pos++] = s[i++];
	  continue;
	}
      }else{
	word[pos++] = s[i++];
      }
    }
    word[pos] = '\0';
    pargv.cmdargv[pargv.size] = (char*) malloc((strlen(word)+1) * sizeof(char));
    strcpy(pargv.cmdargv[pargv.size], word);
    pargv.size++;
    pargv.cmdargv = (char**) realloc(pargv.cmdargv, (pargv.size+1)*sizeof(char*));
    word[0]='\0';
    pos = 0;
    while(isspace(s[i]))
      i++;
  }
  //count the last NULL
  pargv.size++;
  pargv.cmdargv[pargv.size-1] = NULL;
}

void destoryCommandLine(commandLineStruct & pargv){
  if(pargv.size != 0) {
    for(int i = 0; i < pargv.size-1; i++){
      free(pargv.cmdargv[i]);
    }
  }
  free(pargv.cmdargv);
  pargv.size = 0;
}


/*About i/o redirection*/
void redirect(int flag, char* file){
  int fd;
  if(flag == 0){
    fd = open(file, O_RDONLY, S_IREAD | S_IWRITE);
    dup2(fd, 0);
  }else if(flag == 1){
    fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE);
    dup2(fd, 1);
  }else{
    fd = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
    dup2(fd, 2);
  }
  close(fd);
}

void freeVectorWord(vector<char*> args){
  for(int i = 0; i < args.size(); i++){
    free(args[i]);
  }
}

void testPrint(commandLineStruct & cmd){
  if(cmd.size != 0){
    for(int i = 0; i < cmd.size-1; i++){
      std::cout<<"cmd.cmdargv[" << i <<"]: " << cmd.cmdargv[i] <<endl;
    }
    if(cmd.cmdargv[cmd.size - 1] == NULL){
      std::cout << "last Digit is NULL\n";
    }
  }
}


int main(int argc, const char * argv[]) {
  while(true){
    int pipeflag = 0;
    char cwd[MAXLINE];
    getcwd(cwd, sizeof(cwd));
    cout << "myShell:";
    cout << cwd <<" $ ";
    char buffer[1000];
    cin.getline(buffer, 1000);
    if(cin.eof()){
      break;
    }
    int i = 0;
    vector<char*> args;
    //get rid of write space
    while(buffer[i] != '\0'){
      char word[MAXLINE] = {};
      int k = 0;
      while(isspace(buffer[i])){
	i++;
      }
      while(buffer[i] != '\0' && !isspace(buffer[i])){
	word[k++] = buffer[i++];
      }
      word[k] = '\0';
            
      char * vectorWord = (char*) malloc((strlen(word)+1) * sizeof(char));
      strcpy(vectorWord, word);
      args.push_back(vectorWord);
    }
        
    char newBuffer[MAXLINE];
    int pos = 0;
    for(int word = 0; word < args.size(); word++){
      if(strcmp(args[word], "<") == 0 || strcmp(args[word], ">") ==0 || strcmp(args[word], "2>") == 0){
	word++;//jump 2 argments in total
	continue;
      }
      int argsCharPos = 0;
      while(args[word][argsCharPos] != '\0'){
	newBuffer[pos] = args[word][argsCharPos];
	pos++;
	argsCharPos++;
      }
      newBuffer[pos++] = ' ';
    }
    if(pipeflag == 1){
      continue;
    }
    newBuffer[pos] = '\0';
        
    commandLineStruct cmd;
    parseLine(cmd, newBuffer);
        
    if(cmd.cmdargv[0] == NULL){
      destoryCommandLine(cmd);
      freeVectorWord(args);
      continue;
    }
    if(strcmp(cmd.cmdargv[0], "exit") == 0){
      destoryCommandLine(cmd);
      freeVectorWord(args);
      cleandirstack();
      break;
    }
    if(strcmp(cmd.cmdargv[0], "cd") == 0){
      //size is actual size+1, since the last argv element is NULL
      if (cmd.size == 2) {
	chdir(getenv("HOME"));
	//continue???
      }else if (cmd.size == 3) {
	if (chdir(cmd.cmdargv[1]) != 0) {
	  printf("Error: Invalid directory.\n");
	}
	destoryCommandLine(cmd);
	freeVectorWord(args);
	continue;
      } else {
	printf("Error: Invalid use of cd command.\n");
      }
      destoryCommandLine(cmd);
      freeVectorWord(args);
      continue;
    }
    if(strcmp(cmd.cmdargv[0], "pushd") == 0){
      if (cmd.size == 3) {
	pushdir(cmd.cmdargv[1]);
      }else{
	printf("Pushd Usage Error.\n");
      }
      freeVectorWord(args);
      destoryCommandLine(cmd);
      continue;
    }
    if(strcmp(cmd.cmdargv[0], "popd") == 0){
      popdir();
      freeVectorWord(args);
      destoryCommandLine(cmd);
      continue;
    }
    if(strcmp(cmd.cmdargv[0], "dirstack") == 0){
      Printdirstack();
      freeVectorWord(args);
      destoryCommandLine(cmd);
      continue;
    }
        
    pid_t childPid = fork();
    if(childPid == -1){ //error
      fprintf(stderr, "Fork fails: \n");
      return 1;
    }else if(childPid == 0){
      for(int j = 0; j < args.size(); j++){
	if(strcmp(args[j], "<") == 0){
	  redirect(0, args[j+1]);
	  args.erase(args.begin()+j, args.begin()+j+2);
	  j = 0;
	  continue;
	}
	if(strcmp(args[j], ">") == 0){
	  redirect(1, args[j+1]);
	  args.erase(args.begin()+j, args.begin()+j+2);
	  j = 0;
	  continue;
	}
	if(strcmp(args[j], "2>") == 0){
	  redirect(2, args[j+1]);
	  args.erase(args.begin()+j, args.begin()+j+2);
	  j = 0;
	  continue;
	}
                
	if(strcmp(args[j], "|") == 0){
	  int pipeindex = j;
	  int pipefd[2]; //0 for RD, 1 for WR
	  pid_t pipeid, pid;
	  int pipestatus;
	  char ** pipeargs = (char**) malloc(args.size()*sizeof(char*));
                    
	  pipe(pipefd);
	  pipeid = fork();
	  if(pipeid == -1){
	    perror("Pipe2 fails.\n");
	    exit(1);
	  }else if(pipeid == 0){
	    //child
	    for(int k = 0; k < pipeindex; k++){
	      char* c = (char*) malloc(sizeof(char)*strlen(args[k]) + 1);
	      strcpy(c, args[k]);
	      pipeargs[k] = c;
	    }
	    pipeargs[pipeindex] = NULL;
	    close(1); //close standard output
	    dup2(pipefd[1], 1);
	    close(pipefd[0]);
	    execvp(pipeargs[0],pipeargs);
	  }else{
	    pid = waitpid(pipeid, &pipestatus, 0);
	    if(pid == pipeid){
	      args.erase(args.begin(), args.begin()+pipeindex+1);
	      j = 0;
	    }
	    destoryCommandLine(cmd);
	    cmd.cmdargv = (char**) malloc((args.size()+1)*sizeof(char*));
	    for(int rest = 0; rest < args.size(); rest++){
	      cmd.cmdargv[rest] = (char*)malloc(sizeof(char)*(strlen(args[rest])+1));
	      strcpy(cmd.cmdargv[rest], args[rest]);
	    }
	    cmd.cmdargv[args.size()] = NULL;
	    testPrint(cmd);
	    close(0);
	    dup2(pipefd[0], 0);
	    close(pipefd[1]);
	  }
	}
      }
      freeVectorWord(args);
      execvp(cmd.cmdargv[0], cmd.cmdargv);
      fprintf(stderr, "Child process failed\n");
      destoryCommandLine(cmd);
      return 1;
    }else{
      //Parent Process
      int status = 0;
      do {
	pid_t thisChildPID = waitpid(childPid, &status, 0);
	destoryCommandLine(cmd);
	freeVectorWord(args);
	if (thisChildPID == -1) {
	  cout<<"thisChildPID == -1"<<endl;
	  perror("waitpid");
	  exit(EXIT_FAILURE);
	}
	if (WIFEXITED(status)) {
	  printf("Program exited with status %d\n", WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
	  cout<<"WIFSIGNALED(status)"<<endl;
	  printf("Program was killed by signal %d\n", WTERMSIG(status));
	}
	else if (WIFSTOPPED(status)) {
	  printf("stopped by signal %d\n", WSTOPSIG(status));
	}else if (WIFCONTINUED(status)) {
	  printf("continued\n");
	}
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
  }
  return 0;
}
