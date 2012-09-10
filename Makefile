all:
	gcc master.c worker.c reader.c writer.c disc.c -o discsim
