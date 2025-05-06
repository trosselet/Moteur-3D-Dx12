#include "Components/AudioComponent.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "DebugMacro.h"

#include <future>

#include "AudioManager.h"


void Engine::AudioComponent::Play(const char* audioPath)
{
    if (!m_audioData.pBuffer)
    {
        if (!Load(audioPath))
        {
            PRINT_CONSOLE_OUTPUT("[MUSIC] Error loading audio file: " << audioPath << "\n");
            return;
        }
    }

    m_playRequested = true;
}

void Engine::AudioComponent::SetVolume(float32 volume)
{
    if (m_audioData.pSourceVoice)
    {
        float32 audioVolume = std::clamp(volume, 0.0f, 100.0f);

        m_audioData.pSourceVoice->SetVolume(audioVolume / 100.0f);
    }
}

void Engine::AudioComponent::Set3DPosition(Vector3f pos)
{
    m_position = pos;
}

Engine::AudioComponent::~AudioComponent()
{
    Release();
}

bool Engine::AudioComponent::Load(const char* audioPath)
{
    if (!m_pXAudio2 || !m_pMasterVoice)
    {
        AudioManager& audioManager = AudioManager::GetInstance();
        if (!audioManager.Initialize())
        {
            PRINT_CONSOLE_OUTPUT("[MUSIC] AudioManager init failed\n");
            return false;
        }

        m_pXAudio2 = audioManager.GetXAudio2();
        m_pMasterVoice = audioManager.GetMasterVoice();

    }

    std::string path = "../Gameplay/audio/" + std::string(audioPath);


    std::ifstream file(path, std::ios::binary);
    if (!file) return false;

    char chunkId[4], format[4];
    DWORD chunkSize = 0;

    file.read(chunkId, 4);
    file.read((char*)&chunkSize, 4);
    file.read(format, 4);
    if (strncmp(chunkId, "RIFF", 4) || strncmp(format, "WAVE", 4)) return false;

    while (file && !file.eof()) {
        char subChunkId[4];
        DWORD subChunkSize = 0;
        file.read(subChunkId, 4);
        file.read((char*)&subChunkSize, 4);

        if (strncmp(subChunkId, "fmt ", 4) == 0) {
            file.read((char*)&m_audioData.format, subChunkSize);
        }
        else if (strncmp(subChunkId, "data", 4) == 0) {
            m_audioData.pBuffer = new BYTE[subChunkSize];
            file.read((char*)m_audioData.pBuffer, subChunkSize);
            m_audioData.bufferSize = subChunkSize;
            break;
        }
        else {
            file.seekg(subChunkSize, std::ios::cur);
        }
    }

    return m_audioData.pBuffer != nullptr;

}

void Engine::AudioComponent::Release()
{
    delete[] m_audioData.pBuffer;
}
