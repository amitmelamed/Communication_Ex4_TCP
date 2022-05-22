all:  sender Measure
sender: sender.c
	gcc -g sender.c -o sender
Measure: measure.c
	gcc -g measure.c -o measure
.PHONY: clean#.PHONY means that clean is not a file.
clean:
	rm -f sender measure