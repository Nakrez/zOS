_BIN := $(addprefix $(CURDIR)/, $(BIN-y))

$(BIN-y)_OBJ :=

include $(SRCDIR)/mk/binsubdirs.mk

$(_BIN)_LIBS := $(foreach L, $($(BIN-y)_LIBS), $($(L)_PATH))
$(_BIN)_LDFLAGS := $(DEFAULT_LDFLAGS) $($(BIN-y)_LDFLAGS)

$(_BIN): CFLAGS := $(DEFAULT_CFLAGS) $($(BIN-y)_CFLAGS)
$(_BIN): ASFLAGS := $(DEFAULT_ASFLAGS) $($(BIN-y)_ASFLAGS)
$(_BIN): $($(_BIN)_LIBS) $($(BIN-y)_OBJ)

$(_BIN)_OBJ := $($(BIN-y)_OBJ)

$(_BIN):
	$(call run,LD,$(LD) $($@_LDFLAGS) $($@_OBJ) $($@_LIBS) -o $@)

INSTALL_BIN += rootfs/$(INSTALL_DIR)/$(BIN-y)

rootfs/$(INSTALL_DIR)/$(BIN-y) : $(_BIN)
	$(call run,CP,cp $< $@)

ALL_BIN := $(ALL_BIN) $(_BIN)
