all: build/say.com

build/say.com: say.c
	WATCOM=/usr/bin/watcom INCLUDE=/usr/bin/watcom/h PATH=$PATH:/usr/bin/watcom/binl64 wcl -mt say.c -fe=build/say.com

clean:
	rm build/*.com

mount:
	sudo mount -o uid=1026 /dev/sdc1 /mnt/dos

umount:
	sudo umount /mnt/dos

copy:
	sudo umount /mnt/dos || true
	sudo umount /mnt/dos || true
	sudo mount -o uid=1026 /dev/sdc1 /mnt/dos
	cp build/say.com /mnt/dos/AICOM/
	cp ../text/*.txt /mnt/dos/AICOM/
	sudo umount /mnt/dos
