program: shell.c extra.c
	gcc -Wall shell.c -o shell
	gcc -Wall extra.c -o extra

shell: shell.c
	gcc -Wall shell.c -o shell

extra: extra.c
	gcc -Wall extra.c -o extra

