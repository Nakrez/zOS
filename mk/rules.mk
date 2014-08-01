%.o: %.c
	$(call run, CC, $(CC) $(CFLAGS) -c $< -o $@)

%.o: %.S
	$(call run, AS, $(AS) $(ASFLAGS) -c $< -o $@)
