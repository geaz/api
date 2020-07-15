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
                void run();

            private:
                void onConnection(const bool connected);
                void onCommandReceived(const Client::Command command);
                void onFrequencyCalculated(const float decibel, const uint8_t volume, const uint16_t dominantFrequency, const std::array<uint8_t, BAR_COUNT> buckets);
                Server::Command createAnswer(const Client::Command command, const Server::CommandType answerCommandType) const;

                Websocket _websocket;
                FrequencyAnalyzer _freqAnalyzer;
                AnalyzerSource _currentSource = AnalyzerSource::Basis;
        };
    }
}

extern "C" {
    typedef void* glowfly_api;

    __declspec(dllexport) glowfly_api glowfly_api_start(const char *url, int source)
    {
        return new GlowFly::Api::GlowFlyApi(url, static_cast<GlowFly::AnalyzerSource>(source));
    }

    __declspec(dllexport) void glowfly_api_on_freq(
        glowfly_api m,
        void (*fn)(glowfly_api m, uint8_t volume, float decibel, uint16_t dominant_frequency))
    {
       // static_cast<GlowFly::Api::GlowFlyApi*>(m)->
    }
}

#endif // GlowFlyDESKTOP_H