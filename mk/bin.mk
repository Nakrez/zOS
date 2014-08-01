_BIN := $(addprefix $(CURDIR), $(BIN))

$(BIN)_OBJ :=

include ${SRCDIR}/mk/binsubdirs.mk

CFLAGS := $(DEFAULT_CFLAGS) $($(BIN)_CFLAGS)
ASFLAGS := $(DEFAULT_ASFLAGS) $($(BIN)_ASFLAGS)
LDFLAGS := $(DEFAULT_LDFLAGS) $($(BIN)_LDFLAGS)

$(_BIN): $($(BIN)_OBJ)
	$(call run,LD,$(LD) $(LDFLAGS) $^ -o $@)

INSTALL_BIN := ${INSTALL_BIN} rootfs/${INSTALL_DIR}/${BIN}

rootfs/${INSTALL_DIR}/${BIN} : ${_BIN}
	$(call run,CP,cp $< $@)

ALL_BIN := $(ALL_BIN) $(_BIN)
