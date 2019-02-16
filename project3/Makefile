all: pan driver

pan: pan.c panList.c panList.h
	gcc pan.c panList.c -o pan -pthread -Wall -lm

driver: driver.c carList.c carList.h
	gcc driver.c carList.c -o driver -pthread -Wall -lm

clean:
	rm -f pan driver