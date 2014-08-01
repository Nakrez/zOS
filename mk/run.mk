define run
	@printf "\t[%s] %s\n" "$1" "$@"
	@$2
endef
