#pragma once

#include <QObject>
#include <QMutex>
#include <SDL.h>
#include <QVector>

#define CPU_CLOCK 3494400
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1024
#define TIMING_BUFFER_SIZE 2048

class SDLBeeper : public QObject
{
    Q_OBJECT

public:
    explicit SDLBeeper(QObject* parent = nullptr);
    ~SDLBeeper();

    void init();
    void shutdown();

    void setBeeperState(bool state, unsigned long tstate);
    static void audioCallback(void* userdata, Uint8* stream, int len);
    void generateAudio(int16_t* buffer, int samples);
    void setVolume(float volume) { m_volume = volume; }

private:
    struct TimingEntry {
        unsigned long tstate = 0;
        bool state = false;
    };

    bool isStateBetween(unsigned long start, unsigned long end, unsigned long test);

    QMutex m_mutex;

    QVector<TimingEntry> m_timingBuffer;
    int m_timingBufferPos;
    int m_timingBufferReadPos;

    SDL_AudioDeviceID m_audioDevice;

    bool m_beeperState;
    unsigned long m_lastPosition;
    float m_currentLevel;
    float m_filterState;
    float m_volume;
    float m_sampleStep;
    unsigned long m_silenceCounter; 

    bool m_initialized;

    unsigned long m_lastStateChange;    // Track when the beeper state last changed
    bool m_forceQuiet;                  // Force quiet mode flag
};

// C-callable interface
extern "C" {
    void initSDLBeeper(void);
    void deinitSDLBeeper(void);
    void sdlBeeperPortWrite(uint8_t value, unsigned long ticks);
    void sdlBeeperSetVolume(float volume);
}

