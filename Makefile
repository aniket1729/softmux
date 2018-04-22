TOPTARGETS := all clean

SUBDIRS := tsfilter

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

# -C flag indicates to change in directory (equivalent to running 'cd <specified dir>' before calling make). Same approach can be used for calling other targets in the code Makefile



.PHONY: $(TOPTARGETS) $(SUBDIRS)
# .PHONY rule means that dependencies are not a file that needs to be built

# References:
# https://stackoverflow.com/questions/17834582/run-make-in-each-subdirectory?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
