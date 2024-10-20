#pragma once
#include "Vertex/Core/PlatformDetection.h"
#include "Vertex/Core/Base.h"
#include "Vertex/Core/Application.h"
#include "Vertex/Core/Logger.h"
#include "Vertex/Debug/Instrumentor.h"
#include "Vertex/Audio/AudioManager.h"
#include <Windows.h>

extern Vertex::Application* Vertex::CreateApp();
extern int Vertex_main(int argc, char** argv);

int VERTEX_API Vertex_main_getcmdline(std::function<int(int, char**)> vx_main);

#if defined(_MSC_VER)
# define console_ansi_main main
# if UNICODE
#  define console_wmain wmain
# endif
#endif
