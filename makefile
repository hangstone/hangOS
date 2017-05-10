all: BootLoader Disk.img
	mv -f Disk.img C:\Project\hangOS\99.Test\Disk.img

BootLoader:
	@echo 
	@echo ========== Build Boot Loader ==========
	@echo
	
	make -C 00.BootLoader
	
	@echo
	@echo ========== Build Complete ==========
	@echo
	
Disk.img: 00.BootLoader/BootLoader.bin
	@echo
	@echo ========== Disk Image Build Start ==========
	@echo
	
	cp 00.BootLoader/BootLoader.bin Disk.img
	rm -f 00.BootLoader/BootLoader.bin
	
	@echo
	@echo ========== All Build Complete ==========
	@echo
	
clean:
	make -C 00.BootLoader clean
	rm -f Disk.img