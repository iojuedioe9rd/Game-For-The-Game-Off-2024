#include "vxpch.h"
#include "WindowsAudio.h"
#include <Windows.h>
#include <mmsystem.h>
#include <conio.h>
#include <filesystem>

#include "miniaudio.h"

#pragma once(lib, Winmm.lib)


namespace Vertex {



    static ma_engine globalEngine;
    static int engineRefCount = 0;

    void InitializeAudioEngine() {
        if (engineRefCount == 0) {
            ma_result result = ma_engine_init(NULL, &globalEngine);
            if (result != MA_SUCCESS) {
                VX_CORE_ASSERT(false, "Audio Engine Initialization Failed!");
            }
        }
        engineRefCount++;
    }

    void ShutdownAudioEngine() {
        if (engineRefCount > 0) {
            engineRefCount--;
            if (engineRefCount == 0) {
                ma_engine_uninit(&globalEngine);
            }
        }
    }

    WindowsAudio::WindowsAudio(const char* filepath, bool loop)
        : m_filepath(filepath), m_loop(loop)
    {
        InitializeAudioEngine();

        ma_result result = ma_sound_init_from_file(
            &globalEngine,
            m_filepath,
            0,
            NULL,
            NULL,
            &m_sound
        );

        if (result != MA_SUCCESS) {
            std::string error = "Failed to initialize sound: " + std::string(filepath);
            VX_CORE_ASSERT(false, error.c_str());
        }

        ma_sound_set_looping(&m_sound, loop);
    }

    WindowsAudio::~WindowsAudio()
    {
        ma_sound_uninit(&m_sound);
        ShutdownAudioEngine();
    }

    void WindowsAudio::Play()
    {
        VX_CORE_INFO("Playing Audio: {0}", m_filepath);
        ma_sound_start(&m_sound);
    }

    void WindowsAudio::Stop()
    {
        VX_CORE_INFO("Stopping Audio: {0}", m_filepath);
        ma_sound_stop(&m_sound);
    }


}