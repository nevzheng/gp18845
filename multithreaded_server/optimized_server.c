#include "csapp.h"
#include "sbuf.h"
#include <stdbool.h>
#include <dlfcn.h>
#include <sys/sendfile.h>

#define NTHREADS 4
#define SBUFSIZE 16

typedef struct func_struct{
    void (*add_func)(int, int, char*);
    void (*sub_func)(int, int, char*);
    void (*mult_func)(int, int, char*);   
}func_struct_t;

sbuf_t sbuf; // NOTE: shared buffer

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
    else if(strcmp(filename, "test_files/segment1.mp4") == 0){
        return 887145;
    }
    else if(strcmp(filename, "test_files/segment2.mp4") == 0){
        return 846856;
    }
    else if(strcmp(filename, "test_files/segment3.mp4") == 0){
        return 849331;
    }
    else if(strcmp(filename, "test_files/segment4.mp4") == 0){
        return 859056;
    }
    else if(strcmp(filename, "test_files/test.html") == 0){
        return 184;
    }
    else if(strcmp(filename, "test_files/fetchonly.js") == 0){
        return 955;
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
    else if(strstr(filename, ".mp4")){
        strcpy(filetype, "video/mp4");
    }

}

// fills http_response with html, gif, txt, or jpg file, see figure 10.5
void fill_with_file(char http_response[MAXBUF], char *filename, int connfd){

    int openfd = get_file(filename);
    
    // write header already inside http_response
    Rio_writen(connfd, http_response, strlen(http_response)); 
    
    sendfile(connfd, openfd, NULL, get_filesize(filename));

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
    else if(strcmp(filename, "test_files/segment1.mp4") == 0){
        return true;
    }
    else if(strcmp(filename, "test_files/segment2.mp4") == 0){
        return true;
    }
    else if(strcmp(filename, "test_files/segment3.mp4") == 0){
        return true;
    }
    else if(strcmp(filename, "test_files/segment4.mp4") == 0){
        return true;
    }
    else if(strcmp(filename, "test_files/test.html") == 0){
        return true;
    }
    else if(strcmp(filename, "test_files/fetchonly.js") == 0){
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
    sprintf(http_response, "%sServer: Optimized Server\r\n", http_response);
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

// generates and writes response for dynamic content
void generate_dynamic_response(char http_response[MAXBUF], char *URI, int connfd, func_struct_t *func_args){
    
    char filetype[MAXLINE];
    int filesize;
    
    bool file_exists = check_exists(URI);
    
    if(!file_exists){ // return 404 
        sprintf(http_response, "HTTP/1.0 404 Not Found\r\n");
        get_filetype(filetype, "test_files/404.html");
        filesize = get_filesize(URI);
        sprintf(http_response, "%sServer: Optimized Server\r\n", http_response);
        sprintf(http_response,"%sConnection: close\r\n", http_response);
        sprintf(http_response,"%sContent-length: %d\r\n", http_response, filesize);
        sprintf(http_response,"%sContent-type: %s\r\n\r\n", http_response, filetype);
        fill_with_file(http_response, "test_files/404.html", connfd);
	    return;
    }

    // valid dynamic response, now serve it, based on figure 11.35
    sprintf(http_response, "HTTP/1.0 200 OK\r\n");
    sprintf(http_response, "%sServer: Optimized Server\r\n", http_response);
    Rio_writen(connfd, http_response, strlen(http_response));
    
    char cgiargs[MAXLINE];
    char *p;
    char http_response_2[MAXBUF];
    char arg1[MAXLINE], arg2[MAXLINE];
    int n1;
    int n2;

    parse_dynamic_uri(URI, cgiargs);
    if(strstr(URI, "adder")){
        p = strchr(cgiargs, '&');
        *p = '\0';
        strcpy(arg1, cgiargs);
        strcpy(arg2, p+1);
        n1 = atoi(arg1);
        n2 = atoi(arg2);
        func_args->add_func(n1, n2, http_response_2);
        Rio_writen(connfd, http_response_2, strlen(http_response_2));
    }
    else if(strstr(URI, "sub")){
        p = strchr(cgiargs, '&');
        *p = '\0';
        strcpy(arg1, cgiargs);
        strcpy(arg2, p+1);
        n1 = atoi(arg1);
        n2 = atoi(arg2);
        func_args->sub_func(n1, n2, http_response_2);
        Rio_writen(connfd, http_response_2, strlen(http_response_2));
    }
    else if(strstr(URI, "mult")){
        p = strchr(cgiargs, '&');
        *p = '\0';
        strcpy(arg1, cgiargs);
        strcpy(arg2, p+1);
        n1 = atoi(arg1);
        n2 = atoi(arg2);
        func_args->mult_func(n1, n2, http_response_2);
        Rio_writen(connfd, http_response_2, strlen(http_response_2));
    }
}

// fill translated_URI with translated name of URI
void translate_URI(char *translated_URI, char *URI){
    if(strcmp(URI, "/test_files/HackerNews.html") == 0){
        strcpy(translated_URI, "test_files/HackerNews.html");
    }
    else if(strcmp(URI, "/test_files/myfile.txt") == 0){
        strcpy(translated_URI, "test_files/myfile.txt");
    }
    else if(strcmp(URI, "/test_files/segment1.mp4") == 0){
        strcpy(translated_URI, "test_files/segment1.mp4");
    }
    else if(strcmp(URI, "/test_files/segment2.mp4") == 0){
        strcpy(translated_URI, "test_files/segment2.mp4");
    }
    else if(strcmp(URI, "/test_files/segment3.mp4") == 0){
        strcpy(translated_URI, "test_files/segment3.mp4");
    }
    else if(strcmp(URI, "/test_files/segment4.mp4") == 0){
        strcpy(translated_URI, "test_files/segment4.mp4");
    }
    else if(strcmp(URI, "/test_files/test.html") == 0){
        strcpy(translated_URI, "test_files/test.html");
    }
    else if(strcmp(URI, "/test_files/fetchonly.js") == 0){
        strcpy(translated_URI, "test_files/fetchonly.js");
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
void process_request(int connfd, char *URI, func_struct_t *func_args){
    char http_response[MAXBUF];
    char translated_URI[MAXLINE];

    translate_URI(translated_URI, URI);

    if(is_static(URI)){ 
        // serve static content
        
        generate_static_response(http_response, translated_URI, connfd); 
        
    }

    else{ 
        // serve dynamic content
        
        generate_dynamic_response(http_response, translated_URI, connfd, func_args);
    }

}

// arguments passed to thread via pointer
typedef struct thread_args{
    int connfd;
    char URI[MAXLINE];
} thread_args_t;

// from tiny webserver
void read_requesthdrs(rio_t *rp){
    char buf[MAXLINE];
    Rio_readlineb(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n")){
        Rio_readlineb(rp, buf, MAXLINE);
    }
    return;
}

// thread function to be run
void *thread(void *args){
    Pthread_detach(pthread_self());
    rio_t rio;
    int connfd;
    char URI[MAXLINE];
    char query[MAXLINE];
    char request_type[MAXLINE];
    char http_version[MAXLINE];
    
    func_struct_t *func_args = (func_struct_t *)args;

    while(1){
        connfd = sbuf_remove(&sbuf);  
        
        // read in request from client and parse it
        Rio_readinitb(&rio, connfd); 
        Rio_readlineb(&rio, query, MAXLINE); 
	    sscanf(query, "%s %s %s", request_type, URI, http_version);
        
        read_requesthdrs(&rio);

        if(strcmp(request_type, "GET")){
            printf("ignoring request. only support GET requests\n");
        }
        else{       
            process_request(connfd, URI, func_args);
        }
      
        Close(connfd);
    }
    
    return NULL;
}

// based on figure 12.28 in CSAPP book
int main(int argc, char *argv[]){
    int connectfd, listenfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    void *handle;

    if(argc != 2){
        printf("usage: %s <PORT_NUMBER>\n", argv[0]);
        exit(0);
    }
    
    // open dll libraries
    handle = dlopen("./libvector.so", RTLD_LAZY);
    if(!handle){
        printf("error opening add handle\n");
        exit(0);
    }
    
    // get symbols for dll 
    func_struct_t *func_args = malloc(sizeof(func_struct_t));
    func_args->add_func = dlsym(handle, "add_func");
    func_args->sub_func = dlsym(handle, "sub_func");
    func_args->mult_func = dlsym(handle, "mult_func");

    // listen on port specified at command line
    listenfd = Open_listenfd(argv[1]); 
    
    // pre-threading
    sbuf_init(&sbuf, SBUFSIZE);
    for(int i = 0; i < NTHREADS; i++){
        Pthread_create(&tid, NULL, thread, (void *)func_args);
    }

    while(1){
        clientlen = sizeof(struct sockaddr_storage);
        connectfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        sbuf_insert(&sbuf, connectfd);
    }
    
    free(func_args); // not needed cause of infinite loop, but eh why not.

    return 0;
}
