$(LIB)_OBJ += $(addprefix $(DESTDIR), $(addprefix $(CURDIR), $(OBJ)))

DEPS := $(DEPS) $(addprefix $(CURDIR), $(OBJ:.o=.d))

SUBDIRS := $(LIBSUBDIRS)

include $(SRCDIR)mk/subdirs.mk
