all: iT2L.so

iT2L.so: iT2L.cpp
	g++ -o iT2L.so -shared -L ./lib -lASREngineAPI -lACE-5.5.6 -lstdc++ -I /usr/lib/jvm/java-7-oracle/include -I /usr/lib/jvm/java-7-oracle/include/linux -I ./include iT2L.cpp