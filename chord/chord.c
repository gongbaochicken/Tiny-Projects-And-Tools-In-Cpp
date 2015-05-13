/*
 //  Created by Jason Zhuo Jia on 3/15/15.
 //  Copyright (c) 2015 Duke University. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "sha1.h"

//Define Macro

#define MAX_MSG_LENGTH 2048
#define COMMANDLEN 512
#define CMDSIZE 50
#define IPLEN 30
#define PORTLEN 20
#define FINGER_NUM 32
#define TOTAL 4294967296

//Define Struct
typedef struct _node{
    int port;
    //char* IP;
    unsigned int key;
}node;

typedef struct _entry{
    unsigned int start;
    node* nodeInfo;
}entry;

/////*Global Variables */////

entry fin_tb[FINGER_NUM];
node localNode;
char* addr = "127.0.0.1";
//neighbers need to maintain
node* postnode;
node* postnode2;
node* prenode;


/*Declare functions*/

unsigned int hashFuction(char* key);
unsigned int translate(int port);
int findClosestPort(unsigned int hashValue);
void creatFirstNode();
void resetPreNode();
void heartBeat();
void* updateFingerTable();
void* commandLine();
void* printNodeInfo();
//Stabilization part
void fix_fingers(int i);
void join(int connectPort);
void notify();
void stabilize();


///////////////////////////////////////////////////
////////////Deal with hash values//////////////////

unsigned int hashFunction(char* key) {
  SHA1Context sha;
  unsigned int result = 0;
  SHA1Reset(&sha);
  SHA1Input(&sha, (const unsigned char*)key, strlen(key));

  if((SHA1Result(&sha)) == 0) {
    printf("fail to compute message digest!\n");
  }
  else {
    result = sha.Message_Digest[0] ^ sha.Message_Digest[1];
    int i=2;
    for(i = 2; i < 5; i++) {
      result = result ^ sha.Message_Digest[i];
    }
  }
  return result;
}

//translate IP:port into a hash value
unsigned int translate(int port){
	char* str;
	char* strPort;
	str=(char*)malloc(IPLEN*sizeof(char));
	strPort=(char*)malloc(PORTLEN*sizeof(char));
	strcat(str,"127.0.0.1:");
	sprintf(strPort,"%d",port);
	strcat(str,strPort);
	return hashFunction(str);
}

///////////////////////////////////////////////////
/////////////// Printer ////////////////////////////

void* printNodeInfo(){
    while(1){
        if(prenode == NULL) continue;
        printf("You are listening on port %d.\n", localNode.port);
        printf("Your position is 0x%X.\n", localNode.key);
        printf("Your predecessor is node %s, port %d, position 0x%X.\n", addr, prenode->port, prenode->key);
        printf("Your successor is node %s, port %d, position 0x%X.\n", addr, postnode->port, postnode->key);
        printf("=====================================================\n");
        printf("If you want to close the node, enter 'kill'.\n");
        printf("=====================================================\n");
        sleep(5);
    }
}

/////////////////////////////////////////////////////////////
/////////////////working functions////////////////////////////

/* find port according to hash  */
int findClosestPort(unsigned int hashValue){
	if((hashValue > localNode.key) && (hashValue <= postnode->key)){
		return postnode->port;
	}
	else if(((hashValue > localNode.key) || (hashValue < postnode->key))
			&& (localNode.key > postnode->key)){
		return postnode->port;
	}
	else{
		int i;
	    for(i = (FINGER_NUM-1); i>=0;i--){
			if(((localNode.key < fin_tb[i].nodeInfo->key) &&(fin_tb[i].nodeInfo->key < hashValue))||
					((hashValue < localNode.key) &&
							(fin_tb[i].nodeInfo->key > localNode.key))
					||
					((hashValue < localNode.key) &&
							(fin_tb[i].nodeInfo->key < hashValue))){
				int sock;
				struct sockaddr_in server_addr;
				char msg[MAX_MSG_LENGTH], reply[MAX_MSG_LENGTH];
				if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
					perror("Create socket error:");
				    return 0;
				}
				server_addr.sin_addr.s_addr = inet_addr(addr);
				server_addr.sin_family = AF_INET;
				server_addr.sin_port = htons(fin_tb[i].nodeInfo->port);

				if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
					perror("Connect error: ");
					return 0;
				}

				memset(msg,0,sizeof(msg));
				sprintf(msg,"find %u",hashValue);

				if(send(sock, msg, MAX_MSG_LENGTH, 0) < 0){
				    perror("Send error:");
				    return 0;
				}
                
                //Read Reply
				memset(reply,0,sizeof(reply));
				if(recv(sock, reply, MAX_MSG_LENGTH, 0) < 0){
					perror("Recv error: ");
					return 0;
				}

				int recvPort;
				recvPort=atoi(reply);
				close(sock);
				return recvPort;
			}
	    }
    }
	return localNode.port;
}


