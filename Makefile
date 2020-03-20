default:
	@#g++ -o src/memCap src/memCap.c
	g++ -fopenmp -o src/memBw src/memBw.c
	@#g++ -fopenmp -o src/cpu src/cpu.cpp  # -lpthread -lgomp
	@#g++ -fopenmp -o src/cpu-all-cores src/cpu-all-cores.cpp  # -lpthread -lgomp
	g++ -fopenmp -o src/cpu-each-thread-each-core src/cpu-each-thread-each-core.cpp
	@#g++ -o src/l1i src/l1i.c -lrt
	@#g++ -o src/l1d src/l1d.c -lrt
	@#g++ -o src/l2 src/l2.c -lrt
	g++ -o src/l3 src/l3.c -lrt

clean:
	rm -f src/memCap
	rm -f src/memBw
	rm -f src/cpu
	rm -f src/l1i
	rm -f src/l1d
	rm -f src/l3
	rm -f src/l2
	rm -f src/cpu-all-cores
	rm -f src/cpu-each-thread-each-core
