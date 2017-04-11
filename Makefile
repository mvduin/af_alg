programs := ksha256-neon ksha256-omap

all :: ${programs}

${programs}: hasher.o

clean ::
	${RM} ${programs}


# where to look for sources
vpath %.cc src


# all packages
declared_pkgs := 

# default packages
pkgs =


include common.mk

CPPFLAGS += -iquote .
#LDLIBS += -lm -lstdc++ -lstdc++fs
