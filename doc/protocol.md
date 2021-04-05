# Streaming and Distribution Protocol

## Notes
+ Initial version will be done in TCP
* Initial version will use HTTP/1.1

## Roles

### Origin
Origin produces a binary stream of content and streams it to the server with an HTTP Post
request. The Post request shall have the Content-Range header which will specify the
range of the content.
The Keep-Alive Header should be used to communicate whether or not the connection should be maintained between clients and servers.
In addition the Origin will write any content to disk.
https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Range

### Distributor
The Distributor will accept TCP connections from Origin and Consumers.
The Distributor will accept and process POST requests from the Origin and update its in memory buffer.
The Distributor will accept and process GET requests from Consumers 

### Consumer
Consumers send get requests containing the range of content they possess. The Distributor will use that information
to send bytes that it doesn't have.

## Message Types

## Handshaking
+ https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods
+ Connect: Origin:Distributor. Headers shall include a "Connection-Type" indicating Origin connection
+ Connect: Consumer:Distributor. Headers shall include a "Connection-Type" indicating Consumer

## Posting
Origin will send a POST to the distributor which will contain a "Content-Range" header
The Distributor will examine the Content-Range header and write it to disk and the clients and the in memory buffer.

## Consumers
Consumers will send a GET request with a Content Range header. The content range header will specify the range of bytes it has.
The server will try to send the most recent chunks to the Consumer

The Consumer then buffers the content.

We will roll our own video player to work with our simplified protocol.
Open Source Protocols and video players do a lot. We hope that our simplification will both speed development and
make it easier to examine the effects of architecture on content streaming latency.
