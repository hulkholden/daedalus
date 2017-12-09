#include "Base/Daedalus.h"
#include "System/Paths.h"

#include <string>
#include <unordered_map>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"

#include "System/IO.h"

static std::string gDaedalusExePath;

#if defined(DAEDALUS_W32)
constexpr char kRunfilesDir[] = "/daedalus.exe.runfiles/";
#else
constexpr char kRunfilesDir[] = "/daedalus.runfiles/daedalus/";
#endif

static bool gHaveManifest = false;
static std::unordered_map<std::string, std::string> gManifestMap;

void SetExeFilename(const std::string& filename)
{
	std::string path = filename;
	IO::Path::RemoveFileSpec(&path);
	gDaedalusExePath = path;
}

// TODO(strmnnrmn): This doesn't work on Windows. Need to get the callers
// to enumerate all the runfiles matching a glob, or something like that.
std::string GetRunfilePath(absl::string_view filename)
{
	return IO::Path::Join(gDaedalusExePath, kRunfilesDir, filename);
}

static bool _LoadManifest(std::unordered_map<std::string, std::string>* manifest_map)
{
	std::string fullpath = GetRunfilePath("MANIFEST");
	FILE * fh = fopen(fullpath.c_str(), "r");
	if (!fh)
	{
		return false;
	}

	const u32 kBufferLen = 1024;
	char buffer[kBufferLen + 1];
	while (fgets(buffer, kBufferLen, fh))
	{
		absl::string_view line(buffer);
		line = absl::StripTrailingAsciiWhitespace(line);
		std::pair<std::string, std::string> kv = absl::StrSplit(line, absl::MaxSplits(' ', 1));
		if (!kv.first.empty() && !kv.second.empty())
		{
			(*manifest_map)[kv.first] = kv.second;
		}
	}

	fclose(fh);
	return true;
}

static bool _LoadFile(const std::string& fullpath, std::string* out)
{
	FILE * fh = fopen(fullpath.c_str(), "rb");
	if (!fh)
	{
		return false;
	}

	fseek(fh, 0, SEEK_END);
	size_t len = ftell(fh);
	rewind(fh);
	char * p = (char *)malloc(len+1);
	size_t read = fread(p, 1, len, fh);
	if (read != len) {
		free(p);
		fclose(fh);
		return false;
	}
	p[len] = 0;
	fclose(fh);
	*out = p;
	free(p);
	return true;
}

bool LoadRunfile(absl::string_view filename, std::string* out)
{
	if (!gHaveManifest)
	{
		if (!_LoadManifest(&gManifestMap))
		{
			return false;
		}
		gHaveManifest = true;
	}
	std::string key = absl::StrCat("daedalus/", filename);
	std::string fullpath = gManifestMap[key];
	return _LoadFile(fullpath, out);
}

std::string GetDataFilename(absl::string_view filename)
{
	return IO::Path::Join(gDaedalusExePath, filename);
}

std::string GetOutputFilename(absl::string_view filename)
{
	return IO::Path::Join(gDaedalusExePath, filename);
}
