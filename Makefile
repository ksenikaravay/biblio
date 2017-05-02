all: prepare biblio
biblio: myBin/main.o myBin/ArticleInfo.o myBin/BiblioManager.o myBin/BiblioServer.o myBin/BiblioThreadContext.o myBin/Config.o myBin/Database.o myBin/FB2Parser.o myBin/mongoose.o myBin/PictureParser.o myBin/RequesterManager.o myBin/tools.o myBin/Requesters/Requester.o myBin/Requesters/ArxivRequester.o myBin/Requesters/DBLPRequester.o myBin/Requesters/NatureRequester.o myBin/Requesters/ScienceDirectRequester.o myBin/Requesters/ScopusRequester.o myBin/Requesters/SpringerRequester.o myBin/jsoncpp.o
	g++ myBin/main.o myBin/ArticleInfo.o myBin/BiblioManager.o myBin/BiblioServer.o myBin/BiblioThreadContext.o myBin/Config.o myBin/Database.o myBin/FB2Parser.o myBin/mongoose.o myBin/PictureParser.o myBin/RequesterManager.o myBin/tools.o myBin/Requesters/Requester.o myBin/Requesters/ArxivRequester.o myBin/Requesters/DBLPRequester.o myBin/Requesters/NatureRequester.o myBin/Requesters/ScienceDirectRequester.o myBin/Requesters/ScopusRequester.o myBin/Requesters/SpringerRequester.o myBin/jsoncpp.o -o biblio -lexpat -lcurl -lpoppler-cpp -ltesseract -llept -lpthread -lconfig++ -lopencv_core -lopencv_highgui -lopencv_imgproc -ltinyxml2 -lsqlite3

myBin/main.o: src/main.cpp src/Requesters/Requester.h src/BiblioManager.h src/BiblioThreadContext.h
	g++ -std=c++11 -c src/main.cpp -o myBin/main.o

myBin/ArticleInfo.o: src/ArticleInfo.cpp src/ArticleInfo.h lib/json/json.h lib/json/value.h
	g++ -std=c++11 -c src/ArticleInfo.cpp -o myBin/ArticleInfo.o

myBin/BiblioManager.o: src/BiblioManager.cpp src/BiblioManager.h src/RequesterManager.h src/BiblioThreadContext.h src/FB2Parser.h
	g++ -std=c++11 -c src/BiblioManager.cpp -o myBin/BiblioManager.o

myBin/BiblioThreadContext.o: src/BiblioThreadContext.cpp src/BiblioThreadContext.h
	g++ -std=c++11 -c src/BiblioThreadContext.cpp -o myBin/BiblioThreadContext.o

myBin/Config.o: src/Config.cpp src/Config.h
	g++ -std=c++11 -c src/Config.cpp -o myBin/Config.o

myBin/Database.o: src/Database.cpp src/Database.h
	g++ -std=c++11 -c src/Database.cpp -o myBin/Database.o

myBin/PictureParser.o: src/PictureParser.cpp src/PictureParser.h
	g++ -std=c++11 -c src/PictureParser.cpp -o myBin/PictureParser.o

myBin/RequesterManager.o: src/RequesterManager.cpp src/RequesterManager.h src/Requesters/ArxivRequester.h src/Requesters/DBLPRequester.h src/Requesters/NatureRequester.h src/Requesters/ScienceDirectRequester.h src/Requesters/ScopusRequester.h src/Requesters/SpringerRequester.h
	g++ -std=c++11 -c src/RequesterManager.cpp -o myBin/RequesterManager.o

myBin/tools.o: src/tools.cpp src/tools.h src/Config.h lib/tinydir/tinydir.h
	g++ -std=c++11 -c src/tools.cpp -o myBin/tools.o

myBin/Requesters/Requester.o: src/Requesters/Requester.cpp src/Requesters/Requester.h
	g++ -std=c++11 -c src/Requesters/Requester.cpp -o src/Requesters/Requester.o -o myBin/Requesters/Requester.o

myBin/Requesters/ArxivRequester.o: src/Requesters/ArxivRequester.cpp src/Requesters/ArxivRequester.h
	g++ -std=c++11 -c src/Requesters/ArxivRequester.cpp -o myBin/Requesters/ArxivRequester.o
myBin/Requesters/DBLPRequester.o: src/Requesters/DBLPRequester.cpp src/Requesters/DBLPRequester.h
	g++ -std=c++11 -c src/Requesters/DBLPRequester.cpp -o myBin/Requesters/DBLPRequester.o
myBin/Requesters/NatureRequester.o: src/Requesters/NatureRequester.cpp src/Requesters/NatureRequester.h
	g++ -std=c++11 -c src/Requesters/NatureRequester.cpp -o myBin/Requesters/NatureRequester.o
myBin/Requesters/ScienceDirectRequester.o: src/Requesters/ScienceDirectRequester.cpp src/Requesters/ScienceDirectRequester.h
	g++ -std=c++11 -c src/Requesters/ScienceDirectRequester.cpp -o myBin/Requesters/ScienceDirectRequester.o
myBin/Requesters/ScopusRequester.o: src/Requesters/ScopusRequester.cpp src/Requesters/ScopusRequester.h
	g++ -std=c++11 -c src/Requesters/ScopusRequester.cpp -o myBin/Requesters/ScopusRequester.o
myBin/Requesters/SpringerRequester.o: src/Requesters/SpringerRequester.cpp src/Requesters/SpringerRequester.h
	g++ -std=c++11 -c src/Requesters/SpringerRequester.cpp -o myBin/Requesters/SpringerRequester.o

myBin/jsoncpp.o: lib/json/jsoncpp.cpp lib/json/json.h
	g++ -std=c++11 -c lib/json/jsoncpp.cpp -o myBin/jsoncpp.o
myBin/mongoose.o: lib/mongoose/mongoose.c lib/mongoose/mongoose.h
	g++ -c lib/mongoose/mongoose.c -o myBin/mongoose.o
myBin/BiblioServer.o: src/BiblioServer.cpp src/BiblioServer.h
	g++ -std=c++11 -c src/BiblioServer.cpp -o myBin/BiblioServer.o

myBin/FB2Parser.o: src/FB2Parser.cpp src/FB2Parser.h
	g++ -std=c++11 -c src/FB2Parser.cpp -o myBin/FB2Parser.o

prepare:
	mkdir -p myBin
	mkdir -p myBin/Requesters	

clean:
	rm -rf myBin biblio
