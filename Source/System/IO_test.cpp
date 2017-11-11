#include "stdafx.h"
#include "System/IO.h"

#include "gtest/gtest.h"

TEST(Path, Join) {
	EXPECT_EQ("a/b", IO::Path::Join("a", "b"));
	EXPECT_EQ("a/b", IO::Path::Join("a/", "b"));

	EXPECT_EQ("a/b/c", IO::Path::Join("a", "b", "c"));
	EXPECT_EQ("a/b/c", IO::Path::Join("a/", "b", "c"));
	EXPECT_EQ("a/b/c", IO::Path::Join("a/", "b/", "c"));
	EXPECT_EQ("a/b/c", IO::Path::Join("a", "b/", "c"));
}
