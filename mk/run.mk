ifeq ("$(origin V)", "undefined")
	V := 0
endif

ifeq ($V, 0)

define run
	@printf "\t[%s] %s\n" "$1" "$@"
	@$2
endef

else

define run
	$2
endef

endif
