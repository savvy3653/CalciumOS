
export fn __zig_probe_stack() callconv(.naked) void {
    asm volatile("ret");
}

const int_floppy = 6;
const INT_registers = opaque {};

// for 1.44 MB Floppy
const HEADS: u32 = 2;
const SECTORS_PER_TRACK: u32 = 18;

const dtypes = [_][]const u8 {
    "none",
    "360 KB 5.25 Drive",
    "1.2 MB 5.25 Drive",
    "720 KB 3.5 Drive",
    "1.44 MB 3.5 Drive",
    "2.88 MB 3.5 Drive"
};
var received_irq: bool = false;
var motor: bool = false;

const FloppyDir = enum { read, write };

var floppy_dmabuf: [512]u8 align(0x10000) = undefined;
const floppy_dmalen: u32 = 512;

const AddrCount = extern union {
    l: u32,
    b: [4]u8,
};

extern fn irq_install_routine(irq: u8, handler: *const fn (*INT_registers) callconv(.c) void) callconv(.c) void;
extern fn outb(port: u16, val: u8) callconv(.c) void;
extern fn inb(port: u16) callconv(.c) u8;
extern fn PIC_sendEOI(irq: u8) callconv(.c) void;
extern fn ksleep(ms: u32) void;
extern fn kprintf(fmt: [*:0] const u8, ...) callconv(.c) void;
extern fn kpanic() callconv(.c) void;


// irq install
export fn floppy_init() void {
    irq_install_routine(int_floppy, floppy_handler);
    reset_floppy();
    specify();
    recalibrate();
}

pub fn floppy_handler(regs: *INT_registers) callconv(.c) void {
    _ = regs;
    received_irq = true;
    PIC_sendEOI(int_floppy);
}


// main fn's
pub export fn floppy_read_sector(lba: u32, buffer: [*]u8) void {
    const chs = lba_2_chs(lba);

    motor_on();
    dma_init(.read);

    write_byte(0x80 | 0x40 | 0x6);
    write_byte(chs.head << 2 | 0);
    write_byte(chs.cyl);
    write_byte(chs.head);
    write_byte(chs.sector);
    write_byte(2);
    write_byte(chs.sector+1);
    write_byte(0x1B);
    write_byte(0xFF);

    while (!received_irq) {}
    received_irq = false;

    const st0 = read_byte();
    const st1 = read_byte();
    const st2 = read_byte();
    _ = read_byte();
    _ = read_byte();
    _ = read_byte();
    _ = read_byte();

    _ = st0;
    _ = st1;
    _ = st2;

    motor_off();

    @memcpy(buffer, &floppy_dmabuf);
}

pub export fn floppy_write_sector(lba: u32, buffer: [*]u8) void {
    const chs = lba_2_chs(lba);
    
    motor_on();
    dma_init(.write);

    @memcpy(&floppy_dmabuf, buffer[0..512]);

    write_byte(0x80 | 0x40 | 0x5);
    write_byte(chs.head << 2 | 0);
    write_byte(chs.cyl);
    write_byte(chs.head);
    write_byte(chs.sector);
    write_byte(2);
    write_byte(chs.sector+1);
    write_byte(0x1B);
    write_byte(0xFF);

    while (!received_irq) {}
    received_irq = false;

    const st0 = read_byte();
    const st1 = read_byte();
    const st2 = read_byte();
    _ = read_byte();
    _ = read_byte();
    _ = read_byte();
    _ = read_byte();

    _ = st0;
    _ = st1;
    _ = st2;

    motor_off();
}

export fn floppy_detect_drives() void {
    outb(0x70, 0x10);
    ksleep(300);
    const drives: u8 = inb(0x71);
    
    kprintf("Slave floppy drive: %s\n", dtypes[drives >> 4].ptr);
    kprintf("Master floppy drive: %s\n", dtypes[drives & 0xF].ptr);
}

fn lba_2_chs(lba: u32) struct { cyl: u8, head: u8, sector: u8 } {
    const cyl: u8 = @intCast(lba / (HEADS * SECTORS_PER_TRACK));
    const temp: u32 = lba % (HEADS * SECTORS_PER_TRACK);
    const head: u8 = @intCast(temp / SECTORS_PER_TRACK);
    const sector: u8 = @intCast(temp % SECTORS_PER_TRACK + 1);
    return .{ .cyl = cyl, .head = head, .sector = sector };
}

fn motor_on() void {
    // drive 0 / normal operation / enable irq and dma / turn drive 0 motor on 
    outb(0x3F2, 0x1C); 
    ksleep(500);
    motor = true;
}

fn motor_off() void {
    outb(0x3F2, 0xC);
    motor = false;
}


// commands
// i use recommended safe values for each parameter
fn specify() void {
    write_byte(3); // command byte
    write_byte((8 << 4) | 0); 
    write_byte((30 << 1) | 0);
}

fn recalibrate() void {
    write_byte(7);
    write_byte(0); // drive n:0
    ksleep(5000);
    write_byte(8); // SENSE_INTERRUPT
    _ = read_byte();
    _ = read_byte();
}


// R/W
fn read_byte() u8 {
    wait_msr_ready(0x40); // dio=1
    const byte: u8 = inb(0x3F5);
    return byte;
}

fn write_byte(byte: u8) void {
    wait_msr_ready(0);
    outb(0x3F5, byte);
}


fn reset_floppy() void {
    received_irq = false;

    outb(0x3F2, 0);
    outb(0x3F2, 0x0C);

    while (!received_irq) {}

    var i: u32 = 4;
    while (i > 0) : (i -= 1) {
        write_byte(8);  // send SENSE_INTERRUPT to FIFO port
        _ = read_byte(); // st0
        _ = read_byte(); // cyl
    }
}

// ask MSR reg if data is ready
fn wait_msr_ready(dio: u8) void {
    var i: u32 = 0;
    while (i < 100000) : (i += 1) {
        const msr = inb(0x3F4);
        if ((msr & 0xC0) == (0x80 | dio)) {
            return;
        }
    }
    //return error.FDCtimeout;
    kpanic();
}


pub fn dma_init(dir: FloppyDir) void {
    const phys_addr = @intFromPtr(&floppy_dmabuf) - 0xC0000000; 
    const a: AddrCount = .{ .l = phys_addr };
    const c: AddrCount = .{ .l = floppy_dmalen - 1 }; // -1 because of DMA counting

    // check that address is at most 24-bits (under 16MB)
    // check that count is at most 16-bits (DMA limit)
    // check that if we add count and address we don't get a carry
    // (DMA can't deal with such a carry, this is the 64k boundary limit)
    if ((a.l >> 24) != 0 or (c.l >> 16) != 0 or (((a.l & 0xFFFF) + c.l) >> 16) != 0) {
        kpanic();
    }

    const mode: u8 = switch (dir) {
        .read => 0x46,  // single/inc/no-auto/to-mem/chan2
        .write => 0x4A, // single/inc/no-auto/from-mem/chan2
    };

    outb(0x0A, 0x06); // mask chan 2

    outb(0x0C, 0xFF); // reset flip-flop
    outb(0x04, a.b[0]); // address low byte
    outb(0x04, a.b[1]); // address high byte

    outb(0x81, a.b[2]); // external page register

    outb(0x0C, 0xFF); // reset flip-flop
    outb(0x05, c.b[0]); // count low byte
    outb(0x05, c.b[1]); // count high byte

    outb(0x0B, mode); // set mode

    outb(0x0A, 0x02); // unmask chan 2
}



