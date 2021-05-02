#ifndef CFREQUENCYANALYZER_H
#define CFREQUENCYANALYZER_H

#include <chrono>
#include <array>
#include <functional>
#include <RtAudio.h>
#include <kiss_fft.h>
#include <mappings.hpp>
#include <shared_constants.hpp>
#include <band_pass_filter.hpp>
#include <event_registration.hpp>

namespace SyncBlink
{
    namespace Api
    {
        typedef std::function<void(AudioAnalyzerMessage)> FrequencyEvent;

        static const float EfAlpha = 0.4f;
        static const int8_t MinDB = -50;

        class FrequencyAnalyzer
        {
            public:
                bool start();
                void stop();

                EventRegistration<FrequencyEvent> frequencyEvents;

            private:
                std::array<float, HalfFFTDataSize> calculateAmplitudes(const kiss_fft_cpx* cx) const;
                uint16_t getDominantFrequency(std::array<float, HalfFFTDataSize> amplitudes);
                
                static int getAnalyzerResult(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, 
                    double streamTime, RtAudioStreamStatus status, void *userData);

                RtAudio _adc = RtAudio(RtAudio::Api::WINDOWS_WASAPI);
                RtAudio::StreamParameters _parameters;

                BandPassFilter _bandPassFilter;
                float _lastDominantFrequency = 0;    
                std::chrono::system_clock::time_point _lastUpdate;
        };
    }
}

#endif // CFREQUENCYANALYZER_H