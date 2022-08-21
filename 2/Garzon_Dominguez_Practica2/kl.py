
def repeated(lst, string):
	e = 0
	for i in lst:
		if string == i:
			e = 1
			break
	return e


def cartesian_sm(a, b):
	c = []
	for i in a:
		for j in b:
			s = i+j
			if repeated(c, s) == 0:
				c.append(s)
	return c

def kleene(a, lim):
	mv = lim
	rv = []
	while( mv != 0 ):
		rv.append("")
		rv = cartesian_sm(rv, a)
		mv -= 1
	rv.append("")
	return rv

import time
start_time = time.time()

a = ["a", "bc", "cde"]

kl = kleene(a, 1)

print(len(kl))
print("--- %s seconds ---" % (time.time() - start_time))



























