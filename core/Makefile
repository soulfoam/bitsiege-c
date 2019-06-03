CC 			= gcc
EXTENSION 	= c

SRCS_DIR	= src
OBJS_DIR	= objs

CFLAGS		+= -std=gnu99 -Wall -Wextra
LFLAGS		+= -lm

SRCS		= $(shell find $(SRCS_DIR) -type f -name *.$(EXTENSION))
OBJS		= $(patsubst $(SRCS_DIR)/%,$(OBJS_DIR)/%,$(SRCS:.$(EXTENSION)=.o))
DEPS		= $(OBJS:.o=.d)

MAKE		= make --no-print-directory -C
ECHO		= echo

DIRS := . $(shell find $(OBJS_DIR) -type d)
GARBAGE_PATTERNS := *.o *.d
GARBAGE := $(foreach DIR,$(DIRS),$(addprefix $(DIR)/,$(GARBAGE_PATTERNS)))

all		: $(OBJS) tags

$(OBJS_DIR)/%.o	: $(SRCS_DIR)/%.$(EXTENSION)
		  @$(CC) $(CFLAGS) -MD -MF $(@:.o=.d) -c -o $@ $<

clean		:
		  @rm -rf $(GARBAGE)

-include $(DEPS)

.PHONY		: all clean

tags:
	ctags -R --exclude=res --exclude=build --exclude=.ycm_extra_conf.py --exclude=Makefile\
		--exclude=Makefile.WIN32

