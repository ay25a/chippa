/// @file database.hpp
/// @brief Simple file-based storage for fixed-size byte records.
///
/// OVERVIEW:
/// This module is a simple database system that stores structs as a byte
/// sequence in a file.
///
/// KEY DESIGN PRINCIPLES:
/// - Entries are stored and read from a file path that is specified by
/// `ENTITY_TYPE::FILE`.
/// - Records are a fixed-size byte sequence, with the size being
/// `sizeof(ENTITY_TYPE)`.
/// - Records in the file are always sorted by an integer `id` in ascending
/// order.
/// - Operations load the file into memory a `vector`, do functionality, then
/// write back the
///   modified `vector` into the file.
///
/// `ENTITY_TYPE` REQUIREMENTS:
/// - `ENTITY_TYPE` should be a struct or class with an integer `id` for sorting
/// and searching,
///   a static string (or const char*) with a file path (ideally `constexpr`),
///   and a `match` member method that takes an `ENTITY_TYPE` as a parameter,
///   and returns a boolean.
///
/// IMPORTANT NOTES:
/// - File corruption handling, and more ideal storage haven't been added to
/// keep the assignment simple.
/// - `std::lower_bound` is used to perform a binary search, returning an
/// iterator to the first found element,
///   or an iterator to the location where it should've been if it isn't found.
/// - `template` are used to avoid wasting time and memory writing functions
/// with exact functionality for
///   different records. (and new records can be easily added if needed)
#pragma once

#include <algorithm>
#include <fstream>
#include <functional>
#include <vector>

/// @brief Get a list (`vector`) of all existing records in the file.
/// @param file is an open `std::fstream` to read from.
/// @return reference to the local static `vector` containing the records.
/// @note The vector will be created once per runtime (`static`) for each
/// record type; Therefore, data will be loaded once O(n) and cached.
/// Operations will be done on the vector reference then synced to the file
template <class ENTITY_TYPE>
inline std::vector<ENTITY_TYPE> &db_get_records(std::fstream &file) {
  static std::vector<ENTITY_TYPE> records;

  // If the vector contain records, then no need to re-read the file
  // or if the file is not open, then return the empty vector
  if(!records.empty() || !file.is_open())
    return records;

  file.seekg(0, std::ios_base::end);
  size_t fileSize = static_cast<size_t>(file.tellg());
  file.seekg(0, std::ios_base::beg);

  // File is corrupted, since file size should be multiples of
  // `ENTITY_TYPE` size
  if (fileSize % sizeof(ENTITY_TYPE) != 0)
    throw std::runtime_error("Database files had been tampered with! please delete the files");
  
  // Number of records = Total bytes / Record size
  records.resize(fileSize / sizeof(ENTITY_TYPE));
  file.read(reinterpret_cast<char *>(records.data()), records.size() * sizeof(ENTITY_TYPE));

  return records;
}

/// @brief Return an unmodifiable reference to the cached records
/// @return const reference to `vector` containing the records.
template <class ENTITY_TYPE>
inline const std::vector<ENTITY_TYPE> &db_get_records() {
  std::fstream f(ENTITY_TYPE::FILE, std::ios_base::binary | std::ios_base::in);
  return db_get_records<ENTITY_TYPE>(f);
}

/// @brief Finds a record by id using binary search O(log n)
/// @param id The id of the requested record
/// @param out The variable pointer to write the found record into (can be a nullptr)
/// @return true if record is found, and false if not found
template <class ENTITY_TYPE>
inline bool db_find_by_id(int id, ENTITY_TYPE *out) {
  const std::vector<ENTITY_TYPE>& records = db_get_records<ENTITY_TYPE>();
  if (records.size() == 0)
    return false;

  // Binary search
  auto it = std::lower_bound(records.begin(), records.end(), id,
                        [](const ENTITY_TYPE &ent, int v) { return ent.id < v; });

  if (it == records.end() || it->id != id)
    return false;

  if (out != nullptr)
    *out = *it;

  return true;
}

