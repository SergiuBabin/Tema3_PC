#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
#include <stdbool.h> 
#include <ctype.h>
#define BUFFERSIZE 100
//Verifica daca str este numar
bool isNumeric(const char *str) {
    while(*str != '\0')
    {
        if(*str < '0' || *str > '9')
            return false;
        str++;
    }
    return true;
}

int main(int argc, char *argv[]){
    char *message;
    char *response;
    int sockfd;
    //Conditie de iesire
    bool exit = false;
    bool login = false;
    //Pastreaza cookie
    char* cookie;
    //Pastreaza token
    char* token;

    while(!exit) {
    	char *buffer;
    	buffer = (char*) malloc (BUFFERSIZE);
    	fgets(buffer, BUFFERSIZE, stdin);
		strtok(buffer, "\n");

    	if (strcmp(buffer, "register") == 0) {
    		check_username:
    		printf("username=");
    		char *bufuser;
    		bufuser = (char*) malloc (BUFFERSIZE);
    		fgets(bufuser, BUFFERSIZE, stdin);
    		//Tratez exceptiile pentru input = null
    		if (strcmp(bufuser, "\n") == 0){
    			printf("Bad username...\n");
    			printf("Username is null\n");
        		goto check_username;
    		}

			check_password:
    		printf("password=");
    		char *bufpass;
    		bufpass = (char*) malloc (BUFFERSIZE);
    		fgets(bufpass, BUFFERSIZE, stdin);
    		//Tratez exceptiile pentru input = null
			if (strcmp(bufpass, "\n") == 0){
    			printf("Bad password...\n");
    			printf("Password is null\n");
        		goto check_password;
    		}

    		strtok(bufuser, "\n");
    		strtok(bufpass, "\n");
    		//Parsare cu ajutorul functiilor JSON a inputului de la tastatura
    		JSON_Value *root_value = json_value_init_object();
	   		JSON_Object *root_object = json_value_get_object(root_value);
	   		char *serialized_string = NULL;
	   		json_object_set_string(root_object, "username", bufuser);
	   		json_object_set_string(root_object, "password", bufpass);
	    	serialized_string = json_serialize_to_string_pretty(root_value);
   			//formare mesaj
    		message = compute_post_request("3.8.116.10", "/api/v1/tema/auth/register", "application/json",
	                                   serialized_string, NULL, NULL);

	    	sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		printf("%s\n", response);
	   		close_connection(sockfd);

    	} else if (strcmp(buffer, "login") == 0) {
    		if (login == true) {
    			printf("You are already logged\n");
    			printf("Please logout\n");
    			continue;
    		}

    		check_username1:
    		printf("username=");
    		char *bufuser;
    		bufuser = (char*) malloc (BUFFERSIZE);
    		fgets(bufuser, BUFFERSIZE, stdin);
    		//Tratez exceptiile pentru input = null
    		if (strcmp(bufuser, "\n") == 0){
    			printf("Bad username...\n");
    			printf("Username is null\n");
        		goto check_username1;
    		}

			check_password1:
    		printf("password=");
    		char *bufpass;
    		bufpass = (char*) malloc (BUFFERSIZE);
    		fgets(bufpass, BUFFERSIZE, stdin);
    		//Tratez exceptiile pentru input = null
			if (strcmp(bufpass, "\n") == 0){
    			printf("Bad password...\n");
    			printf("Password is null!!\n");
        		goto check_password1;
    		}
			strtok(bufuser, "\n");
    		strtok(bufpass, "\n");
    		//Parsare cu ajutorul functiilor JSON a inputului de la tastatura
    		JSON_Value *root_value = json_value_init_object();
	   		JSON_Object *root_object = json_value_get_object(root_value);
	   		char *serialized_string = NULL;
	   		json_object_set_string(root_object, "username", bufuser);
	   		json_object_set_string(root_object, "password", bufpass);
	    	serialized_string = json_serialize_to_string_pretty(root_value);
   
    		message = compute_post_request("3.8.116.10", "/api/v1/tema/auth/login", "application/json",
	                                   serialized_string, NULL, NULL);
	    	sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		printf("%s\n", response);
	   		char* cookies; 
    		char* rest = response; 
 
   		 	while ((cookies = strtok_r(rest, "\n", &rest))) {
        		if (strstr(cookies, "Set-Cookie:") != NULL) {
        			cookie = strtok(cookies, ";");
        			cookie = strtok(cookie, ":");
        			cookie = strtok(0, " ");
        			break;
        		}
   		 	}

	   		close_connection(sockfd);
	   		login = true;
    	} else if (strcmp(buffer, "enter_library") == 0){

    		message = compute_get_request("3.8.116.10", "/api/v1/tema/library/access", NULL, cookie, NULL);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		printf("%s\n", response);
	   		char* tokens; 
    		char* rest = response; 
 
   		 	while ((tokens = strtok_r(rest, "\n", &rest))) {
        		if (strstr(tokens, "token") != NULL) {
        			token = strtok(tokens, ":");
        			token = strtok(0, "\"");
        			break;
        		}
   		 	}
   		 	close_connection(sockfd);
    	} else if (strcmp(buffer, "get_books") == 0) {

    		message = compute_get_request("3.8.116.10", "/api/v1/tema/library/books", NULL, NULL, token);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		printf("%s\n", response);

	   		close_connection(sockfd);
    	} else if(strcmp(buffer, "add_book") == 0) {

    		check_title:
    		printf("title=");
    		char *buftitle;
    		buftitle = (char*) malloc (BUFFERSIZE);
    		fgets(buftitle, BUFFERSIZE, stdin);
    		//Tratez exceptiile pentru input = null
    		if (strcmp(buftitle, "\n") == 0){
    			printf("Bad title...\n");
    			printf("Title is null\n");
        		goto check_title;
    		}

    		check_author:
    		printf("author=");
    		char *bufauth;
    		bufauth = (char*) malloc (BUFFERSIZE);
    		fgets(bufauth, BUFFERSIZE, stdin);
    		//Tratez exceptiile pentru input = null
    		if (strcmp(bufauth, "\n") == 0){
    			printf("Bad author...\n");
    			printf("Author is null\n");
        		goto check_author;
    		}

    		check_genre:
    		printf("genre=");
    		char *bufgenre;
    		bufgenre = (char*) malloc (BUFFERSIZE);
    		fgets(bufgenre, BUFFERSIZE, stdin);
    		//Tratez exceptiile pentru input = null
    		if (strcmp(bufgenre, "\n") == 0){
    			printf("Bad genre...\n");
    			printf("Genre is null\n");
        		goto check_genre;
    		}

    		check_publisher:
    		printf("publisher=");
    		char *bufpubl;
    		bufpubl = (char*) malloc (BUFFERSIZE);
    		fgets(bufpubl, BUFFERSIZE, stdin);
    		//Tratez exceptiile pentru input = null
    		if (strcmp(bufpubl, "\n") == 0){
    			printf("Bad publisher...\n");
    			printf("Publisher is null\n");
        		goto check_publisher;
    		}

			check_page_count:
    		printf("page_count=");
    		char *bufcount;
    		bufcount = (char*) malloc (BUFFERSIZE);
    		fgets(bufcount, BUFFERSIZE, stdin);
    		strtok(bufcount, "\n");
    		//Tratez exceptiile pentru input = null
        	if (!isNumeric(bufcount)){
           		printf("Bad page_count \"%s\"\n", bufcount);
       			goto check_page_count;
       		}

    		strtok(buftitle, "\n");
    		strtok(bufauth, "\n");
    		strtok(bufgenre, "\n");
    		strtok(bufpubl, "\n");
    		
    		//Parsare cu ajutorul functiilor JSON a inputului de la tastatura
    		JSON_Value *root_value = json_value_init_object();
	   		JSON_Object *root_object = json_value_get_object(root_value);
	   		char *serialized_string = NULL;
	   		json_object_set_string(root_object, "title", buftitle);
	   		json_object_set_string(root_object, "author", bufauth);
	   		json_object_set_string(root_object, "genre", bufgenre);
	   		json_object_set_string(root_object, "publisher", bufpubl);
	   		json_object_set_number(root_object, "page_count", atoi(bufcount));

	    	serialized_string = json_serialize_to_string_pretty(root_value);
   
    		message = compute_post_request("3.8.116.10", "/api/v1/tema/library/books", "application/json",
	                                   serialized_string, NULL, token);
    		//printf("%s\n", message);
	    	sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		printf("%s\n", response);
	   		close_connection(sockfd);
    	} else if (strcmp(buffer, "get_book") == 0) {
    		check_id:
    		printf("id=");
    		char *bufid;
    		bufid = (char*) malloc (BUFFERSIZE);
    		fgets(bufid, BUFFERSIZE, stdin);
    		strtok(bufid, "\n");
    		//Tratez exceptiile pentru input = null
    		if (!isNumeric(bufid)){
           		printf("Bad id \"%s\"\n", bufid);
           		printf("Id need to be numerical\n");
       			goto check_id;
       		}
    		char str[60];
   
   			strcpy(str, "/api/v1/tema/library/books/");
    		strcat(str,bufid);
    		message = compute_get_request("3.8.116.10", str, NULL, NULL, token);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		printf("%s\n", response);
	   		close_connection(sockfd);
    	} else if (strcmp(buffer, "delete_book") == 0) {
    		check_id2:
    		printf("id=");
    		char *bufid;
    		bufid = (char*) malloc (BUFFERSIZE);
    		fgets(bufid, BUFFERSIZE, stdin);
    		strtok(bufid, "\n");
    		//Tratez exceptiile pentru input = null
    		if (!isNumeric(bufid)){
           		printf("Bad id \"%s\"\n", bufid);
           		printf("Id need to be numerical\n");
       			goto check_id2;
       		}

    		char str[60];
   
   			strcpy(str, "/api/v1/tema/library/books/");
    		strcat(str,bufid);
    		message = compute_delete_request("3.8.116.10", str, NULL, token);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		printf("%s\n", response);
	   		close_connection(sockfd);
    	} else if (strcmp(buffer, "logout") == 0) {

    		message = compute_get_request("3.8.116.10", "/api/v1/tema/auth/logout", NULL, cookie, NULL);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	   		send_to_server(sockfd, message);
	   		response = receive_from_server(sockfd);
	   		printf("%s\n", response);

	   		cookie = NULL;
    		token = NULL;
    		close_connection(sockfd);
    		login = false;
    	} else if (strcmp(buffer, "exit") == 0) {
    		exit = true;
    	} else {
    		printf("Bad command \"%s\"\n\n", buffer);
    		printf("Avalible commands is:\n");
    		printf("register\nlogin\nenter_library\nget_books\nget_book\nadd_book\ndelete_book\nlogout\nexit\n");
    		printf("\nPlease enter one of these\n");
    	}
	}  
}
