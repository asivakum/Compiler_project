LEX=flex
LFLAGS=--outfile=generated/Micro.flex.cc
BISON=bison -d
BFLAGS= -o
LDFLAGS=-lfl
MKDIR_P = mkdir -p
LEX_OUT_DIR=generated
OUT_DIR=build
CC=g++ -std=c++0x

.PHONY: directories
all: group compiler 

group:
	@echo "Yiyang Chang Ashiwan Sivakumar"
compiler: directories Micro 
	$(CC) -g -o build/Micro generated/Micro.flex.cc generated/Micro.tab.c $(LDFLAGS)
Micro:
	$(BISON) src/Micro.y $(BFLAGS) generated/Micro.tab.c
	$(LEX) $(LFLAGS) src/Micro.ll

directories: ${LEX_OUT_DIR} ${OUT_DIR}

${LEX_OUT_DIR}:
	${MKDIR_P} ${LEX_OUT_DIR}

${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}
clean:
	rm -rf generated; rm -rf build
