#include "Base/Daedalus.h"
#include "Core/R4300.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Core/CPU.h"
#include "Core/R4300Instruction.h"
#include "Test/TestUtil.h"
#include "Ultra/ultra_R4300.h"


u64 TestSpecial(void (*fn)(R4300_CALL_SIGNATURE), u64 s, u64 t) {
	int rd = REG_v1;
	int rs = REG_a3;
	int rt = REG_v0;
	OpCode op_code;
	op_code.rd = rd;
	op_code.rs = rs;
	op_code.rt = rt;
	gGPR[rd]._u64 = 0xdeadbeef;
	gGPR[rs]._u64 = s;
	gGPR[rt]._u64 = t;
	fn( op_code._u32 );
	//printf("%llx\n", gGPR[rd]._u64);
	return gGPR[rd]._u64;
}

u64 TestShift(void (*fn)(R4300_CALL_SIGNATURE), u64 t, u64 sa) {
	int rd = REG_v1;
	int rt = REG_a3;
	OpCode op_code;
	op_code.rd = rd;
	op_code.rt = rt;
	op_code.sa = sa;
	gGPR[rd]._u64 = 0xdeadbeef;
	gGPR[rt]._u64 = t;
	fn( op_code._u32 );
	//printf("%llx\n", gGPR[rd]._u64);
	return gGPR[rd]._u64;
}

TEST(R4300, ADD) {
	EXPECT_EQ(TestSpecial(R4300_Special_ADD, 1, 2), 3);
	EXPECT_EQ(TestSpecial(R4300_Special_ADD, 1, -1), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_ADD, -1, 1), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_ADD, -1, -1), 0xfffffffffffffffe);
	EXPECT_EQ(TestSpecial(R4300_Special_ADD, 0x70000000, 0x20000000), 0xffffffff90000000);
	// Upper bits should be ignored.
	EXPECT_EQ(TestSpecial(R4300_Special_ADD, 0xdead70000000, 0xdead20000000), 0xffffffff90000000);
}

TEST(R4300, ADDU) {
	EXPECT_EQ(TestSpecial(R4300_Special_ADDU, 1, 2), 3);
	EXPECT_EQ(TestSpecial(R4300_Special_ADDU, 1, -1), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_ADDU, -1, 1), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_ADDU, -1, -1), 0xfffffffffffffffe);
	EXPECT_EQ(TestSpecial(R4300_Special_ADDU, 0x70000000, 0x20000000), 0xffffffff90000000);
	// Upper bits should be ignored.
	EXPECT_EQ(TestSpecial(R4300_Special_ADDU, 0xdead70000000, 0xdead20000000), 0xffffffff90000000);
}

TEST(R4300, SUB) {
	EXPECT_EQ(TestSpecial(R4300_Special_SUB, 1, 2), -1);
	EXPECT_EQ(TestSpecial(R4300_Special_SUB, 1, -1), 2);
	EXPECT_EQ(TestSpecial(R4300_Special_SUB, -1, 1), -2);
	EXPECT_EQ(TestSpecial(R4300_Special_SUB, 0x70000000, -0x20000000), 0xffffffff90000000);
	// Upper bits should be ignored.
	EXPECT_EQ(TestSpecial(R4300_Special_SUB, 0xdead70000000, -0x20000000), 0xffffffff90000000);
}

TEST(R4300, SUBU) {
	EXPECT_EQ(TestSpecial(R4300_Special_SUBU, 1, 2), -1);
	EXPECT_EQ(TestSpecial(R4300_Special_SUBU, 1, -1), 2);
	EXPECT_EQ(TestSpecial(R4300_Special_SUBU, -1, 1), -2);
	EXPECT_EQ(TestSpecial(R4300_Special_SUBU, 0x70000000, -0x20000000), 0xffffffff90000000);
	// Upper bits should be ignored.
	EXPECT_EQ(TestSpecial(R4300_Special_SUBU, 0xdead70000000, -0x20000000), 0xffffffff90000000);
}

TEST(R4300, AND) {
	EXPECT_EQ(TestSpecial(R4300_Special_AND, 0xf0f0f0f0f0f0f0f0, 0x0f0f0f0f0f0f0f0f), 0x0000000000000000);
	EXPECT_EQ(TestSpecial(R4300_Special_AND, 0xf0f0f0f0f0f0f0f0, 0xffffffffffffffff), 0xf0f0f0f0f0f0f0f0);
	EXPECT_EQ(TestSpecial(R4300_Special_AND, 0xffffffffffffffff, 0x0f0f0f0f0f0f0f0f), 0x0f0f0f0f0f0f0f0f);
}

