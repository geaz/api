#include <iostream>
#include <inttypes.h>

#include "websocket.hpp"

namespace SyncBlink
{
    namespace Api
    {
        Websocket::Websocket(const std::string url) : _url(url)
        {
            ix::initNetSystem();

            _socket.setUrl(url);
            _socket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) { onMessage(msg); });
        }

        void Websocket::start()
        {
            _socket.start();
        }

        void Websocket::stop()
        {
            _socket.start();
        }

        void Websocket::send(Client::Message message)
        {
            size_t messageSize = sizeof(message);
            std::vector<uint8_t> serializedMessage;
            serializedMessage.resize(sizeof(message));
            memcpy(&serializedMessage[0], &message, messageSize);                                 

            _socket.sendBinary(std::string(serializedMessage.begin(), serializedMessage.end()));
        }

        void Websocket::onMessage(const ix::WebSocketMessagePtr& msg)
        {
            if(msg->type == ix::WebSocketMessageType::Open)
                for(auto event : connectionEvents.getEventHandlers()) event.second(true);
            else if(msg->type == ix::WebSocketMessageType::Close)
                for(auto event : connectionEvents.getEventHandlers()) event.second(false);
            else if(msg->type == ix::WebSocketMessageType::Message)
            {
                if(msg->binary)
                {
                    Server::Message receivedMessage;
                    memcpy(&receivedMessage, &msg->str.c_str()[0], msg->str.length());
                    for(auto event : messageEvents.getEventHandlers()) event.second(receivedMessage);
                }
                else send({ 0, Client::MOD_DISTRIBUTED });
            }
        }
    }
}