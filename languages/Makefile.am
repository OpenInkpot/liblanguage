LANGFILES=$(wildcard *.UTF-8)
langdir=$(datadir)/i18n/languages

all: $(LANGFILES)

install-data-hook:
	echo "Installing languages"
	for l in $(LANGFILES); do \
	  $(mkdir_p) $(DESTDIR)$(langdir); \
	  $(INSTALL_DATA) `basename $$l` $(DESTDIR)$(langdir); \
	done