void fix_fingers(int i){
	int portNeeded;
	portNeeded = findClosestPort(fin_tb[i].start);
	while(portNeeded == 0){
		portNeeded=findClosestPort(fin_tb[i].start);
	}
	fin_tb[i].nodeInfo->port = portNeeded;
	fin_tb[i].nodeInfo->key = translate(fin_tb[i].nodeInfo->port);
}


void notify(){
	int fd;
	struct sockaddr_in server_addr;
	char msg[MAX_MSG_LENGTH];

	if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Create socket error:");
	    return;
	}
	server_addr.sin_addr.s_addr = inet_addr(addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(postnode->port);

	if(connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		postnode->port = postnode2->port;
		postnode->key = translate(postnode->port);
		postnode2->port = localNode.port;
		postnode2->key = translate(postnode2->port);
		server_addr.sin_port = htons(postnode->port);
		resetPreNode(postnode->port);
		if(connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
			perror("Connect error:");
			return;
		}
	}

    //send nodification
	memset(msg,0,sizeof(msg));
	sprintf(msg,"notify %d",localNode.port);
	if(send(fd, msg, MAX_MSG_LENGTH, 0) < 0){
	    perror("Send error:");
	    return;
	}
	close(fd);
}

void stabilize(){
	int sock;
	struct sockaddr_in server_addr;
	char msg[MAX_MSG_LENGTH], reply[MAX_MSG_LENGTH];

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Create socket error:");
	    return;
	}
	server_addr.sin_addr.s_addr = inet_addr(addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(postnode->port);

	if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		postnode->port = postnode2->port;
		postnode->key = translate(postnode->port);
		postnode2->port = localNode.port;
		postnode2->key = translate(postnode2->port);
		server_addr.sin_port = htons(postnode->port);
		resetPreNode(postnode->port);
		if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
			perror("Connect error:");
			return;
		}
	}

	memset(msg,0,sizeof(msg));
	sprintf(msg,"stable %d",localNode.port);

	if(send(sock, msg, MAX_MSG_LENGTH, 0) < 0){
	    perror("Send error:");
	    return;
	}

	memset(reply,0,sizeof(reply));
	if(recv(sock, reply, MAX_MSG_LENGTH, 0) < 0){
		perror("Recv error: ");
		return;
	}

	int recvPort;
	unsigned int recvHash;
	recvPort=atoi(reply);
	recvHash=translate(recvPort);
	if(recvPort>0){
	    if((postnode->key > localNode.key) && (localNode.key < recvHash) && (recvHash < postnode->key)){
	    	postnode->port = recvPort;
	    	postnode->key = translate(postnode->port);
	    }
	    else if((postnode->key < localNode.key) && (recvPort!=localNode.port) &&
	    		((recvHash > localNode.key) || (recvHash < postnode->key))){
	    	postnode->port = atoi(reply);
	    	postnode->key = translate(postnode->port);
	    }
	}
	close(sock);
}

void resetPreNode(int port){
	int sock;
	struct sockaddr_in server_addr;
	char msg[MAX_MSG_LENGTH];

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Create socket error:");
	    return;
	}
	
    //server_addr struct initialization
    
    server_addr.sin_addr.s_addr = inet_addr(addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		perror("Connect error in reset:");
		return;
	}

	memset(msg,0,sizeof(msg));
	sprintf(msg,"reset-pre");

	if(send(sock, msg, MAX_MSG_LENGTH, 0) < 0){
	    perror("Send error:");
	    return;
	}
	close(sock);
}

