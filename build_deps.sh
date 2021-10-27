mkdir install_deps
cd install_deps

git clone https://github.com/libevent/libevent.git
cd libevent
git checkout release-2.1.12-stable
mkdir build
cd build
cmake .. -DEVENT__DISABLE_OPENSSL=ON
make -j16
sudo make install
cd ../..

git clone https://github.com/google/googletest.git
cd googletest
mkdir build
cd build
cmake ..
make -j16
sudo make install
cd ../..

git clone https://github.com/open-source-parsers/jsoncpp.git
cd jsoncpp
mkdir build
cd build
cmake -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF ..
make -j16
sudo make install
cd ../..