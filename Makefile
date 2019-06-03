COREPATH  = core
CLIENTPATH = client
SERVERPATH = server

CORETAGS = $(shell find $(COREPATH) -type f -iname 'tags')
CLIENTTAGS = $(shell find $(CLIENTPATH) -type f -iname 'tags')
SERVERTAGS = $(shell find $(SERVERPATH) -type f -iname 'tags')

all: cleantags makeall

emcc:
	+$(MAKE) -C $(CLIENTPATH) -f Makefile.EMCC

win:
	+$(MAKE) -C $(COREPATH) -f Makefile.WIN32
	+$(MAKE) -C $(CLIENTPATH) -f Makefile.WIN32

nix:
	+$(MAKE) -C $(COREPATH) -f Makefile
	+$(MAKE) -C $(CLIENTPATH) -f Makefile
	+$(MAKE) -C $(SERVERPATH) -f Makefile

client:
	+$(MAKE) -C $(COREPATH) -f Makefile
	+$(MAKE) -C $(CLIENTPATH) -f Makefile

server:
	+$(MAKE) -C $(SERVERPATH) -f Makefile

makeall: clean cleantags nix server clean win

clean:
	+$(MAKE) clean -C $(COREPATH) -f Makefile
	+$(MAKE) clean -C $(CLIENTPATH) -f Makefile
	+$(MAKE) clean -C $(SERVERPATH) -f Makefile

cleantags:
	rm -rf $(CORETAGS)
	rm -rf $(CLIENTTAGS)
	rm -rf $(SERVERTAGS)


