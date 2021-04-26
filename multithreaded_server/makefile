optimized: csapp.c sbuf.c optimized_server.c
	gcc -Wall -Wextra -Werror -Wno-sign-compare -rdynamic -g -O3 -pthread optimized_server.c lib_func.c -ldl csapp.c sbuf.c -o optimized_server

baseline: csapp.c sbuf.c baseline_server.c
	gcc -Wall -Wextra -Wno-sign-compare -g -O3 -pthread baseline_server.c csapp.c sbuf.c -o baseline_server

compile_lib_func: csapp.c lib_func.c
	gcc -shared -fpic -o libvector.so lib_func.c 
