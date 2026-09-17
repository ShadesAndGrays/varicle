.PHONY: all build clean lib

CXX = g++
WSL = wsl --shell-type login
EMXX = em++
EMAR = emar

CXXFLAGS = -std=c++20 -O2

SRC_FILES = ./src/main.cpp \
			./src/lib.cpp


LIB_FILES = ./src/lib.cpp

all: build-lib build 

run: build
	./bin/assetpacker.exe

build-lib:
	@mkdir -p bin
	@mkdir -p bin/include/asset_packer
	@cp -r ./src/include/* ./bin/include/asset_packer
	$(CXX) $(CXXFLAGS) -I./src/include -c $(LIB_FILES) -o ./bin/asset_packer.o
	ar rcs ./bin/libassetpacker.a ./bin/asset_packer.o

build-lib-web:
	@mkdir -p bin
	@mkdir -p bin/include/asset_packer
	@cp -r ./src/include/* ./bin/include/asset_packer
	$(EMXX) $(CXXFLAGS) -I./src/include -c $(LIB_FILES) -o ./bin/asset_packer.web.o
	$(EMAR) rcs ./bin/libassetpacker.web.a ./bin/asset_packer.web.o

build:
	@mkdir -p bin
	@cp -r ./asset/ ./bin/
	$(CXX) $(CXXFLAGS) $(SRC_FILES) -I./src/include -o ./bin/assetpacker.exe

ccj: clean
	@echo "Generating compile_commands.json..."
	bear -- $(MAKE)

clean: 
	@rm -r -f ./bin
	@rm -f compile_commands.json

test:
	@mkdir -p bin 
	@cp -r ./asset/ ./bin/stuff/
	$(CXX) $(CXXFLAGS) -I./src/include $(LIB_FILES) ./src/test.cpp -o ./bin/test.exe
	./bin/test.exe
	

