mkdir -p depends
cd depends
rm -rf *
cp -vrdf ../../depends/lib/*.so* .

cp -vrdf /usr/lib/x86_64-linux-gnu/libstdc++.so.* .


cd ..
#ln -s ../depends/bin/* .
