#include <iostream>
#include "glowfly_api.hpp"

namespace GlowFly
{
    namespace Api
    {
        GlowFlyApi::GlowFlyApi(std::string url, AnalyzerSource source) : websocket(url), _apiSource(source)
        { 
            websocket.commandEvents.addEventHandler(
                [this](GlowFly::Client::Command command) { onCommandReceived(command); });
            websocket.connectionEvents.addEventHandler(
                [this](bool connected) { onConnection(connected); });
            _freqAnalyzer.frequencyEvents.addEventHandler(
                [this](AnalyzerCommand command) { onFrequencyCalculated(command); });
        }

        void GlowFlyApi::start()
        {
            websocket.start();
        }

        void GlowFlyApi::stop()
        {
            _freqAnalyzer.stop();
            websocket.stop();
        }

        void GlowFlyApi::onConnection(const bool connected)
        {
            if(connected) _freqAnalyzer.start();
            else _freqAnalyzer.stop();
        }

        void GlowFlyApi::onCommandReceived(const Client::Command command)
        {
            switch(command.commandType)
            {
                case GlowFly::Client::CommandType::MESH_COUNT_REQUEST:
                    std::cout << "MESH COUNT REQUEST\n";
                    websocket.send(createAnswer(command, GlowFly::Server::CommandType::MESH_COUNTED));
                    break;
                case GlowFly::Client::CommandType::MESH_UPDATE:
                    std::cout << "MESH UPDATE\n";
                    websocket.send(createAnswer(command, GlowFly::Server::CommandType::MESH_UPDATED));
                    break;
                case Client::CommandType::SOURCE_UPDATE:
                    std::cout << "SOURCE UPDATE\n";
                    _currentSource = command.sourceCommand.source;
                    break;
                case Client::CommandType::MOD_REMOVED:
                    std::cout << "MOD REMOVED\n";
                    break;
                case Client::CommandType::ANALYZER_UPDATE:
                    break;
            }
        }

        void GlowFlyApi::onFrequencyCalculated(AnalyzerCommand command)
        {
            if(_currentSource != AnalyzerSource::Desktop) return;

            Server::Command serverCommand = { 0, Server::CommandType::EXTERNAL_ANALYZER };
            serverCommand.analyzerCommand = command;

            websocket.send(serverCommand); 
        }

        Server::Command GlowFlyApi::createAnswer(const Client::Command command, const Server::CommandType answerCommandType) const
        {
            Server::Command answerCommand = { command.id, answerCommandType };
            if(answerCommandType == GlowFly::Server::MESH_COUNTED)
            {
                Server::CountedCommand counted = { command.countCommand.meshLedCount };
                answerCommand.countedCommand = counted;
            }
            return answerCommand;
        }
    }
}