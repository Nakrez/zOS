$(DESTDIR)%.o: $(SRCDIR)%.c
	$(call run,CC,$(CC) $(CFLAGS) -MD -MP -c $< -o $@)

$(DESTDIR)%.o: $(SRCDIR)%.S
	$(call run,AS,$(AS) $(ASFLAGS) -MD -MP -c $< -o $@)
