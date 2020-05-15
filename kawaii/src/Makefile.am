
DIR = @MUSICDIR@
LIB = @MUSICLIB@
SRC = main.c introsystem.c start.c title.c metaballs.c keftales.c cellular.c config.h introsystem.h bitmap.h

SUBDIRS = data	$(DIR)
DIST_SUBDIRS = data mikmod minifmod


noinst_PROGRAMS = kawaii
kawaii_SOURCES = $(SRC)
kawaii_LDADD = data/libdata.a $(LIB)
kawaii_DEPENDENCIES = data/libdata.a $(LIB)

if USE_UPX
bin_SCRIPTS = kawaii_upx
kawaii_upx: kawaii
	-@$(UPX) --best kawaii$(EXEEXT)
	@du -h kawaii
endif
