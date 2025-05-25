read_count=20

function random_read() {
    for i in `seq 0 $read_count`; do
        echo "Read($1) - $i:"
        cat /dev/counter0 | hexdump
    done
}

for i in `seq 1 10`; do
    random_read $i &
done