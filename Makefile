CC = g++
CFLAGS = -c -Wall -I. -I./src -D_DEBUG
RM = rm -rf

SRC = ./src
BIN = ./bin
TMP = ./tmp

COMPILE = $(CC) $(CFLAGS) $^ -o $(TMP)/$@

target: init demo

demo: demo.o thread.o  threadlead.o threadstuff.o 
	cd $(TMP); $(CC) $^ -lpthread -o ../$(BIN)/$@

init:
	[ -d '$(TMP)' ] || mkdir $(TMP)
	[ -d '$(BIN)' ] || mkdir $(BIN)

demo.o: demo.cpp
	$(COMPILE)  	
thread.o: $(SRC)/thread.cpp
	$(COMPILE)  	
threadlead.o: $(SRC)/threadlead.cpp 
	$(COMPILE)  	
threadstuff.o: $(SRC)/threadstuff.cpp
	$(COMPILE)  	

clean:
	$(RM) *.o $(BIN)/* $(TMP) $(BIN)
