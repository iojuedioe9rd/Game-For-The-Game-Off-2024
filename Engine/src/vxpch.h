#pragma once
#include "Vertex/Core/PlatformDetection.h"
#include "Vertex/Core/Base.h"
#include "Vertex/Core/Logger.h"
#include "Vertex/Debug/Instrumentor.h"
#include "Vertex/CommandBuffer/CommandBuffer.h"
#include "Vertex/Utils/Utils.h"
#include "Vertex/Core/Buffer.h"

#include <glm/glm.hpp>

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <array>
#include <functional>
#include <execution> // For parallel execution policies
#include <random>


#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>


#ifdef VX_PLATFORM_WINDOWS

#include <Windows.h>

#endif // VX_PLATFORM_WINDOWS

