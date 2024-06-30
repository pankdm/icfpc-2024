

def convert(num, base):
    res = []
    while num > 0:
        res.append(num % base)
        num //= base
    return res


def check(num, q):
    digits = convert(num, q)
    if digits == list(reversed(digits)):
        return True
    return False


def solve(num):
    for q in range(2, num + 10):
        if check(num, q):
            return q
    assert False, q


print (solve(1233321))
print (solve(3123))


# print (convert(4, 2))
worst = (2, None)
for num in range(2, 100000 + 1):
    q = solve(num)
    if worst[0] < q:
        worst = max(worst, (q, num))
        print (worst)

print (worst)
    
