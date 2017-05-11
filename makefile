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
	
Kernel32:
	@echo 
	@echo ========== Build 32bit Kernel ==========
	@echo
	
	make -C 01.Kernel32
	
	@echo
	@echo ========== Build Complete ==========
	@echo
	
Disk.img: BootLoader Kernel32
	@echo
	@echo ========== Disk Image Build Start ==========
	@echo
	
	cat 00.BootLoader/BootLoader.bin 01.Kernel32/VirtualOS.bin > Disk.img
	rm -f 00.BootLoader/BootLoader.bin
	rm -f 01.Kernel32/VirtualOS.bin
	
	@echo
	@echo ========== All Build Complete ==========
	@echo
	
clean:
	make -C 00.BootLoader clean
	make -C 01.Kernel32 clean
	rm -f Disk.img