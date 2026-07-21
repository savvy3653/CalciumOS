#include <stdbool.h>

#include "../../include/vga.h"
#include "../../include/stdlib.h"

size_t vga_row;
size_t vga_column;
uint8_t vga_color;
uint16_t* vga_buffer = (uint16_t*)VGA_MEMORY;


void vga_init(void) {
	vga_row = 0;
	vga_column = 0;
	vga_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			vga_buffer[index] = vga_entry(' ', vga_color);
		}
	}
}

void vga_setcolor(uint8_t color) {
	vga_color = color;
}

void vga_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	vga_buffer[index] = vga_entry(c, color);
}

void vga_putchar(char c) {
	// escape sequences
	if (c == '\n') {
		vga_column = 0;
		vga_putentryat(' ', vga_color, vga_column, ++vga_row);
		return;
	}

	// default
	vga_putentryat(c, vga_color, vga_column, vga_row);
	if (++vga_column == VGA_WIDTH) {
		vga_column = 0;
		if (++vga_row == VGA_HEIGHT)
			vga_row = 0;
	}
}


// PRINT FUNCTIONS
void kputchar(const char* data, size_t size)  {
	for (size_t i = 0; i < size; i++)
		vga_putchar(data[i]);
}

void kprint(const char* data) {
	kputchar(data, strlen(data));
}

void kprintln(const char* data) {
	kputchar(data, strlen(data));
	kputchar("\n", 1);
}