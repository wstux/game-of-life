export NPROC	?= $(shell (nproc))
export NJOB	?= $(shell expr '(' $(NPROC) + 1 ')')

define common_rule

build_$(1)/Makefile:
	@mkdir -p $$(@D) && cd $$(@D) && cmake -DCMAKE_BUILD_TYPE="$(1)" ../

$(1)/%: build_$(1)/Makefile
	@make -j $(NJOB) --output-sync=target --no-print-directory -C build_$(1) $$*

$(1)/clean:
	@rm -rf build_$(1)

endef

$(eval $(call common_rule,debug))
$(eval $(call common_rule,release))

