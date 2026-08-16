
const std = @import("std");
const fs = @import("fs.zig");

extern fn floppy_read_sector(lba: u32, buffer: [*]u8) void;
extern fn ata_read_sector(lba: u32, buffer: [*]u8) void;
extern fn memcmp(a: *const anyopaque, b: *const anyopaque, n: usize) callconv(.c) i32;
extern fn kprintf(fmt: [*:0] const u8, ...) callconv(.c) void;
extern fn ext2_init() void;
extern fn fat12_init(bpb_s: *const fs.BPB) void;

extern var FAT_start_sector: u32;
extern var FAT_sectors: u32;
extern var rood_dir_start_sector: u32;
extern var root_dir_sectors: u32;
extern var data_start_sector: u32;
extern var data_sectors: u32;

pub export fn vfs_init(read_sector: *const fn(u32, [*]u8) callconv(.c) void) u16 {
    // identify fs
    var buffer: [2048]u8 align(2048) = undefined;
    var i: u32 = 0;
    while (i < 4) : (i += 1) {
        read_sector(i, buffer[i*512 ..][0..512].ptr);
    }
    
    const extMagic = std.mem.readInt(u16, buffer[1024 + 56 ..][0..2], .little);
    if (extMagic == 0xEF53) {
        kprintf("Ext2 Detected.\n");
        ext2_init();
        return 0xEF53;
    }
    if (buffer[38] == 0x28 or buffer[38] == 0x29) {
        const fat = fat_detect(&buffer);
        return fat;
    }

    kprintf("Unknown or unsupported filesystem.\n");
    return 0; 
}

fn fat_detect(buffer: *const [2048]u8) u16 {
    var bpb: fs.BPB = undefined;
    const dest = std.mem.asBytes(&bpb);
    @memcpy(dest[0..], buffer[0..@sizeOf(fs.BPB)]);

    const sectors_total: u32 = 
    if (bpb.sectors_total != 0)
        bpb.sectors_total
    else
        bpb.expanded_sectors_total;

    FAT_start_sector = bpb.reserved_sector_count;
    FAT_sectors = bpb.sectors_per_fat * bpb.fat_numbers;
    rood_dir_start_sector = FAT_start_sector + FAT_sectors;
    root_dir_sectors = (32 * bpb.root_entry_count + bpb.bytes_per_sector - 1) / bpb.bytes_per_sector; 
    data_start_sector = rood_dir_start_sector + root_dir_sectors;
    data_sectors = sectors_total - data_start_sector;

    const cnt_of_clusters: u32 = data_sectors / bpb.sectors_per_cluster;
    if (cnt_of_clusters <= 4085) {
        kprintf("FAT12 Detected.\n");
        fat12_init(&bpb);
        return 0x2912;
    } // TODO: checkup for other fats later
    else {
        kprintf("Unknown FAT!\n");
    }
    return 0;
}
