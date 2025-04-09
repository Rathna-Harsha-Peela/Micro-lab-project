#include <AT89C5131.h>
#include "lcd.h"

bit transmit_completed = 0;
unsigned char temp_spi_data;
unsigned char data_buffer[2];
unsigned char idx = 0;
char sum_ascii[3] = {0, 0, '\0'};

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

bit is_prime(unsigned int num) {
    int o = 0;
    if (num < 2) return 0;
    for (o = 2; o * o <= num; o++) {
        if (num % o == 0) return 0;
    }
    return 1;
}

void spi_interrupt(void) interrupt 9 {
    if (SPSTA & 0x80) {
        temp_spi_data = SPDAT;
        data_buffer[idx++] = temp_spi_data;

        if (idx == 2) {
            unsigned int sum = data_buffer[0] + data_buffer[1];
            bit result = is_prime(sum);
						SPDAT = result ? 1 : 0; // Send result back to master
            idx = 0;
					
            lcd_init();
            lcd_cmd(0x80);
            lcd_write_string("N1: ");
            int_to_string(data_buffer[0], sum_ascii);
            lcd_write_string(sum_ascii);
						lcd_write_char(32);
						lcd_write_string("N2: ");
            int_to_string(data_buffer[1], sum_ascii);
            lcd_write_string(sum_ascii);
            msdelay(10);

            lcd_cmd(0xC0);
            lcd_write_string("Sum: ");
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
