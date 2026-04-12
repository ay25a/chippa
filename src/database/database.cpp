#ifdef _WIN32
#include <windows.h>
#include <limits.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include <fstream>

#include "database.hpp"


static std::string s_DBPath;
static std::string GetExecutablePath();
static void CreateDirectory(std::string_view path);

void InitializeDatabase(){
  std::string path = GetExecutablePath();
  path = path.substr(0, path.find_last_of("\\/"));

  s_DBPath = path + "/storage";
  CreateDirectory(path);
}


std::vector<std::string> ReadAttributes(std::string_view content, size_t &i) {
  std::vector<std::string> res;
    
  size_t end = content.find(DB_ENTRY_DILM, i);
  if (end == std::string_view::npos) 
    end = content.size();
    
  std::string_view section = content.substr(i, end - i);
  i = end;

  while (!section.empty()) {
    size_t delim = section.find(DB_ATTR_DILM);
    res.emplace_back(section.substr(0, delim));
    if (delim == std::string_view::npos) 
      break;

    section.remove_prefix(delim + 1);
  }
  return res;
}

std::string ReadFile(std::string_view path){
  std::ifstream file(path.data());
  if(!file.is_open()){
    WriteFile(path, "");
    return "";
  }

  file.seekg(0, std::ios_base::end);

  std::streampos pos = file.tellg();
  size_t size = static_cast<size_t>(pos);
  file.seekg(0, std::ios_base::beg);

  std::string res;
  res.resize(size);
  file.read(res.data(), size);
  file.close();

  return res;
}

void WriteFile(std::string_view path, std::string_view content){
  std::ofstream file(path.data(), std::ios_base::app);
  file.write(content.data(), content.size());
  file.close();
}

std::string GetUsersFilePath(){ return s_DBPath + "/Users.txt"; }


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

void CreateDirectory(std::string_view path){
#ifdef _WIN32
  DWORD attrs = GetFileAttributesA(path.c_str());
  if (attrs == INVALID_FILE_ATTRIBUTES)
    CreateDirectoryA(path.data(), NULL);
#else
  struct stat st;
  if (stat(path.data(), &st) != 0)
    mkdir(path.data(), 0755);
#endif
}