#include <AT89C5131.h>
#include "lcd.h"
#include "serial.h"

sbit SS = P1^4;  // Slave Select

bit transmit_completed = 0;
unsigned char temp_spi_data;
char int_ascii[3] = {0, 0, '\0'};

void delay() {
    int i, j;
    for(i=0;i<300;i++) for(j=0;j<1275;j++);  // Short delay
}

void spi_init(void) {
    SPCON = 0xBE;  // Master mode, CPOL=1, CPHA=1, Fclk/16
    IEN1 |= 0x04;  // Enable SPI interrupt
    EA = 1;        // Enable global interrupt
    SPCON |= 0x40; // Enable SPI
}

void spi_interrupt(void) interrupt 9 {
    if (SPSTA & 0x80) {
        temp_spi_data = SPDAT;
        transmit_completed = 1;
    }
}

unsigned char spi_trx(unsigned char dat) {
    transmit_completed = 0;
    SPDAT = dat;
    while (!transmit_completed);  // Wait for transfer to complete
    return temp_spi_data;
}

void main() {
    unsigned char prime11= 0,prime12 =0;
	unsigned char prime1=0, prime2 =0;
    unsigned char result;
	unsigned char ch;
	
    lcd_init();
    lcd_cmd(0x80);
	
		uart_init(); //Initialising UART

		transmit_string("Send the primes\r\n");
		
ch = receive_char();
prime11 = ch - 48;
ch=receive_char();
prime12 = ch - 48;
	
prime1 = prime11*10 + prime12;

lcd_write_string("Received ");
int_to_string(prime1, int_ascii);
lcd_write_string(int_ascii);
lcd_cmd(0xC0);
lcd_write_string("Send second..");
msdelay(50);

	
ch = receive_char();
prime11 = ch - 48;
ch=receive_char();
prime12 = ch- 48;
	
prime2 = prime11*10 + prime12;

lcd_cmd(0x10);
lcd_cmd(0x80);
lcd_write_string("Received ");
int_to_string(prime2, int_ascii);
lcd_write_string(int_ascii);
msdelay(1000);
		
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		lcd_write_string("Sending Primes");
		
			    spi_init();
    SS = 0;  // Select slave

    delay();                // Short delay to ensure slave is ready
    spi_trx(prime1);        // Send first prime
    msdelay(10);
    spi_trx(prime2);        // Send second prime
    msdelay(10);
    result = spi_trx(0x00); // Send dummy byte, receive result
		
    SS = 1; // Deselect slave

    lcd_cmd(0xC0);
    if (result == 1)
        lcd_write_string("Result: PRIME");
    else
        lcd_write_string("Result: NOT PRIME");

    while(1);
}
