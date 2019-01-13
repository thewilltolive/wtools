
OPTIONS="BKV_JSON_YAJL BKV_CACHED_DICO BKV_FILE_SUPPORT"

rm -rf build

build_option() {
  echo "BUILD OPTION $1"
  echo ""
  mkdir build
  cd build
  cmake -D$1=y ..
  make 
  cd ..
  rm -rf build
}

build_and_pass_test() {
  echo "BUILD OPTION $1"
  echo ""
  mkdir build
  cd build
  cmake -D$1=y -DBUILD_TEST=y ..
  make 
  make test
  cd ..
  rm -rf build 
}


build_option() {
  echo "BUILD OPTION $1"
  echo ""
  mkdir build
  cd build
  cmake -D$1=y ..
  make 
  cd ..
  rm -rf build 
}

for o in $OPTIONS; do
  build_option $o
  build_and_pass_test $o
done


