# New

- Storage: Handles index and data files, calls Entity::write and Entity::read.
  Has an integrity check, id collision, and cache.
- Entity: Has the actual data and the logic to write and read itself.
- Index: Maps id to offset and length in the data file.
