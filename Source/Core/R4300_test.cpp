#include "Base/Daedalus.h"
#include "Core/R4300.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Core/CPU.h"
#include "Core/R4300Instruction.h"
#include "Test/TestUtil.h"
#include "Ultra/ultra_R4300.h"


void TestSpecial(void (*fn)(R4300_CALL_SIGNATURE), u64 d, u64 s, u64 t) {
	int rd = REG_v1;
	int rs = REG_a3;
	int rt = REG_v0;
	OpCode op_code;
	op_code.rd = rd;
	op_code.rs = rs;
	op_code.rt = rt;
	gGPR[rd]._u64 = ~d;
	gGPR[rs]._u64 = s;
	gGPR[rt]._u64 = t;
	fn( op_code._u32 );
	printf("%llx\n", gGPR[rd]._u64);
	EXPECT_EQ( d, gGPR[rd]._u64 );
}

TEST(R4300, ADD) {
	TestSpecial(R4300_Special_ADD, 3, 1, 2);
	TestSpecial(R4300_Special_ADD, 0, 1, -1);
	TestSpecial(R4300_Special_ADD, 0, -1, 1);
	TestSpecial(R4300_Special_ADD, 0xfffffffffffffffe, -1, -1);
	TestSpecial(R4300_Special_ADD, 0xffffffff90000000, 0x70000000, 0x20000000);
}

TEST(R4300, ADDU) {
	TestSpecial(R4300_Special_ADD, 3, 1, 2);
	TestSpecial(R4300_Special_ADD, 0, 1, -1);
	TestSpecial(R4300_Special_ADD, 0, -1, 1);
	TestSpecial(R4300_Special_ADD, 0xfffffffffffffffe, -1, -1);
	TestSpecial(R4300_Special_ADD, 0xffffffff90000000, 0x70000000, 0x20000000);
}
