#!/bin/sh
appname=`basename $0|sed s,\.sh$,,`
dirname=`dirname $0`   #注意appname与dirname内容外面并不是单引号，而是撇号,笔者曾因此花费大量时间
tmp="${dirname#?}"
if[ "${dirname%$tmp}" != "/" ]; then
	dirname=$PWD/$dirname
fi
LD_LIBRARY_PATH=$dirname
export LD_LIBRARY_PATH
$dirname/$appname "$@"
