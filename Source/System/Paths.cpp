#include "Base/Daedalus.h"
#include "Paths.h"

#include "absl/strings/str_cat.h"

#include "System/IO.h"

static std::string gDaedalusExePath;

constexpr char kRunfilesDir[] = "/daedalus.runfiles/daedalus/";

void SetExeFilename(const std::string& filename)
{
	std::string path = filename;
	IO::Path::RemoveFileSpec(&path);
	gDaedalusExePath = path;
}

std::string GetRunfilePath(absl::string_view filename)
{
	return IO::Path::Join(gDaedalusExePath, kRunfilesDir, filename);
}

std::string GetDataFilename(absl::string_view filename)
{
	return IO::Path::Join(gDaedalusExePath, filename);
}

std::string GetOutputFilename(absl::string_view filename)
{
	return IO::Path::Join(gDaedalusExePath, filename);
}