void heartBeat(){
	if(prenode!=NULL){
		int sock;
		struct sockaddr_in server_addr;
		char msg[MAX_MSG_LENGTH];

		if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			perror("Create socket error:");
		    return;
		}
		server_addr.sin_addr.s_addr = inet_addr(addr);
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(prenode->port);
		if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
			free(prenode);
			prenode = NULL;
			perror("Connect error in heartBeat: ");
			return;
		}

		memset(msg,0,sizeof(msg));
		sprintf(msg,"keep-alive %d",postnode->port);

		if(send(sock, msg, MAX_MSG_LENGTH, 0) < 0){
		    perror("Send error:");
		    return;
		}
		close(sock);
	}
}

void* updateFingerTable(){
	int i;
	while(1){
		for(i = 0; i < FINGER_NUM; i++){
			fix_fingers(i);
		}
		sleep(1);
		stabilize();
		notify();
		heartBeat();
	}
	pthread_exit(0);
}

void* commandLine(){
	char* cmd = malloc(MAX_MSG_LENGTH*sizeof(char));
	memset(cmd,0,MAX_MSG_LENGTH);
	while(strcmp(cmd,"kill") != 0){
		fscanf(stdin,"%s",cmd);
	}
	exit(0);
}

void* chordNode(void* sock){
	char msg[MAX_MSG_LENGTH];
	char reply[MAX_MSG_LENGTH];
	int result;
	char* cmd = (char*)malloc(30*sizeof(char));
	char* token = " ";
	int connfd = *(int*)sock;
	free(sock);
	while((recv(connfd,msg,sizeof(msg),0))>0){
    	cmd = strtok(msg,token);
    	if(!strcmp(cmd,"join")){
    		cmd = strtok(NULL,token);
    		node* n;
    		n = (node*)malloc(sizeof(node));
    		n->port = atoi(cmd);
    		n->key = translate(n->port);
    		if(postnode->port == localNode.port){
    			postnode->port = n->port;
        		postnode->key = translate(postnode->port);
   	    		prenode->port = n->port;
   	    		prenode->key = translate(prenode->port);
   	    		result = localNode.port;
    		}
    		else {
    			result = findClosestPort(n->key);
    			while(result == 0){
    				result = findClosestPort(n->key);
    			}
    		}
	    	memset(reply,0,MAX_MSG_LENGTH);
	    	sprintf(reply, "%d", result);
	    	if(send(connfd, reply, MAX_MSG_LENGTH, 0) < 0){
	    		perror("Send error: ");
	    		return NULL;
	    	}
    	}
    	else if(!strcmp(cmd,"find")){
    		cmd = strtok(NULL,token);
    		unsigned int findHash;
    		findHash = atoi(cmd);
    		result = findClosestPort(findHash);
    		while(result == 0){
    			result=findClosestPort(findHash);
    		}
    		sprintf(reply,"%d",result);
    		if(send(connfd, reply, MAX_MSG_LENGTH, 0) < 0){
    			perror("Send error: ");
        		return NULL;
    	   	}
    	}
    	else if(!strcmp(cmd,"query")){
    		while((recv(connfd, msg, MAX_MSG_LENGTH,0))>0){
    			unsigned int mHash;
    			mHash = hashFunction(msg);
    			result = findClosestPort(mHash);
    			while(result == 0){
    				result = findClosestPort(mHash);
    			}
    			sprintf(reply,"%d",result);
    			if(send(connfd, reply, MAX_MSG_LENGTH, 0) < 0){
    				perror("Send error: ");
    				return NULL;
    			}
    		}
    	}
    	else if(!strcmp(cmd,"stable")){
    		memset(reply,0,MAX_MSG_LENGTH);
    		if(prenode == NULL){
    			sprintf(reply,"0");
    		}
    		else{
    			sprintf(reply,"%d",prenode->port);
    		}
    		if(send(connfd, reply, MAX_MSG_LENGTH, 0) < 0){
    			perror("Send error: ");
    			return NULL;
    	    }
    		break;
    	}
    	else if(!strcmp(cmd,"notify")){
    		cmd = strtok(NULL,token);
    		result = atoi(cmd);
    		if(prenode == NULL){
    			prenode = (node*)malloc(sizeof(node));
    			prenode->port = result;
    			prenode->key = translate(prenode->port);
    		}

    		else if((prenode->key > localNode.key) &&
    				((translate(result) > prenode->key) || (translate(result) < localNode.key))){
    			prenode->port = result;
    			prenode->key = translate(prenode->port);
    		}
            
            else if((prenode->key < localNode.key) && (prenode->key < translate(result)) && (translate(result) < localNode.key)){
                prenode->port = result;
                prenode->key = translate(prenode->port);
            }
        }
    	else if(!strcmp(cmd,"keep-alive")){
    		cmd=strtok(NULL,token);
    		result=atoi(cmd);
    		postnode2->port = result;
    		postnode2->key = translate(postnode2->port);
    	}
    	else if(!strcmp(cmd,"reset-pre")){
    		free(prenode);
    		prenode = NULL;
    	}
    	memset(msg,0,sizeof(msg));
	}
	close(connfd);
	pthread_exit(0);
}

