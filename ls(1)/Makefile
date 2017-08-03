ls : ls.c
	gcc -o $@ ls.c

run :
	@echo -e "*****show cwd*****\n"
	./ls
	@echo -e "\n\n*****show .. and /*****\n"
	./ls .. /
.PHONY : run
