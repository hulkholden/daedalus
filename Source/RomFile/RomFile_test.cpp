#include "stdafx.h"
#include "RomFile/RomFile.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Test/TestUtil.h"

TEST(RomFile, IsRomFilename) {
	EXPECT_TRUE(IsRomFilename("foo.v64"));
	EXPECT_TRUE(IsRomFilename("foo.n64"));
	EXPECT_TRUE(IsRomFilename("foo.bin"));
	EXPECT_TRUE(IsRomFilename("foo.pal"));
	EXPECT_TRUE(IsRomFilename("foo.zip"));
	EXPECT_TRUE(IsRomFilename("foo.z64"));
	EXPECT_TRUE(IsRomFilename("foo.rom"));
	EXPECT_TRUE(IsRomFilename("foo.jap"));
	EXPECT_TRUE(IsRomFilename("foo.usa"));

	EXPECT_FALSE(IsRomFilename(""));
	EXPECT_FALSE(IsRomFilename("foo"));
	EXPECT_FALSE(IsRomFilename("rom"));
	EXPECT_FALSE(IsRomFilename("."));
}

TEST(RomFile, Create) {
	//u8 p[] = {0x80, 0x37, 0x12, 0x40};
	u8 p[] = {0x40, 0x12, 0x37, 0x80};
	std::string fn = testing::GetTestTmpFilename("foo.rom");
	ASSERT_TRUE(testing::WriteFile(fn, p, 4));

	ROMFile* romfile = ROMFile::Create(fn.c_str());
	ASSERT_NE(nullptr, romfile);
	ASSERT_FALSE(romfile->IsCompressed());
	//ASSERT_FALSE(romfile->RequiresSwapping());
	delete romfile;
}
