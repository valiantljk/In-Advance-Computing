#!/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/jialliu/libmemcached/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/jialliu/pnetcdf/lib
export C_INCLUDE_PATH=$C_INCLUDE_PATH:/home/jialliu/libmemcached/include
export C_INCLUDE_PATH=$C_INCLUDE_PATH:/home/jialliu/pnetcdf/include
echo $LD_LIBRARY_PATH
echo $C_INCLUDE_PATH
