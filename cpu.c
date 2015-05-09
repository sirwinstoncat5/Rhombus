//CPU section
//CPU runs at 63 MHz?
#include <stdio.h>
#include <stdint.h>
#define CPU_SWITCH_DEBUG 0
#define BINARY_BIG_ENDIAN 1
//*****Begin definitions/prototypes*****

//Memory stuff
/* TODO (sirwinstoncat5#9#): Redo memory handling from using words to 
                             using bytes */
extern uint8_t fpga_mem[2097152]; 
uint16_t GPreg[8], PC, SP, productHigh, productLow;
int zeroFlag, carryFlag;
inline uint8_t CPUReadMemory(int address);
inline void CPUWriteMemory(int address, uint8_t value);

//CPU cycle variables
uint16_t instruction, opgroup, opcodea, opcodeb, opcodec, opera1, opera2, opera3, operb, operc1, operc2, operc3;

//Test functions
void ROMList(void) //Lists the ROM in hex
{
	int i;
	for(i=0; i<16384; i++)
	{
		printf("%x\n", CPUReadMemory(i));
	}
}

//Main CPU functions
void CPUinit(void);
inline void CPUcycle(void);

//Opcode function prototypes
void OpcodeNotDone(void) {printf("CPU Error: Opcode not written yet!\n");}
inline void movih(void);
inline void add(void);
inline void addi(void);
inline void adc(void);
inline void adci(void);
inline void sub(void);
inline void subi(void);
inline void and_inst(void);
inline void andi(void);
inline void or_inst(void);
inline void ori(void);
inline void xor_inst(void);
inline void xori(void);
inline void cmp(void);
inline void cmpi(void);
inline void mov(void);
inline void movi(void);
inline void bgt(void);
inline void bne(void);
inline void bcc(void);
inline void bcs(void);
inline void beq(void);
inline void ble(void);
inline void bal(void);
inline void cgt(void);
inline void cne(void);
inline void ccc(void);
inline void ccs(void);
inline void ceq(void);
inline void cle(void);
inline void cal(void);
inline void ld(void);
inline void st(void);
inline void in(void);
inline void out(void);
inline void jv(void);
inline void cv(void);
inline void lea(void);
inline void push(void);
inline void pop(void);
inline void nop(void);
inline void mul(void);
inline void stsp(void);
inline void prod(void);
inline void jr(void);
inline void cr(void);
inline void ret(void);
inline void wait(void);
inline void send(void);
inline void ldsf(void);
inline void stsf(void);
inline void initv(void);

//*****Begin functions*****

inline uint8_t CPUReadMemory(int address)
{
	return fpga_mem[address];
}
inline void CPUWriteMemory(int address, uint8_t value)
{
	fpga_mem[address]=value;
}

void CPUinit(void)
{
	PC=0;
	//ROM is loaded by main.c
}

