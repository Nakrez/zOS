#include <string.h>

#include <kernel/types.h>
#include <kernel/zos.h>
#include <kernel/console.h>

#include <arch/mp.h>

static void *mp_find_magic(void *start, size_t len)
{
    for (size_t i = 0; i < len / 16; start += 16)
    {
        if (!strncmp(start, "_MP_", 4))
            return start;
    }

    return NULL;
}

void mp_parse_tables(void)
{
    void *table;

    if ((table = mp_find_magic((void *)EBDA_ADDR, 1024)))
        goto mp_found;

    if ((table = mp_find_magic((void *)BASE_MEM_ADDR, 1024)))
        goto mp_found;

    if ((table = mp_find_magic((void *)BIOS_ROM_START, BIOS_ROM_LEN)))
        goto mp_found;

    return;

mp_found:
    console_message(T_INF, "MP tables found at 0x%x", table);
}
