all:
	gcc ./lib/demo.c ./lib/simulation.c ./lib/rngs.c -lm -o demo

debug:
	gcc -g ./lib/demo.c ./lib/simulation.c ./lib/rngs.c -lm -o demo

clean:
	rm demo
