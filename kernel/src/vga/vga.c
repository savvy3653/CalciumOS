#include <stdbool.h>

#include "../../include/vga.h"
#include "../../include/stdlib.h"

size_t vga_row;
size_t vga_column;

enum vga_color vga_fg = VGA_COLOR_LIGHT_GREY;
enum vga_color vga_bg = VGA_COLOR_BLACK;
uint8_t vga_color;
uint16_t* vga_buffer = (uint16_t*)VGA_MEMORY;


void vga_init(void) {
	vga_row = 0;
	vga_column = 0;
	vga_color = vga_entry_color(vga_fg, vga_bg);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			vga_buffer[index] = vga_entry(' ', vga_color);
		}
	}
	enable_cursor(0, 2);
}

void vga_setcolor(uint8_t color) {
	vga_color = color;
}

void vga_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	vga_buffer[index] = vga_entry(c, color);
	update_cursor(x, y+1);
}

int vga_putchar(char c) {
	// escape sequences
	switch(c) {
		case '\0':
			return 0;

		case '\n':
			vga_column = 0;
			if (vga_row < 2) {
				vga_putentryat(' ', vga_color, vga_column, ++vga_row);
			}
			else {
				vga_putentryat('>', vga_color, vga_column++, ++vga_row);
				update_cursor(vga_column, vga_row+1);
			}
			return 1;
			break;

		case '\t':
			if (vga_column + 4 > VGA_WIDTH) {
				vga_putchar('\n');	// go to newline if no space for \t
			} else {
				for (int i = 0; i < 4; i++) {
					vga_putentryat(' ', vga_color, ++vga_column, vga_row);
				}
			}
			return 1;
			break;

		case '\b':
			if ((vga_column > 1) && (vga_row > 2)) {
				vga_putentryat(' ', vga_color, --vga_column, vga_row);
			}
			return 1;
			break;

		default:
			vga_putentryat(c, vga_color, vga_column, vga_row);
			if (++vga_column == VGA_WIDTH) {
				vga_column = 0;
				if (++vga_row == VGA_HEIGHT)
					vga_row = 0;
			}
			update_cursor(vga_column, vga_row+1);
			return 1;	
			break;	
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

// void kprintln(const char* data) {
// 	kputchar(data, strlen(data));
// 	kputchar("\n", 1);
// }


// RED SCREEN
void rs_init(void) {
	vga_fg = VGA_COLOR_WHITE;
	vga_bg = VGA_COLOR_RED;
	vga_init();
}
