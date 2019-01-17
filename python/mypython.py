#!/usr/bin/env python3

###############################################################################
# Name:         Marc Tibbs
# Email:        tibbsm@oregonstate.edu
# Class:        CS344 - Winter 2019
# Assignment:   Program Py
# Due date:     Feb 27 2019
###############################################################################

import random
import string

def main():

	# Create 3 files named testfile[x] where x is its order number	
	for i in range(1,4):
		f = open("testfile" + str(i), "w+")

		# Create a random, 10-letter string, print it, and write it to file
		randoms=(''.join(random.choice(string.ascii_lowercase) for x in range(10)))	
		print(randoms)
		f.write(randoms+"\n")

	# Randomly generate 2 numbers print them and the product
	num1, num2=random.randint(1, 42), random.randint(1, 42)
	print(num1)
	print(num2)
	print(num1 * num2)

if __name__ == "__main__":
    main()
