#include "stdafx.h"
#include "Test/TestUtil.h"

#include <stdlib.h>

#include "absl/strings/str_cat.h"

namespace testing {

std::string GetTestTmpDir()
{
	const char* test_srcdir = getenv("TEST_TMPDIR");
	if (test_srcdir)
	{
		return test_srcdir;
	}
	return getenv("TMPDIR");
}

std::string GetTestTmpFilename(absl::string_view filename)
{
	return absl::StrCat(GetTestTmpDir(), "/", filename);
}

bool WriteFile(const std::string& filename, absl::string_view data)
{
	FILE* fh = fopen(filename.c_str(), "w");
	if (!fh)
	{
		return false;
	}
	fwrite(data.begin(), 1, data.length(), fh);
	fclose(fh);
	return true;
}

}  // testing
