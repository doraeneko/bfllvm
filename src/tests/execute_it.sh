# Check if the file exists
if [ -f "$1.bc" ]; then
    rm "$1.bc"
fi
../../build/bfllvm -o "$1.bc" < $1
lli "$1.bc"
rm "$1.bc"

