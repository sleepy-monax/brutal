#include <brutal/base/attributes.h>
#include <brutal/base/macros.h>
#include <brutal/debug/assert.h>
#include <brutal/io/window.h>
#include <brutal/math/clamp.h>

void window_init(Window *self, size_t capacity, Alloc *alloc)
{
    *self = (Window){
        .data = (uint8_t *)alloc_malloc(alloc, capacity * 3),
        .used = 0,
        .capacity = capacity,
        .alloc = alloc};
}

void window_deinit(Window *self)
{
    alloc_free(self->alloc, self->data);
}

static IoResult window_write_impl(Window *self, char const *data, size_t size)
{
    size_t to_write = m_min(self->capacity - self->used, size);
    mem_cpy(self->data + self->used, data, to_write);
    self->used += to_write;

    // Put the latest data back to the front
    if (self->used > self->capacity * 2)
    {
        mem_move(self->data, self->data + (self->used - self->capacity), self->capacity);
        self->used = self->capacity;
    }
    return OK(IoResult, to_write);
}

IoWriter window_writer(Window *self)
{
    return (IoWriter){
        .write = (IoWriteFn *)window_write_impl,
        .context = self,
    };
}

uint8_t window_peek_from_back(Window *self, size_t offset)
{
    assert_greater_equal(self->used, offset);
    return self->data[self->used - offset];
}