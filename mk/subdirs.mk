SUBDIRS := $(addprefix $(CURDIR), $(SUBDIRS))
$(foreach DIR, $(SUBDIRS), $(shell mkdir -p $(DIR)))

include $(addsuffix Makefile,$(addprefix $(SRCDIR), $(SUBDIRS)))
