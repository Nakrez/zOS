SUBDIRS := $(addprefix $(SRCDIR)/$(CURDIR)/, $(SUBDIRS))
$(foreach DIR, $(SUBDIRS), $(shell mkdir -p $(DIR)))

include $(addsuffix /Makefile, $(SUBDIRS))
