#ifndef SYNCBLINKDESKTOPTCPCLIENT_H
#define SYNCBLINKDESKTOPTCPCLIENT_H

#include <vector>
#include <functional>
#include <asio.hpp>
#include <client_messages.hpp>
#include <server_messages.hpp>
#include <event_registration.hpp>

namespace SyncBlink
{
    namespace Api
    {
        typedef std::function<void(Server::MessageType, std::vector<uint8_t>)> MessageReceivedEvent;
        typedef std::function<void(bool)> ConnectionEvent;

        struct TcpMessage
        {
            uint8_t messageType;
            std::vector<uint8_t> message;
        };

        class TcpClient
        {
            public:
                TcpClient(const std::string tcpServerIp);

                void start();
                void stop();
                void send(void* message, uint32_t messageSize, Client::MessageType messageType);

                EventRegistration<ConnectionEvent> connectionEvents;
                EventRegistration<MessageReceivedEvent> messageEvents;

            private:
                void startRead();

                std::string _tcpServerIp;
                asio::io_context _ioContext;
                asio::ip::tcp::socket _socket;
                asio::ip::tcp::resolver _resolver;

                uint8_t _singleByte[1];
                bool _connected = false;
        };
    }
}

#endif // SYNCBLINKDESKTOPTCPCLIENT_H