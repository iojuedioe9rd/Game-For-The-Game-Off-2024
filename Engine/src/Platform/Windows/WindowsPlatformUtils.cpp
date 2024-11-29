#include "vxpch.h"
#include "Vertex/Utils/PlatformUtils.h"

#include <sstream>
#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Vertex/Core/Application.h"
#include "Vertex/Core/Logger.h"
#include <stb_image.h>

wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

namespace Vertex {

	// Convert narrow string to wide string
	std::wstring StringToWideString(const std::string& str) {
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
		std::wstring wideStr(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wideStr[0], size_needed);
		return wideStr;
	}

	unsigned char* LoadTextureFromResource(int resourceID, const char* format, int* width, int* height, int* channels) {
		// Get the handle to the current module
		HMODULE hModule = GetModuleHandle(NULL);

		// Convert format to wide string for FindResourceW
		std::wstring wideFormat = StringToWideString(format);

		// Use FindResourceW to find the resource
		HRSRC hResource = FindResourceW(hModule, MAKEINTRESOURCE(resourceID), wideFormat.c_str());
		if (!hResource) {
			std::cerr << "Failed to find resource!" << std::endl;
			return nullptr;
		}

		// Load the resource
		HGLOBAL hLoadedResource = LoadResource(hModule, hResource);
		if (!hLoadedResource) {
			std::cerr << "Failed to load resource!" << std::endl;
			return nullptr;
		}

		// Get a pointer to the resource data
		void* pResourceData = LockResource(hLoadedResource);
		DWORD resourceSize = SizeofResource(hModule, hResource);
		if (!pResourceData || resourceSize == 0) {
			std::cerr << "Failed to lock resource!" << std::endl;
			return nullptr;
		}

		// Load the image using stbi_load_from_memory
		stbi_set_flip_vertically_on_load(1);
		unsigned char* imageData = stbi_load_from_memory(reinterpret_cast<unsigned char*>(pResourceData), resourceSize, width, height, channels, 0);
		if (!imageData) {
			std::cerr << "Failed to load image from memory!" << std::endl;
		}

		return imageData;
	}


	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[568] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[568] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}

	tex_id* TextureUtils::load_tex(const std::string& path, int* x, int* y, int* channels_in_file, int desired_channels)
	{
		
		return stbi_load(path.c_str(), x, y, channels_in_file, desired_channels);
	}

	tex_id* TextureUtils::load_tex_win_rc(int resourceID, const std::string& format, int* width, int* height, int* channels)
	{
		return LoadTextureFromResource(resourceID, format.c_str(), width, height, channels);
	}


	void TextureUtils::kill_tex(tex_id* tex)
	{
		stbi_image_free(tex);
	}

	Timestep Time::s_Timestep;
	float Time::GetTime()
	{
		return glfwGetTime();
	}
	/*
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	uint32_t counter = 0;
	*/

	float Time::FPS;
	uint32_t Time::s_Ticks;
	
	float Time::GetFPS()
	{
		return FPS;
	}

	void* DynamicLibraryInstance::GetProcAddressWrapper(void* hLibrary, const char* funcName)
	{
		return GetProcAddress(static_cast<HMODULE>(hLibrary), funcName);
	}
	

	void* DynamicLibraryInstance::LoadLibraryWrapper(const char* name)
	{
		void* hLibrary = LoadLibrary(convertCharArrayToLPCWSTR(name));
		if (!hLibrary) {
			DWORD errorCode = GetLastError();
			LPVOID errorMsg;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMsg, 0, NULL);
			VX_CORE_ERROR("Failed to load library \"{}\". {} - {}", name, errorCode, static_cast<char*>(errorMsg));
			//printLastError();
			return false;
		}
		return hLibrary;
	}
	void DynamicLibraryInstance::FreeLibraryWrapper(void* hLibrary) 
	{
		
		FreeLibrary(static_cast<HMODULE>(hLibrary));
	}

	

	// unsigned char* LoadTextureFromResource(int resourceID, const char* format, int* width, int* height, int* channels)
}
