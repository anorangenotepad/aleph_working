module_name = dacs_env
prefix = 

audio = ../../dsp
bfin = ../../bfin_lib/src

include version.mk
version = $(maj).$(min).$(rev)
ldr_name = $(prefix)$(module_name)-$(version).ldr

module_obj = dacs_env.o \
	$(audio)/filter_1p.o \

all: $(prefix)$(module_name).ldr

include ../../bfin_lib/bfin_lib.mk

CFLAGS += -D ARCH_BFIN=1
CFLAGS += -Wall
#CFLAGS += --verbose

desc_src = \
	$(bfin_lib_srcdir)desc.c \
	$(bfin_lib_srcdir)pickle.c \
	params.c

desc:
	gcc $(desc_src) \
	$(INC) \
	-D NAME=\"$(module_name)\" \
	-o $(module_name)_desc_build

$(module_obj): %.o : %.c	
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(prefix)$(module_name): bfin_lib_target $(module_obj)
	$(CC) $(LDFLAGS) -T $(prefix)$(module_name).lds \
	$(patsubst %.o, $(bfin_lib_objdir)%.o, $(bfin_lib_obj)) \
	$(module_obj) \
	-o $(prefix)$(module_name) \
	-lm -lbfdsp -lbffastfp

clean: bfin_lib_clean
	rm -f $(module_obj)
	rm -f $(prefix)$(module_name).ldr 
	rm -f $(prefix)$(module_name)
	rm -f $(prefix)$(module_name).dsc
	rm -f $(module_name)_desc_build

deploy: $(prefix)$(module_name).ldr
	make desc
	./$(module_name)_desc_build
	cp $(prefix)$(module_name).ldr $(prefix)$(module_name)-$(maj).$(min).$(rev).ldr

.PHONY: clean
	deploy

sim_sourcefiles = dacs.c \
	$(audio)/filter_1p.c \
	params.c \
	../../utils/bfin_sim/main.c \
	../../utils/bfin_sim/src/cv.c \
	../../utils/bfin_sim/fract_math.c

sim_inc = -I ../../dsp \
	-I ../../utils/bfin_sim/src \
	-I ../../utils/bfin_sim/ \
	../../utils/bfin_sim/fract2float_conv.c \
	-I ../../utils/bfin_sim/src/libfixmath \
	-I ./ \
	-I ../../common\

sim_outfile = dacs_jack

sim_flags = -ljack -llo -D ARCH_LINUX=1

sim:
	touch $(sim_outfile)
	rm ./$(sim_outfile)
	gcc $(sim_sourcefiles) $(sim_flags) $(sim_inc) -o $(sim_outfile) -g
