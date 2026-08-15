
extern fn kprintf(fmt: [*:0] const u8, ...) callconv(.c) void;

pub export fn ata_init() void {
    kprintf("ATA Detected.\n");    
}
