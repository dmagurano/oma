import sys

if __name__ == '__main__':
	if (len(sys.argv) >= 2):
		heur = float(sys.argv[1])
		opt = float(sys.argv[2])
		gap = round((heur - opt) / opt * 100.0, 2)
		print str(gap).replace(".", ",")
		if (gap > 2.0):
			exit(1)
		else:
			exit(0)
		