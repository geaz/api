#include <iostream>
#include "syncblink_api.hpp"

namespace SyncBlink
{
    namespace Api
    {
        SyncBlinkApi::SyncBlinkApi(std::string url, AudioAnalyzerSource source) : _tcpClient(url), _apiSource(source)
        { 
            _tcpClient.messageEvents.addEventHandler(
                [this](Server::MessageType messageType, std::vector<uint8_t> payload) { onMessageReceived(messageType, payload); });
            _freqAnalyzer.frequencyEvents.addEventHandler(
                [this](AudioAnalyzerMessage message) { onFrequencyCalculated(message); });
        }

        void SyncBlinkApi::start()
        {
            _freqAnalyzer.start();
            _tcpClient.start();
        }

        void SyncBlinkApi::stop()
        {
            _freqAnalyzer.stop();
            _tcpClient.stop();
        }

        void SyncBlinkApi::onMessageReceived(Server::MessageType messageType, std::vector<uint8_t> payload)
        {
            switch(messageType)
            {
                case Server::DISTRIBUTE_MOD:
                    _tcpClient.send(0, 0, Client::MOD_DISTRIBUTED);
                    break;
                case Server::SOURCE_UPDATE:                    
                    _currentSource = (AudioAnalyzerSource)payload[0];
                    break;
            }
        }

        void SyncBlinkApi::onFrequencyCalculated(AudioAnalyzerMessage analyzerMessage)
        {
            if(_currentSource != AudioAnalyzerSource::Desktop) return;
            _tcpClient.send(&analyzerMessage, sizeof(analyzerMessage), Client::EXTERNAL_ANALYZER); 
        }
    }
}