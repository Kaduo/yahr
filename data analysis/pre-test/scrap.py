from random import random

n = 1000000
k = n
s = 0
for _ in range(n):
    if random() < 0.9:
        if random() < 0.8:
            if random() < 0.7:
                k -= 1
        else:
            s += 1

print(s/k)