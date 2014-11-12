$(BIN-y)_OBJ += $(addprefix $(DESTDIR)/, $(addprefix $(CURDIR)/, $(OBJ-y)))

DEPS := $(DEPS) $(addprefix $(CURDIR)/, $(OBJ-y:.o=.d))

SUBDIRS := $(BINSUBDIRS-y)

include $(SRCDIR)/mk/subdirs.mk
