Application.c - Handles and sends user input to the client for getting data from the server. Runs constantly.

Client.c - Bridges the gap between the application and the server. Only runs when the application calls it. Sends user input data from the application to the server and returns information from the server back to the application. Communicates with the server through TCP and UDP sockets. Decrypts the data received on the TCP socket before returning to the client.

Server.c. - Runs constantly waiting for a client to connect. Returns data to the client using TCP and UDP sockets and encrypts the data sent through the TCP socket.

Makefile - Compiles the programs and makes sure functions are compiled with the main programs they're called in.

CaesarCipher.c - Encrypts and decrypts strings passed to it and prints and returns them.

random_quote.c - Takes a file (Quote_File.txt), parses it and returns a random line (quote).

Application <-> Client Protocol: The application calls the client and tells it what information the user wants and sometimes provides information from the user (e.g. a country ID). The client takes this information from the application and returns the requested data, decrypted. Unless the application explicitly calls the client it doesn't run.

Client <-> Server Protocol: The client uses the appropriate socket (TCP or UDP) to request data from the server and provide user information (e.g. likes/dislikes). If using a TCP socket the server returns encrypted data to the client for it decrypt for the client. Otherwise it sends cleartext data. The server runs constantly waiting for a client to connect to it.

What I Learned
This project made me learn 
-how to use TCP and UDP sockets on the same server and choose depending on which client was connected
-how to configure a makefile so that called functions ran properly on the main programs that called them
-how to parse and return information from a CSV file
-how to test and debug programs
-how to comment (which I already knew to an extent, but I've never worked with a project this big before and so commenting became a necessity instead of an option so my code would make sense to myself and others)