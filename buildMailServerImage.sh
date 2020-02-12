cd build
make clean;
cmake ..
make -j4
cd ..
cp -r lib/ docker/
cp -r etc/ docker/
cd bin
cp mailServer ../docker
