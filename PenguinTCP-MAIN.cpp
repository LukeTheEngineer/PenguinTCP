//Source files
#include"TCPMain.cpp"

//System includes
#include<iostream>

// TODO: Create a port number and simply plug it into the io_context_ptr variable to get started.

int main(){
    const unsigned short CONSTANT_PORT; //1234;

    
    //std::shared_ptr<boost::asio::io_context> io_context_ptr = Penguin::Server::InitializeServer(CONSTANT_PORT);

    std::shared_ptr<boost::asio::io_context> io_context_ptr = Penguin::Server::InitializeChronoServer(CONSTANT_PORT, 25000);

    return 0;
}