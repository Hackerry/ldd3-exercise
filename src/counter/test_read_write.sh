loop=10

for i in `seq 2 $loop`; do
    echo "Iteration $i:"
    cat /dev/counter0 | hexdump
done

echo "Write 3 to /dev/counter0"
echo -ne '\x03' > /dev/counter0

for i in `seq 2 $loop`; do
    echo "Iteration $i:"
    cat /dev/counter0 | hexdump
done

echo "Write 0xfd to /dev/counter0"
echo -ne '\xfd' > /dev/counter0

for i in `seq 2 $loop`; do
    echo "Iteration $i:"
    cat /dev/counter0 | hexdump
done

echo "Write 0xacd to /dev/counter0"
echo -ne '\xac\x0d' > /dev/counter0