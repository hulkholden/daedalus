
#pragma once

#ifndef TEST_TESTUTIL_H_
#define TEST_TESTUTIL_H_

#include "absl/strings/string_view.h"

namespace testing {

std::string GetTestTmpDir();
std::string GetTestTmpFilename(absl::string_view filename);

bool WriteFile(const std::string& filename, absl::string_view data);

}  // testing

#endif  // TEST_TESTUTIL_H_
