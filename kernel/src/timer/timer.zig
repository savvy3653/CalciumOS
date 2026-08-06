
const int_timer = 0;
const INT_registers = opaque {};

var ticks: u32 = 0;
const PIT_BASE_FRQ: u32 = 1193182;

extern fn outb(port: u16, val: u8) callconv(.c) void;
extern fn PIC_sendEOI(irq: u8) callconv(.c) void;
extern fn irq_install_routine(irq: u8, handler: *const fn (*INT_registers) callconv(.c) void) callconv(.c) void;

export fn pit_init(frq_hz: u32) void {
    irq_install_routine(int_timer, pit_handler);
    const divisor: u16 = @intCast(PIT_BASE_FRQ / frq_hz);

    outb(0x43, 0x36);

    outb(0x40, @truncate(divisor));
    outb(0x40, @truncate(divisor >> 8));
}

pub fn pit_handler(regs: *INT_registers) callconv(.c) void {
    ticks += 1;
    _ = regs;
    PIC_sendEOI(int_timer);   
}

export fn ksleep(ms: u32) void {
    const target = ticks + ms;
    while (ticks < target) {
        asm volatile ("hlt");
    }
}
