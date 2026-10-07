#include "pch.h"
#include "../cpu6502/cpu6502.h";

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

class Ccpu6502Test : public ::testing::Test {
public:
    Ccpu6502 cpu;
    
    virtual void SetUp() {
        cpu.Reset();
        cpu.MachineStartup();
    }
};

TEST_F(Ccpu6502Test, TestFetch) {
    cpu.SystemMemory->at(0xFFFC) = 0xA9; // LDA Immediate
    cpu.SystemMemory->at(0xFFFD) = 0x42; // Value to load
    UINT8 opcode = cpu.Fetch();
    EXPECT_EQ(opcode, 0xA9);
    UINT8 value = cpu.Fetch();
    EXPECT_EQ(value, 0x42);
}

////
// LD_A
////

TEST_F(Ccpu6502Test, TestExecuteLDAImmediate) {
    cpu.SystemMemory->at(0xFFFC) = 0xA9; // LDA Immediate
    cpu.SystemMemory->at(0xFFFD) = 0x42; // Value to load
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAImmediateZeroFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xA9; // LDA Immediate
    cpu.SystemMemory->at(0xFFFD) = 0x00; // Value to load
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, Ccpu6502::ZEROFLAG); // Zero flag should be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAImmediateNegativeFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xA9; // LDA Immediate
    cpu.SystemMemory->at(0xFFFD) = 0xFF; // Value to load
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0xFF);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAZeroPage) {
    cpu.SystemMemory->at(0xFFFC) = 0xA5; // LDA Zero Page
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0x37; // Value at Zero Page address
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAZeroPageZeroFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xA5; // LDA Zero Page
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0x00; // Value at Zero Page address
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, Ccpu6502::ZEROFLAG); // Zero flag should be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAZeroPageNegativeFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xA5; // LDA Zero Page
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Value at Zero Page address
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0xFF);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAAbsolute) {
    cpu.SystemMemory->at(0xFFFC) = 0xAD; // LDA Absolute
    cpu.SystemMemory->at(0xFFFD) = 0x00; // Low byte of address
    cpu.SystemMemory->at(0xFFFE) = 0x20; // High byte of address (address = 0x2000)
    cpu.SystemMemory->at(0x2000) = 0x55; // Value at Absolute address
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x55);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAAbsoluteZeroFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xAD; // LDA Absolute
    cpu.SystemMemory->at(0xFFFD) = 0x00; // Low byte of address
    cpu.SystemMemory->at(0xFFFE) = 0x20; // High byte of address (address = 0x2000)
    cpu.SystemMemory->at(0x2000) = 0x00; // Value at Absolute address
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, Ccpu6502::ZEROFLAG); // Zero flag should be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAAbsoluteNegativeFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xAD; // LDA Absolute
    cpu.SystemMemory->at(0xFFFD) = 0x00; // Low byte of address
    cpu.SystemMemory->at(0xFFFE) = 0x20; // High byte of address (address = 0x2000)
    cpu.SystemMemory->at(0x2000) = 0xFF; // Value at Absolute address
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0xFF);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAAbsoluteX) {
    cpu.SystemMemory->at(0xFFFC) = 0xBD; // LDA Absolute,X
    cpu.SystemMemory->at(0xFFFD) = 0x00; // Low byte of address
    cpu.SystemMemory->at(0xFFFE) = 0x20; // High byte of address (base address = 0x2000)
    cpu.X = 0x05; // X register offset
    cpu.SystemMemory->at(0x2005) = 0x77; // Value at Absolute,X address (address = 0x2005)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x77);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAAbsoluteXZeroFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xBD; // LDA Absolute,X
    cpu.SystemMemory->at(0xFFFD) = 0x00; // Low byte of address
    cpu.SystemMemory->at(0xFFFE) = 0x20; // High byte of address (base address = 0x2000)
    cpu.X = 0x05; // X register offset
    cpu.SystemMemory->at(0x2005) = 0x00; // Value at Absolute,X address (address = 0x2005)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, Ccpu6502::ZEROFLAG); // Zero flag should be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAAbsoluteXNegativeFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xBD; // LDA Absolute,X
    cpu.SystemMemory->at(0xFFFD) = 0x00; // Low byte of address
    cpu.SystemMemory->at(0xFFFE) = 0x20; // High byte of address (base address = 0x2000)
    cpu.X = 0x05; // X register offset
    cpu.SystemMemory->at(0x2005) = 0xFF; // Value at Absolute,X address (address = 0x2005)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0xFF);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAAbsoluteY) {
    cpu.SystemMemory->at(0xFFFC) = 0xB9; // LDA Absolute,Y
    cpu.SystemMemory->at(0xFFFD) = 0x00; // Low byte of address
    cpu.SystemMemory->at(0xFFFE) = 0x20; // High byte of address (base address = 0x2000)
    cpu.Y = 0x05; // Y register offset
    cpu.SystemMemory->at(0x2005) = 0x88; // Value at Absolute,Y address (address = 0x2005)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x88);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAAbsoluteYZeroFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB9; // LDA Absolute,Y
    cpu.SystemMemory->at(0xFFFD) = 0x00; // Low byte of address
    cpu.SystemMemory->at(0xFFFE) = 0x20; // High byte of address (base address = 0x2000)
    cpu.Y = 0x05; // Y register offset
    cpu.SystemMemory->at(0x2005) = 0x00; // Value at Absolute,Y address (address = 0x2005)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, Ccpu6502::ZEROFLAG); // Zero flag should be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAAbsoluteYNegativeFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB9; // LDA Absolute,Y
    cpu.SystemMemory->at(0xFFFD) = 0x00; // Low byte of address
    cpu.SystemMemory->at(0xFFFE) = 0x20; // High byte of address (base address = 0x2000)
    cpu.Y = 0x05; // Y register offset
    cpu.SystemMemory->at(0x2005) = 0xFF; // Value at Absolute,Y address (address = 0x2005)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0xFF);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndexedIndirect) {
    cpu.SystemMemory->at(0xFFFC) = 0xA1; // LDA (Indirect,X)
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.X = 0x04; // X register offset
    cpu.SystemMemory->at(0x0014) = 0x00; // Low byte of effective address
    cpu.SystemMemory->at(0x0015) = 0x30; // High byte of effective address (effective address = 0x3000)
    cpu.SystemMemory->at(0x3000) = 0x99; // Value at effective address
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x99);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexed) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0x00; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x40; // High byte of base address (base address = 0x4000)
    cpu.Y = 0x05; // Y register offset
    cpu.SystemMemory->at(0x4005) = 0x77; // Value at effective address (effective address = 0x4005)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x77);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedZeroFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0x00; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x40; // High byte of base address (base address = 0x4000)
    cpu.Y = 0x05; // Y register offset
    cpu.SystemMemory->at(0x4005) = 0x00; // Value at effective address (effective address = 0x4005)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, Ccpu6502::ZEROFLAG); // Zero flag should be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedNegativeFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0x00; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x40; // High byte of base address (base address = 0x4000)
    cpu.Y = 0x05; // Y register offset
    cpu.SystemMemory->at(0x4005) = 0xFF; // Value at effective address (effective address = 0x4005)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0xFF);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossing) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0xAB; // Value at effective address (effective address = 0x0101)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0xAB);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingZeroFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0x00; // Value at effective address (effective address = 0x0101)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, Ccpu6502::ZEROFLAG); // Zero flag should be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should not be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingNegativeFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0xFF; // Value at effective address (effective address = 0x0101)
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0xFF);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should not be set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should be set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingAndCarryFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0x01; // Value at effective address (effective address = 0x0101)
    cpu.P |= Ccpu6502::CARRYFLAG; // Set Carry flag before execution
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x01);
    EXPECT_EQ(cpu.P & Ccpu6502::CARRYFLAG, Ccpu6502::CARRYFLAG); // Carry flag should remain set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingAndClearCarryFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0x01; // Value at effective address (effective address = 0x0101)
    cpu.P &= ~Ccpu6502::CARRYFLAG; // Clear Carry flag before execution
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x01);
    EXPECT_EQ(cpu.P & Ccpu6502::CARRYFLAG, 0); // Carry flag should remain clear
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingAndSetCarryFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0x01; // Value at effective address (effective address = 0x0101)
    cpu.P |= Ccpu6502::CARRYFLAG; // Set Carry flag before execution
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x01);
    EXPECT_EQ(cpu.P & Ccpu6502::CARRYFLAG, Ccpu6502::CARRYFLAG); // Carry flag should remain set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingAndSetNegativeFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0x80; // Value at effective address (effective address = 0x0101)
    cpu.P |= Ccpu6502::NEGATIVEFLAG; // Set Negative flag before execution
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x80);
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should remain set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingAndClearNegativeFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0x7F; // Value at effective address (effective address = 0x0101)
    cpu.P &= ~Ccpu6502::NEGATIVEFLAG; // Clear Negative flag before execution
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x7F);
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should remain clear
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingAndSetZeroFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0x00; // Value at effective address (effective address = 0x0101)
    cpu.P |= Ccpu6502::ZEROFLAG; // Set Zero flag before execution
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x00);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, Ccpu6502::ZEROFLAG); // Zero flag should remain set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingAndClearZeroFlag) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0x01; // Value at effective address (effective address = 0x0101)
    cpu.P &= ~Ccpu6502::ZEROFLAG; // Clear Zero flag before execution
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x01);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should remain clear
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingAndSetCarryAndNegativeFlags) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0x80; // Value at effective address (effective address = 0x0101)
    cpu.P |= Ccpu6502::CARRYFLAG | Ccpu6502::NEGATIVEFLAG; // Set Carry and Negative flags before execution
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x80);
    EXPECT_EQ(cpu.P & Ccpu6502::CARRYFLAG, Ccpu6502::CARRYFLAG); // Carry flag should remain set
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, Ccpu6502::NEGATIVEFLAG); // Negative flag should remain set
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingAndClearCarryAndNegativeFlags) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0x7F; // Value at effective address (effective address = 0x0101)
    cpu.P &= ~(Ccpu6502::CARRYFLAG | Ccpu6502::NEGATIVEFLAG); // Clear Carry and Negative flags before execution
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x7F);
    EXPECT_EQ(cpu.P & Ccpu6502::CARRYFLAG, 0); // Carry flag should remain clear
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should remain clear
}

