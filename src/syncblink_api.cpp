#include <iostream>
#include "syncblink_api.hpp"

namespace SyncBlink
{
    namespace Api
    {
        SyncBlinkApi::SyncBlinkApi(std::string url, uint64_t analyzerId, std::string analyzerName) : _tcpClient(url), _analyzerId(analyzerId)
        { 
            _analyzerNameLength = analyzerName.size() > MaxNodeLabelLength ? MaxNodeLabelLength : analyzerName.size();
            memcpy(&_analyzerName[0], &analyzerName[0], _analyzerNameLength);

            _tcpClient.messageEvents.addEventHandler([this](Server::MessageType messageType, std::vector<uint8_t> payload) { onMessageReceived(messageType, payload); });
            _tcpClient.connectionEvents.addEventHandler([this](bool connected) { onConnection(connected); });
            _freqAnalyzer.frequencyEvents.addEventHandler([this](AudioAnalyzerMessage message) { onFrequencyCalculated(message); });
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

        void SyncBlinkApi::onConnection(bool connected)
        {
            if(connected) 
            {                            
                Client::ConnectionMessage message = { false, true, false, _analyzerId, 0, 0, 0, 0 };
                memcpy(&message.nodeLabel[0], &_analyzerName[0], _analyzerNameLength);

                _tcpClient.send(&message, sizeof(message), Client::MESH_CONNECTION);
            }
        }

        void SyncBlinkApi::onMessageReceived(Server::MessageType messageType, std::vector<uint8_t> payload)
        {
            switch(messageType)
            {
                case Server::DISTRIBUTE_SCRIPT:
                    _tcpClient.send(0, 0, Client::SCRIPT_DISTRIBUTED);
                    break;
                case Server::MESH_UPDATE:                
                {
                    Server::UpdateMessage message;
                    memcpy(&message, &payload[0], payload.size());
                    _currentSource = message.activeAnalyzer;
                    break;
                }
                case Server::SOURCE_UPDATE:
                {
                    uint64_t targetClientId = 0;
                    memcpy(&targetClientId, &payload[0], payload.size());
                    _currentSource = targetClientId;
                    break;
                }
            }
        }

        void SyncBlinkApi::onFrequencyCalculated(AudioAnalyzerMessage analyzerMessage)
        {
            if(_currentSource != _analyzerId) return;
            _tcpClient.send(&analyzerMessage, sizeof(analyzerMessage), Client::EXTERNAL_ANALYZER); 
        }
    }
}