/// @brief Insert a record into the file while keeping it sorted
/// @param rec a Record to insert into the file
/// @return true if added, and false if nothing added
template <class ENTITY_TYPE> 
inline bool db_add_record(const ENTITY_TYPE &rec) {
  std::fstream f(ENTITY_TYPE::FILE, 
    std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  // File doesn't exist; Create the file then open it again
  if (!f.is_open()) {
    std::ofstream temp(ENTITY_TYPE::FILE, std::ios_base::app);
    temp.close();

    f.open(ENTITY_TYPE::FILE,
           std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  }

  std::vector<ENTITY_TYPE>& records = db_get_records<ENTITY_TYPE>(f);

  // Get the insertion position
  auto it = std::lower_bound(records.begin(), records.end(), rec.id,
                       [](const ENTITY_TYPE &e, int v) { return e.id < v; });

  // The record already exists
  if (it != records.end() && it->id == rec.id)
    return false;

  records.insert(it, rec);

  // Move the cursor to the beginning of the file and override it
  f.seekp(0);
  f.write(reinterpret_cast<char *>(records.data()), records.size() * sizeof(ENTITY_TYPE));
  f.close();

  return true;
}

/// @brief Delete a record using the record id
/// @param id The id of the record to be deleted
/// @return true if deleted, and false if not deleted
template <class ENTITY_TYPE> 
inline bool db_delete_record(int id) {
  std::fstream f(ENTITY_TYPE::FILE, 
    std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  // File doesn't exist; No records yet
  if (!f.is_open())
    return false;

  std::vector<ENTITY_TYPE>& records = db_get_records<ENTITY_TYPE>(f);
  if(records.empty())
    return false;

  auto it = std::lower_bound(records.begin(), records.end(), id,
                       [](const ENTITY_TYPE &e, int v) { return e.id < v; });

  // Item doesn't exist
  if (it == records.end() || it->id != id)
    return false;

  records.erase(it);

  // NOTE: Closes then reopens the file in `std::ios_base::trunc` mode deleting
  // all its content. Mainly done because the new records size is less than the
  // exisiting records size (If wrote normally, last record will be duplicated)
  f.close();
  f.open(ENTITY_TYPE::FILE, std::ios_base::trunc | std::ios_base::out | std::ios_base::binary);
  f.write(reinterpret_cast<char *>(records.data()), records.size() * sizeof(ENTITY_TYPE));
  f.close();

  return true;
}

/// @brief Updates a record using the record.id
/// @param mod The modified record (must match the id)
/// @return true if updated, and false if nothing updated
template <class ENTITY_TYPE>
inline bool db_update_record(const ENTITY_TYPE &mod) {
  std::fstream f(ENTITY_TYPE::FILE, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  
  // File doesn't exist; No records
  if (!f.is_open())
    return false;

  std::vector<ENTITY_TYPE>& records = db_get_records<ENTITY_TYPE>(f);
  auto it = std::lower_bound(records.begin(), records.end(), mod.id,
                       [](const ENTITY_TYPE &e, int v) { return e.id < v; });

  // Record id doesn't exist
  if (it == records.end() || it->id != mod.id)
    return false;

  *it = mod;
  // Move the cursor to the beginning of the file and override it
  f.seekp(0);
  f.write(reinterpret_cast<char *>(records.data()), records.size() * sizeof(ENTITY_TYPE));
  f.close();

  return true;
}

/// @brief Finds a record based on a predicate using linear search O(n)
/// @param pred a predicate function that takes `ENTITY_TYPE` record as a parameter,
/// returning true if the record matches, and false if doesn't match
/// @return All records with values that match with the predicate
template <class ENTITY_TYPE>
inline std::vector<ENTITY_TYPE> db_find(const std::function<bool(const ENTITY_TYPE& e)> pred) {
  const std::vector<ENTITY_TYPE>& records = db_get_records<ENTITY_TYPE>();

  // Match using the predicate function
  std::vector<ENTITY_TYPE> res;
  for (const auto &rec : records) {
    if (pred(rec))
      res.push_back(rec);
  }

  return res;
}

/// @brief Returns a new id based on the last record id (Incremental ID)
/// @return (last record id + 1) or 1 if there is no records
template <class ENTITY_TYPE> 
inline int db_get_next_id() {
  const std::vector<ENTITY_TYPE>& records = db_get_records<ENTITY_TYPE>();
  if (records.empty()) 
    return 1;

  return records.back().id + 1;
}
