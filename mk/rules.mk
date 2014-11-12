$(DESTDIR)/%.o: $(SRCDIR)/%.c
	$(call run,CC,$(CC) $(CFLAGS) -c $< -o $@)

$(DESTDIR)/%.o: $(SRCDIR)/%.S
	$(call run,AS,$(AS) $(ASFLAGS) -c $< -o $@)
