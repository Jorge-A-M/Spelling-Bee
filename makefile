build:
	rm -f spellingBee.exe
	gcc main.c -o spellingBee.exe

run:
	./spellingBee.exe

run_simp:
	./spellingBee.exe -d newsamp.txt

valgrind:
	rm -f spellB_debug.exe
	gcc -g main.c -o spellB_debug.exe
	echo "watched w" > sampleIn.txt
	valgrind -s --tool=memcheck --leak-check=yes --track-origins=yes ./spellB_debug.exe < sampleIn.txt

clean:
	rm -f spellingBee.exe
	rm -f spellB_debug.exe

run_play:
	./spellingBee.exe -r 7 -p

run_sample:
	echo "acdeit e" > sampleIn.txt
	./spellingBee.exe -d newsamp.txt < sampleIn.txt

valgrind_play:
	rm -f spellB_debug.exe
	gcc -g main.c -o spellB_debug.exe
	valgrind -s --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes ./spellB_debug.exe -p

gprof_brute:
	gcc -pg main.c -o spBee_gprof.exe
	echo "ecmoprt e" > gprof_hive.txt
	./spBee_gprof.exe < gprof_hive.txt
	gprof ./spBee_gprof.exe > gprof_bruteforce.txt

gprof_opt:
	gcc -pg main.c -o spBee_gprof.exe
	echo "ecmoprt e" > gprof_hive.txt
	./spBee_gprof.exe -o < gprof_hive.txt
	gprof ./spBee_gprof.exe > gprof_optimized.txt
