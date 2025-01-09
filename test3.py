import random as rand




entryCost = 1

entries = 100

startAmount = entries * entryCost

winChance = .01

win = 100

profits = []

games = 1000000

for i in range(games):

    bal = startAmount

    for j in range(entries):
    
        bal -= 1

        if rand.random() <= winChance:
        
            bal += win

        
    profits.append(bal - startAmount)


print(sum(profits)/len(profits))




