#include <stdio.h>
#include <string.h>
#include "csapp.h"

void add_func(int n1, int n2, char http_response[MAXBUF]){
    char content_body[MAXLINE];

    sprintf(content_body, "Welcome to add.com: ");
    sprintf(content_body, "%sTHE Internet addition portal.\r\n<p>", content_body);
    sprintf(content_body, "%sThe answer is: %d + %d = %d\r\n<p>", content_body, n1, n2, n1 + n2);
    sprintf(content_body, "%sThanks for visiting!\r\n", content_body);

    sprintf(http_response, "Connection: close\r\n");
    sprintf(http_response, "Content-length: %d\r\n", (int)strlen(content_body));
    sprintf(http_response, "Content-type: text/html\r\n\r\n");
 
    strcat(http_response, content_body);

}

void sub_func(int n1, int n2, char http_response[MAXBUF]){
    char content_body[MAXLINE];

    sprintf(content_body, "Welcome to subtract.com: ");
    sprintf(content_body, "%sTHE Internet subtraction portal.\r\n<p>", content_body);
    sprintf(content_body, "%sThe answer is: %d - %d = %d\r\n<p>", content_body, n1, n2, n1 - n2);
    sprintf(content_body, "%sThanks for visiting!\r\n", content_body);

    sprintf(http_response, "Connection: close\r\n");
    sprintf(http_response, "Content-length: %d\r\n", (int)strlen(content_body));
    sprintf(http_response, "Content-type: text/html\r\n\r\n");
 
    strcat(http_response, content_body);

}

void mult_func(int n1, int n2, char http_response[MAXBUF]){
    char content_body[MAXLINE];

    sprintf(content_body, "Welcome to multiply.com: ");
    sprintf(content_body, "%sTHE Internet multiplication portal.\r\n<p>", content_body);
    sprintf(content_body, "%sThe answer is: %d * %d = %d\r\n<p>", content_body, n1, n2, n1 * n2);
    sprintf(content_body, "%sThanks for visiting!\r\n", content_body);

    sprintf(http_response, "Connection: close\r\n");
    sprintf(http_response, "Content-length: %d\r\n", (int)strlen(content_body));
    sprintf(http_response, "Content-type: text/html\r\n\r\n");
 
    strcat(http_response, content_body);

}

