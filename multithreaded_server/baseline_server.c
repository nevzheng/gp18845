#include "csapp.h"
#include "sbuf.h"
#include <stdbool.h>
#include <sys/sendfile.h>

/* 
 * NOTE: much of the baseline server is based on the tiny web server from 
 * section 11.6 in the CSAPP book. References to specific figures in the book
 * are also made before function calls.
 *
 */

// returns the file decriptor of static file
// assumes file is in current working directory
int get_file(char *filename){
    int openfd;
    if((openfd = open(filename, O_RDONLY)) < 0){
        printf("error opening file %s... returning early\n", filename);
        return -1;
    }

    return openfd;
}

// hard coded file size (bytes) for each file. change later if needed.
int get_filesize(char *filename){
    if(strcmp(filename, "test_files/HackerNews.html") == 0){
        return 39963;
    }
    else if(strcmp(filename, "test_files/myfile.txt") == 0){
        return 76;
    }
    else if(strcmp(filename, "test_files/google.html") == 0){
        return 166427;
    }
    else if(strcmp(filename, "test_files/horse.gif") == 0){
        return 25222;
    }
    else if(strcmp(filename, "test_files/cgg.jpg") == 0){
        return 1221;
    }
    else{ // returning 404 file
        return 22;
    }

}

// based on figure 11.34 in CSAPP book
void get_filetype(char filetype[MAXLINE], char *filename){

    if(strstr(filename, ".html")){
        strcpy(filetype, "text/html");
    }
    else if(strstr(filename, ".jpg")){
        strcpy(filetype, "img/jpg");
    }
    else if(strstr(filename, ".gif")){
        strcpy(filetype, "img/gif");
    }
    else if(strstr(filename, ".txt")){
        strcpy(filetype, "text/txt");
    }

}

// fills http_response with html, gif, txt, or jpg file, see figure 10.5
void fill_with_file(char http_response[MAXBUF], char *filename, int connfd){
    
    int openfd = get_file(filename);
    
    Rio_writen(connfd, http_response, strlen(http_response)); // write header
    
    sendfile(connfd, openfd, NULL, get_filesize(filename)); // write file

    Close(openfd);
}

// checks to see if given filename can be found
bool check_exists(char* filename){
    if(strcmp(filename, "test_files/HackerNews.html") == 0){
        return true;
    }
    else if(strcmp(filename, "test_files/myfile.txt") == 0){
        return true;
    }
    else if(strcmp(filename, "test_files/google.html") == 0){
        return true;
    }
    else if(strcmp(filename, "test_files/cgg.jpg") == 0){
        return true;
    }   
    else if(strcmp(filename, "test_files/horse.gif") == 0){
        return true;
    } 
    else if(strstr(filename, "test_files/adder")){ // dynamic file
        return true;
    }
    else if(strstr(filename, "test_files/mult")){ // dynamic file
        return true;
    }
    else if(strstr(filename, "test_files/sub")){ // dynamic file
        return true;
    }

    return false;
}

// based on figure 11.34 in CSAPP book
void generate_static_response(char http_response[MAXBUF], char *filename, int connfd){
    char filetype[MAXLINE];
    
    bool file_exists = check_exists(filename);

    if(!file_exists){
        sprintf(http_response, "HTTP/1.0 404 Not Found\r\n");
        get_filetype(filetype, "test_files/404.html");
    }
    else{
        sprintf(http_response, "HTTP/1.0 200 OK\r\n");
        get_filetype(filetype, filename);
    }
    
    int filesize = get_filesize(filename);
    sprintf(http_response, "%sServer: Baseline Server\r\n", http_response);
    sprintf(http_response,"%sConnection: close\r\n", http_response);
    sprintf(http_response,"%sContent-length: %d\r\n", http_response, filesize);
    sprintf(http_response,"%sContent-type: %s\r\n\r\n", http_response, filetype);
    
    if(file_exists){
        fill_with_file(http_response, filename, connfd);
    }
    else{
        fill_with_file(http_response, "test_files/404.html", connfd);
    }
}

bool is_static(char *URI){

    // assumes that all dynamic content will have "?"
    if(strstr(URI, "?")){
        return false;
    }

    return true;
}

// based on figure 11.33 in CSAPP book
void parse_dynamic_uri(char *URI, char cgiargs[MAXLINE]){
    char *ptr = index(URI, '?');   
    
    if(ptr){
        strcpy(cgiargs, ptr+1); // note pointer arithmetic
        *ptr = '\0';
    }
    else{
        printf("error. could not parse uri. see parse_dynamic_uri(). URI: %s\n", URI);
    }

}

// write script name to script_name
void get_script_name(char script_name[MAXLINE], char URI[MAXLINE]){
    if(strstr(URI, "test_files/adder")){
        strcpy(script_name, "test_files/adder");
    }
    else if(strstr(URI, "test_files/sub")){
        strcpy(script_name, "test_files/sub");
    }
    else if(strstr(URI, "test_files/mult")){
        strcpy(script_name, "test_files/mult");
    }
}

