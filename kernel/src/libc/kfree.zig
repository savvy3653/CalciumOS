
const HEAP_block = extern struct {
    vaddr: usize,
    size: usize,
    free: bool,
    next: ?*HEAP_block,
};

extern var heap_blocks: [*]HEAP_block;
extern fn memset(ptr: *anyopaque, value: c_int, num: usize) callconv(.c) *anyopaque;

export fn kfree(ptr: ?*anyopaque ) void {
    if (ptr == null) return;

    var it: ?*HEAP_block = &heap_blocks[0];
    while (it) |block| {
        if (block.vaddr == @intFromPtr(ptr.?)) {
            _ = memset(ptr.?, 0, block.size);
            block.free = true;
            return;
        }
        it = block.next;
    }
}
