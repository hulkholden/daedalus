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

TEST(Path, AddExtension) {
	std::string filename;
	filename = "foo";
	IO::Path::AddExtension(&filename, ".bar");
	EXPECT_EQ("foo.bar", filename);
	IO::Path::AddExtension(&filename, ".baz");
	EXPECT_EQ("foo.bar.baz", filename);
}

TEST(Path, RemoveExtension) {
	std::string filename;
	filename = "foo.bar.baz";
	EXPECT_TRUE(IO::Path::RemoveExtension(&filename));
	EXPECT_EQ("foo.bar", filename);
	EXPECT_TRUE(IO::Path::RemoveExtension(&filename));
	EXPECT_EQ("foo", filename);
	EXPECT_FALSE(IO::Path::RemoveExtension(&filename));
	EXPECT_EQ("foo", filename);
}

TEST(Path, RemoveFileSpec) {
	std::string filename;
	filename = "foo/bar/baz.txt";
	EXPECT_TRUE(IO::Path::RemoveFileSpec(&filename));
	EXPECT_EQ("foo/bar", filename);
	EXPECT_TRUE(IO::Path::RemoveFileSpec(&filename));
	EXPECT_EQ("foo", filename);
	EXPECT_FALSE(IO::Path::RemoveFileSpec(&filename));
	EXPECT_EQ("foo", filename);
}

TEST(Path, RemoveBackslash) {
	std::string filename;
	filename = "foo//";
	EXPECT_TRUE(IO::Path::RemoveBackslash(&filename));
	EXPECT_EQ("foo", filename);
	EXPECT_FALSE(IO::Path::RemoveBackslash(&filename));
	EXPECT_EQ("foo", filename);
}
