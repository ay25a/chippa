#include "helpers.hpp"

#ifdef _WIN32
#include <windows.h>
#include <limits.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

std::string GetExecutablePath(){
#ifdef _WIN32
  char buffer[MAX_PATH];
  DWORD size = GetModuleFileNameA(NULL, buffer, MAX_PATH);
  if (size == 0 || size == MAX_PATH) 
    return "";

  return std::string(buffer, size);
#elif defined(__APPLE__)
  char buffer[4096];
  uint32_t size = sizeof(buffer);
  if (_NSGetExecutablePath(buffer, &size) != 0) 
    return "";

  return std::string(buffer);
#else
  char buffer[4096];
  ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
  if (len == -1) 
    return "";
  
  buffer[len] = '\0';
  return std::string(buffer);
#endif
}

bool CreateDirectory(std::string_view path){
#ifdef _WIN32
  DWORD attrs = GetFileAttributesA(path.c_str());
  if (attrs != INVALID_FILE_ATTRIBUTES)
    return (attrs & FILE_ATTRIBUTE_DIRECTORY);
    
  CreateDirectoryA(path.data(), NULL);
#else
  struct stat st;
  if (stat(path.data(), &st) == 0)
    return S_ISDIR(st.st_mode);

  return mkdir(path.data(), 0755) == 0;
#endif
}