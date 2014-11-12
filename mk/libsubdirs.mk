$(LIB-y)_OBJ += $(addprefix $(DESTDIR)/, $(addprefix $(CURDIR)/, $(OBJ-y)))

DEPS := $(DEPS) $(addprefix $(CURDIR)/, $(OBJ-y:.o=.d))

SUBDIRS := $(LIBSUBDIRS-y)

include $(SRCDIR)/mk/subdirs.mk
