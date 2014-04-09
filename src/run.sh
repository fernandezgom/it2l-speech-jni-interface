#!/bin/sh
cd /usr/share/MMIndexer6.2/bin/
export LD_LIBRARY_PATH=.
export PERL5LIB=../libx

cp iT2L.so libiT2L.so.so

echo "Compiling $1.java"

javac $1.java

echo "Running $1 with strace. See outputJNI.log"

strace -f java -Djava.library.path=/usr/share/MMIndexer6.2/bin:. $1 2>outputJNI.log

echo "Running $1 Standard output"
java -Djava.library.path=/usr/share/MMIndexer6.2/bin:. $1