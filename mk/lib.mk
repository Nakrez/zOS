_LIB := $(addprefix $(CURDIR)/, $(addsuffix .a, $(LIB-y)))

$(LIB-y)_OBJ :=

include $(SRCDIR)/mk/libsubdirs.mk

$(LIB-y)_PATH := $(_LIB)
$(_LIB): CFLAGS := $(DEFAULT_CFLAGS) $($(LIB-y)_CFLAGS)
$(_LIB): ASFLAGS := $(DEFAULT_ASFLAGS) $($(LIB-y)_ASFLAGS)
$(_LIB): $($(LIB-y)_OBJ)

$(_LIB)_OBJ := $($(LIB-y)_OBJ)

$(_LIB):
	$(call run,AR,$(AR) rcs $@ $($@_OBJ))

ALL_LIB := $(ALL_LIB) $(_LIB)
