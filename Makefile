all:	 sender receiver

sender: sender.c
	gcc -Wall $< -o $@
	
receiver: receiver.c
	gcc -Wall $< -o $@

clean:
	rm -f sender receiver *.o *~ core

