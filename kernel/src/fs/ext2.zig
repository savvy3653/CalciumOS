
const fs = @import("fs.zig");

// TODO: make error handling
// const FsErrors = error {};

extern fn floppy_read_sector(lba: u32, buffer: [*]u8) void;
extern fn kprintf(fmt: [*:0] const u8, ...) callconv(.c) void;
extern fn ceil(a: u32, b: u32) callconv(.c) u32;

var superblock: *fs.Superblock = undefined;

pub export fn ext2_init() void {
    var buffer: [512]u8 align(@alignOf(fs.Superblock)) = undefined;
    floppy_read_sector(2, &buffer);
    superblock = @ptrCast(@alignCast(&buffer));

    const bg1 = ceil(superblock.blocks_count, superblock.blocks_per_group);
    const bg2 = ceil(superblock.inodes_count, superblock.inodes_per_group);
    if (bg1 != bg2) kprintf("Undefined block groups!");
    kprintf("%x", superblock.magic);
}



