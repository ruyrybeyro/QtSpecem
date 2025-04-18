// SDLBeeper.cpp - Rewritten with persistent timing buffer
#include "SDLBeeper.h"
#include <QDebug>
#include <cmath>
#include <cstring>

// Global instance
SDLBeeper* g_sdlBeeper = nullptr;

extern "C" {

void initSDLBeeper(void)
{
    if (!g_sdlBeeper) {
        g_sdlBeeper = new SDLBeeper();
        g_sdlBeeper->init();
    }
}

void deinitSDLBeeper(void)
{
    if (g_sdlBeeper) {
        delete g_sdlBeeper;
        g_sdlBeeper = nullptr;
    }
}

void sdlBeeperPortWrite(uint8_t value, unsigned long ticks)
{
    if (g_sdlBeeper) {
        bool ear = (value & 0x10) != 0;
        g_sdlBeeper->setBeeperState(ear, ticks % 69888);
    }
}

void sdlBeeperSetVolume(float volume)
{
    if (g_sdlBeeper) {
        g_sdlBeeper->setVolume(volume);
    }
}

} // extern "C"

SDLBeeper::SDLBeeper(QObject* parent)
    : QObject(parent),
      m_timingBufferPos(0),
      m_timingBufferReadPos(0),
      m_audioDevice(0),
      m_beeperState(false),
      m_lastPosition(0),
      m_currentLevel(0.0f),
      m_filterState(0.0f),
      m_volume(0.75f),
      m_sampleStep(static_cast<float>(CPU_CLOCK) / SAMPLE_RATE),
      m_silenceCounter(0),  // Initialize the new silence counter
      m_initialized(false)
{
    m_timingBuffer.resize(TIMING_BUFFER_SIZE);
}

SDLBeeper::~SDLBeeper()
{
    shutdown();
}

void SDLBeeper::init()
{
    if (m_initialized) return;

    if (!(SDL_WasInit(0) & SDL_INIT_AUDIO)) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            qWarning() << "Failed to initialize SDL Audio:" << SDL_GetError();
            return;
        }
    }

    SDL_AudioSpec wanted;
    SDL_memset(&wanted, 0, sizeof(wanted));
    wanted.freq = SAMPLE_RATE;
    wanted.format = AUDIO_S16;
    wanted.channels = 2;
    wanted.samples = BUFFER_SIZE;
    wanted.callback = audioCallback;
    wanted.userdata = this;

    SDL_AudioSpec obtained;
    m_audioDevice = SDL_OpenAudioDevice(nullptr, 0, &wanted, &obtained, 0);

    if (m_audioDevice == 0) {
        qWarning() << "Failed to open SDL audio:" << SDL_GetError();
        return;
    }

    SDL_PauseAudioDevice(m_audioDevice, 0);
    m_initialized = true;
}

void SDLBeeper::shutdown()
{
    if (!m_initialized) return;

    if (m_audioDevice) {
        SDL_CloseAudioDevice(m_audioDevice);
        m_audioDevice = 0;
    }

    m_initialized = false;
}

void SDLBeeper::setBeeperState(bool state, unsigned long tstate)
{
    QMutexLocker lock(&m_mutex);

    int nextPos = (m_timingBufferPos + 1) % TIMING_BUFFER_SIZE;
    if (nextPos != m_timingBufferReadPos) {
        m_timingBuffer[m_timingBufferPos] = { tstate, state };
        m_timingBufferPos = nextPos;
        m_beeperState = state;
    }
}

bool SDLBeeper::isStateBetween(unsigned long start, unsigned long end, unsigned long test)
{
    return (start <= end) ? (test >= start && test < end) : (test >= start || test < end);
}

void SDLBeeper::audioCallback(void* userdata, Uint8* stream, int len)
{
    std::memset(stream, 0, len);
    SDLBeeper* beeper = static_cast<SDLBeeper*>(userdata);
    int16_t* buffer = reinterpret_cast<int16_t*>(stream);
    int samples = len / (sizeof(int16_t) * 2);
    beeper->generateAudio(buffer, samples);
}

void SDLBeeper::generateAudio(int16_t* buffer, int samples)
{
    QMutexLocker lock(&m_mutex);
    
    // Very aggressive silence detection
    if (m_timingBufferReadPos == m_timingBufferPos) {
        m_silenceCounter += samples;
        
        // After ~0.25 seconds of silence (11025 samples), force quiet
        if (m_silenceCounter > 11025) {
            m_currentLevel = 0.0f;
            m_filterState = 0.0f;
            m_forceQuiet = true;
            std::memset(buffer, 0, samples * sizeof(int16_t) * 2);
            return;
        }
        
        // If current level is very small, output silence
        if (fabsf(m_currentLevel) < 0.001f) {
            m_currentLevel = 0.0f;
            std::memset(buffer, 0, samples * sizeof(int16_t) * 2);
            return;
        }
    } else {
        m_silenceCounter = 0;
        m_forceQuiet = false;
    }
    
    // If we're in forced quiet mode, output silence
    if (m_forceQuiet) {
        std::memset(buffer, 0, samples * sizeof(int16_t) * 2);
        return;
    }

    const float tstatesPerSample = m_sampleStep;
    unsigned long currentTState = m_lastPosition;
    bool currentState = m_beeperState;
    int pos = 0;
    bool hadActivity = false;

    for (int i = 0; i < samples; ++i) {
        unsigned long nextTState = (currentTState + (unsigned long)tstatesPerSample) % 69888;
        unsigned long highTicks = 0;
        unsigned long lowTicks = 0;
        unsigned long start = currentTState;
        unsigned long end = nextTState;

        while (m_timingBufferReadPos != m_timingBufferPos) {
            hadActivity = true;
            auto& entry = m_timingBuffer[m_timingBufferReadPos];
            if (!isStateBetween(start, end, entry.tstate)) break;

            unsigned long delta = (entry.tstate >= start)
                                  ? (entry.tstate - start)
                                  : (69888 - start + entry.tstate);

            if (currentState) highTicks += delta;
            else lowTicks += delta;

            start = entry.tstate;
            currentState = entry.state;
            m_timingBufferReadPos = (m_timingBufferReadPos + 1) % TIMING_BUFFER_SIZE;
        }

        unsigned long delta = (end >= start) ? (end - start) : (69888 - start + end);
        if (currentState) highTicks += delta;
        else lowTicks += delta;

        float rawSample = 0.0f;
        
        if (highTicks + lowTicks > 0) {
            rawSample = ((float)highTicks - (float)lowTicks) / (highTicks + lowTicks + 1);
        }

        // Very aggressive decay and filtering
        if (hadActivity) {
            // Normal filtering when we have activity
            float decay = 0.9975f;
            float stiffness = 0.015f;
            m_currentLevel = m_currentLevel * decay + (rawSample - m_currentLevel) * stiffness;
        } else {
            // Much more aggressive decay when no activity
            m_currentLevel *= 0.98f;  // Significantly increase decay rate Rui
        }
        
        // Zero out very small values
        if (fabsf(m_currentLevel) < 0.001f) {
            m_currentLevel = 0.0f;
        }

        float out = std::clamp(m_currentLevel * m_volume, -1.0f, 1.0f);
        int16_t output = static_cast<int16_t>(out * 32767.0f);
        buffer[pos++] = output;
        buffer[pos++] = output;
        currentTState = nextTState;
    }

    m_lastPosition = currentTState;
}

