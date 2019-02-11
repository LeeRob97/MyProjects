#This makefile links .h files with there .cpp files

#compiler variables
# ** NOTE ** -c makes .o file (object file)
CFLAGS = -w -O #set flags
CC = g++ -std=c++11 #set type of compiler | here it is g++ can change to "gcc"

#HERE IS WHERE YOU SET THE EXECUTABLE NAME: Here it is: "proj2"
# NOTE more files added requires the .o file to be added below
<exec name>: <main>.o 
	$(CC) $(CFLAGS) -o <exec name> <main>.o <other files>.o
	rm *.o

<main>.o: <main>.cpp
	$(CC) $(CFLAGS) -c <main>.cpp

#Just add more of these but with appropiate file names
# <filename>.o: <filename>.cpp
#	$(CC) $(CFLAGS) -c <filename>.cpp **NOTE SAME NAME AS LINE ABOVE

#run program command: "make run"
run:
	./<exec name>

#rm all .o files
#rm <executable>
clean:
	rm -f core *.o 
	rm <exec name>  