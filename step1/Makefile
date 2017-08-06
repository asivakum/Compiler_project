LEX=flex
LFLAGS=--outfile=generated/Micro.flex.cc
LDFLAGS=-lfl
MKDIR_P = mkdir -p
LEX_OUT_DIR=generated
OUT_DIR=build
CC=g++
.PHONY: directories
all: group compiler 

group:
	@echo "Yiyang Chang Ashiwan Sivakumar"
compiler: directories Micro 
	$(CC) -o build/Micro generated/Micro.flex.cc $(LDFLAGS)
Micro:
	$(LEX) $(LFLAGS) src/Micro.ll

directories: ${LEX_OUT_DIR} ${OUT_DIR}

${LEX_OUT_DIR}:
	${MKDIR_P} ${LEX_OUT_DIR}

${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}
clean:
	rm -rf generated; rm -rf build
