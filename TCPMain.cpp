//Boost includes
#include<boost/asio.hpp>


//System includes
#include<iostream>
#include<memory>



namespace Penguin{
    class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)) {}

        void start() {
            do_read();
        }

    private:
        void do_read() {
            auto self(shared_from_this());
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                [this, self](boost::system::error_code ec, std::size_t length) {
                    if (!ec) {
                        std::cout<<"[SUCCESSFUL] Established connection to TCP server. ";
                        do_write(length);
                    }
                    if(ec){
                        std::cout<<"Error connecting to TCP server. ";
                        return;
                    }
                });
        }

        void do_write(std::size_t length) {
            auto self(shared_from_this());
            boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                [this, self](boost::system::error_code ec, std::size_t) {
                    if (!ec) {
                        do_read();
                    }
                });
        }

        boost::asio::ip::tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
    };

    class Server {
    public:
    //Constructor that initializes a Server obj using a reference to an io_context and initializing a port within params.
        Server(boost::asio::io_context& io_context, short port)
            : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
            do_accept();
            std::cout<<"Constructor 1 of Server class selected. ";
        }

    //Constructor 2 that initializes an io_context within the params and creates and unsigned short port instead of just "short".
        Server(boost::asio::io_context& io_context, unsigned short port)
            : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)){
            do_accept();
            std::cout<<"Constructor 2 of Server class selected. ";
        }

    //Constructor similar to #number 2 but the only different is port is a reference rather than being defined within the params.
        Server(boost::asio::io_context& io_context, unsigned short &port)
            : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)){
            do_accept();
            std::cout<<"Constructor 3 of Server class selected. ";
        }

    //Initialize server using a shared pointer.
        static inline std::shared_ptr<boost::asio::io_context> InitializeServer(const unsigned short& port) {
            std::shared_ptr<boost::asio::io_context> io_context_ptr = std::make_shared<boost::asio::io_context>();
            try {
                Penguin::Server serverINIT(*io_context_ptr, port);
                std::cout << "\nInitialized server on port: " << port << std::endl;
                io_context_ptr->run();
            } catch(const std::exception& ec) {
                const char* Server_Exc = ec.what();
                std::cerr << "server exception: " << Server_Exc;
            }
            // Return a shared pointer to the io_context object
            return io_context_ptr;
        }

    //Deinitialize server using the same shared pointer from the InitializeServer function.
        static inline std::shared_ptr<boost::asio::io_context> DeinitializeServer(const std::shared_ptr<boost::asio::io_context>& Shared_Ptr){
            try{
                Shared_Ptr->stop();
                if(Shared_Ptr->stopped()){
                    std::cout<<"Deinitialized server."<<std::endl;
                }else{
                    std::cout<<"An error occurred while deinitializing server. "<<std::endl;
                }
            }
            catch(boost::system::system_error& BE){
                const char* Boost_ASIO_exc = BE.what();
                std::cerr<<"Boost ASIO exception: "<<Boost_ASIO_exc<<std::endl;
            }
            catch(std::exception& stdex){
                const char* STRD_exc = stdex.what();
                std::cerr<<"Standard exception: "<<STRD_exc<<std::endl;
            }
            return Shared_Ptr;
        }

    private:
        void do_accept() {
            acceptor_.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket))->start();
                }

                do_accept();
            });
        }
        boost::asio::ip::tcp::acceptor acceptor_;
    };

} 