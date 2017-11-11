#include "Utility/IniFile.h"

#include "absl/strings/str_cat.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Test/TestUtil.h"

namespace {

TEST(IniFileSection, Name) {
	IniFileSection section("foo");
	EXPECT_EQ("foo", section.name());
}

TEST(IniFileSection, GetProperty) {
	IniFileSection section("foo");
	section.AddProperty("a", "123");
	section.AddProperty("b", "456");

	std::string value;

	ASSERT_TRUE(section.GetProperty("a", &value));
	EXPECT_EQ("123", value);

	ASSERT_TRUE(section.GetProperty("b", &value));
	EXPECT_EQ("456", value);

	EXPECT_FALSE(section.GetProperty("c", &value));
}

TEST(IniFileSection, GetPropertyAsBool) {
	const std::vector<std::string> truthy = { "yes", "true", "1", "on", "TRUE", };
	for (auto t : truthy) {
		IniFileSection section("foo");
		section.AddProperty("a", t);

		bool value;
		ASSERT_TRUE(section.GetProperty("a", &value));
		EXPECT_TRUE(value);
	}

	const std::vector<std::string> falsey = { "no", "false", "0", "off", "FALSE", };
	for (auto f : falsey) {
		IniFileSection section("foo");
		section.AddProperty("a", f);

		bool value;
		ASSERT_TRUE(section.GetProperty("a", &value));
		EXPECT_FALSE(value);
	}

	const std::vector<std::string> other = { "a", "" };
	for (auto o : other) {
		IniFileSection section("foo");
		section.AddProperty("a", o);

		bool value;
		EXPECT_FALSE(section.GetProperty("a", &value));
	}

	{
		IniFileSection section("foo");
		bool value;
		EXPECT_FALSE(section.GetProperty("not present", &value));
	}
}

TEST(IniFileSection, GetPropertyAsInt) {
	IniFileSection section("foo");

	int value;
	section.AddProperty("a", "1234");
	ASSERT_TRUE(section.GetProperty("a", &value));
	EXPECT_EQ(1234, value);

	section.AddProperty("b", "-6");
	ASSERT_TRUE(section.GetProperty("b", &value));
	EXPECT_EQ(-6, value);

	section.AddProperty("c", "not an int");
	EXPECT_FALSE(section.GetProperty("c", &value));

	EXPECT_FALSE(section.GetProperty("not present", &value));
}

TEST(IniFileSection, GetPropertyAsU32) {
	IniFileSection section("foo");

	u32 value;
	section.AddProperty("a", "1234");
	ASSERT_TRUE(section.GetProperty("a", &value));
	EXPECT_EQ(1234, value);

	section.AddProperty("b", "-6");
	ASSERT_FALSE(section.GetProperty("b", &value));

	section.AddProperty("c", "not an int");
	EXPECT_FALSE(section.GetProperty("c", &value));

	EXPECT_FALSE(section.GetProperty("not present", &value));
}

TEST(IniFileSection, GetPropertyAsFloat) {
	IniFileSection section("foo");

	float value;
	section.AddProperty("a", "1234.5");
	ASSERT_TRUE(section.GetProperty("a", &value));
	EXPECT_EQ(1234.5, value);

	section.AddProperty("b", "-6");
	ASSERT_TRUE(section.GetProperty("b", &value));
	EXPECT_EQ(-6.0, value);

	section.AddProperty("c", "not an int");
	EXPECT_FALSE(section.GetProperty("c", &value));

	EXPECT_FALSE(section.GetProperty("not present", &value));
}

TEST(IniFileSection, ParseFile) {
const char *p = R"<<<(
defaultkey=defaultvalue
// Some comment
[section-1]
foo=123
bar=1024.5

{section-2}
bar=fish
baz=true

)<<<";
	std::string fn = testing::GetTestTmpFilename("foo.ini");
	ASSERT_TRUE(testing::WriteFile(fn, p));

	IniFile* inifile = IniFile::Create(fn);
	ASSERT_NE(nullptr, inifile);

	std::string str_value;
	int int_value;
	float float_value;
	bool bool_value;

	EXPECT_EQ(2, inifile->GetNumSections());

	const IniFileSection* default_section = inifile->GetDefaultSection();
	ASSERT_TRUE(default_section->GetProperty("defaultkey", &str_value));
	EXPECT_EQ("defaultvalue", str_value);

	const IniFileSection* section1 = inifile->GetSectionByName("section-1");
	ASSERT_NE(nullptr, section1);
	ASSERT_TRUE(section1->GetProperty("foo", &int_value));
	EXPECT_EQ(123, int_value);
	ASSERT_TRUE(section1->GetProperty("bar", &float_value));
	EXPECT_EQ(1024.5, float_value);
	EXPECT_FALSE(section1->GetProperty("baz", &str_value));

	const IniFileSection* section2 = inifile->GetSectionByName("section-2");
	ASSERT_NE(nullptr, section2);
	EXPECT_FALSE(section2->GetProperty("foo", &str_value));
	ASSERT_TRUE(section2->GetProperty("bar", &str_value));
	EXPECT_EQ("fish", str_value);
	ASSERT_TRUE(section2->GetProperty("baz", &bool_value));
	EXPECT_TRUE(bool_value);

	delete inifile;
}

}  // namespace
