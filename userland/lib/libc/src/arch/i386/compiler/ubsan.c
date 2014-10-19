#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <zos/print.h>

struct ubsan_source_location {
    const char *name;
    uint32_t line;
    uint32_t column;
};

struct ubsan_type_descriptor {
    uint16_t type_kind;
    uint16_t type_info;
    char type_name[];
};

struct ubsan_type_mismatch_data {
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *type;
    uintptr_t alignment;
    unsigned char type_check_kind;
};

struct ubsan_overflow_data {
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *type;
};

struct usban_shift_out_of_bounds_data {
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *lhs_type;
    struct ubsan_type_descriptor *rhs_type;
};

struct usban_out_of_bounds_data {
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *array_type;
    struct ubsan_type_descriptor *index_type;
};

static void ubsan_report(struct ubsan_source_location *location,
                         const char *reason)
{
    char buf[1024];

    sprintf(buf, "Undefined behaviour: %s at %s:%i:%i", reason,
            location->name, location->line, location->column);
    uprint(buf);
    exit(1);
}

void __ubsan_handle_type_mismatch(struct ubsan_type_mismatch_data *data,
                                  uintptr_t ptr)
{
    (void) ptr;

    ubsan_report(&data->location, "type mismatch");
}

void __ubsan_handle_add_overflow(struct ubsan_overflow_data *data,
                                 uintptr_t lhs, uintptr_t rhs)
{
    (void) lhs;
    (void) rhs;

    ubsan_report(&data->location, "add overflow");
}

void __ubsan_handle_sub_overflow(struct ubsan_overflow_data *data,
                                 uintptr_t lhs, uintptr_t rhs)
{
    (void) lhs;
    (void) rhs;

    ubsan_report(&data->location, "sub overflow");
}

void __ubsan_handle_mul_overflow(struct ubsan_overflow_data *data,
                                 uintptr_t lhs, uintptr_t rhs)
{
    (void) lhs;
    (void) rhs;

    ubsan_report(&data->location, "mul overflow");
}

void __ubsan_handle_negate_overflow(struct ubsan_overflow_data *data,
                                    uintptr_t lhs, uintptr_t rhs)
{
    (void) lhs;
    (void) rhs;


    ubsan_report(&data->location, "negate overflow");
}

void __ubsan_handle_divrem_overflow(struct ubsan_overflow_data *data,
                                    uintptr_t lhs, uintptr_t rhs)
{
    (void) lhs;
    (void) rhs;


    ubsan_report(&data->location, "divrem overflow");
}

void __ubsan_handle_shift_out_of_bounds(
        struct usban_shift_out_of_bounds_data *data,
        uintptr_t lhs,
        uintptr_t rhs)
{
    (void) lhs;
    (void) rhs;


    ubsan_report(&data->location, "shift out of bound");
}

void __ubsan_handle_out_of_bounds(struct usban_out_of_bounds_data *data,
                                  uintptr_t ptr)
{
    (void) ptr;

    ubsan_report(&data->location, "out of bound");
}
