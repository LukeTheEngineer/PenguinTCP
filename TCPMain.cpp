//Boost includes
#include<boost/asio.hpp>
#include<boost/chrono.hpp>
#include<boost/thread.hpp>

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
            } catch(const boost::system::system_error& ex) {
                std::cerr << "Boost system error: " << ex.what() << std::endl;
                throw;
            } catch(const std::exception& ex) {
                std::cerr << "Standard exception: " << ex.what() << std::endl;
                throw;
            } catch(...) {
                std::cerr << "Unknown exception." << std::endl;
                throw;
            }
            // Return a shared pointer to the io_context object
            return io_context_ptr;
        }

    //Initialize server using a shared pointer and with a specified amount of time.
        static inline std::shared_ptr<boost::asio::io_context> InitializeChronoServer(const unsigned short& port, unsigned short specTime)
        {
            std::shared_ptr<boost::asio::io_context> io_context_ptr = std::make_shared<boost::asio::io_context>();
            try {
                // Create the server object
                Penguin::Server serverINIT(*io_context_ptr, port);

                // Create the time functionality
                boost::chrono::duration<int> duration(specTime);
                auto std_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<int, std::milli>(duration.count()));

                std::cout << "Initializing server on port: " << port << "..." << std::endl;

                // Wait for a short time before continuing
                boost::chrono::seconds wait_time{3};
                boost::this_thread::sleep_for(wait_time);


                // Run the event loop for the specified duration
                do {
                    std::cout << "\nInitialized server on port: " << port << std::endl;
                } while (io_context_ptr->run_for(std_duration));
                // Count down from the duration in seconds to 0
                for (int i = std_duration.count() / 1000; i >= 0; --i) {
                    std::cout << i << " seconds remaining" << std::endl;

                    // Wait for 1 second
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }

                std::cout << "Server cleaned up." << std::endl;

            } catch (const boost::system::system_error& ex) {
                std::cerr << "Boost system error: " << ex.what() << std::endl;
                throw;
            } catch (const std::exception& ex) {
                std::cerr << "Standard exception: " << ex.what() << std::endl;
                throw;
            } catch (...) {
                std::cerr << "Unknown exception." << std::endl;
                throw;
            }
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