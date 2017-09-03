# MiniShell
### Date: 2016

I finished the mini command shell as an individual project at Duke ECE 551. C++ and Data Structure. 

More specially, my command shell can print prompts at appropriate positions, parse the line it reads, and users can use it execute common commands. When the command shell parses the commands, it can gracefully take care of the white spaces and non-escaped spaces. Also, it supports the directory operations, including “cd”, “pushd”, “popd”, 
and “dirstack. Moreover it supports redirection and pipe. 

Valgrind clean: All malloc blocks were freed and there is no leak.


================================================
### Test Cases:

1. Basic commands: 
In the following part, I will test the basic commands, including ls, emacs, mkdir a new folder called "new folder", exit, etc.


2. Directory Operations:

```
zj18@linux:~/ece551/miniproject$ ./myShell 
myShell:/home/zj18/ece551/miniproject $ cd ..
myShell:/home/zj18/ece551 $ cd ..
myShell:/home/zj18 $ pushd ece551
myShell:/home/zj18/ece551 $ pushd ece558
pushd Error: Invalid directory.
myShell:/home/zj18/ece551 $ pushd miniproject
myShell:/home/zj18/ece551/miniproject $ dirstack
/home/zj18
/home/zj18/ece551
myShell:/home/zj18/ece551/miniproject $ popd
myShell:/home/zj18/ece551 $ popd
myShell:/home/zj18 $ 
```

If people pushd serveral directory but forget them and exit the shell, my program will free the 
stack for them to avoid memory leak.


3.redirect and pipe:

```
myShell:/home/zj18/ece551/miniproject $ ls > lsOutput
Program exited with status 0
myShell:/home/zj18/ece551/miniproject $ emacs lsOutput  
Program exited with status 0
```

*You will see the standard output has been redirected to the lsOutput*

I have a executalbe file iotest, which is a simple test file.

```
#include <iostream>
#include <stdio.h>
#include <string.h>

int main(int argc, const char * argv[]) {
  char str[256];
  if(argc >= 2){
    fprintf(stderr, "No argument need!\n");
  }
  while(std::cin.getline(str,256)){
    std::cout<<"I read a new line:";
    std::cout<<str<<std::endl;
  }
  return 0;
}
```

It can read and write, which can be used to test redirection and pipe. Yon can provide the input.txt yourself.

Now, let's test:

```
myShell:/home/zj18/ece551/miniproject $ ./iotest < input.txt > Hi
Program exited with status 0
myShell:/home/zj18/ece551/miniproject $ emacs Hi     
Program exited with status 0

myShell:/home/zj18/ece551/miniproject $ cat Hi | ./iotest
```
I read a new line:I read a new line:This is an input text file. If you see this from shell screen prints or any file you redirect >, then you have read it successfully.