# gp18845

## How to run consumer with node server backend
> npm install express

> node nodeServer.js

go to http://localhost:3000/test.html

## How to run consumer with multi-threaded server backend
> cd multithreaded_server

> make

> ./optimized_server 8080
go to http://localhost:8080/test_files/test.html

## How to run consumer with rust server backend
use "export" to se environment variable CONTENT_ROOT to point to content folder
> cargo run
