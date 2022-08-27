* Dumitru Radu-Andrei 322CB

# Tema 2 PCOM

* A short description of the implementation. The header files contain
descriptions for the functions used.

## Send and Receive
* Very basic implementations: The number of bytes is sent first, then the
message. A while loop assures that all the bytes are received.

* I have created 3 structures according to the description of the homework, in
order to receive the UDP and TCP messages directly, without copying the values
from a buffer to a struct.

## server
---

* Initialise the sockets, file descriptors, the sockaddr structs, disables the
Nagle's algorithm and binds the sockets. Also, it listens to the TCP socket.

* Uses 3 maps: one for the connected clients, one for the stored clients (those
who disconnected, but had at least one topic with SF 1) and one for the topics
with the subscribers.

* In the endless loop, it selects from the set.

* Checks for input from stdin (actually checks only for exit).

* If there is no input on stdin, then checks the UDP socket. If is set, then
receives a message from the UDP client. It searches the topics map and sends
 the message to all the subscribers.

* If there is no input from UDP, then checks the TCP socket for a new client.
It checks if the ID of the new client isn't already in use. Adds the new client
and if it is a reconnection then it sends it everything he has missed.

* Otherwise, receives a message from the TCP client. It identifies the request
and treats it.

## subscriber
---

* A initialisation similar to the server.

* Checks for input from stdin (exit, subscribe, unsubscribe). If subscribe /
unsubscribe is received then sends a request to the server.

* Otherwise, it receives a message from the server. Checks if it is an opcode
(as an example, if the client wanted to subscribe to a topic, the server sends
the confirmation that he has succesfuly subscribed). If it is an opcode, then
it does a switch (implemented with a nice function with lambdas, not my idea,
found it somewhere on stackoverflow because I couldn't switch on a string) and
prints something accordingly to the operation. (Subscribed / Unsubscribed ...).

* If is not an opcode, then it is a message and proceeds to convert it
following the explanation given in the PDF.

## Coding style details
---

* I have tried to keep every line under 90 characters, not only 80.
* ClientMap is a typedef for map<string, Client> ... because this was too long
to write everytime.
* BUFF_CLEAR is a macro that sets the content of a buffer to 0.
