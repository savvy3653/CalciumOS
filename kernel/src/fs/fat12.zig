
const std = @import("std");
const fs = @import("fs.zig");

const FileError = error{
    FileNotFound,
};

const FLOPPY = 0x8814;
const ATA    = 0x1818;

extern fn kprintf(fmt: [*:0] const u8, ...) callconv(.c) void;
extern fn ceil(a: u32, b: u32) callconv(.c) u32;
extern fn memcmp(a: *const anyopaque, b: *const anyopaque, n: usize) callconv(.c) i32;

extern fn floppy_read_sector(lba: u32, buffer: [*]u8) void;
extern fn floppy_write_sector(lba: u32, buffer: [*]u8) void;
extern fn ata_read_sector(lba: u32, buffer: [*]u8) void;
extern fn ata_write_sector(lba: u32, buffer: [*]u8) void;

var bpb: fs.BPB = undefined;

export var FAT_start_sector: u32 = undefined;
export var FAT_sectors: u32 = undefined;
export var rood_dir_start_sector: u32 = undefined;
export var root_dir_sectors: u32 = undefined;
export var data_start_sector: u32 = undefined;
export var data_sectors: u32 = undefined;

const sector_size: u16 = 512;

pub export fn fat12_init(bpb_s: *const fs.BPB) void {
    bpb = bpb_s.*;
}

pub export fn fat12_read_file(fname: [*:0]u8, mode: u16) void {
    var read_sector: *const fn(u32, [*]u8) callconv(.c) void = undefined; 
    if (mode == FLOPPY) {
        read_sector = floppy_read_sector;
    } else if (mode == ATA) {
        read_sector = ata_read_sector;
    } else {
        kprintf("Invalid drive mode.\n");
        return;
    }

    var de: fs.FAT12_Directory_Entry = undefined;
    if (fat12_read_directory(fname, read_sector)) |value| {
        de = value;
    } else |err| {
        switch(err) {
            FileError.FileNotFound =>  {
                kprintf("File not found.\n");
                return;
            }
        }
    }

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

        read_sector(fat_sector, &buffer); // using buffer as FAT table
        cluster_info_num = fat12_parse_infonum(&buffer, entry_offset, cluster_num, cluster_info_num);
        
        if (cluster_info_num >= 0xFF8) eof = true;
        if (cluster_info_num == 0xFF7) continue; // 'bad' cluster

        // cluster is okay? let's fuckin' read it!
        const first_sector_of_cluster: u32 = data_start_sector + (cluster_num - 2) * bpb.sectors_per_cluster;
        read_sector(first_sector_of_cluster, &buffer); // now use buffer as buffer
        
        for (buffer) |byte| {
            if (byte == 0) break;
            kprintf("%c", @as(c_int, byte));
        }
        if (eof == true) break;

        // goto next cluster;
        cluster_num = cluster_info_num;
    }
}

pub export fn fat12_write_file(fname: [*:0]u8, data: [*]u8, mode: u16) void { 
    var read_sector: *const fn(u32, [*]u8) callconv(.c) void = undefined;
    var write_sector: *const fn(u32, [*]u8) callconv(.c) void = undefined;
    if (mode == FLOPPY) {
        read_sector = floppy_read_sector;
        write_sector = floppy_write_sector;
    } else if (mode == ATA) {
        read_sector = ata_read_sector;
        write_sector = ata_write_sector;
    } else {
        kprintf("Invalid drive mode.\n");
        return;
    }

    var de: fs.FAT12_Directory_Entry = undefined;
    if (fat12_read_directory(fname, read_sector)) |value| {
        de = value;
    } else |err| {
        switch(err) {
            FileError.FileNotFound =>  {
                kprintf("File not found.\n");
                return;
            }
        }
    }

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

        read_sector(fat_sector, &buffer); // using buffer as FAT table
        cluster_info_num = fat12_parse_infonum(&buffer, entry_offset, cluster_num, cluster_info_num);
        
        if (cluster_info_num >= 0xFF8) eof = true;
        if (cluster_info_num == 0xFF7) continue; // 'bad' cluster

        // cluster is okay? let's fuckin' write it!
        const first_sector_of_cluster: u32 = data_start_sector + (cluster_num - 2) * bpb.sectors_per_cluster;
        write_sector(first_sector_of_cluster, data);
        
        if (eof == true) break;

        // goto next cluster;
        cluster_num = cluster_info_num;
    }
}


fn fat12_parse_infonum(buffer: []u8, entry_offset: u32, cluster_num: u32, cluster_info_num: u32) u32 {
    var cin: u32 = cluster_info_num;
    if (cluster_num & 1 == 0) { // even
        cin = @as(u32, buffer[entry_offset]) | ((@as(u32, buffer[entry_offset+1]) & 0x0F) << 8);
    } else { // odd
        cin = (@as(u32, buffer[entry_offset]) >> 4) | (@as(u32, buffer[entry_offset+1]) << 4) | ((@as(u32, buffer[entry_offset+2]) & 0xF0) << 8);
    } 
    return cin & 0xFFF;
}


// goal: find directory entry for specified file
fn fat12_read_directory(fname: [*:0]u8, read_sector: *const fn(u32, [*]u8) callconv(.c) void) !fs.FAT12_Directory_Entry {
    const file_struct = parse_c_filename(fname);
    var buffer: [sector_size]u8 align(@alignOf(fs.FAT12_Directory_Entry)) = undefined;
    
    var dir_sector: u32 = rood_dir_start_sector;
    while (dir_sector < data_start_sector) : (dir_sector += 1) {
        read_sector(dir_sector, &buffer);

        var offset: u32 = 0;
        while (offset < sector_size) : (offset += @sizeOf(fs.FAT12_Directory_Entry)) {
            const tmp: *fs.FAT12_Directory_Entry = @ptrCast(@alignCast(&buffer[offset]));
            const de: fs.FAT12_Directory_Entry = tmp.*;

            const filename1 = std.mem.trim(u8, &de.file_name, " ");
            const filext1 = std.mem.trim(u8, &de.file_ext, " ");
            if (de.file_name[0] == 0) return FileError.FileNotFound; // no more files/dirs in this dir
            if (de.file_name[0] == 0xE5) continue; // unused entry
            if (std.mem.eql(u8, filename1, file_struct.filename) == true and std.mem.eql(u8, filext1, file_struct.filext) == true)  return de;
        }
    }

    return FileError.FileNotFound;
}

const FileName = struct {
    filename: []const u8,
    filext: []const u8,
};

fn parse_c_filename(fname: [*:0]u8) FileName {
    var target = std.mem.span(fname);
    var ext: []const u8 = "";
    var name: []const u8 = target;
    var i: u32 = 0;
    while (i < target.len) : (i += 1) {
        if (std.mem.eql(u8, target[i..i+1], ".")) {
            ext = target[i+1..];
            name = target[0..i];      
        }
    }

    const trimmed_ext = std.mem.trim(u8, ext, " ");
    const trimmed_name = std.mem.trim(u8, name, " ");
    return .{
        .filename = trimmed_name,
        .filext = trimmed_ext,
    };
}



