
#ifndef SYSTEM_PATHS_H_
#define SYSTEM_PATHS_H_

#include <string>

#include "absl/strings/string_view.h"

extern std::string gDaedalusExePath;

std::string GetRunfilePath(absl::string_view filename);

#endif  // SYSTEM_PATHS_H_
