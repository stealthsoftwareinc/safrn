/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 *
 * Description: Utility functions for reading files.
 */

#include <set>
#include <string>
#include <vector>

#ifndef READ_FILE_UTILS_H
#define READ_FILE_UTILS_H

namespace file_reader_utils {

/* =============================== File Info ================================ */

// Returns just the filename portion of the filepath.
// For example: "foo/bar/zed.txt" would return "zed.txt".
extern std::string GetFileName(const std::string & filepath);
// Returns the directory of the given (full) filepath, or "." if the filepath
// contains no directory (as determined by presence of "/").
// For example, input "foo/bar/file.txt" would return "foo/bar".
extern std::string GetDirectory(
    const bool include_trailing_slash, const std::string & filepath);
// Same as above, using default include_trailing_slash = false.
inline std::string GetDirectory(const std::string & filepath) {
  return GetDirectory(false, filepath);
}

// Returns true if 'filename' exists. The input filename can either be w.r.t.
// working directory, or can be the full path (w.r.t. $HOME directory).
extern bool FileExists(const std::string & filename);
// Returns true if the directory exists. The input directory name can be
// the full path, or w.r.t. working directory.
extern bool DirectoryExists(const std::string & directory);

// Returns true if the system uses forward slashes for directory separators.
extern bool PathsUseForwardSlash();

/* =========================== Terminal Commands ============================ */
// This implements common terminal commands:
//   ls, pwd, mkdir
// Returns the working directory (where the executable is running).
extern std::string
GetWorkingDirectory(const bool include_trailing_slash);
// Same as above, using default include_trailing_slash = false.
inline std::string GetWorkingDirectory() {
  return GetWorkingDirectory(false);
}
// Returns CodeBase directory (currently: C:/Users/paulb/Stealth/CodeBase/)
// if the path exists; otherwise (e.g. if running on another machine) returns
// current working directory (via GetWorkingDirectory()).
extern std::string
GetCodeBaseDirectory(const bool include_trailing_slash);
// Same as above, using default include_trailing_slash = false.
inline std::string GetCodeBaseDirectory() {
  return GetCodeBaseDirectory(false);
}
// For given directory, find all the (non-directory) files in that directory.
// Returns false if the directory is not found; if directory is found but empty,
// returns true (but 'files' will be empty). 'directory' can be empty (interpreted as "./")
// and can either contain the final "/" suffix or not. Set 'full_path' to true if
// you want the list of files to include the full path (w.r.t. $HOME directory);
// o.w., 'files' just contains the local (i.e. within that directory) file names.
// NOTE: This method is robust for running on windows or unix: as long as it is
// compiled on the same system that it is run.
extern bool GetFilesInDirectory(
    const bool full_path,
    const std::string & directory,
    std::vector<std::string> * files);
// Same as above, uses default value 'true' for full_path.
inline bool GetFilesInDirectory(
    const std::string & directory, std::vector<std::string> * files) {
  return GetFilesInDirectory(true, directory, files);
}
// Same as above, with a 'set' instead of 'vector' as the output container.
extern bool GetFilesInDirectory(
    const bool full_path,
    const std::string & directory,
    std::set<std::string> * files);
// Same as above, uses default value 'true' for full_path.
inline bool GetFilesInDirectory(
    const std::string & directory, std::set<std::string> * files) {
  return GetFilesInDirectory(true, directory, files);
}
// Same as above, but returns all contents (files and directories) of the directory.
extern bool GetFilesAndFoldersInDirectory(
    const bool full_path,
    const std::string & directory,
    std::vector<std::string> * files);
inline bool GetFilesAndFoldersInDirectory(
    const std::string & directory, std::vector<std::string> * files) {
  return GetFilesAndFoldersInDirectory(true, directory, files);
}
extern bool GetFilesAndFoldersInDirectory(
    const bool full_path,
    const std::string & directory,
    std::set<std::string> * files);
inline bool GetFilesAndFoldersInDirectory(
    const std::string & directory, std::set<std::string> * files) {
  return GetFilesAndFoldersInDirectory(true, directory, files);
}
// Same as above, but returns only folders (not files) of the directory.
extern bool GetFoldersInDirectory(
    const bool full_path,
    const std::string & directory,
    std::vector<std::string> * files);
inline bool GetFoldersInDirectory(
    const std::string & directory, std::vector<std::string> * files) {
  return GetFoldersInDirectory(true, directory, files);
}
extern bool GetFoldersInDirectory(
    const bool full_path,
    const std::string & directory,
    std::set<std::string> * files);
inline bool GetFoldersInDirectory(
    const std::string & directory, std::set<std::string> * files) {
  return GetFoldersInDirectory(true, directory, files);
}

// Creates a directory at the indicated path (which is either an absolute path,
// or w.r.t. current working directory); possibly creating multiple directories
// along the way (if path is a nested, and multiple directories need creating).
// If the directory already exists, nothing is done.
// Returns false if directory did NOT already exist and was NOT able to be
// constructed; returns true otherwise.
extern bool CreateDir(const std::string & directory);
// Note: We do not provide a RemoveDir/RemoveFile() API, as this opens up the
// danger of programmtically removing files/directories on accident; i.e.
// forcing removal to be manual ensures that any such deletions are truly
// deliberate/intentional. This is *not* a feature of implementation
// difficulty, as such a function could be easily implemented, e.g. it
// could be exactly like CreateDir(), replacing the system call to
// 'mkdir' with 'rmdir'.

/* =========================== File Manipulation ============================ */
// Removes Windows formating. New lines on windows are marked by \r\n, where
// \r is Carriage Return and \n is New Line.
// Typically, when RemoveWindowsTrailingCharacters is called, it is from
// a line retrieved via 'getline', and hence the trailing \n has already
// been removed. But, we go ahead and check for it here anyway (in case
// caller did not first use 'getline').
// Thus, this functions removes \r and \r\n suffixes (\r is identified as
// char index 13, and \n is char index 10).
extern void RemoveWindowsTrailingCharacters(std::string * input);

/* ============================== Read File  ================================ */
// Reads the contents of 'filename' into 'output':
//   - If *output is null, will allocate memory (caller becomes owner) as needed,
//     not to excede max_bytes (if max_bytes == 0, no limit imposed).
//     Also, in this case, max_bytes will be updated with actual size.
//   - If *output is non-null, caller has pre-allocated max_bytes bytes.
//     In this case, only read up to this many bytes.
//     As above, max_bytes will be updated with number of bytes actually written.
extern bool ReadFile(
    const std::string & filename, uint64_t * max_bytes, char ** output);
// Same as above, but reads file in 'binary' mode.
extern bool ReadBinaryFile(
    const std::string & filename, uint64_t * max_bytes, char ** output);

// Reads (appends) the lines of a file into 'lines'.
extern bool ReadLines(
    const bool remove_windows_trailing_chars,
    const std::string & filename,
    std::vector<std::string> * lines);
/* ============================== Write File  =============================== */
// Writes 'num_bytes' of 'to_write' to the specified filename. With either
// overwrite existing file of that name or append to it, based on 'append'.
extern bool WriteFile(
    const bool append,
    const std::string & filename,
    const uint64_t & num_bytes,
    const char * to_write);
// Same as above, with default value append=false.
inline bool WriteFile(
    const std::string & filename,
    const uint64_t & num_bytes,
    const char * to_write) {
  return WriteFile(false, filename, num_bytes, to_write);
}
// Same as above, but write file in 'binary' format.
extern bool WriteBinaryFile(
    const bool append,
    const std::string & filename,
    const uint64_t & num_bytes,
    const char * to_write);
// Same as above, with default value append=false.
inline bool WriteBinaryFile(
    const std::string & filename,
    const uint64_t & num_bytes,
    const char * to_write) {
  return WriteBinaryFile(false, filename, num_bytes, to_write);
}

extern bool WriteLines(
    const std::string & filename,
    const std::vector<std::string> & lines);
} // namespace file_reader_utils

#endif
