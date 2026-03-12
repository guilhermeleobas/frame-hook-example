PYTHON   := python3
PYINC    := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('INCLUDEPY'))")
PYLIB    := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))")
PYVER    := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('LDVERSION'))")
EXT_SUFFIX := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))")

TARGET   := hook$(EXT_SUFFIX)

CC       := gcc
CFLAGS   := -O2 -Wall -fPIC -I$(PYINC)
LDFLAGS  := -shared -L$(PYLIB) -lpython$(PYVER)

$(TARGET): hook.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

run: $(TARGET)
	$(PYTHON) demo.py

clean:
	rm -f $(TARGET)

.PHONY: run clean
