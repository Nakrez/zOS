_BIN := $(addprefix $(CURDIR), $(BIN))

$(BIN)_OBJ :=

include ${SRCDIR}mk/binsubdirs.mk

$(_BIN)_LDFLAGS := $(DEFAULT_LDFLAGS) $($(BIN)_LDFLAGS)

$(_BIN): CFLAGS := $(DEFAULT_CFLAGS) $($(BIN)_CFLAGS)
$(_BIN): ASFLAGS := $(DEFAULT_ASFLAGS) $($(BIN)_ASFLAGS)
$(_BIN): $($(BIN)_OBJ)

$(_BIN)_OBJ := $($(BIN)_OBJ)

$(_BIN):
	$(call run,LD,$(LD) $($@_LDFLAGS) $($@_OBJ) -o $@)

INSTALL_BIN += rootfs/${INSTALL_DIR}/${BIN}

rootfs/${INSTALL_DIR}/${BIN} : ${_BIN}
	$(call run,CP,cp $< $@)

ALL_BIN := $(ALL_BIN) $(_BIN)
