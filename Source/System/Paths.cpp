#include "stdafx.h"
#include "Paths.h"

#include "absl/strings/str_cat.h"

IO::Filename gDaedalusExePath;

constexpr char kRunfilesDir[] = "/daedalus.runfiles/daedalus/";


std::string GetRunfilePath(absl::string_view filename)
{
	return IO::Path::Join(gDaedalusExePath, kRunfilesDir, filename);
}
