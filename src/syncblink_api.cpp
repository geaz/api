#include <iostream>
#include "syncblink_api.hpp"

namespace SyncBlink
{
    namespace Api
    {
        SyncBlinkApi::SyncBlinkApi(std::string url, AudioAnalyzerSource source) : websocket(url), _apiSource(source)
        { 
            websocket.messageEvents.addEventHandler(
                [this](SyncBlink::Server::Message message) { onMessageReceived(message); });
            websocket.connectionEvents.addEventHandler(
                [this](bool connected) { onConnection(connected); });
            _freqAnalyzer.frequencyEvents.addEventHandler(
                [this](AudioAnalyzerMessage message) { onFrequencyCalculated(message); });
        }

        void SyncBlinkApi::start()
        {
            websocket.start();
        }

        void SyncBlinkApi::stop()
        {
            _freqAnalyzer.stop();
            websocket.stop();
        }

        void SyncBlinkApi::onConnection(const bool connected)
        {
            if(connected) _freqAnalyzer.start();
            else _freqAnalyzer.stop();
        }

        void SyncBlinkApi::onMessageReceived(const Server::Message message)
        {
            switch(message.messageType)
            {
                case Server::MESH_COUNT_REQUEST:
                    websocket.send(createAnswer(message, Client::MESH_COUNTED));
                    break;
                case Server::MESH_UPDATE:
                    websocket.send(createAnswer(message, Client::MESH_UPDATED));
                    break;
                case Server::SOURCE_UPDATE:
                    _currentSource = message.sourceMessage.source;
                    break;
            }
        }

        void SyncBlinkApi::onFrequencyCalculated(AudioAnalyzerMessage analyzerMessage)
        {
            if(_currentSource != AudioAnalyzerSource::Desktop) return;

            Client::Message message = { 0, Client::EXTERNAL_ANALYZER };
            message.audioAnalyzerMessage = analyzerMessage;

            websocket.send(message); 
        }

        Client::Message SyncBlinkApi::createAnswer(const Server::Message message, const Client::MessageType answerMessageType) const
        {
            Client::Message answerMessage = { message.id, answerMessageType };
            if(answerMessageType == Client::MESH_COUNTED)
            {
                Client::CountedMessage counted = { message.countMessage.meshLedCount };
                answerMessage.countedMessage = counted;
            }
            return answerMessage;
        }
    }
}