TEST(R4300, OR) {
	EXPECT_EQ(TestSpecial(R4300_Special_OR, 0xf0f0f0f0f0f0f0f0, 0x0f0f0f0f0f0f0f0f), 0xffffffffffffffff);
	EXPECT_EQ(TestSpecial(R4300_Special_OR, 0xf0f0f0f0f0f0f0f0, 0x0000000000000000), 0xf0f0f0f0f0f0f0f0);
	EXPECT_EQ(TestSpecial(R4300_Special_OR, 0x0000000000000000, 0x0f0f0f0f0f0f0f0f), 0x0f0f0f0f0f0f0f0f);
}

TEST(R4300, XOR) {
	EXPECT_EQ(TestSpecial(R4300_Special_XOR, 0x0000000000000000, 0x0000000000000000), 0x0000000000000000);
	EXPECT_EQ(TestSpecial(R4300_Special_XOR, 0x1111111111111111, 0x0000000000000000), 0x1111111111111111);
	EXPECT_EQ(TestSpecial(R4300_Special_XOR, 0x0000000000000000, 0x1111111111111111), 0x1111111111111111);
	EXPECT_EQ(TestSpecial(R4300_Special_XOR, 0x1111111111111111, 0x1111111111111111), 0x0000000000000000);
}

TEST(R4300, NOR) {
	EXPECT_EQ(TestSpecial(R4300_Special_NOR, 0x0000000000000000, 0x0000000000000000), 0xffffffffffffffff);
	EXPECT_EQ(TestSpecial(R4300_Special_NOR, 0x1111111111111111, 0x0000000000000000), 0xeeeeeeeeeeeeeeee);
	EXPECT_EQ(TestSpecial(R4300_Special_NOR, 0x0000000000000000, 0x1111111111111111), 0xeeeeeeeeeeeeeeee);
	EXPECT_EQ(TestSpecial(R4300_Special_NOR, 0x1111111111111111, 0x1111111111111111), 0xeeeeeeeeeeeeeeee);
}

TEST(R4300, SLT) {
	EXPECT_EQ(TestSpecial(R4300_Special_SLT, 0, 0), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_SLT, 0, -1), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_SLT, 1, 0), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_SLT, -1, 0), 1);
	EXPECT_EQ(TestSpecial(R4300_Special_SLT, 0, 1), 1);
}

TEST(R4300, SLTU) {
	EXPECT_EQ(TestSpecial(R4300_Special_SLTU, 0, 0), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_SLTU, 0, -1), 1);
	EXPECT_EQ(TestSpecial(R4300_Special_SLTU, 1, 0), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_SLTU, -1, 0), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_SLTU, 0, 1), 1);
}

TEST(R4300, DADD) {
	EXPECT_EQ(TestSpecial(R4300_Special_DADD, 1, 2), 3);
	EXPECT_EQ(TestSpecial(R4300_Special_DADD, 1, -1), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_DADD, -1, 1), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_DADD, -1, -1), 0xfffffffffffffffe);
	EXPECT_EQ(TestSpecial(R4300_Special_DADD, 0x70000000, 0x20000000), 0x90000000);
	// Upper bits should not be ignored.
	EXPECT_EQ(TestSpecial(R4300_Special_DADD, 0xdead70000000, 0xdead20000000), 0x1bd5a90000000);
}

TEST(R4300, DADDU) {
	EXPECT_EQ(TestSpecial(R4300_Special_DADDU, 1, 2), 3);
	EXPECT_EQ(TestSpecial(R4300_Special_DADDU, 1, -1), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_DADDU, -1, 1), 0);
	EXPECT_EQ(TestSpecial(R4300_Special_DADDU, -1, -1), 0xfffffffffffffffe);
	EXPECT_EQ(TestSpecial(R4300_Special_DADDU, 0x70000000, 0x20000000), 0x90000000);
	// Upper bits should not be ignored.
	EXPECT_EQ(TestSpecial(R4300_Special_DADDU, 0xdead70000000, 0xdead20000000), 0x1bd5a90000000);
}

TEST(R4300, DSUB) {
	EXPECT_EQ(TestSpecial(R4300_Special_DSUB, 1, 2), -1);
	EXPECT_EQ(TestSpecial(R4300_Special_DSUB, 1, -1), 2);
	EXPECT_EQ(TestSpecial(R4300_Special_DSUB, -1, 1), -2);
	EXPECT_EQ(TestSpecial(R4300_Special_DSUB, 0x70000000, -0x20000000), 0x90000000);
	// Upper bits should not be ignored.
	EXPECT_EQ(TestSpecial(R4300_Special_DSUB, 0xdead70000000, -0x20000000), 0xdead90000000);
}

