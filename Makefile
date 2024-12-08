hello:
	echo "Hello, World!"

command: command_shell.c
	gcc -o command_shell command_shell.c

run:
	./command_shell

clean:
	rm -f command_shell