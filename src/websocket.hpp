#ifndef SYNCBLINKDESKTOPWEBSOCKET_H
#define SYNCBLINKDESKTOPWEBSOCKET_H

#include <vector>
#include <functional>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <client_messages.hpp>
#include <server_messages.hpp>
#include <event_registration.hpp>

namespace SyncBlink
{
    namespace Api
    {
        typedef std::function<void(Server::Message)> MessageReceivedEvent;
        typedef std::function<void(bool)> ConnectionEvent;

        class Websocket
        {
            public:
                Websocket(const std::string url);

                void start();
                void stop();
                void send(Client::Message message);

                EventRegistration<ConnectionEvent> connectionEvents;
                EventRegistration<MessageReceivedEvent> messageEvents;

            private:
                void onMessage(const ix::WebSocketMessagePtr& msg);

                std::string _url;
                ix::WebSocket _socket;
        };
    }
}

#endif // SYNCBLINKDESKTOPWEBSOCKET_H