#include "arch/pmm.h"
#include "arch.h"
#include "base/macros.h"
#include "host/mem.h"
#include <library/mem.h>

#define PMM_DEBUG_PRINT

struct bitmap pmm_bitmap = {};

size_t last_free_bitmap_entry = 0;
size_t available_memory = 0;
size_t bitmap_target_size = 0;

static uintptr_t memory_map_get_highest_address(struct stivale2_struct_tag_memmap *memory_map)
{
    size_t length = memory_map->memmap[memory_map->entries - 1].length;
    size_t start = memory_map->memmap[memory_map->entries - 1].base;

    return length + start;
}

static void pmm_bitmap_find_addr(struct stivale2_struct_tag_memmap *memory_map)
{
    for (size_t i = 0; i < memory_map->entries; i++)
    {
        if (memory_map->memmap[i].type != STIVALE2_MMAP_USABLE)
        {
            continue;
        }

        // + 2 PAGE FOR SECURITY
        if (memory_map->memmap[i].length > ((bitmap_target_size / 8) + (HOST_MEM_PAGESIZE * 2)))
        {
            pmm_bitmap =
                bitmap((void *)memory_map->memmap[i].base, bitmap_target_size);
            return;
        }
    }
}

static void clear_pmm_bitmap()
{
    bitmap_fill(&pmm_bitmap, true);
    last_free_bitmap_entry = 0;
}

void init_pmm_bitmap_memory_map(struct stivale2_struct_tag_memmap *memory_map)
{
    for (size_t i = 0; i < memory_map->entries; i++)
    {
        size_t start = ALIGN_UP(memory_map->memmap[i].base, HOST_MEM_PAGESIZE);
        size_t size = ALIGN_DOWN(memory_map->memmap[i].length, HOST_MEM_PAGESIZE);

#ifdef PMM_DEBUG_PRINT
        print(arch_debug(), "memory map: type: {x} {x}-{x}\n",
              memory_map->memmap[i].type, start, start + size);
#endif

        if (memory_map->memmap[i].type != STIVALE2_MMAP_USABLE)
        {
            continue;
        }

        pmm_bitmap_free((void *)start, size / HOST_MEM_PAGESIZE);

        available_memory += size / HOST_MEM_PAGESIZE;
    }
}

static size_t pmm_bitmap_find_free(size_t page_count)
{
    size_t free_entry_count_in_a_row = 0;
    size_t free_entry_start = 0;

    for (size_t i = last_free_bitmap_entry; i < pmm_bitmap.size; i++)
    {
        if (!bitmap_get(&pmm_bitmap, i))
        {
            if (free_entry_count_in_a_row == 0)
            {
                free_entry_start = i;
            }

            free_entry_count_in_a_row++;
        }
        else
        {
            free_entry_count_in_a_row = 0;
            free_entry_start = 0;
        }

        if (page_count <= free_entry_count_in_a_row)
        {
            last_free_bitmap_entry = free_entry_start + page_count;
            return free_entry_start;
        }
    }

    if (last_free_bitmap_entry == 0)
    {
        return 0;
    }
    else
    {
        // retry but from 0 instead of from the last free entry
        last_free_bitmap_entry = 0;
        return pmm_bitmap_find_free(page_count);
    }
}

void *pmm_bitmap_alloc(size_t page_count)
{
    size_t page = pmm_bitmap_find_free(page_count);
    bitmap_set_range(&pmm_bitmap, page, page_count, true);
    return (void *)(page * HOST_MEM_PAGESIZE);
}

void *pmm_bitmap_alloc_zero(size_t page_count)
{
    void *result = pmm_bitmap_alloc(page_count);

    if (result)
    {
        mem_set(result, 0, page_count * HOST_MEM_PAGESIZE);
    }

    return result;
}

int pmm_bitmap_free(void *addr, size_t page_count)
{
    if (addr == NULL)
    {
        return -1;
    }

    size_t page_idx = (uintptr_t)addr / HOST_MEM_PAGESIZE;

    bitmap_set_range(&pmm_bitmap, page_idx, page_count, false);

    last_free_bitmap_entry = page_idx;
    return 0;
}

void init_pmm_bitmap(struct stivale2_struct_tag_memmap *memory_map)
{
    bitmap_target_size = memory_map_get_highest_address(memory_map) /
                         HOST_MEM_PAGESIZE; // load bitmap size

    pmm_bitmap_find_addr(memory_map);

    clear_pmm_bitmap();

    init_pmm_bitmap_memory_map(memory_map);

    // we set the first page used, as the page 0 is NULL
    bitmap_set(&pmm_bitmap, 0, true);

    // we set where the bitmap is located to used
    bitmap_set_range(&pmm_bitmap, (uintptr_t)pmm_bitmap.data / HOST_MEM_PAGESIZE, (pmm_bitmap.size / HOST_MEM_PAGESIZE) / 8 + 2, true);
}