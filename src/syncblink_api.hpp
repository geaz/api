#ifndef SYNCBLINKAPI_H
#define SYNCBLINKAPI_H

#include <event_registration.hpp>
#include <client_commands.hpp>
#include "websocket.hpp"
#include "frequency_analyzer.hpp"

namespace SyncBlink
{
    namespace Api
    {
        class SyncBlinkApi
        {
            public:
                SyncBlinkApi(std::string url, AnalyzerSource source);
                void start();
                void stop();

                Websocket websocket;

            private:
                void onConnection(const bool connected);
                void onCommandReceived(const Client::Command command);
                void onFrequencyCalculated(AnalyzerCommand command);
                Server::Command createAnswer(const Client::Command command, const Server::CommandType answerCommandType) const;

                AnalyzerSource _apiSource;
                FrequencyAnalyzer _freqAnalyzer;
                AnalyzerSource _currentSource = AnalyzerSource::Base;
        };
    }
}

extern "C" {
    typedef void* syncblink_api;

    __declspec(dllexport) syncblink_api syncblink_api_init(const char *url, int source)
    {
        return new SyncBlink::Api::SyncBlinkApi(url, static_cast<SyncBlink::AnalyzerSource>(source));
    }

    __declspec(dllexport) void syncblink_api_start(syncblink_api g)
    {
        static_cast<SyncBlink::Api::SyncBlinkApi*>(g)->start();
    }

    __declspec(dllexport) void syncblink_api_stop(syncblink_api g)
    {
        static_cast<SyncBlink::Api::SyncBlinkApi*>(g)->stop();
    }

    __declspec(dllexport) void syncblink_api_on_freq(
        syncblink_api g,
        void (*fn)(uint8_t volume, uint16_t dominant_frequency))
    {
        static_cast<SyncBlink::Api::SyncBlinkApi*>(g)->websocket
            .commandEvents
            .addEventHandler([=](SyncBlink::Client::Command command)
            {
                switch(command.commandType)
                {
                    case SyncBlink::Client::CommandType::ANALYZER_UPDATE:
                        SyncBlink::AnalyzerCommand analyzerCommand = command.analyzerCommand;
                        fn(analyzerCommand.volume, analyzerCommand.frequency);
                        break;
                }
            });
    }
}

#endif // SyncBlinkDESKTOP_H