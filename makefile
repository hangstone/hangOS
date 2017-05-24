all: BootLoader Kernel32 Kernel64 Utility Disk.img
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
	rm -f 01.Kernel32/Temp/Dependency.dep
	rm -f 01.Kernel32/Temp/EntryPoint.bin
	rm -f 01.Kernel32/Temp/Kernel32.elf
	rm -f 01.Kernel32/Temp/Main.o
	rm -f 01.Kernel32/Temp/Kernel32.elf.bin
	rm -f 01.Kernel32/Temp/Page.o
	rm -f 01.Kernel32/Temp/ModeSwitch.o
	
	@echo
	@echo ========== Build Complete ==========
	@echo
	
Kernel64:
	@echo 
	@echo ========== Build 64bit Kernel ==========
	@echo
	
	make -C 02.Kernel64
	rm -f 02.Kernel64/Temp/Dependency.dep
	rm -f 02.Kernel64/Temp/Kernel64.elf
	rm -f 02.Kernel64/Temp/Main.o
	rm -f 02.Kernel64/Temp/EntryPoint.o
	rm -f 02.Kernel64/Temp/AssemblyUtility.o
	rm -f 02.Kernel64/Temp/Keyboard.o
	
	@echo
	@echo ========== Build Complete ==========
	@echo
	
Disk.img: 00.BootLoader/BootLoader.bin 01.Kernel32/Kernel32.bin 02.Kernel64/Kernel64.bin
	@echo
	@echo ========== Disk Image Build Start ==========
	@echo
	
#	cat $^ > Disk.img
	./04.Utility/00.ImageMaker/ImageMaker.exe $^
	rm -f 00.BootLoader/BootLoader.bin
	rm -f 01.Kernel32/Kernel32.bin
	rm -f 02.Kernel64/Kernel64.bin
	rm -f 04.Utility/00.ImageMaker/ImageMaker.exe
	
	@echo
	@echo ========== All Build Complete ==========
	@echo

Utility:
	@echo 
	@echo =========== Utility Build Start ===========
	@echo 

	make -C 04.Utility

	@echo 
	@echo =========== Utility Build Complete ===========
	@echo 
	
clean:
	make -C 00.BootLoader clean
	make -C 01.Kernel32 clean
	make -C 02.Kernel64 clean
	make -C 04.Utility clean
	rm -f Disk.img