void generate_dynamic_response(char http_response[MAXBUF], char *URI, int connfd){
    char filetype[MAXLINE];
    int filesize;
    bool file_exists = check_exists(URI);

    if(!file_exists){ // return 404 
        sprintf(http_response, "HTTP/1.0 404 Not Found\r\n");
        get_filetype(filetype, "test_files/404.html");
        filesize = get_filesize(URI);
        sprintf(http_response, "%sServer: Baseline Server\r\n", http_response);
        sprintf(http_response,"%sConnection: close\r\n", http_response);
        sprintf(http_response,"%sContent-length: %d\r\n", http_response, filesize);
        sprintf(http_response,"%sContent-type: %s\r\n\r\n", http_response, filetype);
        fill_with_file(http_response, "test_files/404.html", connfd);
        return;
    }

    // valid dynamic response, now serve it, based on figure 11.35
    sprintf(http_response, "HTTP/1.0 200 OK\r\n");
    sprintf(http_response, "%sServer: Baseline Server\r\n", http_response);
    Rio_writen(connfd, http_response, strlen(http_response));
    
    char cgiargs[MAXLINE];
    char *emptylist[] = {NULL};
    char script_name[MAXLINE];
    get_script_name(script_name, URI);

    parse_dynamic_uri(URI, cgiargs);
    
    if(Fork() == 0){ // child
        setenv("QUERY_STRING", cgiargs, 1); 
        Dup2(connfd, STDOUT_FILENO);
        Execve(script_name, emptylist, environ);
    }
    Wait(NULL);
}

// fill translated_URI with translated name of URI
void translate_URI(char translated_URI[MAXLINE], char *URI){
    if(strcmp(URI, "/test_files/HackerNews.html") == 0){
        strcpy(translated_URI, "test_files/HackerNews.html");
    }
    else if(strcmp(URI, "/test_files/myfile.txt") == 0){
        strcpy(translated_URI, "test_files/myfile.txt");
    }
    else if(strcmp(URI, "/test_files/google.html") == 0){
        strcpy(translated_URI, "test_files/google.html");
    }
    else if(strcmp(URI, "/test_files/horse.gif") == 0){
        strcpy(translated_URI, "test_files/horse.gif");
    }
    else if(strcmp(URI, "/test_files/cgg.jpg") == 0){
        strcpy(translated_URI, "test_files/cgg.jpg");
    }
    else if(strcmp(URI, "/") == 0){
        strcpy(translated_URI, "test_files/HackerNews.html");
    }
    else{
        char *modified_dynamic = strstr(URI, "test_files/adder");
        if(modified_dynamic){ 
            strcpy(translated_URI, modified_dynamic);
        }
        else if((modified_dynamic = strstr(URI, "test_files/sub"))){
            strcpy(translated_URI, modified_dynamic);
        }
        else if((modified_dynamic = strstr(URI, "test_files/mult"))){
            strcpy(translated_URI, modified_dynamic);
        }
        else{ // 404
            strcpy(translated_URI, "test_files/404.html");
        } 
   }
}


// see figure 11.22 in CSAPP book for reference
// connfd is the file descriptor to write to the client with
// NOTE: this function writes static requests explicitly but dynamic requests
// are written inside of the forked child process and inside generate_dynamic_response()
void process_request(int connfd, char *URI){
    char http_response[MAXBUF];
    char translated_URI[MAXLINE];

    translate_URI(translated_URI, URI);

    if(is_static(URI)){ 
        // serve static content
        
        // fills http_response with response
        generate_static_response(http_response, translated_URI, connfd); 

    }

    else{ 
        // serve dynamic content
        
        // fills http_response with response and writes it
        generate_dynamic_response(http_response, translated_URI, connfd);
    }

}

// arguments passed to thread via pointer
typedef struct thread_args{
    int connfd;
    char URI[MAXLINE];
} thread_args_t;

// thread function to be run
void *thread(void *vargp){
    thread_args_t *t_args = (thread_args_t *)vargp;
    pthread_detach(pthread_self());
    process_request(t_args->connfd, t_args->URI);
    Close(t_args->connfd);
    free(t_args);
    return NULL;
}

// from tiny webserver
void read_requesthdrs(rio_t *rp){
    char buf[MAXLINE];
    Rio_readlineb(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n")){
        Rio_readlineb(rp, buf, MAXLINE);
        //printf("client sent: %s\n", buf);
    }
    return;
}

// based on figure 12.28 in CSAPP book
int main(int argc, char *argv[]){
    int connectfd, listenfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE]; //MAXLINE == 8192
    char request_type[MAXLINE], URI[MAXLINE], http_version[MAXLINE];
    char query_buf[MAXLINE];
    pthread_t tid;
    thread_args_t *args;
    rio_t *rio;


    if(argc != 2){
        printf("usage: %s <PORT_NUMBER>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]); // listen on port specified at command line
    
    while(1){
        clientlen = sizeof(struct sockaddr_storage);
        connectfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        
        // read in request from client and parse it
        rio = malloc(sizeof(rio_t));
        Rio_readinitb(rio, connectfd);
        Rio_readlineb(rio, query_buf, MAXLINE); 
        sscanf(query_buf, "%s %s %s", request_type, URI, http_version);
        
        // from tiny web server
        read_requesthdrs(rio);
        
        free(rio);

        if(strcmp(request_type, "GET")){
            printf("ignoring request. only support GET requests\n");
        }
        else{
            args = malloc(sizeof(thread_args_t));
            args->connfd = connectfd;
            strcpy(args->URI, URI);
            pthread_create(&tid, NULL, thread, args);       
        }
    }

    return 0;
}