TEST_F(Ccpu6502Test, TestExecuteLDAIndirectIndexedWithPageCrossingAndClearZeroAndNegativeFlags) {
    cpu.SystemMemory->at(0xFFFC) = 0xB1; // LDA (Indirect),Y
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0xFF; // Low byte of base address
    cpu.SystemMemory->at(0x0011) = 0x00; // High byte of base address (base address = 0x00FF)
    cpu.Y = 0x02; // Y register offset, will cause page crossing to 0x0101
    cpu.SystemMemory->at(0x0101) = 0x01; // Value at effective address (effective address = 0x0101)
    cpu.P &= ~(Ccpu6502::ZEROFLAG | Ccpu6502::NEGATIVEFLAG); // Clear Zero and Negative flags before execution
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.A, 0x01);
    EXPECT_EQ(cpu.P & Ccpu6502::ZEROFLAG, 0); // Zero flag should remain clear
    EXPECT_EQ(cpu.P & Ccpu6502::NEGATIVEFLAG, 0); // Negative flag should remain clear
}

TEST_F(Ccpu6502Test, TestExecuteLDXImmediate) {
    cpu.SystemMemory->at(0xFFFC) = 0xA2; // LDX Immediate
    cpu.SystemMemory->at(0xFFFD) = 0x01; // Value to load into X
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.X, 0x01);
}

TEST_F(Ccpu6502Test, TestExecuteLDYImmediate) {
    cpu.SystemMemory->at(0xFFFC) = 0xA0; // LDY Immediate
    cpu.SystemMemory->at(0xFFFD) = 0x02; // Value to load into Y
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.Y, 0x02);
}

TEST_F(Ccpu6502Test, TestExecuteLDXZeroPage) {
    cpu.SystemMemory->at(0xFFFC) = 0xA6; // LDX Zero Page
    cpu.SystemMemory->at(0xFFFD) = 0x10; // Zero Page address
    cpu.SystemMemory->at(0x0010) = 0x03; // Value at Zero Page address
    cpu.ExecuteInstruction();
    EXPECT_EQ(cpu.X, 0x03);
}

