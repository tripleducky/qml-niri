all: build

build:
  mkdir -p build
  cd build && cmake ..
  cd build && make

clean:
  rm -rf build

test component:
  QML_IMPORT_PATH=$PWD/build qml6 test/test_{{component}}.qml
