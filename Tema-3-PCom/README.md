* Dumitru Radu-Andrei 322CB

# Tema 3 PCOM

* A short description of the implementation. The header files contain
descriptions for the functions used.

## JSON parser
* I've used nlohmann json because it was easy to install and to use.

## Register and Login
* They both use the same template. Basically, they make a json with username
and password and send a POST request. For login, I have implemented a 'parsing'
method for the cookie received in the response.

## Entry library
* Using the cookie recieved, a GET request is sent to the server. The server
should respond with a JWT token, which will be needed later.

## Get books
* Using the JWT token received, a GET request is sent to the server. The server
should respond with an array of JSON objects. The objects are printed to
stdout.

## Get book
* Using the JWT token received, the client is asked for a book id. A GET
request is sent to the server. The server should respond with a json object
which is printed to stdout.

## Add book
* Using the JWT token received, the client is asked for the details of the book.
A POST request is sent to the server. The server should respond with a message
with no body if it is a success.

## Delete book
* Using the JWT token received, the client is asked for a book id. A DELETE
request is sent to the server. The server should respond with a message with
no body if it is a success.

## Logout
* Using the cookie received at login, a GET request is sent to the server.
