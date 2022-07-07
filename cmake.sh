export CPLEX_ROOT=/opt/applis/cplex-12.10
export CC=gcc-9
export CXX=g++-9
cpu=`getconf _NPROCESSORS_ONLN`
export BAPCOD_ROOT=`pwd`/../../bapcodframework
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=1
