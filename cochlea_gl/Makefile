# Makefile for cochlea_gl using meson+ninja backend

BUILD_DIR = build
MESON = meson
NINJA = ninja

.PHONY: all build clean rebuild install configure help

all: build

build: configure
	$(NINJA) -C $(BUILD_DIR)

configure:
	$(MESON) setup $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean build

install: build
	$(NINJA) -C $(BUILD_DIR) install

help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  build    - Build the project"
	@echo "  clean    - Remove build directory"
	@echo "  rebuild  - Clean and rebuild"
	@echo "  install  - Install the built executable"
	@echo "  help     - Show this help message"