inline void CPUcycle(void)
{
	if(CPU_SWITCH_DEBUG==1) printf("CPU Cycle! PC=%d\n", PC);
	instruction=CPUReadMemory(PC++); //Increment PC after accessing it
	//*********NOTE********* This should be big-endian.
	//if(BINARY_BIG_ENDIAN==1) instruction=(instruction>>8)+(instruction<<8);
	if(CPU_SWITCH_DEBUG==1) printf("Instruction in hex: %x\n", instruction);
	//Decode instruction into opcode and operands
	//Note that depending on addressing mode, some operands may be garbage
	opgroup=instruction>>6;
	opcodea=(instruction>>3);
	opcodeb=(instruction>>1);
	opcodec=opcodea;
	opera1=instruction-(opcodea<<3);
	operb=instruction-(opcodeb<<1);
	operc1=opera1;
	instruction=CPUReadMemory(PC++); //Get second byte of instruction
	if(CPU_SWITCH_DEBUG==1) printf("Instruction in hex: %x\n", instruction);
	opera2=instruction>>3;
	opera3=instruction-(opera2<<3);
	operb=instruction+(operb<<8);
	operc2=instruction>>7;
	operc3=instruction-(operc2<<7);
	
	if(CPU_SWITCH_DEBUG==1) printf("OpcodeA: %d\nOperA1: %d\nOperA2: %d\nOperA3: %d\n----------------------\n", opcodea, opera1, opera2, opera3);
	//Determine addressing mode
	switch(opgroup)
	{
		case 0b10: 
			switch(opcodeb) //Addressing mode B, branches/subroutine calls
			{
				case 0b1000001:
					bgt();
					break;
				case 0b1000011: 
					bne();
					break;
				case 0b1000101: 
					bcc();
					break;
				case 0b1001010: 
					bcs();
					break;
				case 0b1001100: 
					beq();
					break;
				case 0b1001110: 
					ble();
					break;
				case 0b1001111: 
					bal();
					break;
				case 0b1010001:
					cgt();
					break;
				case 0b1010011: 
					cne();
					break;
				case 0b1010101: 
					ccc();
					break;
				case 0b1011010: 
					ccs();
					break;
				case 0b1011100: 
					ceq();
					break;
				case 0b1011110: 
					cle();
					break;
				case 0b1011111: 
					cal();
					break;  
				default: 
					OpcodeNotDone();
			}
			break;
		case 0b00:
			movih(); //Movih has its own opgroup, addressing mode A
			break;
		case 0b01:
			modeA: //Addressing mode A...
			if(CPU_SWITCH_DEBUG==1) printf("Mode A!\n");
			switch(opcodea)
			{
				case 0b01000: 
					if(opera2==0b00000) add();
					else addi();
					break;
				case 0b01001: 
					if(opera2==0b00000) adc();
					else adci();
					break;
				case 0b01010: 
					if(opera2==0b00000) sub();
					else subi();
					break;
				case 0b01011: 
					if(opera2==0b00000) and_inst();
					else andi();
					break;
				case 0b01100: 
					if(opera2==0b00000) or_inst();
					else ori();
					break;
				case 0b01101: 
					if(opera2==0b00000) xor_inst();
					else xori();
					break;
				case 0b01110: 
					if(opera2==0b00000) cmp();
					else cmpi();
					break;
				case 0b01111: 
					if(opera2==0b00000) mov();
					else movi();
					break;
				case 0b11000: 
					ld();
					break;
				case 0b11001: 
					st();
					break;
				case 0b11010: 
					in();
					break;
				case 0b11011: 
					out();
					break;
				case 0b11100: 
					jv();
					break;
				case 0b11101:
					if(operc2==1) lea();
					else cv();
					break;
				case 0b11111: 
					if(CPU_SWITCH_DEBUG==1) printf("Other Instructions!\n");
					switch(opera2)
					{
						case 0b00000: 
							push();
							break;
						case 0b00001: 
							pop();
							break;
						case 0b00010: 
							nop();
							break;
						case 0b00011: 
							mul();
							break;
						case 0b00100: 
							stsp();
							break;
						case 0b00101: 
							prod();
							break;
						case 0b00110: 
							jr();
							break;
						case 0b00111: 
							cr();
							break;
						case 0b01000: 
							ret();
							break;
						case 0b01001: 
							wait();
							break;
						case 0b01010: 
							send();
							break;
						case 0b01011: 
							ldsf();
							break;
						case 0b01100: 
							stsf();
							break;
						case 0b01101: 
							initv();
							break;
						default: OpcodeNotDone();
					}
					break;
				default: OpcodeNotDone();
			}
			break; 
		case 0b11: //...or mode C?
			goto modeA;
			break;
		default: 
			printf("CPU Error: unimplemented opgroup!\n");
			break;
	}
	if(CPU_SWITCH_DEBUG==1) getchar();
}

