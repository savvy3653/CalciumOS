#include "../../../include/stdlib.h"
#include "../../../include/pic.h"

void PIC_sendEOI(uint8_t irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND, PIC_EOI); // PIC slave
	
	outb(PIC1_COMMAND, PIC_EOI);	// PIC master
}
