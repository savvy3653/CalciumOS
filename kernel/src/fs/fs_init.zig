
const std = @import("std");

extern fn floppy_read_sector(lba: u32, buffer: [*]u8) void;
extern fn memcmp(a: *const anyopaque, b: *const anyopaque, n: usize) callconv(.c) i32;
extern fn kprintf(fmt: [*:0] const u8, ...) callconv(.c) void;
extern fn ext2_init() void;
extern fn fat12_init() void;

pub export fn vfs_init() u16 {
    // identify fs
    var buffer: [2048]u8 align(2048) = undefined;
    var i: u32 = 0;
    while (i < 4) : (i += 1) {
        floppy_read_sector(i, buffer[i*512 ..][0..512].ptr);
    }
    
    const extMagic = std.mem.readInt(u16, buffer[1024 + 56 ..][0..2], .little);
    if (extMagic == 0xEF53) {
        kprintf("Ext2 Detected.\n");
        ext2_init();
        return 0xEF53;
    }
    if (buffer[38] == 0x28 or buffer[38] == 0x29) {
        kprintf("FAT12 Detected.\n");
        fat12_init();
        return 0x29;
    }

    kprintf("Unknown or unsupported filesystem.\n");
    return 0; 
}
