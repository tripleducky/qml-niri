.PHONY: all build clean test

all: build

build:
	mkdir -p build
	cd build && cmake ..
	cd build && $(MAKE)

clean:
	rm -rf build

test:
	QML_IMPORT_PATH=$$PWD/build /usr/lib/qt6/bin/qml test/test_events.qml
