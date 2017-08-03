test : test.c daemon.c daemon.h
	gcc -o $@ test.c daemon.c

run :
	@echo "*****start to run*****"
	-./test
	@echo "*****show pstree*****"
	-pstree | grep --color=auto -5 "test"
	@echo "*****show ps*****"
	-ps -efj | grep --color=auto "test"
	@echo "*****show log*****"
	-sleep 5
	-cat log
.PHONY : run

clean :
	-pkill test
	-rm -f log test
.PHONY : clean

