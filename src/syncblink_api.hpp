#ifndef SYNCBLINKAPI_H
#define SYNCBLINKAPI_H

#include <event_registration.hpp>
#include <server_messages.hpp>
#include "websocket.hpp"
#include "frequency_analyzer.hpp"

namespace SyncBlink
{
    namespace Api
    {
        class SyncBlinkApi
        {
            public:
                SyncBlinkApi(std::string url, AudioAnalyzerSource source);
                void start();
                void stop();

                Websocket websocket;

            private:
                void onConnection(const bool connected);
                void onMessageReceived(const Server::Message message);
                void onFrequencyCalculated(AudioAnalyzerMessage message);
                Client::Message createAnswer(const Server::Message message, const Client::MessageType answerMessageType) const;

                AudioAnalyzerSource _apiSource;
                FrequencyAnalyzer _freqAnalyzer;
                AudioAnalyzerSource _currentSource = AudioAnalyzerSource::Station;
        };
    }
}

extern "C" {
    typedef void* syncblink_api;

    __declspec(dllexport) syncblink_api syncblink_api_init(const char *url, int source)
    {
        return new SyncBlink::Api::SyncBlinkApi(url, static_cast<SyncBlink::AudioAnalyzerSource>(source));
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
            .messageEvents
            .addEventHandler([=](SyncBlink::Server::Message message)
            {
                switch(message.messageType)
                {
                    case SyncBlink::Server::MessageType::ANALYZER_UPDATE:
                        SyncBlink::AudioAnalyzerMessage analyzerMessage = message.analyzerMessage;
                        fn(analyzerMessage.volume, analyzerMessage.frequency);
                        break;
                }
            });
    }
}

#endif // SyncBlinkDESKTOP_H