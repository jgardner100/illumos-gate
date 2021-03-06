#!/bin/bash

file=$1
project=$(echo "$2" | cut -d = -f 2)

if [[ "$file" = "" ]] ; then
    echo "Usage:  $0 <file with smatch messages> -p=<project>"
    exit 1
fi

if [[ "$project" != "kernel" ]] ; then
    exit 0
fi

bin_dir=$(dirname $0)
remove=$(echo ${bin_dir}/../smatch_data/kernel.frees_argument.remove)
tmp=$(mktemp /tmp/smatch.XXXX)

echo "// list of functions and the argument they free." > kernel.frees_argument
echo '// generated by `gen_frees_list.sh`' >> kernel.frees_argument
grep -w free_arg $file | cut -d ' ' -f 5- >> $tmp
cat $tmp $remove $remove 2> /dev/null | sort | uniq -u >> kernel.frees_argument
rm $tmp
echo "Done.  List saved as 'kernel.frees_argument'"

