all: asn3
asn3: assignment3.c
	gcc -o asn3.out assignment3.c -lpthread
clean:
	rm -f *.out assignment_3_output_file.txt core