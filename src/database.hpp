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
#include <vector>

/// @brief Get a list (`vector`) of all existing records in the file.
/// @param file is an open `std::fstream` to read from.
/// @return reference to the local static `vector` containing the records.
/// @note The vector will be created once per runtime (`static`) for each
/// record type; Therefore, data will be loaded once O(n) and cached; Therefore,
/// operations will be done on the vector reference then synced to the file
template <class ENTITY_TYPE>
inline std::vector<ENTITY_TYPE> &db_get_records(std::fstream &file) {
  static std::vector<ENTITY_TYPE> records;

  // If the vector contain records, then no need to re-read the file
  if(!records.empty())
    return records;

  file.seekg(0, std::ios_base::end);
  size_t fileSize = static_cast<size_t>(file.tellg());
  file.seekg(0, std::ios_base::beg);

  // Number of records = Total bytes / Record size
  records.resize(fileSize / sizeof(ENTITY_TYPE));

  file.read(reinterpret_cast<char *>(records.data()), records.size() * sizeof(ENTITY_TYPE));

  return records;
}

/// @brief Finds a record by id using binary search O(log n)
/// @param id The id of the requested record
/// @param out The `ENTITY_TYPE` variable pointer to write the found element
/// into (can be a nullptr)
/// @note If the `out` doesn't point to any variable (nullptr), the record won't
/// be written to it
/// @return a valid `ENTITY_TYPE` if found, and an default `ENTITY_TYPE` (with
/// id = -1) if not found
template <class ENTITY_TYPE>
inline bool db_find_by_id(int id, ENTITY_TYPE *out) {
  std::fstream f(ENTITY_TYPE::FILE, std::ios_base::binary | std::ios_base::in);

  // File doesn't exist; No records yet
  if (!f.is_open())
    return false;

  const std::vector<ENTITY_TYPE>& records = db_get_records<ENTITY_TYPE>(f);
  if (records.size() == 0)
    return false;

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
  f.open(ENTITY_TYPE::FILE, std::ios_base::trunc | std::ios_base::out);
  f.write(reinterpret_cast<char *>(records.data()),
          records.size() * sizeof(ENTITY_TYPE));

  return true;
}

/// @brief Updates a record using the record.id
/// @param mod The modified record entity to replace the old one (must match the
/// id)
/// @return true if updated, and false if nothing updated
template <class ENTITY_TYPE>
inline bool db_update_record(const ENTITY_TYPE &mod) {
  std::fstream f(ENTITY_TYPE::FILE, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  
  // File doesn't exist; No records yet!
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

/// @brief Finds a record based on a filter using linear search O(n)
/// @param filter an `ENTITY_TYPE` instance with the required values to search
/// for
/// @return All records with values that match with the filter values
template <class ENTITY_TYPE>
inline std::vector<ENTITY_TYPE> db_find(const ENTITY_TYPE &filter) {
  std::fstream f(ENTITY_TYPE::FILE, std::ios_base::binary | std::ios_base::in);

  // File doesn't exist; No records yet
  if (!f.is_open())
    return {};

  const std::vector<ENTITY_TYPE>& records = db_get_records<ENTITY_TYPE>(f);
  if(records.empty())
    return {};

  std::vector<ENTITY_TYPE> res;
  for (const auto &rec : records) {
    if (rec.match(filter))
      res.push_back(rec);
  }
  return res;
}

/// @brief Returns a new id based on the last record id (Incremental ID)
/// @return (last record id + 1) or 1 if there is no records
template <class ENTITY_TYPE> 
inline int db_get_next_id() {
  std::fstream f(ENTITY_TYPE::FILE, std::ios_base::binary | std::ios_base::in);

  // File doesn't exist; No records yet
  if (!f.is_open()) 
    return 1;

  const std::vector<ENTITY_TYPE>& records = db_get_records<ENTITY_TYPE>(f);
  if (records.empty()) 
    return 1;

  return records.back().id + 1;
}