TEST(R4300, DSUBU) {
	EXPECT_EQ(TestSpecial(R4300_Special_DSUBU, 1, 2), -1);
	EXPECT_EQ(TestSpecial(R4300_Special_DSUBU, 1, -1), 2);
	EXPECT_EQ(TestSpecial(R4300_Special_DSUBU, -1, 1), -2);
	EXPECT_EQ(TestSpecial(R4300_Special_DSUBU, 0x70000000, -0x20000000), 0x90000000);
	// Upper bits should not be ignored.
	EXPECT_EQ(TestSpecial(R4300_Special_DSUBU, 0xdead70000000, -0x20000000), 0xdead90000000);
}

TEST(R4300, DSLL) {
	EXPECT_EQ(TestShift(R4300_Special_DSLL, 0x12345678abcd, 4), 0x12345678abcd0);
	EXPECT_EQ(TestShift(R4300_Special_DSLL, 0x12345678abcd, 31), 0x2b3c55e680000000);
	// A shift of 32 is the same as a shift of 0 as .sa is only 5 bits.
	EXPECT_EQ(TestShift(R4300_Special_DSLL, 0x12345678abcd, 0), 0x12345678abcd);
	EXPECT_EQ(TestShift(R4300_Special_DSLL, 0x12345678abcd, 32), 0x12345678abcd);
}

TEST(R4300, DSRL) {
	EXPECT_EQ(TestShift(R4300_Special_DSRL, 0x12345678abcd, 4), 0x12345678abc);
	EXPECT_EQ(TestShift(R4300_Special_DSRL, 0x12345678abcd, 31), 0x2468);
	EXPECT_EQ(TestShift(R4300_Special_DSRL, 0xffffffffffffffff, 1), 0x7fffffffffffffff);
	// A shift of 32 is the same as a shift of 0 as .sa is only 5 bits.
	EXPECT_EQ(TestShift(R4300_Special_DSRL, 0x12345678abcd, 0), 0x12345678abcd);
	EXPECT_EQ(TestShift(R4300_Special_DSRL, 0x12345678abcd, 32), 0x12345678abcd);
}

TEST(R4300, DSRA) {
	EXPECT_EQ(TestShift(R4300_Special_DSRA, 0x12345678abcd, 4), 0x12345678abc);
	EXPECT_EQ(TestShift(R4300_Special_DSRA, 0x12345678abcd, 31), 0x2468);
	EXPECT_EQ(TestShift(R4300_Special_DSRA, 0xfffffffffffffffe, 1), 0xffffffffffffffff);
	// A shift of 32 is the same as a shift of 0 as .sa is only 5 bits.
	EXPECT_EQ(TestShift(R4300_Special_DSRA, 0x12345678abcd, 0), 0x12345678abcd);
	EXPECT_EQ(TestShift(R4300_Special_DSRA, 0x12345678abcd, 32), 0x12345678abcd);
}

TEST(R4300, DSLL32) {
	EXPECT_EQ(TestShift(R4300_Special_DSLL32, 0x12345678abcd, 4), 0x678abcd000000000);
	EXPECT_EQ(TestShift(R4300_Special_DSLL32, 0x12345678abcd, 31), 0x8000000000000000);
	// A shift of 32 is the same as a shift of 0 as .sa is only 5 bits.
	EXPECT_EQ(TestShift(R4300_Special_DSLL32, 0x12345678abcd, 0), 0x5678abcd00000000);
	EXPECT_EQ(TestShift(R4300_Special_DSLL32, 0x12345678abcd, 32), 0x5678abcd00000000);
}

TEST(R4300, DSRL32) {
	EXPECT_EQ(TestShift(R4300_Special_DSRL32, 0x12345678abcd, 4), 0x123);
	EXPECT_EQ(TestShift(R4300_Special_DSRL32, 0x12345678abcd, 31), 0);
	EXPECT_EQ(TestShift(R4300_Special_DSRL32, 0xffffffffffffffff, 1), 0x7fffffff);
	// A shift of 32 is the same as a shift of 0 as .sa is only 5 bits.
	EXPECT_EQ(TestShift(R4300_Special_DSRL32, 0x12345678abcd, 0), 0x1234);
	EXPECT_EQ(TestShift(R4300_Special_DSRL32, 0x12345678abcd, 32), 0x1234);
}

TEST(R4300, DSRA32) {
	EXPECT_EQ(TestShift(R4300_Special_DSRA32, 0x12345678abcd, 4), 0x123);
	EXPECT_EQ(TestShift(R4300_Special_DSRA32, 0x12345678abcd, 31), 0);
	EXPECT_EQ(TestShift(R4300_Special_DSRA32, 0xfffffffffffffffe, 1), 0xffffffffffffffff);
	// A shift of 32 is the same as a shift of 0 as .sa is only 5 bits.
	EXPECT_EQ(TestShift(R4300_Special_DSRA32, 0x12345678abcd, 0), 0x1234);
	EXPECT_EQ(TestShift(R4300_Special_DSRA32, 0x12345678abcd, 32), 0x1234);
}
