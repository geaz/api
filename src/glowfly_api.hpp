#ifndef GLOWFLYAPI_H
#define GLOWFLYAPI_H

#include <event_registration.hpp>
#include <client_commands.hpp>
#include "websocket.hpp"
#include "frequency_analyzer.hpp"

namespace GlowFly
{
    namespace Api
    {
        class GlowFlyApi
        {
            public:
                GlowFlyApi(std::string url, AnalyzerSource source);
                void start();
                void stop();

                Websocket websocket;

            private:
                void onConnection(const bool connected);
                void onCommandReceived(const Client::Command command);
                void onFrequencyCalculated(const float decibel, const uint8_t volume, const uint16_t dominantFrequency, const std::array<uint8_t, BAR_COUNT> buckets);
                Server::Command createAnswer(const Client::Command command, const Server::CommandType answerCommandType) const;

                AnalyzerSource _apiSource;
                FrequencyAnalyzer _freqAnalyzer;
                AnalyzerSource _currentSource = AnalyzerSource::Basis;
        };
    }
}

extern "C" {
    typedef void* glowfly_api;

    __declspec(dllexport) glowfly_api glowfly_api_init(const char *url, int source)
    {
        return new GlowFly::Api::GlowFlyApi(url, static_cast<GlowFly::AnalyzerSource>(source));
    }

    __declspec(dllexport) void glowfly_api_start(glowfly_api g)
    {
        static_cast<GlowFly::Api::GlowFlyApi*>(g)->start();
    }

    __declspec(dllexport) void glowfly_api_stop(glowfly_api g)
    {
        static_cast<GlowFly::Api::GlowFlyApi*>(g)->stop();
    }

    __declspec(dllexport) void glowfly_api_on_freq(
        glowfly_api g,
        void (*fn)(uint8_t volume, uint16_t dominant_frequency))
    {
        static_cast<GlowFly::Api::GlowFlyApi*>(g)->websocket
            .commandEvents
            .addEventHandler([=](GlowFly::Client::Command command)
            {
                switch(command.commandType)
                {
                    case GlowFly::Client::CommandType::ANALYZER_UPDATE:
                        GlowFly::Client::AnalyzerCommand analyzerCommand = command.analyzerCommand;
                        fn(analyzerCommand.volume, analyzerCommand.frequency);
                        break;
                }
            });
    }
}

#endif // GlowFlyDESKTOP_H