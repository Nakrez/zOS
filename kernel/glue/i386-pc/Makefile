CURDIR := kernel/glue/i386-pc/

OBJ := console.o segment.o as.o

DEPS := $(DEPS) $(addprefix $(CURDIR), $(OBJ:.o=.d))

$(BIN)_OBJ += $(addprefix $(DESTDIR), $(addprefix $(CURDIR), $(OBJ)))

