#ifndef SYNCBLINKDESKTOPWEBSOCKET_H
#define SYNCBLINKDESKTOPWEBSOCKET_H

#include <vector>
#include <functional>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <client_commands.hpp>
#include <server_commands.hpp>
#include <event_registration.hpp>

namespace SyncBlink
{
    namespace Api
    {
        typedef std::function<void(Client::Command)> CommandReceivedEvent;
        typedef std::function<void(bool)> ConnectionEvent;

        class Websocket
        {
            public:
                Websocket(const std::string url);

                void start();
                void stop();
                void send(Server::Command command);

                EventRegistration<ConnectionEvent> connectionEvents;
                EventRegistration<CommandReceivedEvent> commandEvents;

            private:
                void onMessage(const ix::WebSocketMessagePtr& msg);

                std::string _url;
                ix::WebSocket _socket;
        };
    }
}

#endif // SYNCBLINKDESKTOPWEBSOCKET_H