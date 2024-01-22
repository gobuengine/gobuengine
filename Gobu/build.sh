meson build --wip
cd build
ninja && meson install
cd ..
cd bin
./gobu-editor