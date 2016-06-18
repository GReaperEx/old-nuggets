
ISO.iso :
	cd kernel/ ; make
	cp kernel/image.sys ISO/boot/kernel/
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o ISO.iso ISO/

.PHONY : ISO.iso clean

clean :
	rm ISO.iso ISO/boot/kernel/image.sys -f
	cd kernel/ ; make clean

