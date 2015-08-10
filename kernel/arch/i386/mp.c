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

static void mp_parse_config(struct mp_table *table)
{
    struct mp_config *config = (void *)(table->conf_table_ptr + 0xC0000000);
    uint8_t *type;

    if (strncmp(config->signature, "PCMP", 4))
        return;

    if (config->oem_table_ptr)
        console_message(T_ERR, "MP: OEM Table not supported");

    if (config->ext_table_length)
        console_message(T_ERR, "MP: Extended MP Table not supported");

    type = (void *)(config + 1);

    for (uint16_t i = 0; i < config->entry_count; ++i)
    {
        switch (*type)
        {
            case MP_PROCESSOR_ENTRY:
                type += sizeof (struct mp_proc);
                break;
            case MP_BUS_ENTRY:
                type += sizeof (struct mp_bus);
                break;
            case MP_IOAPIC_ENTRY:
                type += sizeof (struct mp_ioapic);
                break;
            case MP_IOINT_ENTRY:
                type += sizeof (struct mp_ioint);
                break;
            case MP_LOCINT_ENTRY:
                type += sizeof (struct mp_locint);
                break;
            default:
                console_message(T_ERR, "MP: Unknown entry %u", *type);
                return;
        }
    }

    return;
}

void mp_parse_tables(void)
{
    struct mp_table *table;

    if ((table = mp_find_magic((void *)EBDA_ADDR, 1024)))
        goto mp_found;

    if ((table = mp_find_magic((void *)BASE_MEM_ADDR, 1024)))
        goto mp_found;

    if ((table = mp_find_magic((void *)BIOS_ROM_START, BIOS_ROM_LEN)))
        goto mp_found;

    return;

mp_found:
    console_message(T_INF, "MP tables found at 0x%x", table);

    if (table->length != 1 || !table->conf_table_ptr)
        return;

    /* TODO: Checksum */

    mp_parse_config(table);
}
