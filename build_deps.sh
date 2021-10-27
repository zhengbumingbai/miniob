cd deps
cd libevent
git checkout release-2.1.12-stable
mkdir build
cd build
cmake .. -DEVENT__DISABLE_OPENSSL=ON
make
sudo make install
cd ../../..

cd deps
cd googletest
mkdir build
cd build
cmake ..
make
sudo make install
cd ../../..

cd deps
cd jsoncpp
mkdir build
cd build
cmake -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF ..
make
sudo make install

cd ../../..