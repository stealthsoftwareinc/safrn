/**
 * Copyright (C) 2020 Stealth Software Technologies, Inc.
 *
 * Developed under IR&D
 */

#include <fstream> // For fstream.
#include <iostream> // For cout.
#include <string>
#include <vector>

#include "read_file_utils.h"

// Start "ls" block: This block is necessary to do a system-independent "ls".
// Required for GetWorkingDirectory() and FilesInDirectory().
// NOTE: This page gives a useful overview of all OS macros that may be encountered:
// http://nadeausoftware.com/articles/2012/01/
//      c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
// End "ls" block.

using namespace std;

namespace file_reader_utils {

namespace {

// Helper function used below to remove unwanted characters from filepath
// strings.
string Replace(
    const string & orig,
    const char target,
    const string & replace_with) {
  size_t slash_pos = orig.find(target);
  string to_return = orig;

  while (slash_pos != string::npos) {
    to_return = to_return.replace(slash_pos, 1, replace_with);
    slash_pos =
        to_return.find(target, slash_pos + replace_with.length());
  }
  return to_return;
}

} // namespace

static const char kCodeBaseRoot[] = "CodeBase";
static const char kCodeBaseDir[] = "C:/Users/paulb/Stealth/CodeBase";
static const char kCodeBaseBackslashDir[] =
    "C:\\Users\\paulb\\Stealth\\CodeBase";
static const char kCodeBaseLinuxDir[] = "/home/paul/CodeBase";

/* =============================== Functions ================================ */
string GetFileName(const string & filepath) {
  size_t last_dir = filepath.find_last_of("/");
  if (last_dir == string::npos) {
    // Windows may use '\' instead of '/'. Check for that as well.
    last_dir = filepath.find_last_of("\\");
    if (last_dir == string::npos)
      return filepath;
  }
  if (last_dir == filepath.length() - 1)
    return "";
  return filepath.substr(last_dir + 1);
}

string GetDirectory(
    const bool include_trailing_slash, const string & filepath) {
  const string trailing_slash = include_trailing_slash ?
      (PathsUseForwardSlash() ? "/" : "\\") :
      "";
  size_t last_dir = filepath.find_last_of("/");
  if (last_dir == string::npos) {
    // Windows may use '\' instead of '/'. Check for that as well.
    last_dir = filepath.find_last_of("\\");
    if (last_dir == string::npos)
      return ("." + trailing_slash);
  }
  return (filepath.substr(0, last_dir) + trailing_slash);
}

string GetWorkingDirectory(const bool include_trailing_slash) {
  // Get working dir, either via GetModuleFileName (if working on WINDOWS)
  // or via getcwd otherwise.
  char dir_name[FILENAME_MAX];
  string dir_path = "";
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
  GetModuleFileName(nullptr, dir_name, FILENAME_MAX);
  dir_path = string(dir_name);
#else
  if (getcwd(dir_name, FILENAME_MAX) != nullptr) {
    // getcwd() does not include the trailing slash, which GetDirectory() expects.
    dir_path = string(dir_name) + "/";
  }
#endif
  return GetDirectory(include_trailing_slash, dir_path);
}

string GetCodeBaseDirectory(const bool include_trailing_slash) {
  // See if any of the 'known' locations for PaulsCode repository exist on this system
  // (will check default locations for Pauls machine, virtual machine, and default
  // Linux setup).
  if (DirectoryExists(kCodeBaseDir)) {
    return (string(kCodeBaseDir) + (include_trailing_slash ? "/" : ""));
  }
  if (DirectoryExists(kCodeBaseBackslashDir)) {
    return (
        string(kCodeBaseBackslashDir) +
        (include_trailing_slash ? "\\" : ""));
  }
  if (DirectoryExists(kCodeBaseLinuxDir)) {
    return (
        string(kCodeBaseLinuxDir) +
        (include_trailing_slash ? "/" : ""));
  }

  // Failed to find it among the 'known' locations. Attempt to find it as a
  // super directory of the current directory.
  string working_dir = GetWorkingDirectory(include_trailing_slash);
  size_t code_base_pos = working_dir.rfind(kCodeBaseRoot);
  if (code_base_pos != string::npos) {
    return working_dir.substr(
        0, code_base_pos + string(kCodeBaseRoot).length() + 1);
  }

  // Failed to find PaulsCode home directory as subdirectory of the current
  // (working) directory. In this case, just return the working directory.
  return working_dir;
}

bool GetFilesInDirectory(
    const bool full_path,
    const bool include_file,
    const bool include_directory,
    const string & directory,
    vector<string> * files) {
  if (files == nullptr)
    return false;

  // We don't know if user passed in trailing "/" or not; also, handle
  // case directory is empty.
  string dir = directory;
  if (dir.empty()) {
    dir = ".";
  } else if (
      directory.substr(directory.length() - 1) == "/" ||
      directory.substr(directory.length() - 1) == "\\") {
    dir = directory.substr(0, directory.length() - 1);
  }

// Get files in dir, either via Windows' Find[First|Next]File() or
// linux's [open | read]dir.
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
  // First check directory exists.
  DWORD file_attribute = GetFileAttributesA(dir.c_str());
  if (file_attribute ==
          INVALID_FILE_ATTRIBUTES /* Invalid path structure */
      || !(file_attribute &
           FILE_ATTRIBUTE_DIRECTORY) /* No such directory */) {
    return false;
  }

  // Check if there are any files in this directory.
  string dir_regexp =
      dir + "\\*"; // First check Windows format of '\' for dirs.
  WIN32_FIND_DATA file_data;
  HANDLE dir_handle = FindFirstFile(dir_regexp.c_str(), &file_data);
  if (dir_handle == INVALID_HANDLE_VALUE) {
    dir_regexp =
        dir + "/*"; // Now check standard format of '\' for dirs.
    dir_handle = FindFirstFile(dir_regexp.c_str(), &file_data);
    if (dir_handle == INVALID_HANDLE_VALUE) {
      // No files in dir.
      return true;
    }
  }

  // Directory contains at least one file. Get them all.
  do {
    const string file_name = file_data.cFileName;

    // Exclude files beginning with "." (e.g. self-reference ".", parent
    // directory "..", and "hidden" files).
    if (file_name[0] == '.')
      continue;

    // Exclude child directories.
    const bool is_directory =
        (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    if (is_directory && !include_directory)
      continue;
    if (!is_directory && !include_file)
      continue;

    // Add file.
    files->push_back(full_path ? dir + "/" + file_name : file_name);
  } while (FindNextFile(dir_handle, &file_data));

  // Close directory.
  FindClose(dir_handle);
#else // Linux.
  DIR * dir_ptr = opendir(dir.c_str());
  if (dir_ptr == nullptr)
    return false; // No such directory.

  struct dirent * ent;
  while ((ent = readdir(dir_ptr)) != nullptr) {
    const string file_name = ent->d_name;
    // Exclude files beginning with "." (e.g. self-reference ".", parent
    // directory "..", and "hidden" files).
    if (file_name[0] == '.')
      continue;

    const string full_file_name = directory + "/" + file_name;

    // Exclude child directories.
    class stat st;
    if (stat(full_file_name.c_str(), &st) == -1)
      continue; // Unable to get filetype.
    if (!include_directory && (st.st_mode & S_IFDIR) != 0)
      continue; // Is Directory.
    if (!include_file && (st.st_mode & S_IFDIR) == 0)
      continue; // Not a Directory.

    // Add file.
    files->push_back(full_path ? dir + "/" + file_name : file_name);
  }

  // Close directory.
  closedir(dir_ptr);
#endif

  return true;
}

bool GetFilesInDirectory(
    const bool full_path,
    const bool include_file,
    const bool include_directory,
    const string & directory,
    set<string> * files) {
  if (files == nullptr)
    return false;

  // We don't know if user passed in trailing "/" or not; also, handle
  // case directory is empty.
  string dir = directory;
  if (dir.empty()) {
    dir = ".";
  } else if (
      directory.substr(directory.length() - 1) == "/" ||
      directory.substr(directory.length() - 1) == "\\") {
    dir = directory.substr(0, directory.length() - 1);
  }

// Get files in dir, either via Windows' Find[First|Next]File() or
// linux's [open | read]dir.
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
  // First check directory exists.
  DWORD file_attribute = GetFileAttributesA(dir.c_str());
  if (file_attribute ==
          INVALID_FILE_ATTRIBUTES /* Invalid path structure */
      || !(file_attribute &
           FILE_ATTRIBUTE_DIRECTORY) /* No such directory */) {
    return false;
  }

  // Check if there are any files in this directory.
  string dir_regexp =
      dir + "\\*"; // First check Windows format of '\' for dirs.
  WIN32_FIND_DATA file_data;
  HANDLE dir_handle = FindFirstFile(dir_regexp.c_str(), &file_data);
  if (dir_handle == INVALID_HANDLE_VALUE) {
    dir_regexp =
        dir + "/*"; // Now check standard format of '\' for dirs.
    dir_handle = FindFirstFile(dir_regexp.c_str(), &file_data);
    if (dir_handle == INVALID_HANDLE_VALUE) {
      // No files in dir.
      return true;
    }
  }

  // Directory contains at least one file. Get them all.
  do {
    const string file_name = file_data.cFileName;

    // Exclude files beginning with "." (e.g. self-reference ".", parent
    // directory "..", and "hidden" files).
    if (file_name[0] == '.')
      continue;

    // Exclude child directories.
    const bool is_directory =
        (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    if (is_directory && !include_directory)
      continue;
    if (!is_directory && !include_file)
      continue;

    // Add file.
    files->insert(full_path ? dir + "/" + file_name : file_name);
  } while (FindNextFile(dir_handle, &file_data));

  // Close directory.
  FindClose(dir_handle);
#else // Linux.
  DIR * dir_ptr = opendir(dir.c_str());
  if (dir_ptr == nullptr)
    return false; // No such directory.

  struct dirent * ent;
  while ((ent = readdir(dir_ptr)) != nullptr) {
    const string file_name = ent->d_name;
    // Exclude files beginning with "." (e.g. self-reference ".", parent
    // directory "..", and "hidden" files).
    if (file_name[0] == '.')
      continue;

    const string full_file_name = directory + "/" + file_name;

    // Exclude child directories.
    class stat st;
    if (stat(full_file_name.c_str(), &st) == -1)
      continue; // Unable to get filetype.
    if (!include_directory && (st.st_mode & S_IFDIR) != 0)
      continue; // Is Directory.
    if (!include_file && (st.st_mode & S_IFDIR) == 0)
      continue; // Not a Directory.

    // Add file.
    files->insert(full_path ? dir + "/" + file_name : file_name);
  }

  // Close directory.
  closedir(dir_ptr);
#endif

  return true;
}

bool GetFilesInDirectory(
    const bool full_path,
    const string & directory,
    vector<string> * files) {
  return GetFilesInDirectory(full_path, true, false, directory, files);
}

bool GetFilesInDirectory(
    const bool full_path,
    const string & directory,
    set<string> * files) {
  return GetFilesInDirectory(full_path, true, false, directory, files);
}

bool GetFoldersInDirectory(
    const bool full_path,
    const string & directory,
    vector<string> * files) {
  return GetFilesInDirectory(full_path, false, true, directory, files);
}

bool GetFoldersInDirectory(
    const bool full_path,
    const string & directory,
    set<string> * files) {
  return GetFilesInDirectory(full_path, false, true, directory, files);
}

bool GetFilesAndFoldersInDirectory(
    const bool full_path,
    const string & directory,
    vector<string> * files) {
  return GetFilesInDirectory(full_path, true, true, directory, files);
}

bool GetFilesAndFoldersInDirectory(
    const bool full_path,
    const string & directory,
    set<string> * files) {
  return GetFilesInDirectory(full_path, true, true, directory, files);
}

bool DirectoryExists(const string & directory) {
  string dir = directory;
  if (dir.empty()) {
    dir = ".";
  } else if (
      directory.substr(directory.length() - 1) == "/" ||
      directory.substr(directory.length() - 1) == "\\") {
    dir = directory.substr(0, directory.length() - 1);
  }

#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
  DWORD file_attribute = GetFileAttributesA(dir.c_str());
  if (file_attribute ==
          INVALID_FILE_ATTRIBUTES /* Invalid path structure */
      || !(file_attribute &
           FILE_ATTRIBUTE_DIRECTORY) /* No such directory */) {
    return false;
  }
  return true;
#else // Linux.
  DIR * dir_ptr = opendir(dir.c_str());
  if (dir_ptr == nullptr)
    return false; // No such directory.
  closedir(dir_ptr);
  return true;
#endif

  // Code should never reach here.
  return false;
}

bool FileExists(const string & filename) {
  // Get the path to the file (everything in 'filename' up through the
  // final '/').
  const string dir = GetDirectory(filename);
  const string local_filename = GetFileName(filename);

  set<string> files;
  return (
      GetFilesInDirectory(false, dir, &files) &&
      files.find(local_filename) != files.end());
}

void RemoveWindowsTrailingCharacters(string * input) {
  if (input == nullptr)
    return;
  if (input->length() > 0) {
    if ((*input)[input->length() - 1] == 10) {
      *input = input->substr(0, input->length() - 1);
    }
  }
  if (input->length() > 0) {
    if ((*input)[input->length() - 1] == 13) {
      *input = input->substr(0, input->length() - 1);
    }
  }
}

bool CreateDir(const string & directory) {
  // Return true immediately for system-existing directories.
  if (directory == "." || directory == "..")
    return true;
// For each string adjustment performed in this function, we check
// whether the adjustment is necessary before doing it, as all of the
// adjustments are uncommon and it's better to have some extra CPU in
// the uncommon cases than useless memory copying in the common cases.
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
  // On Windows, MKDIR does exactly what we want except that (1) we
  // need to immediately reject paths that contain double quotes to
  // avoid quoting problems, (2) we need to replace slashes with
  // backslashes, and (3) MKDIR fails when the directory already
  // exists. To handle (3), we do *not* check whether the directory
  // already exists before running MKDIR, as this would introduce a
  // race condition where two concurrent calls trying to create the
  // same directory could both see that the directory does not exist
  // and both run MKDIR, guaranteeing that at least one of them fails.
  // Instead, we only check whether the directory already exists when
  // MKDIR fails.
  if (directory.find('"') != string::npos) {
    return false;
  }
  const bool s2nop = directory.find('/') == string::npos;
  const string & s2 = s2nop ? directory : Replace(directory, '/', "\\");
  // Windows has MKDIR log a warning/error if the directory already exists.
  // We want to repress this, so caller's terminal isn't spammed with this,
  // hence the '2>mkdir_cmd_output.trash' in the command below.
  if (system(string("MKDIR \"" + s2 + "\" 2>mkdir_cmd_output.trash")
                 .c_str()) == 0) {
    return true;
  }
  return DirectoryExists(s2);
#else
  // On *nix, mkdir -p does exactly what we want except that (1) we
  // need to replace backslashes with slashes, (2) we need to quote
  // properly, and (3) we need to fix paths that begin with '-' so
  // that they don't get misinterpreted as options.
  const bool s2nop = directory.find('\\') == string::npos;
  const string & s2 = s2nop ? directory : Replace(directory, '\\', "/");
  const bool s3nop = s2.find('\'') == string::npos;
  const string & s3 = s3nop ? s2 : Replace(s2, '\'', "'\\''");
  const bool s4nop = s3.length() == 0 || s3[0] != '-';
  const string & s4 = s4nop ? s3 : "./" + s3;
  return system(string("mkdir -p '" + s4 + "'").c_str()) == 0;
#endif
}

bool PathsUseForwardSlash() {
  // Hack: Grab the path of the present directory, and inspect it for forward
  // or backward slashes.
  char dir_name[FILENAME_MAX];
  string current_dir = "";
#if defined(WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || \
    defined(_WIN32) || defined(_WIN64)
  GetModuleFileName(nullptr, dir_name, FILENAME_MAX);
  current_dir = string(dir_name);
#else
  if (getcwd(dir_name, FILENAME_MAX) != nullptr) {
    current_dir = string(dir_name);
  }
#endif
  size_t last_dir = current_dir.find_last_of("/");
  if (last_dir == string::npos) {
    last_dir = current_dir.find_last_of("\\");
    if (last_dir == string::npos) {
      // TODO(paul): We found neither "/" nor "\" in the present working dir.
      // This can either happen if the working dir is root, or if a different
      // symbol is used altogether (not sure this would/could ever happen?!).
      // Handle this case...
      return true;
    } else {
      return false;
    }
  } else {
    return true;
  }

  // Code will never reach here.
  return false;
}

bool ReadFile(
    const string & filename, uint64_t * max_bytes, char ** output) {
  ifstream file(filename, ios::in | ios::ate);
  if (!file.is_open()) {
    cout << "ERROR: Unable to open file: " << filename << ". Aborting."
         << endl;
    return false;
  }
  ssize_t file_size = file.tellg();
  if (*max_bytes > 0 && (ssize_t)*max_bytes < file_size) {
    cout << "ERROR: File too big (max allowable bytes=" << *max_bytes
         << ", but actual file is " << file_size << " bytes." << endl;
    return false;
  }
  *max_bytes = (uint64_t)file_size;
  if (*output == nullptr) {
    *output = (char *)malloc((size_t)file_size);
  }
  if (!*output) {
    cout << "Unable to allocate " << file_size << " bytes." << endl;
    return false;
  }

  file.seekg(0, ios::beg);
  // Read block.
  file.read(*output, file_size);
  file.close();
  return true;
}

bool ReadBinaryFile(
    const string & filename, uint64_t * max_bytes, char ** output) {
  ifstream file(filename, ios::in | ios::binary | ios::ate);
  if (!file.is_open()) {
    cout << "ERROR: Unable to open file: " << filename << ". Aborting."
         << endl;
    return false;
  }
  ssize_t file_size = file.tellg();
  if (*max_bytes > 0 && (ssize_t)*max_bytes < file_size) {
    cout << "ERROR: File too big (max allowable bytes=" << *max_bytes
         << ", but actual file is " << file_size << " bytes." << endl;
    return false;
  }
  *max_bytes = (uint64_t)file_size;
  if (*output == nullptr) {
    *output = (char *)malloc((size_t)file_size);
  }
  if (!*output) {
    cout << "Unable to allocate " << file_size << " bytes." << endl;
    return false;
  }

  file.seekg(0, ios::beg);
  // Read block.
  file.read(*output, file_size);
  file.close();
  return true;
}

bool ReadLines(
    const bool remove_windows_trailing_chars,
    const string & filename,
    vector<string> * lines) {
  ifstream input_file(filename.c_str());
  if (!input_file.is_open()) {
    cout << "Unable to open file '" << filename << "'." << endl;
    return false;
  }
  string line;
  while (getline(input_file, line)) {
    if (remove_windows_trailing_chars)
      RemoveWindowsTrailingCharacters(&line);
    lines->push_back(line);
  }

  return true;
}

bool WriteFile(
    const bool append,
    const string & filename,
    const uint64_t & num_bytes,
    const char * to_write) {
  ofstream output_file;
  if (append) {
    output_file.open(filename, ofstream::out | ofstream::app);
  } else {
    output_file.open(filename, ofstream::out);
  }
  if (!output_file.is_open()) {
    cout << "Unable to open output file '" << filename << "'." << endl;
    return false;
  }
  output_file.write(to_write, (ssize_t)num_bytes);
  output_file.close();
  return true;
}

bool WriteBinaryFile(
    const bool append,
    const string & filename,
    const uint64_t & num_bytes,
    const char * to_write) {
  ofstream output_file;
  if (append) {
    output_file.open(
        filename, ofstream::out | ofstream::app | ofstream::binary);
  } else {
    output_file.open(filename, ofstream::out | ofstream::binary);
  }
  if (!output_file.is_open()) {
    cout << "Unable to open output file '" << filename << "'." << endl;
    return false;
  }
  output_file.write(to_write, (ssize_t)num_bytes);
  output_file.close();
  return true;
}

bool WriteLines(const string & filename, const vector<string> & lines) {
  ofstream output_file;
  output_file.open(filename, ofstream::out);
  if (!output_file.is_open()) {
    cout << "Unable to open output file '" << filename << "'." << endl;
    return false;
  }

  for (const string & line : lines) {
    output_file << line << endl;
  }
  output_file.close();
  return true;
}
/* ============================= END Functions ============================== */

} // namespace file_reader_utils
