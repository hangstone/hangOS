##################################################
# configuration build environment & rules
##################################################
NASM64 = nasm -f elf64
GCC64 = x86_64-pc-linux-gcc.exe -c -m64 -ffreestanding
LD64 = x86_64-pc-linux-ld.exe -melf_x86_64 -T ../elf_x86_64.x -nostdlib -e Main -Ttext 0x200000
OBJCOPY64 = x86_64-pc-linux-objcopy.exe -j .text -j .data -j .rodata -j .bss -S -O binary

OBJECTDIRECTORY = Temp
SOURCEDIRECTORY = Source


##################################################
# configuration to build contents & method
##################################################
all: prepare Kernel64.bin

prepare:
	mkdir -p $(OBJECTDIRECTORY)
	
dep:
	@echo === Make Dependency File ===
	make -C $(OBJECTDIRECTORY) -f ../makefile InternalDependency
	@echo === Dependency Search Complete ====
	
ExecuteInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../makefile Kernel64.elf	
	
Kernel64.bin: ExecuteInternalBuild
	$(OBJCOPY64) $(OBJECTDIRECTORY)/Kernel64.elf $@
	
clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*
	
	
##################################################
# recalled by Make, Executed by Temp directory
##################################################
#	������ ��������� ��Ʈ�� ����Ʈ �ҽ� ���� ����, Temp ���丮�� �������� ����
ENTRYPOINTSOURCEFILE = ../$(SOURCEDIRECTORY)/EntryPoint.s
ENTRYPOINTOBJECTFILE = EntryPoint.o
#	������ C �ҽ� ���� ����, Temp ���丮�� �������� ����
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
COBJECTFILES = $(notdir $(patsubst %.c,%.o,$(CSOURCEFILES)))
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCEFILES)))

# ��������� ��Ʈ�� ����Ʈ ����
#	kernel entry point ������ object ������ ���·� �����ϵǾ� C��� Ŀ�ΰ� �Բ� ��ũ�ȴ�
#	�׷��Ƿ� kernel entry point ������ ��ũ ����� ���� �տ� ��ġ�ϵ��� �����Ѵ�
$(ENTRYPOINTOBJECTFILE): $(ENTRYPOINTSOURCEFILE)
	$(NASM64) -o $@ $<
	
#	.c ������ .o ���Ϸ� �ٲٴ� ��Ģ ����
%.o: ../$(SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<
	
# .asm ������ .o ���Ϸ� �ٲٴ� ��Ģ ����
%.o: ../$(SOURCEDIRECTORY)/%.asm
	$(NASM64) -o $@ $<
	
InternalDependency:
	$(GCC64) -MM $(CSOURCEFILES) > Dependency.dep
	
Kernel64.elf: $(CENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	$(LD64) -o $@ $^
	
ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif








