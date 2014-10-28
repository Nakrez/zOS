#include <unistd.h>
#include <sys/io.h>
#include <sys/mman.h>

#include "vbe.h"

uint16_t bga_read_register(uint16_t index)
{
    outw(VBE_DISPI_IOPORT_INDEX, index);
    return inw(VBE_DISPI_IOPORT_DATA);
}

void bga_write_register(uint16_t index, uint16_t data)
{
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, data);
}

void bga_bank_set(uint16_t bank)
{
    bga_write_register(VBE_DISPI_INDEX_BANK, bank);
}

int vbe_initialize(void)
{
    if (bga_read_register(VBE_DISPI_INDEX_ID) != VBE_DIPSI_ID0)
        return -1;

    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, 1024);
    bga_write_register(VBE_DISPI_INDEX_YRES, 768);
    bga_write_register(VBE_DISPI_INDEX_BPP, 8);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED);

    return 0;
}
