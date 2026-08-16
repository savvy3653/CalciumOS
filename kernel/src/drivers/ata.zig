
extern fn irq_install_routine(irq: u8, handler: *const fn (*INT_registers) callconv(.c) void) callconv(.c) void;
extern fn outb(port: u16, val: u8) callconv(.c) void;
extern fn inb(port: u16) callconv(.c) u8;
extern fn inw(port: u16) callconv(.c) u16;
extern fn PIC_sendEOI(irq: u8) callconv(.c) void;
extern fn kprintf(fmt: [*:0] const u8, ...) callconv(.c) void;

const primary_irq = 14;
const secondary_irq = 15;
const INT_registers = opaque {};

var received_irq: bool = false;

pub export fn ata_init() void {
    const regular_status: u8 = inb(0x1F7);
    if (regular_status == 0xFF) {
        kprintf("No active ATA drives found.\n");
        return;
    }
    irq_install_routine(primary_irq, ata_handler);
    kprintf("ATA Detected.\n");    
}

pub fn ata_handler(regs: *INT_registers) callconv(.c) void {
    _ = regs;
    received_irq = true;
    PIC_sendEOI(primary_irq);
}

pub export fn ata_read_sector(lba: u32, buffer: [*]u8) void {
    outb(0x1F6, @intCast(0xE0 | (0 << 4) | ((lba >> 24) & 0x0F)));
    outb(0x1F2, 1);
    outb(0x1F3, @intCast(lba & 0xFF));
    outb(0x1F4, @intCast((lba >> 8) & 0xFF));
    outb(0x1F5, @intCast((lba >> 16) & 0xFF));
    outb(0x1F7, 0x20);

    while (!received_irq) {}
    received_irq = false;

    var i: u32 = 0;
    while (i < 512) : (i += 2) { // 256 times * 2 bytes
        const bytes: u16 = inw(0x1F0);
        const slice: [2]u8 = .{ @truncate(bytes & 0xFF), @truncate(bytes >> 8) };
        @memcpy(buffer[i..i+2], &slice);
    }
}
