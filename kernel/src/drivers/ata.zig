
extern fn irq_install_routine(irq: u8, handler: *const fn (*INT_registers) callconv(.c) void) callconv(.c) void;
extern fn outb(port: u16, val: u8) callconv(.c) void;
extern fn outw(port: u16, val: u16) callconv(.c) void;
extern fn inb(port: u16) callconv(.c) u8;
extern fn inw(port: u16) callconv(.c) u16;
extern fn PIC_sendEOI(irq: u8) callconv(.c) void;
extern fn kprintf(fmt: [*:0] const u8, ...) callconv(.c) void;
extern fn ksleep(ms: u32) void;

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
    outb(0x1F7, 0x20);  // 'read' command
    status_reg_check();

    while (!received_irq) {}
    received_irq = false;

    var i: u32 = 0;
    while (i < 512) : (i += 2) { // 256 times * 2 bytes
        const bytes: u16 = inw(0x1F0);
        const slice: [2]u8 = .{ @truncate(bytes & 0xFF), @truncate(bytes >> 8) };
        @memcpy(buffer[i..i+2], &slice);
    }
    ksleep(1);
}

pub export fn ata_write_sector(lba: u32, buffer: [*]u8) void {
    outb(0x1F6, @intCast(0xE0 | (0 << 4) | ((lba >> 24) & 0x0F)));
    outb(0x1F2, 1);
    outb(0x1F3, @intCast(lba & 0xFF));
    outb(0x1F4, @intCast((lba >> 8) & 0xFF));
    outb(0x1F5, @intCast((lba >> 16) & 0xFF));
    outb(0x1F7, 0x30);  // 'write' command
    status_reg_check();

    var i: u32 = 0;
    while (i < 512) : (i += 2) { // 256 times * 2 bytes
        const bytes: u16 = @as(u16, buffer[i]) | (@as(u16, buffer[i+1]) << 8);
        outw(0x1F0, bytes);
    }
    ksleep(1);    
    outb(0x1F7, 0xE7);  // cache flush
    while (true) {  // waiting for BSY to clear
        const ready: u8 = inb(0x1F7);
        if (ready & 0x80 == 0) break;
    }
}

fn status_reg_check() void {
    while (true) {
        const ready: u8 = inb(0x1F7); 
        if ((ready & 0x80) == 0 and (ready & 0x8) != 0) break; // BSY = 0 ; DRQ = 1 
    }
}
