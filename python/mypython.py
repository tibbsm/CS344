#!/usr/bin/env python3
import random
import string

def main():
	
	for i in range(1,4):
		filename="testfile" + str(i)
		f = open(filename, "w+")

		randoms=(''.join(random.choice(string.ascii_lowercase) for x in range(10)))	
		print(randoms)
		f.write(randoms+"\n")

	num1=random.randint(1, 42)
	num2=random.randint(1, 42)
	print(num1)
	print(num2)
	print(num1 * num2)

if __name__ == "__main__":
    main()
