
# change this to your module name
module_name = mix_io

# paths to aleph repository sources
audio = ../../dsp
bfin = ../../bfin_lib/src

include version.mk
version = $(maj).$(min).$(rev)
ldr_name = $(module_name)-$(version).ldr

# add sources from here/audio library.
module_obj = mix_io.o \
	$(audio)/filter_1p.o \


# -----  below here, probably dont need to customize.

all: $(module_name).ldr

# this gets the core configuration and sources
include ../../bfin_lib/bfin_lib.mk

CFLAGS += -D ARCH_BFIN=1
# diagnose gcc errors
# CFLAGS += --verbose

desc_src = \
	$(bfin_lib_srcdir)desc.c \
	$(bfin_lib_srcdir)pickle.c \
	params.c

# this target generates the descriptor helper program
desc:
	gcc $(desc_src) \
	$(INC) \
	-D NAME=\"$(module_name)\" \
	-o $(module_name)_desc_build

$(module_obj): %.o : %.c	
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(module_name): bfin_lib_target $(module_obj)
	$(CC) $(LDFLAGS) -T $(module_name).lds \
	$(patsubst %.o, $(bfin_lib_objdir)%.o, $(bfin_lib_obj)) \
	$(module_obj) \
	-o $(module_name) \
	-lm -lbfdsp -lbffastfp

clean: bfin_lib_clean
	rm $(module_obj)
	rm $(module_name).ldr 
	rm $(module_name)

# this generates the module and descriptor helper app,
# runs the descrptor app to generate .dsc,
# and makes copies with version number strings.
# best used after "make clean"
deploy: $(module_name).ldr
	make desc
	./$(module_name)_desc_build
	cp $(module_name).ldr $(module_name)-$(maj).$(min).$(rev).ldr
	cp $(module_name).dsc $(module_name)-$(maj).$(min).$(rev).dsc

.PHONY: clean
	deploy


sim_sourcefiles = mix.c \
	$(audio)/filter_1p.c \
	params.c \
	../../utils/bfin_sim/main.c \
	../../utils/bfin_sim/src/cv.c \
	../../utils/bfin_sim/fract2float_conv.c \
	../../utils/bfin_sim/fract_math.c

sim_inc = -I ../../dsp \
	-I ../../utils/bfin_sim/src \
	-I ../../utils/bfin_sim/ \
	-I ../../utils/bfin_sim/src/libfixmath \
	-I ./ \
	-I ../../common\

sim_outfile = mix_jack

sim_flags = -ljack -llo -D ARCH_LINUX=1

sim:
	touch $(sim_outfile)
	rm ./$(sim_outfile)
	gcc $(sim_sourcefiles) $(sim_flags) $(sim_inc) -o $(sim_outfile) -g
