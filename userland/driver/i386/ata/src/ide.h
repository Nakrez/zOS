#ifndef IDE_H
# define IDE_H

# include <stdint.h>

# define PCI_CMD 0xCF8
# define PCI_DATA 0xCFC

# define MASS_STORAGE_CLASS 0x1
# define IDE_SUBCLASS 0x1

# define IDE_ATA 0x1
# define IDE_ATAPI 0x2

# define IDE_NIEN 0x2

# define ATA_PRIMARY 0x0
# define ATA_SECONDARY 0x1

# define ATA_TIMEOUT 500

/* ATA SR bit mask */
# define ATA_SR_ERR 0x01
# define ATA_SR_DRQ 0x08
# define ATA_SR_BSY 0x80

/* ATA Commands */
# define ATA_CMD_IDENTIFY_PACKET 0xA1
# define ATA_CMD_IDENTIFY 0xEC

/* ATA Register */
# define ATA_REG_DATA 0x0
# define ATA_REG_LBA1 0x04
# define ATA_REG_LBA2 0x05
# define ATA_REG_HDDSEL 0x6
# define ATA_REG_CMD 0x7
# define ATA_REG_STATUS 0x7
# define ATA_REG_CONTROL 0xC

struct ide_channel {
    uint16_t io_base;
    uint16_t io_ctrl;
};

struct identify_device {
    struct {
        uint16_t : 7;
        uint16_t is_removable : 1;
        uint16_t : 7;
        uint16_t is_atapi : 1;
    } __attribute__ ((packed)) configuration;

    uint16_t cyliders;
    uint16_t : 16;
    uint16_t heads;

    /* Retired */
    uint16_t : 16;
    uint16_t : 16;

    uint16_t sectors_per_track;
    uint16_t vendor_unique1[3];
    uint8_t serial[20];

    /* Retired */
    uint16_t : 16;
    uint16_t : 16;

    /* Obsolete */
    uint16_t : 16;

    uint8_t firmware_revision[8];
    uint8_t model_number[40];
    uint8_t max_block_transfer;

    uint8_t vendor_unique2;

    /* Reserved */
    uint16_t : 16;

    struct {
        /* Reserved */
        uint8_t : 8;

        uint8_t dma : 1;
        uint8_t lba : 1;
        uint8_t iordy_disable : 1;
        uint8_t iordy_supported : 1;

        /* Reserved */
        uint8_t : 1;

        uint8_t standby_timer_support : 1;

        /* Reserved */
        uint8_t : 2;
        uint16_t : 16;
    } __attribute__ ((packed));

    /* Obsolete */
    uint16_t : 16;
    uint16_t : 16;

    uint16_t translation_fields_valid : 3;

    /* Reserved */
    uint16_t : 13;

    uint16_t num_current_cylinders;
    uint16_t num_current_heads;
    uint16_t current_sectors_per_track;
    uint32_t current_sector_capacity;

    uint8_t current_multisectors_setting;
    uint8_t multisectors_setting_valid : 1;

    /* Reserved */
    uint8_t : 7;

    uint32_t user_addressable_sectors;

    /* Obsolete */
    uint16_t : 16;

    uint16_t multiword_dma_support : 8;
    uint16_t multiword_dma_active : 8;
    uint16_t advanced_pio_modes : 8;

    /* Reserved */
    uint16_t : 8;

    uint16_t min_mwdmat_transfert_time_per_word;
    uint16_t rec_mwdmat_transfert_time;
    uint16_t min_pio_transfert_time;
    uint16_t min_pio_transfert_time_iordy;

    /* Reserved */
    uint16_t : 16;
    uint16_t : 16;
    uint16_t : 16;
    uint16_t : 16;
    uint16_t : 16;
    uint16_t : 16;

    uint16_t queue_depth : 5;

    /* Reserved */
    uint16_t : 11;
    uint16_t : 16;
    uint16_t : 16;
    uint16_t : 16;
    uint16_t : 16;

    uint16_t major_revision;
    uint16_t minor_revision;

    struct {
        uint16_t smart_commands : 1;
        uint16_t security_mode : 1;
        uint16_t removable : 1;
        uint16_t power_management : 1;

        /* Reserved */
        uint16_t : 1;

        uint16_t write_cache : 1;
        uint16_t look_ahead : 1;
        uint16_t release_interrupt : 1;
        uint16_t service_interrupt : 1;
        uint16_t device_reset: 1;
        uint16_t host_protected_area: 1;

        /* Obsolete */
        uint16_t : 1;

        uint16_t write_buffer : 1;
        uint16_t read_buffer : 1;
        uint16_t nop : 1;

        /* Obsolete */
        uint16_t : 1;

        uint16_t download_microcode : 1;
        uint16_t dma_queued : 1;
        uint16_t cfa : 1;
        uint16_t advanced_pm : 1;
        uint16_t msn : 1;
        uint16_t powerup_in_standby : 1;
        uint16_t manual_powerup : 1;

        /* Reserved */
        uint16_t : 1;

        uint16_t set_max : 1;
        uint16_t acoustics: 1;
        uint16_t big_lba : 1;
        uint16_t device_config_overlay : 1;
        uint16_t flush_cache : 1;
        uint16_t flush_cache_ext : 1;

        /* Reserved */
        uint16_t : 2;

        uint16_t smart_error_log : 1;
        uint16_t smart_self_test : 1;
        uint16_t media_serial_number : 1;
        uint16_t media_card_pass_through : 1;
        uint16_t streaming_feature : 1;
        uint16_t gplogging : 1;
        uint16_t write_queued_fua : 1;
        uint16_t wn64bit : 1;
        uint16_t urg_read_stream : 1;
        uint16_t urg_write_stream : 1;

        /* Reserved */
        uint16_t : 2;

        uint16_t idle_with_uload_feature : 1;

        /* Reserved */
        uint16_t : 2;
    } __attribute__ ((packed)) features;

    uint16_t reserved[171];

} __attribute__ ((packed));

struct ide_device {
    struct ide_channel* chan;
    uint8_t driver;
    uint8_t id;
    struct identify_device infos;

    int exists;
};

struct ide_controller {
    int device_id;
    int vendor_id;

    struct ide_channel channels[2];
    struct ide_device devices[4];
};

/*
 * Detect the IDE controller on the PCI bus
 *
 * Return:
 *          0: The controller has been found
 *          -1: No controller found
 */
int ide_detect(struct ide_controller* ctrl);
int ide_initialize(struct ide_controller *ctrl);

#endif /* !IDE_H */