void creatFirstNode(){
    prenode=(node*)malloc(sizeof(node));
    postnode->key=localNode.key;
    postnode->port=localNode.port;
    prenode->key=localNode.key;
    prenode->port=localNode.port;
}

void join(int connectPort){
    char msg[MAX_MSG_LENGTH], reply[MAX_MSG_LENGTH];
    memset(msg,0,sizeof(msg));
    memset(reply,0,sizeof(msg));
    
    printf("Joining the Chord ring.\n");
    sprintf(msg,"join %d",localNode.port);
    int sock;
    struct sockaddr_in server_addr;
    
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Create socket error: ");
        return;
    }
    
    server_addr.sin_addr.s_addr = inet_addr(addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(connectPort);
    
    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Connect error in join: ");
        return;
    }
    
    if(send(sock, msg, sizeof(msg),0) < 0){
        perror("Send error: ");
        return;
    }
    
    fflush(stdin);
    if((read(sock, reply, MAX_MSG_LENGTH)) < 0){
        perror("Recv error: ");
        return;
    }
    postnode->port=atoi(reply);
    postnode->key=translate(postnode->port);
    close(sock);
}


int main(int argc, char** argv){
	//initialize
    char* cmd, *parse;
    cmd=(char*)malloc(50*sizeof(char));
	parse=(char*)malloc(50*sizeof(char));
    int connectPort;
    localNode.key = 0;
	localNode.port = 0;
	postnode=(node*)malloc(sizeof(node));
	postnode2=(node*)malloc(sizeof(node));
	prenode=NULL;
    char* token = " \r\n";
    void* self;

	printf("Please enter command: chord [port] OR chord [port] [Target ID] [Target Port]\n");
	fgets(cmd, COMMANDLEN, stdin);
	parse=strtok(cmd, token);
	parse=strtok(NULL, token);
	localNode.port=atoi(parse);
	localNode.key=translate(localNode.port);
	parse=strtok(NULL, token);

    int i;
	for(i = 0; i < FINGER_NUM; i++){
		fin_tb[i].start=localNode.key+pow(2,i);
		fin_tb[i].nodeInfo=(node*)malloc(sizeof(node));
		fin_tb[i].nodeInfo->port=localNode.port;
		fin_tb[i].nodeInfo->key=translate(fin_tb[i].nodeInfo->port);
	}
	postnode2->port=localNode.port;
	postnode2->key=translate(postnode2->port);

	if(parse == NULL){
		creatFirstNode();
	}
	else{
        //a node wants to join!
		parse = strtok(NULL, token);
		connectPort = atoi(parse);
		join(connectPort);
	}

	int sock;
	int connfd;
	socklen_t sockLen;
	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	pthread_t pthreadID;

	memset((char*)&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_port = htons(localNode.port);

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket creat failed: ");
		return 1;
	}
	if((bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr))) < 0){
		perror("Bind failed: ");
		return 1;
	}
	if(listen(sock, 5) < 0){
		perror("Listen failed: ");
		return 1;
	}
    
	pthread_create(&pthreadID,NULL,updateFingerTable,NULL);
	pthread_create(&pthreadID,NULL,printNodeInfo,NULL);
	pthread_create(&pthreadID,NULL,commandLine,NULL);

    while(1){
    	if((connfd=accept(sock, (struct sockaddr *)&client_addr, &sockLen)) < 0){
    		perror("Accept failed: ");
    		continue;
    	}
    	int* sockpointer;
    	sockpointer=(int*)malloc(sizeof(int));
    	*sockpointer=connfd;
    	pthread_create(&pthreadID,NULL,chordNode,(void*)sockpointer);
    	pthread_join(pthreadID,&self);
	}

	return 0;
}
