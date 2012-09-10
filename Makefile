all:
	gcc master.c worker.c reader.c writer.c disc.c -o sim -lpthread -W -Wall
clean:
	rm sim
