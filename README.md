# Virtual Cache Simulator
mem_sim.c file acts as a cache simulator, taking in the address from the trace text file and coverting them into the tag,
index and offset bits. It then will store the cache line according to the method chosen (FIFO, LRU, Random). Each of these methods
are corrsponding to the way a full cache block would be replaced.

The mem_trace.txt(large text file! (10000 lines+)) file contains all of the addresses fed into the virtual cache along with the statistics calculated by the program.
