#ifndef I386_MP_H
# define I386_MP_H

# include <kernel/types.h>

# define BDA_ADDR_START (0xC0000000 + 0x400)
# define BDA_EBDA_ADDR (BDA_ADDR_START + 0xE)
# define EBDA_ADDR ((uint32_t)((*(uint16_t *)BDA_ADDR_START) << 4) + 0xC0000000)

# define BASE_MEM_ADDR ((*(uint16_t *)0xC0000413 * 1024) + 0xC0000000)

# define BIOS_ROM_START 0xC00F0000
# define BIOS_ROM_LEN (0xFFFFF - 0xF0000)

void mp_parse_tables(void);

#endif /* !I386_MP_H */
