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
#include "sha1.h"

#define MAX_MSG_LENGTH 2048
#define BUFSIZE 50
#define MSGSIZE 30
#define PORTSIZE 20
#define MAX_LINE 2048

unsigned int hash(char* key) {
    unsigned int value = 0;
    SHA1Context sha;
    SHA1Reset(&sha);
    SHA1Input(&sha, (const unsigned char*)key, strlen(key));
    
    if((SHA1Result(&sha)) == 0) {
        printf("fail to compute message digest!\n");
    }
    else {
        value = sha.Message_Digest[0] ^ sha.Message_Digest[1];
        int i=2;
        for(i = 2; i < 5; i++) {
            value = value ^ sha.Message_Digest[i];
        }
    }
    return value;
}


int main(int argc, char* argv[]){
    char* cmd, *response, *saveptr;
    cmd = (char*)malloc(BUFSIZE * sizeof(char));
    response = (char*)malloc(BUFSIZE * sizeof(char));
    
    printf("Please enter command： query [IP] [Port] :\n");
    fgets(cmd, BUFSIZE, stdin);
    memset(response, 0, BUFSIZE);
    
    response = strtok_r(cmd, " ", &saveptr);
    while(strcmp(response, "query")){
        memset(cmd, 0, BUFSIZE);
        printf("Incorrect request, please enter command(query) ip(first) port(second) again:\n");
        fgets(cmd, BUFSIZE, stdin);
        response = strtok_r(cmd, " ", &saveptr);
    }
    response = strtok_r(NULL, " ", &saveptr);
    response = strtok_r(NULL, " ", &saveptr);
    int port=atoi(response);
    char* ipaddr = "127.0.0.1";
    int sock;
    struct sockaddr_in server_addr;
    
    char buf_receive[MAX_LINE], buf_response[MAX_LINE];
    memset(buf_receive,0,MAX_LINE);
    memset(buf_response,0,MAX_LINE);
    if ((sock = socket(AF_INET, SOCK_STREAM/* use tcp */, 0)) < 0) {
        perror("Create socket error: ");
        return 1;
    }
    server_addr.sin_addr.s_addr = inet_addr(ipaddr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect error: ");
        return 1;
    }
    
    char* str;
    char* portstr;
    
    str = (char*)malloc(30 * sizeof(char));
    portstr = (char*)malloc(20 * sizeof(char));
    
    strcat(str,ipaddr);
    strcat(str," ");
    sprintf(portstr, "%d", port);
    strcat(str, portstr);
    
    printf("Connection to node 127.0.0.1, port %d, position %u\n", port, hash(str));
    
    memcpy(buf_receive,"query",sizeof("query"));
    
    if (send(sock, buf_receive, sizeof(buf_receive), 0) < 0) {
        perror("Send error: ");
        return 1;
    }
    
    //receive fingertable from port node
    while(1){
        fflush(stdin);
        memset(buf_receive, 0, MAX_LINE);
        memset(buf_response, 0, MAX_LINE);
        printf("Please enter your search key (or type 'quit' to leave):\n");
        fgets(buf_receive,MAX_LINE,stdin);
        
        if(!strcmp(buf_receive,"quit\n")) break;
        buf_receive[sizeof(buf_receive)-1] = '\0';
        printf("%sHash value is 0x%X\n",buf_receive,hash(buf_receive));
        
        if((send(sock, buf_receive, sizeof(buf_receive),0)) < 0){
            perror("Send error: ");
            return 1;
        }
        if((recv(sock, buf_response, MAX_LINE ,0)) < 0) {
            perror("Recv error: ");
            return 1;
        }
        memset(str,0,30);
        strcat(str,ipaddr);
        strcat(str," ");
        strcat(str,buf_response);
        printf("Response from node 127.0.0.1, port %d, position 0x%x:\nNot found.\n", atoi(buf_response),hash(str));
    }
    printf("query quited\n");
    return 0;
}
