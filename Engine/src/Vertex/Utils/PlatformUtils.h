#pragma once

#include "Vertex/Core/Base.h"
#include "Vertex/Core/Timestep.h"
#include <string>



typedef unsigned char tex_id;
// 568

namespace Vertex {

	class VERTEX_API FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

	class VERTEX_API TextureUtils
	{
	public:
		static tex_id* load_tex(const std::string& path, int* x, int* y, int* channels_in_file, int desired_channels);
		static tex_id* load_tex_win_rc(int resourceID, const std::string& format, int* width, int* height, int* channels);

		static void kill_tex(tex_id* tex);
	};

	class VERTEX_API Time
	{
	public:
		static float GetTime();

		static Timestep& GetTS()
		{
			return s_Timestep;
		}
		static float GetFPS();

        static uint32_t GetTicks()
        {
            return s_Ticks;
        }
		
	private:
		static Timestep s_Timestep;

		static float FPS;
        static uint32_t s_Ticks;



		friend class Application;
		
	};

    class VERTEX_API DynamicLibraryInstance {
    public:
        DynamicLibraryInstance(const std::string& libName) : libName(libName), hLibrary(nullptr) {}

        ~DynamicLibraryInstance() {
            if (hLibrary) {
                FreeLibraryWrapper(hLibrary);
            }
        }

        bool loadLibrary() {
            hLibrary = LoadLibraryWrapper(libName.c_str());
            if (!hLibrary) {
                std::cerr << "Could not load the library!" << std::endl;
                return false;
            }
            return true;
        }

        bool loadFunction(const std::string& funcName) {
            if (!hLibrary) {
                std::cerr << "Library not loaded!" << std::endl;
                return false;
            }

            void* funcAddr = GetProcAddressWrapper(hLibrary, funcName.c_str());
            if (!funcAddr) {
                std::cerr << "Could not locate the function: " << funcName << std::endl;
                return false;
            }

            // Store the function pointer in the map
            functionPointers[funcName] = funcAddr;
            return true;
        }

        template <typename Func>
        Func getFunction(const std::string& funcName) {
            if (functionPointers.find(funcName) != functionPointers.end()) {
                return reinterpret_cast<Func>(functionPointers[funcName]);
            }
            if (loadFunction(funcName))
            {
                return reinterpret_cast<Func>(functionPointers[funcName]);
            }
            return nullptr;
        }

        void* getRawFunctionPointer(const std::string& funcName) {
            if (functionPointers.find(funcName) != functionPointers.end()) {
                return functionPointers[funcName];
            }
            return nullptr;
        }

    private:
        static void* GetProcAddressWrapper(void* hLibrary, const char* funcName);
        static void* LoadLibraryWrapper(const char* name);
        static void FreeLibraryWrapper(void* hLibrary);

        std::string libName;
        void* hLibrary;
        std::unordered_map<std::string, void*> functionPointers;  // Map of function names to pointers
    };
}