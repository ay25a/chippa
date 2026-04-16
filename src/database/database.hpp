#pragma once

#include <memory.h>
#include <vector>
#include <fstream>
#include <cassert>
#include <algorithm>

/*
* Translate the whole file into a vector of `Entity_T` and
* returns the vector
*/
template <class Entity_T>
inline std::vector<Entity_T> db_get_records(std::fstream& f){
  f.seekg(0, std::ios_base::end);
  size_t size = static_cast<size_t>(f.tellg());
  f.seekg(0, std::ios_base::beg);

  std::vector<Entity_T> arr(size / sizeof(Entity_T));

  // Reading failure due to corrupted data, unlikely if integration check had ran first
  assert(f.read(reinterpret_cast<char*>(arr.data()), arr.size() * sizeof(Entity_T)));

  return arr;
}

#define ENTRY_NOT_FOUND -1
/*
* Uses binary search to find an entity based on a unique id in the file
* without loading every entity into memory.
* Returns either `ENTITY_NOT_FOUND` or the index of the found entity.
*/
template<class Entity_T>
inline int db_find_by_id(int id, Entity_T& out){
  std::fstream f(Entity_T::FILE, std::ios_base::binary | std::ios_base::in);

  // File couldn't be open, likely empty
  if(!f.is_open()) 
    return ENTRY_NOT_FOUND;

  auto arr = db_get_records<Entity_T>(f);
  if(arr.size() == 0) 
    return ENTRY_NOT_FOUND;

  // Binary search the array
  auto it = std::lower_bound(arr.begin(), arr.end(), id, [](const Entity_T& ent, int v){
    return ent.id < v;
  });

  if(it == arr.end() || it->id != id)
    return ENTRY_NOT_FOUND;
  else
    return static_cast<size_t>(it - arr.begin());
}

/*
* 
*/
template <class Entity_T>
inline bool db_add_record(const Entity_T& rec){
  std::fstream f(Entity_T::FILE, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  // Creates the file if not exit
  if(!f.is_open()){
    std::ofstream temp(Entity_T::FILE, std::ios_base::app);
    temp.close();
    f.open(Entity_T::FILE, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  }

  // File should be open
  assert(f.is_open());

  auto arr = db_get_records<Entity_T>(f);

  // Binary search for inseartion position
  auto it = std::lower_bound(arr.begin(), arr.end(), rec.id, [](const Entity_T& e, int v){
    return e.id < v;
  });

  if(it != arr.end() && it->id == rec.id)
    return false;

  arr.insert(it, rec);
  
  f.seekp(0);
  f.write(reinterpret_cast<char*>(arr.data()), arr.size() * sizeof(Entity_T));

  return true;
}

template <class Entity_T>
inline bool db_delete_record(int id){
  std::fstream f(Entity_T::FILE, std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::app);

  // File couldn't be open, likely empty
  if(!f.is_open()) 
    return false;

  auto arr = db_get_records<Entity_T>(f);
  auto it = std::lower_bound(arr.begin(), arr.end(), id, [](const Entity_T& e, int v){
    return e.id < v;
  });
  
  if(it == arr.end() || it->id != id)
    return false;

  arr.erase(it);
  f.write(reinterpret_cast<char*>(arr.data()), arr.size() * sizeof(Entity_T));
  f.close();

  return true;
}

template <class Entity_T>
inline bool db_update_record(const Entity_T& mod){
  std::ofstream f(Entity_T::FILE, std::ios_base::out | std::ios_base::binary);
  if(!f.is_open()) 
    return false;

  Entity_T old{};
  auto index = db_find_by_id(mod.id, old);
  if(index == ENTRY_NOT_FOUND)
    return false;
  
  f.seekp(index * sizeof(Entity_T));
  f.write(reinterpret_cast<char*>(&mod), sizeof(Entity_T));
  f.close();

  return true;
}

template<class Entity_T>
inline std::vector<Entity_T> db_find(const Entity_T& filter){
  std::ifstream f(Entity_T::FILE, std::ios_base::binary | std::ios_base::in);
  if(!f.is_open()) 
    return {};

  auto arr = db_get_records<Entity_T>(f);

  std::vector<Entity_T> res;
  for(const auto& e: arr){
    if(e.match(filter)) 
      res.push_back(e);
  }

  f.close();
  return res;
}