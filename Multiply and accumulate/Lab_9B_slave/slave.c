#include <AT89C5131.h>
#include "lcd.h"

bit transmit_completed = 0;
unsigned char temp_spi_data;
unsigned char data_buffer[6];
unsigned char idx = 0;
char sum_ascii[3] = {0, 0, '\0'};
unsigned char data *point1 = (unsigned char *)0x70;
unsigned char data *point2 = (unsigned char *)0x71;
unsigned char data *point3 = (unsigned char *)0x72;
unsigned char data *point4 = (unsigned char *)0x73;
unsigned char data *point5 = (unsigned char *)0x74;
unsigned char data *point6 = (unsigned char *)0x75;
unsigned char data *pointr1 = (unsigned char *)0x50;
unsigned char data *pointr2 = (unsigned char *)0x51;
unsigned char data *pointr3 = (unsigned char *)0x52;
extern void MAC();

void delay() {
    int i, j;
    for(i=0;i<300;i++) for(j=0;j<1275;j++);
}

void spi_init(void) {
    SPCON = 0x2C;  // Slave mode, CPOL=1, CPHA=1
    IEN1 |= 0x04;  
    EA = 1;        
	SPCON |= 0x40; 
}

void spi_interrupt(void) interrupt 9 {
    if (SPSTA & 0x80) {
        temp_spi_data = SPDAT;
        data_buffer[idx++] = temp_spi_data;

        if (idx == 6) {
						unsigned char sum =0;
					*point1 = data_buffer[0];
					*point2 = data_buffer[1];
					*point3 = data_buffer[2];
					*point4 = data_buffer[3];
					*point5 = data_buffer[4];
					*point6 = data_buffer[5];
						MAC();
           sum = *pointr3;
						SPDAT = sum; // Send result back to master
            idx = 0;
					
            lcd_init();
            lcd_cmd(0x80);
            lcd_write_string("Value: ");
            int_to_string(sum, sum_ascii);
            lcd_write_string(sum_ascii);
					msdelay(10);
            
        }
    }
}

void main() {
    spi_init();
    lcd_init();
    lcd_cmd(0x80);
    lcd_write_string("Waiting...");
    while(1);
}
