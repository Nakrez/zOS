_BIN := $(addprefix $(CURDIR), $(BIN))

$(_BIN): $($(BIN)_OBJ)
	$(call run, LD, $(LD) $(LDFLAGS) $^ -o $@

INSTALL_BIN := ${INSTALL_BIN} rootfs/${INSTALL_DIR}/${BIN}

rootfs/${INSTALL_DIR}/${BIN} : ${_BIN}
	$(call run, CP, cp $< $@)
