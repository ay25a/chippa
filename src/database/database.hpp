/** @file database.hpp
 *  @brief File-based binary database for storing fixed-size entity records.
 *
 *  DESIGN OVERVIEW:
 *  ================
 *  This module is a simple file-based database system for fixed-size binary records. 
 *  Essentially, it implements core CRUD operations (Create, Read, Update, Delete).
 *
 *  KEY DESIGN PRINCIPLES:
 *  - Each entity type stores all records in a single binary file (defined by Entity_T::FILE)
 *    (ex. `User::FILE` should contain the binary file name we read from)
 *  - Records are stored as fixed-size binary sequence (`sizeof(Entity_T)` bytes per record)
 *    (ex. sizeof(User) is 76 bytes, so each record in the file should be 76 bytes sequence)
 *  - Stored records are always sorted by `Entity_T.id` in ascending order, enabling searching
 *    using binary search O(log n) instead of linear search O(n)
 *  - Operations load the file into memory, modify it, and rewrite the entire file
 *
 *  ENTITY REQUIREMENTS:
 *  - Each entity struct must have:
 *    * An `int id` field (unique and used for sorting)
 *    * A static `const char* FILE` (ideally `constexpr`) representing the file path
 *    * A `bool match(const Entity_T& filter)` method for filtering operations
 *
 *  IMPORTANT NOTES:
 *  - Corruption handling is not implemented, and inefficient file handling is used
 *    instead of a more ideal version to not overcomplicate the assignment
 */

#pragma once

#include <vector>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <memory.h>


// Load all records from a binary file into an array `vector`.
// NOTE: Assumes the file and records aren't corrupted
template <class ENTITY_TYPE>
inline std::vector<ENTITY_TYPE> db_get_records(std::fstream& f){
  // Gets the file size in bytes, and moves the cursor to the beginning
  f.seekg(0, std::ios_base::end);
  size_t size = static_cast<size_t>(f.tellg());
  f.seekg(0, std::ios_base::beg);

  // Number of records = Total bytes / Record size
  std::vector<ENTITY_TYPE> arr(size / sizeof(ENTITY_TYPE));

  // Load all the bytes into the vector (if fails then the file has been tampered with)
  assert(f.read(reinterpret_cast<char*>(arr.data()), arr.size() * sizeof(ENTITY_TYPE)));

  return arr;
}

#define ENTRY_NOT_FOUND -1

// Try finding a record by its unique `id`
// If found, then copy the record into the `out` param and returns the index
// If not found, returns `ENTRY_NOT_FOUND`
template<class Entity_T>
inline int db_find_by_id(int id, Entity_T* out){
  std::fstream f(Entity_T::FILE, std::ios_base::binary | std::ios_base::in);

  // File couldn't be open: File doesn't exist yet (no content)
  if(!f.is_open()) 
    return ENTRY_NOT_FOUND;

  auto arr = db_get_records<Entity_T>(f);
  if(arr.size() == 0) 
    return ENTRY_NOT_FOUND;

  // `std::lower_bound` performs binary search on the sorted array.
  // It returns an iterator to the first element whose `id` is >= the target `id`.
  // If the exact `id` exists, it will point to that element.
  auto it = std::lower_bound(arr.begin(), arr.end(), id, [](const Entity_T& ent, int v){
    return ent.id < v;
  });

  // If iterator is out of bounds OR does not match the target id, record doesn't exist
  if(it == arr.end() || it->id != id)
    return ENTRY_NOT_FOUND;
  else {
    // If we want to get the Entry (`out` is a pointer), then we copy the 
    // found entry into `out`
    if(out != nullptr) *out = *it;
    // Return the position of the found element
    return static_cast<int>(it - arr.begin());
  }
}

// Insert a record into the file while keeping it SORTED by `record.id`
// Returns `true` if added, and `false` if not added
template <class Entity_T>
inline bool db_add_record(const Entity_T& rec){
  std::fstream f(Entity_T::FILE, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  // If file doesn't exist yet, We create the file
  if(!f.is_open()){
    std::ofstream temp(Entity_T::FILE, std::ios_base::app);
    temp.close();
    f.open(Entity_T::FILE, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  }

  auto arr = db_get_records<Entity_T>(f);

  // Binary search the array for an insertion position that keeps the array sorted
  auto it = std::lower_bound(arr.begin(), arr.end(), rec.id, [](const Entity_T& e, int v){
    return e.id < v;
  });

  // If the record already exists, then we don't add anything
  if(it != arr.end() && it->id == rec.id)
    return false;

  // Inserts the record into the `it` position, and shifts other elements
  arr.insert(it, rec);

  // Moves the cursor position to the beginning, then writes the whole vector
  // `arr` into the file
  f.seekp(0);
  f.write(reinterpret_cast<char*>(arr.data()), arr.size() * sizeof(Entity_T));

  return true;
}

// Deletes a record from the file based on the record `id`.
// Returns `true` if deleted and `false` if not deleted
template <class Entity_T>
inline bool db_delete_record(int id){
  std::fstream f(Entity_T::FILE, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  // File doesn't exist yet
  if(!f.is_open()) 
    return false;

  auto arr = db_get_records<Entity_T>(f);

  // Find the location of the record 
  auto it = std::lower_bound(arr.begin(), arr.end(), id, [](const Entity_T& e, int v){
    return e.id < v;
  });
  
  // IF the record doesn't exist, we don't do anything
  if(it == arr.end() || it->id != id)
    return false;

  // Erases the record
  arr.erase(it);

  // Reopens the file in `trunc` mode, to erase the whole file content
  // NOTE: we do this because the new content length is less than the old content
  // Then writes the array into file overriding its content
  f.close();
  f.open(Entity_T::FILE, std::ios_base::trunc | std::ios_base::out);
  f.write(reinterpret_cast<char*>(arr.data()), arr.size() * sizeof(Entity_T));

  return true;
}

// Updates a record directly in the file
// NOTE: The only operation at which we don't override the file
// Because the output size is always equal to the input size
template <class Entity_T>
inline bool db_update_record(const Entity_T& mod){
  std::fstream f(Entity_T::FILE, std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  // File doesn't exist yet
  if(!f.is_open()) 
    return false;

  // We get the location of the record we want to modify
  auto index = db_find_by_id<Entity_T>(mod.id, nullptr);
  if(index == ENTRY_NOT_FOUND)
    return false;

  // Moves the cursor to the record location in the file
  // Location in file = Index in an array * Record size in bytes 
  f.seekp(index * sizeof(Entity_T));
  f.write(reinterpret_cast<const char*>(&mod), sizeof(Entity_T));
  f.close();

  return true;
}

// Searches for all the records with a matching values from `filter`.
// Returns all the matched records
template<class Entity_T>
inline std::vector<Entity_T> db_find(const Entity_T& filter){
  std::fstream f(Entity_T::FILE, std::ios_base::binary | std::ios_base::in);

  // File doesn't exist yet
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