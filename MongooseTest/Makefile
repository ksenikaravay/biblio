All: prepare MongooseTest

MongooseTest: bin/main.o bin/mongoose.o
	g++ bin/main.o bin/mongoose.o -o MongooseTest

bin/mongoose.o: mongoose.c mongoose.h
	g++ -c mongoose.c -o bin/mongoose.o

bin/main.o: main.cpp mongoose.h
	g++ -std=c++11 -c main.cpp -o bin/main.o

prepare:
	mkdir -p bin

clean:
	rm -rf bin MongooseTest
