
const fs = @import("fs.zig");

extern fn floppy_read_sector(lba: u32, buffer: [*]u8) void;
extern fn kprintf(fmt: [*:0] const u8, ...) callconv(.c) void;

var superblock: *fs.Superblock = undefined;

pub export fn parse_superblock() void {
    var buffer: [512]u8 align(@alignOf(fs.Superblock)) = undefined;
    floppy_read_sector(2, &buffer);

    superblock = @ptrCast(@alignCast(&buffer));
    kprintf("%d\n", superblock.blocks);
}
