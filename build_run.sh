# echo "$(cat src/gobu-app/version.h | sed 's/"//g ' | awk -F. -v OFS=. 'NF==1{print ++$NF}; NF>1{if(length($NF+1)>length($NF))$(NF-1)++; $NF=sprintf("%0*d", length($NF), ($NF+1)%(10^length($NF))); print}')" \" > src/gobu-app/version.h

cd builddir
ninja
meson install
cd ../bin
./gobueditor