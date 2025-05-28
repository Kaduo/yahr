from random import random, choices
from math import log10, pi

ev = 0
for r in range(6, 11):
    ev += -2*(pi*r**2 - pi*(r-1)**2)/(100*pi)

for r in range(1, 6):
    ev += (11 - r)*(pi*r**2 - pi*(r-1)**2)/(100*pi)

print(ev)

# TODO : Faire la visualisation avec les groupements sur une grande feuille A3 avec des pastilles
# TODO : 