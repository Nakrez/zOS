#ifndef I386_MP_H
# define I386_MP_H

# include <kernel/types.h>

# define BDA_ADDR_START (0xC0000000 + 0x400)
# define BDA_EBDA_ADDR (BDA_ADDR_START + 0xE)
# define EBDA_ADDR ((uint32_t)((*(uint16_t *)BDA_ADDR_START) << 4) + 0xC0000000)

# define BASE_MEM_ADDR ((*(uint16_t *)0xC0000413 * 1024) + 0xC0000000)

# define BIOS_ROM_START 0xC00F0000
# define BIOS_ROM_LEN (0xFFFFF - 0xF0000)

# define MP_PROCESSOR_ENTRY 0
# define MP_BUS_ENTRY 1
# define MP_IOAPIC_ENTRY 2
# define MP_IOINT_ENTRY 3
# define MP_LOCINT_ENTRY 4

struct mp_locint {
    uint8_t type;

    uint8_t int_type;

    uint16_t flags;

    uint8_t bus_id;

    uint8_t bus_irq;

    uint8_t lapic_id;

    uint8_t lapic_pin;
} __attribute__ ((packed));

struct mp_ioint {
    uint8_t type;

    uint8_t int_type;

    uint16_t flags;

    uint8_t bus_id;

    uint8_t bus_irq;

    uint8_t ioapic_id;

    uint8_t ioapic_pin;
} __attribute__ ((packed));

struct mp_ioapic {
    uint8_t type;

    uint8_t id;

    uint8_t version;

    uint8_t flags;

    uint32_t addr;
} __attribute__ ((packed));

struct mp_bus {
    uint8_t type;

    uint8_t id;

    char bus_type[6];
} __attribute__ ((packed));

struct mp_proc {
    uint8_t type;

    uint8_t lapic_id;

    uint8_t lapic_version;

    uint8_t flags;

    uint32_t signature;

    uint32_t feature;

    uint64_t reserved;
} __attribute__ ((packed));

struct mp_config {
    char signature[4];

    uint16_t length;

    uint8_t spec_rec;

    uint8_t checksum;

    char oem_id[8];

    char product_id[12];

    uint32_t oem_table_ptr;

    uint16_t oem_table_size;

    uint16_t entry_count;

    uint32_t local_apic_addr;

    uint16_t ext_table_length;

    uint16_t ext_table_checksum;
} __attribute__ ((packed));

struct mp_table {
    char signature[4];

    uint32_t conf_table_ptr;

    uint8_t length;

    uint8_t spec_rev;

    uint8_t checksum;

    uint8_t features[5];
} __attribute__ ((packed));

/**
 *  \brief  A pointer to the MP tables
 */
extern struct mp_table *mp_table;

/**
 *  \brief  Parse the MP tables
 *
 *  \return A non null integer if they exist and were parsed successfully, 0
 *          otherwise
 */
int mp_parse_tables(void);

#endif /* !I386_MP_H */
