 
# Get list of directories under the current, that contain makefile
BUILDPROJS := $(subst /,,$(dir $(foreach fldr,$(shell ls),$(wildcard $(fldr)/makefile))))

.PHONY:	    $(BUILDPROJS)
default:    $(BUILDPROJS)

all:		default
clean:		default
clean_all:	default
release:	default
debug:		default
ru:		default
du:		default
$(BUILDPROJS):
	+@make -C $@ $(MAKECMDGOALS)

