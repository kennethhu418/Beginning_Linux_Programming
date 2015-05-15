#!/bin/bash


#test whether the env variable is changed after this script exists
ENVTEST="Subscript Value"

list_files() {
    if [ -n $1 ]; then
        ls $1 2>/dev/null
    else
        ls ./ 2>/dev/null
    fi

    return $?
}


files=$(list_files $1)
declare -i retval=$?
echo "Return value is $retval"
if [ $retval -eq 0 ]; then
    echo "There are following files in directory $1";
    for file in $files
    do
        echo "  "$file
    done
    exit 0
else
    exit $retval
fi


