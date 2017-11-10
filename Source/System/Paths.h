
#ifndef SYSTEM_PATHS_H_
#define SYSTEM_PATHS_H_

#include <string>

#include "System/IO.h"

#include "absl/strings/string_view.h"

extern IO::Filename gDaedalusExePath;

std::string GetRunfilePath(absl::string_view filename);

#endif  // SYSTEM_PATHS_H_
