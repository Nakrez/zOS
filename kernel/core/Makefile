CURDIR := kernel/core/

OBJ := main.o string.o console.o kmalloc.o

DEPS := $(DEPS) $(addprefix $(CURDIR), $(OBJ:.o=.d))

$(BIN)_OBJ += $(addprefix $(DESTDIR), $(addprefix $(CURDIR), $(OBJ)))
