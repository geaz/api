#include <iostream>
#include <inttypes.h>
#include <network/tcp/messages/client_messages.hpp>

#include "tcp_client.hpp"

namespace SyncBlink
{
    namespace Api
    {
        TcpClient::TcpClient(const std::string tcpServerIp) : 
            _tcpServerIp(tcpServerIp), _socket(_ioContext), _resolver(_ioContext)
        { }

        void TcpClient::start()
        {
            std::cout << "Connecting...\n";
            _socket.async_connect(_resolver.resolve(_tcpServerIp, "81")->endpoint(), [this](const asio::error_code& ec) {                
                if (ec)
                {
                    std::cout << "Connect error: " << ec.message() << "\n";
                    _socket.close();
                }
                else
                {
                    std::cout << "Connected!\n";
                    _connected = true;
                    startRead();

                    for(auto event : connectionEvents.getEventHandlers())
                        event.second(_connected);
                }
            });
            _ioContext.run();
        }

        void TcpClient::stop()
        {
            asio::error_code ignored_ec;
            _socket.close(ignored_ec);
            _connected = false;
        }

        void TcpClient::send(void* message, uint32_t messageSize, Client::MessageType messageType)
        {
            if(_connected)
            {
                uint32_t packageSize = messageSize + SocketHeaderSize;
            
                std::vector<uint8_t> package;
                package.resize(packageSize);

                package[0] = SocketMagicBytes[0];
                package[1] = SocketMagicBytes[1];
                package[2] = SocketMagicBytes[2];
                package[3] = (uint8_t)(messageSize>>24);
                package[4] = (uint8_t)(messageSize>>16);
                package[5] = (uint8_t)(messageSize>>8);
                package[6] = (uint8_t)(messageSize>>0);

                uint8_t checksum = 0;
                for(uint8_t i = 0; i<7; i++)
                {
                    checksum += package[i] % 2;
                }
                package[7] = checksum;
                package[8] = messageType;
                if(messageSize > 0) memcpy(&package[9], message, messageSize);

                asio::write(_socket, asio::buffer(package, packageSize));
            }
        }

        void TcpClient::startRead()
        {
            asio::async_read(_socket, asio::buffer(_singleByte, 1), [this](const asio::error_code& ec, std::size_t n) {
                if (ec)
                {
                    std::cout << "Error: " << ec.message() << "\n";
                    _socket.close();
                }
                else
                {
                    if(_singleByte[0] == SocketMagicBytes[0])
                    {
                        uint8_t magicBuf[2];
                        size_t readLen = asio::read(_socket, asio::buffer(magicBuf, 2));
                        if(readLen == sizeof(magicBuf) && magicBuf[0] == SocketMagicBytes[1] && magicBuf[1] == SocketMagicBytes[2])
                        {
                            uint8_t messageHeader[6];
                            readLen = asio::read(_socket, asio::buffer(messageHeader, 6));
                            if(readLen == sizeof(messageHeader))
                            {
                                uint32_t messageSize = 
                                    (messageHeader[0]<<24) + 
                                    (messageHeader[1]<<16) + 
                                    (messageHeader[2]<<8) + 
                                    messageHeader[3];
                                uint8_t messageChecksum = messageHeader[4];
                                uint8_t messageType = messageHeader[5];

                                uint8_t checksum = 0;
                                checksum += SocketMagicBytes[0] % 2;                    
                                checksum += SocketMagicBytes[1] % 2;
                                checksum += SocketMagicBytes[2] % 2;
                                checksum += messageHeader[0] % 2;
                                checksum += messageHeader[1] % 2;
                                checksum += messageHeader[2] % 2;
                                checksum += messageHeader[3] % 2;

                                if(checksum == messageChecksum)
                                {
                                    TcpMessage tcpMessage;
                                    tcpMessage.messageType = messageType;
                                    tcpMessage.message.resize(messageSize);

                                    uint32_t readBytes = 0;
                                    while(readBytes < messageSize)
                                    {
                                        readBytes += asio::read(_socket, asio::buffer(&tcpMessage.message[readBytes], messageSize-readBytes));
                                    }
                                    //printf("Found message - Size: %zi, Type: %i\n", tcpMessage.message.size() + SocketHeaderSize, tcpMessage.messageType);
                                    for(auto event : messageEvents.getEventHandlers())
                                        event.second((Server::MessageType)tcpMessage.messageType, tcpMessage.message);
                                }
                            }
                        }
                    }
                    startRead();
                }                
            });
        }
    }
}