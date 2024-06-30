
import sys
from math import sin


factor = 1e9
scale = 1e12


def fact(n):
    if n == 1:  return 1
    return n * fact(n - 1)

def sin_precise(a):
    return sin(a / factor) * factor

def sin_approx(a, n_terms):
    # 1
    x = float(a) / factor

    terms = []
    for i in range(0, n_terms + 1):
        if i % 4 == 3:
            terms.append( -1. / fact(i) )
        elif i % 4 == 1:
            terms.append( 1. / fact(i) )
        else:
            terms.append(0)

    acc = terms[n_terms]
    for i in reversed(range(0, n_terms)):
        acc *= x
        acc += terms[i]
                
    return acc * factor


def compare(a):
    x1 = sin_precise(a)

    print (f"precise = {x1}")
    for n_terms in range(2, 20):
        x2 = sin_approx(a, n_terms)
        delta = abs(x2 - x1)
        print (f"n = {n_terms},\t\tapprox = {x2}, \t\tdelta = {delta}")

def compare_int(a, n_terms):
    x1 = sin_precise(a)
    x2 = sin_approx_int(a, n_terms)
    delta = abs(x2 - x1)

    print (f"precise = {x1}")
    print (f"n = {n_terms}, approx = {x2:.3f}, delta = {delta:.3f}")



def sin_approx_int(a, n_terms):
    # 1
    # x = float(a) / factor

    terms = []
    for i in range(0, n_terms + 1):
        term = 0
        if i % 4 == 3:
            term = -1 * scale // fact(i)
        elif i % 4 == 1:
            term = scale // fact(i)
        else:
            term = 0
        terms.append(int(round(term)))

    print(f"terms = {terms}")


    acc = terms[n_terms]
    for i in reversed(range(0, n_terms)):
        acc *= a
        acc /= factor
        acc += terms[i]
                
    return acc * factor / scale

if __name__ == "__main__":
    a = 1570796327
    compare_int(a, 13)

