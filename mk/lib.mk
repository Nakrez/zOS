_LIB := $(addprefix $(CURDIR), $(addsuffix .a, $(LIB)))

$(LIB)_OBJ :=

include ${SRCDIR}mk/libsubdirs.mk

$(LIB)_PATH := $(_LIB)
$(_LIB): CFLAGS := $(DEFAULT_CFLAGS) $($(LIB)_CFLAGS)
$(_LIB): ASFLAGS := $(DEFAULT_ASFLAGS) $($(LIB)_ASFLAGS)
$(_LIB): $($(LIB)_OBJ)

$(_LIB)_OBJ := $($(LIB)_OBJ)

$(_LIB):
	$(call run,AR,$(AR) rcs $@ $($@_OBJ))

ALL_LIB := $(ALL_LIB) $(_LIB)
