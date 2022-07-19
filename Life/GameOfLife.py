# Game of Life
# Rorik Melberg

import random
import time
import os

# Constants
HEIGHT = 20
WIDTH = 20

def establishGrid():

    life = []

    for x in range(WIDTH):
        life.append([0] * WIDTH)

    return life

def outputLife(life):
    
    clear = lambda: os.system('cls')
    clear()

    for h in life:
        for w in h:
            if w == 0:
                w = ' '
            else:
                w = '0'
            print('{} '.format(w), end='')
        print('')
    print('')

def getTestGrid(h, w):
    testGridFinal = []
    testGridStart = [
                    [int(h-1),int(w-1)], [int(h-1),w], [int(h-1),int(w+1)],
                    [h,w-1],            [h,w+1],
                    [h+1,w-1], [h+1,w], [h+1,w+1]
               ]
    for test in testGridStart:
        if test[0] < 0:
            test[0] = HEIGHT - 1
        elif test[0] >= HEIGHT:
            test[0] = 0

        if test[1] < 0:
            test[1] = WIDTH - 1
        elif test[1] >= WIDTH:
            test[1] = 0
        testGridFinal.append(test)
    
    return testGridFinal

def randomize(life, numberToEstablish):

    for x in range(numberToEstablish):
        h = random.randint(0, HEIGHT-1)
        w = random.randint(0, WIDTH-1)
        
        # now randomiz around that point
        life[h][w] = 1
        testGrid = getTestGrid(h,w)

        for testLoc in testGrid:
            life[testLoc[0]][testLoc[0]] = random.randint(0, 1)
                    

    return life

def lifeSum(life):
    total = 0

    for x in life:
        total = total + sum(x)

    return total

# Return values
# 0 - Stay the same
# 1 - Make Dead
# 2 - Make Alive
def scoreCell(life, h, w):
    
    liveCellCount = 0
    testGrid = getTestGrid(h,w)

    for testLoc in testGrid:
        if life[testLoc[0]][testLoc[1]] > 0:
            liveCellCount = liveCellCount + 1
    
    result = 0

    if life[h][w] == 0 and liveCellCount == 3:
        result = 2
    else:
        if liveCellCount <= 1:
            result = 1
        elif liveCellCount == 2 or liveCellCount == 3:
            result = 2
        else:
            result = 1
    
    return result

# Any live cell with fewer than two live neighbours dies, as if by underpopulation.
# Any live cell with two or three live neighbours lives on to the next generation.
# Any live cell with more than three live neighbours dies, as if by overpopulation.
# Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
# 
# These rules, which compare the behavior of the automaton to real life, can be condensed into the following:

def process(life):
    
    newLife = establishGrid()

    for h in range(len(life)):
        for w in range(len(life[h])):
            score = scoreCell(life, h, w)
            # print(score)
            if score == 1:
                newLife[h][w] = 0
            elif score == 2:
                if life[h][w] == 0:
                    newLife[h][w] = 1
            else:
                newLife[h][w] = life[h][w]

            
        # Age
        # if newLife[h][w] > 0:
        #     newLife[h][w]  = newLife[h][w] + 1


    return newLife

life = establishGrid()

while True:
    if lifeSum(life) == 0:
        life = randomize(life, 1)
    
    outputLife(life)
    life = process(life)
    outputLife(life)

    time.sleep(2)



