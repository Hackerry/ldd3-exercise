if [ -n "$LDD_ROOT" ]; then
	cd $LDD_ROOT/initramfs
	find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../initramfs.cpio.gz

	sleep 1

	qemu -enable-kvm \
	     -kernel $LDD_ROOT/kernels/linux-6.14.4/arch/x86_64/boot/bzImage \
	     -initrd $LDD_ROOT/initramfs.cpio.gz \
	     -append 'console=ttyS0' \
	     -nographic \
	     -netdev user,id=host_net,hostfwd=tcp::7023-:23 \
	     -device e1000,mac=52:54:00:12:34:50,netdev=host_net
else
	echo '$LDD_ROOT variable not set'
	exit 1
fi

