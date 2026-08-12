
const fs = @import("fs.zig");

const FileError = error{
    FileNotFound,
};

extern fn floppy_read_sector(lba: u32, buffer: [*]u8) void;
extern fn kprintf(fmt: [*:0] const u8, ...) callconv(.c) void;
extern fn ceil(a: u32, b: u32) callconv(.c) u32;
extern fn memcmp(a: *const anyopaque, b: *const anyopaque, n: usize) callconv(.c) i32;

var bpb: fs.BPB = undefined;

var FAT_start_sector: u32 = undefined;
var FAT_sectors: u32 = undefined;
var rood_dir_start_sector: u32 = undefined;
var root_dir_sectors: u32 = undefined;
var data_start_sector: u32 = undefined;
var data_sectors: u32 = undefined;

const sector_size: u16 = 512;

pub export fn fat12_init() void {
    var buffer: [512]u8 align(@alignOf(fs.BPB)) = undefined;
    floppy_read_sector(0, &buffer);
    const tmp: *fs.BPB = @ptrCast(@alignCast(&buffer));
    bpb = tmp.*;

    FAT_start_sector = bpb.reserved_sector_count;
    FAT_sectors = bpb.sectors_per_fat * bpb.fat_numbers;
    rood_dir_start_sector = FAT_start_sector + FAT_sectors;
    // 32 stands for 'size of dir entry': 
    root_dir_sectors = (32 * bpb.root_entry_count + bpb.bytes_per_sector - 1) / bpb.bytes_per_sector; 
    data_start_sector = rood_dir_start_sector + root_dir_sectors;
    data_sectors = bpb.sectors_total - data_start_sector;

    const cnt_of_clusters: u32 = data_sectors / bpb.sectors_per_cluster;
    if (cnt_of_clusters <= 4085) {
        kprintf("FAT12 detected.\n");
    } // TODO: checkup for other fats later
    else {
        kprintf("No filesystem detected!\n");
    }
}

pub export fn read_file(fname: [*]u8) void {
    var de: fs.FAT12_Directory_Entry = undefined;
    if (read_directory(fname)) |value| {
        de = value;
    } else |err| {
        switch(err) {
            FileError.FileNotFound =>  {
                kprintf("File not found.\n");
                return;
            }
        }
    }
    //if (de == null) {
    //    kprintf("File error occured!\n");
    //    return;
    //}

    // parsing clusters
    // TODO: get de.file_size and add support for a few sectors in file?
    var cluster_num: u32 = de.first_cluster;
    var cluster_info_num: u32 = 0;  
    while (true) {
        if (cluster_num < 2) {
            kprintf("Invalid cluster used!\n");
            break;
        }

        var eof: bool = false;
        var buffer: [sector_size]u8 align(512) = undefined;
        const fat_offset: u32 = cluster_num + (cluster_num / 2);
        const fat_sector: u32 = FAT_start_sector + (fat_offset / sector_size);
        const entry_offset: u32 = fat_offset % sector_size;

        floppy_read_sector(fat_sector, &buffer); // using buffer as FAT table

        if (cluster_num & 1 == 0) { // even
            cluster_info_num = @as(u32, buffer[entry_offset]) | ((@as(u32, buffer[entry_offset+1]) & 0x0F) << 8);
        } else { // odd
            cluster_info_num = (@as(u32, buffer[entry_offset]) >> 4) | (@as(u32, buffer[entry_offset+1]) << 4) | ((@as(u32, buffer[entry_offset+2]) & 0xF0) << 8);
        }
        cluster_info_num = cluster_info_num & 0xFFF;
        
        if (cluster_info_num >= 0xFF8) eof = true;
        if (cluster_info_num == 0xFF7) continue; // 'bad' cluster

        // cluster is okay? let's fuckin' read it!
        const first_sector_of_cluster: u32 = data_start_sector + (cluster_num - 2) * bpb.sectors_per_cluster;
        floppy_read_sector(first_sector_of_cluster, &buffer); // now use buffer as buffer
        
        for (buffer) |byte| {
            if (byte == 0) break;
            kprintf("%c", @as(c_int, byte));
        }
        if (eof == true) break;

        // goto next cluster;
        cluster_num = cluster_info_num;
    }
}


// goal: find directory entry for specified file
fn read_directory(fname: [*]u8) !fs.FAT12_Directory_Entry {
    var buffer: [sector_size]u8 align(@alignOf(fs.FAT12_Directory_Entry)) = undefined;
    
    var dir_entry: u32 = rood_dir_start_sector;
    while (dir_entry < data_start_sector) : (dir_entry += @sizeOf(fs.FAT12_Directory_Entry)) {
        floppy_read_sector(dir_entry, &buffer);
        const tmp: *fs.FAT12_Directory_Entry = @ptrCast(@alignCast(&buffer));
        const de: fs.FAT12_Directory_Entry = tmp.*;

        if (de.file_name & 0xFF == 0) break; // no more files/dirs in this dir
        if (de.file_name & 0xFF == 0xE5) continue; // unused entry
        if (memcmp(@ptrCast(&de.file_name), fname, 8) == 0)  return de;
    }

    return FileError.FileNotFound;
}