//Opcode functions
inline void movih(void) {OpcodeNotDone();}
inline void add(void) 
{
	int i=GPreg[opera1]+GPreg[opera3];
	if(i>65535) carryFlag=1;
	else carryFlag=0;
	GPreg[opera1]+=GPreg[opera3];
}
inline void addi(void)
{
	int i=GPreg[opera1]+(opera3+(opera2<<3)-0b10000000);
	if(i>65535) carryFlag=1;
	else carryFlag=0;
	GPreg[opera1]+=(opera3+(opera2<<3)-0b10000000);
}
inline void adc(void)
{
	int i=GPreg[opera1]+GPreg[opera3];
	if(carryFlag==1) i++;
	GPreg[opera1]+=GPreg[opera3];
	if(carryFlag==1) GPreg[opera1]++;
	if(i>65535) carryFlag=1;
	else carryFlag=0;
}
inline void adci(void)
{
	int i=GPreg[opera1]+(opera3+(opera2<<3)-0b10000000);
	if(carryFlag==1) i++;
	GPreg[opera1]+=(opera3+(opera2<<3)-0b10000000);
	if(carryFlag==1) GPreg[opera1]++;
	if(i>65535) carryFlag=1;
	else carryFlag=0;
}
inline void sub(void)
{
	GPreg[opera1]-=GPreg[opera3];
}
inline void subi(void)
{
	GPreg[opera1]-=(opera3+(opera2<<3)-0b10000000);
}
inline void and_inst(void)
{
	GPreg[opera1]=GPreg[opera1]&GPreg[opera3];
}
inline void andi(void)
{
	GPreg[opera1]=GPreg[opera1]&(opera3+(opera2<<3)-(0b10000000));
}
inline void or_inst(void)
{
	GPreg[opera1]=GPreg[opera1]|GPreg[opera3];
}
inline void ori(void)
{
	GPreg[opera1]=GPreg[opera1]|(opera3+(opera2<<3)-(0b10000000));
}
inline void xor_inst(void)
{
	GPreg[opera1]=GPreg[opera1]^GPreg[opera3];
}
inline void xori(void)
{
	GPreg[opera1]=GPreg[opera1]^(opera3+(opera2<<3)-(0b10000000));
}
inline void cmp(void)
{
	if(GPreg[opera1]==GPreg[opera3]) zeroFlag=1;
	else zeroFlag=0;
}
inline void cmpi(void)
{
	if(GPreg[opera1]==(opera3+(opera2<<3)-(0b10000000))) zeroFlag=1;
	else zeroFlag=0;
}
inline void mov(void)
{
	GPreg[opera1]=GPreg[opera3];
}
inline void movi(void)
{
	GPreg[opera1]=(opera3+(opera2<<3)-(0b10000000));
}
//Branches
inline void bgt(void) {OpcodeNotDone();}
inline void bne(void) {OpcodeNotDone();}
inline void bcc(void) {OpcodeNotDone();}
inline void bcs(void) {OpcodeNotDone();}
inline void beq(void) {OpcodeNotDone();}
inline void ble(void) {OpcodeNotDone();}
inline void bal(void) {OpcodeNotDone();}
//Subroutine calls
inline void cgt(void) {OpcodeNotDone();}
inline void cne(void) {OpcodeNotDone();}
inline void ccc(void) {OpcodeNotDone();}
inline void ccs(void) {OpcodeNotDone();}
inline void ceq(void) {OpcodeNotDone();}
inline void cle(void) {OpcodeNotDone();}
inline void cal(void) {OpcodeNotDone();}
//Memory
inline void ld(void) {OpcodeNotDone();}
inline void st(void) {OpcodeNotDone();}
//IO
inline void in(void) {OpcodeNotDone();}
inline void out(void) {OpcodeNotDone();}
//Vector jump/call
inline void jv(void) {OpcodeNotDone();}
inline void cv(void) {OpcodeNotDone();}
//Load effective address
inline void lea(void) {OpcodeNotDone();}
//Miscellaneous
inline void push(void) {OpcodeNotDone();}
inline void pop(void) {OpcodeNotDone();}
inline void nop(void)
{
	return;
}
inline void mul(void)
{
	uint32_t i=GPreg[opera1]*GPreg[opera3];
	productHigh=i>>16;
	productLow=i-(productHigh<<16);
}
inline void stsp(void) {OpcodeNotDone();}
inline void prod(void) {OpcodeNotDone();}
inline void jr(void) {OpcodeNotDone();}
inline void cr(void) {OpcodeNotDone();}
inline void ret(void) {OpcodeNotDone();}
inline void wait(void) {OpcodeNotDone();}
inline void send(void)
{
	if(CPU_SWITCH_DEBUG==1) printf("Serial Send!\n");
	printf("%c", GPreg[opera1]);
}
inline void ldsf(void) {OpcodeNotDone();}
inline void stsf(void) {OpcodeNotDone();}
inline void initv(void) {OpcodeNotDone();}
