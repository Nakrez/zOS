$(BIN)_OBJ += $(addprefix $(DESTDIR), $(addprefix $(CURDIR), $(OBJ)))

DEPS := $(DEPS) $(addprefix $(CURDIR), $(OBJ:.o=.d))

SUBDIRS := $(BINSUBDIRS)

include $(SRCDIR)/mk/subdirs.mk
