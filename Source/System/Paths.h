
#ifndef SYSTEM_PATHS_H_
#define SYSTEM_PATHS_H_

#include <string>

#include "absl/strings/string_view.h"

void SetExeFilename(const std::string& filename);

std::string GetRunfilePath(absl::string_view filename);
bool LoadRunfile(absl::string_view filename, std::string* out);

// TODO(strmnnrmn): Eventually this function and GetRunfilePath should be
// equivalent.
std::string GetDataFilename(absl::string_view filename);

// Returns a path suitable for writing outputs.
std::string GetOutputFilename(absl::string_view filename);

#endif  // SYSTEM_PATHS_H_
