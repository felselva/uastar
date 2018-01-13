if [ -n $CC ]
then
  if [ $CC=clang ]
  then
    clang test.c -include uastar.h -c uastar.c && clang -o test_clang test.o uastar.o -lm && chmod +x test_clang
    ./test_clang
  elif [ $CC=gcc ]
  then
    gcc -o test_gcc test.c uastar.c -I. -lm && chmod +x test_gcc
    ./test_gcc
  else
    echo -e "unrecognized compiler"
    exit
  fi
else
  echo -e "CC is not defined, compiler cannot be identified"
fi
