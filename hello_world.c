#include <avr/io.h>

#define LED PB6
#define F_CPU 16000000

#include <util/delay.h>
#include <avr/pgmspace.h>

#define MSG_ADDR 0x4000

#define DOT_DELAY 700
#define LINE_DELAY 3*DOT_DELAY
#define SYMBOL_DELAY 2*DOT_DELAY // 3 - 1
#define WORD_DELAY 4*DOT_DELAY // 7 - 3

#define MYMEM __attribute__((section(".mymem")))
const char g_message[16] MYMEM = "HELLO WORLD";

// alphabetical symbol to morze code
// minimum amount of symbols 1
// maximum amount of symbols 4

typedef struct {
  unsigned char op0 : 2;
  unsigned char op1 : 2;
  unsigned char op2 : 2;
  unsigned char op3 : 2;
} morze_t;

// using only english alphabet
// ABCDEFGHIJKLMNOPQRSTUVWXYZ
morze_t g_symbol_table[26] = {
  { 0, 1, 3, 3 },   //A
  { 1, 0, 0, 0 },   //B
  { 1, 0, 1, 0 },   //C
  { 1, 0, 0, 3 },   //D
  { 0, 3, 3, 3 },   //E
  { 0, 0, 1, 0 },   //F
  { 1, 1, 0, 3 },   //G
  { 0, 0, 0, 0 },   //H
  { 0, 0, 3, 3 },   //I
  { 0, 1, 1, 1 },   //J
  { 1, 0, 1, 3 },   //K
  { 0, 1, 0, 0 },   //L
  { 1, 1, 3, 3 },   //M
  { 1, 0, 3, 3 },   //N
  { 1, 1, 1, 3 },   //O
  { 0, 1, 1, 0 },   //P
  { 1, 1, 0, 1 },   //Q
  { 0, 1, 0, 3 },   //R
  { 0, 0, 0, 3 },   //S
  { 1, 3, 3, 3 },   //T
  { 0, 0, 1, 3 },   //U
  { 0, 0, 0, 1 },   //V
  { 0, 1, 1, 3 },   //W
  { 1, 0, 0, 1 },   //X
  { 1, 0, 1, 1 },   //Y
  { 1, 1, 0, 0 }    //Z
};

void symbol_to_delay_code(char symbol, morze_t* ops)
{
  if ('A' <= symbol && 'Z' >= symbol)
  {
    *ops = g_symbol_table[symbol - 'A'];
  }  
  else
  {
    morze_t tmp = { 3, 3, 3, 3 };
    *ops = tmp;
  }
    
  return;
}

void string_2_morze_seq(char* str)
{
  
  while(*str != 0)
  {
    if(*str == ' ') {_delay_ms(WORD_DELAY); str++; continue;} // 4pt (1pt + 2pt + 4pt = 7pt)

    morze_t code;
    symbol_to_delay_code(*str, &code);
    int code_2[4] = {code.op0, code.op1, code.op2, code.op3};

    for(int i = 0; i < 4; i++) {
      if (code_2[i] == 3) break;

      if (code_2[i] == 0) {PORTB |= (1 << LED); _delay_ms(DOT_DELAY) ; PORTB &= ~(1 << LED);} // 1pt
      if (code_2[i] == 1) {PORTB |= (1 << LED); _delay_ms(LINE_DELAY); PORTB &= ~(1 << LED);} // 3pt

      _delay_ms(DOT_DELAY); // 1pt
    }

    str++;
    _delay_ms(SYMBOL_DELAY); // 2pt (1pt + 2pt = 3pt)
  }
  
}

void strcpy_P_s(unsigned char* dest, uint16_t from_addr, uint16_t size_of_buffer)
{
	uint16_t i = 0;
	unsigned char b_ch = pgm_read_byte(from_addr + i);
	
	while(b_ch != '\0' && i < size_of_buffer)
	{
		b_ch = pgm_read_byte(from_addr + i);
		*dest = b_ch;
		dest++;
		i++;
	}
}

int main(void)
{
	DDRB |= (1 << LED);
	
	_delay_ms(1000);
	
	char msg[16];

	strcpy_P_s(msg, MSG_ADDR, sizeof(msg));
	
	msg[15] = '\0';
	
	string_2_morze_seq(msg);
}