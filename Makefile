all:
	$(MAKE) -f Makefile.board
	$(MAKE) -f Makefile.model

clean:
	rm -f *.a
	rm -f *.o  asm/*.o
	rm -f *.elf
	rm -f *.v
	rm -f *.bin
	rm -f *.md5
	rm -f *.hex
	rm -f *.dmp

