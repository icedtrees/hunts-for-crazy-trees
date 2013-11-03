#-------------------------------------------------------------------------------
# Name:        DraculaMovie
# Purpose:
#
# Author:      icedtrees
#
# Created:     26/10/2013
# Copyright:   (c) icedtrees 2013
# Licence:     GNU GPL 3.0
#-------------------------------------------------------------------------------

#!/usr/bin/python2.7

import sys
import pygame
import random

green, red = (0, 255, 0), (255, 0, 0)
blue, yellow, purple, cyan = (0, 0, 255), (255, 255, 0), (255, 0, 255), (0, 255, 255)

cityPositions = {"AL":(339, 811), "AM":(562, 276), "AT":(1177, 820), "BA":(406, 719), "BI":(970, 758), "BE":(1053, 630), "BR":(767, 277), "BO":(350, 550), "BU":(539, 353), "BC":(1177, 559), "BD":(975, 464), "CA":(117, 859), "CG":(662, 818), "CD":(1149, 437), "CF":(476, 537), "CO":(628, 309), "CN":(1296, 518), "DU":(262, 228), "ED":(373, 119), "FL":(754, 635), "FR":(675, 367), "GA":(1210, 482), "GW":(184, 206), "GE":(599, 518), "GO":(678, 616), "GR":(217, 835), "HA":(665, 255), "JM":(954, 582), "KL":(1090, 486), "LE":(410, 399), "LI":(733, 334), "LS":(33, 761), "LV":(329, 238), "LO":(423, 313), "MA":(205, 724), "MN":(388, 216), "MR":(575, 650), "MI":(669, 565), "MU":(764, 469), "NA":(326, 468), "NP":(863, 759), "NU":(743, 419), "PA":(491, 455), "PL":(314, 344), "PR":(810, 398), "RO":(782, 712), "SA":(1154, 733), "SN":(239, 614), "SR":(313, 689), "SJ":(990, 647), "SO":(1156, 638), "ST":(617, 434), "SW":(326, 296), "SZ":(1011, 547), "TO":(421, 611), "VA":(1073, 795), "VR":(1281, 629), "VE":(776, 571), "VI":(880, 463), "ZA":(890, 558), "ZU":(657, 496), "NS":(497, 171), "EC":(376, 369), "IS":(253, 291), "AO":(129, 413), "BB":(246, 549), "MS":(489, 913), "TS":(745, 869), "IO":(1055, 931), "AS":(905, 672), "BS":(1341, 573)}
class Player(object):
    def __init__(self, name, currentCity, colour, size):
        self.currentCity = currentCity
        self.colour = colour
        self.size = size
        self.name = name
        if name == "D":
            self.thickness = 0
        else:
            self.thickness = 3

    def get_position(self):
        # Randomises current location
        return cityPositions[self.currentCity]

    def move(self, cityFrom, cityTo):
        pass

    def display(self):
        # Dracula is big red circle, hunters are green bordered
        position = self.get_position()
        if self.name == "D":
            pygame.draw.circle(screen, self.colour, position, self.size, self.thickness)
        else:
            pygame.draw.circle(screen, self.colour, position, self.size, 0)
            pygame.draw.circle(screen, green, position, self.size, self.thickness)



inputSource = raw_input("What input would you like to use?")

# Basic display settings
screen = pygame.display.set_mode((1405, 993))
pygame.display.set_caption("Dracula: THE MOVIE")

# Get rectangle background from Europe map
europeMap = pygame.image.load("Europe.png")
backgroundRect = europeMap.get_rect()



# Get plays input from source
pastPlays = [] # Format: (player, city), (player, city), etc.
if inputSource == "stdin":
    pass
else:
    textPlays = open(inputSource, "r").read().split(" ")
    for play in textPlays:
        pastPlays.append((play[0], play[1:3]))

players = {}
# Initialise players
godalming = Player("G", "??", blue, 15)
seward = Player("S", "??", yellow, 15)
helsing = Player("H", "??", purple, 15)
mina = Player("M", "??", cyan, 15)
dracula = Player("D", "??", red, 20)

for player in (godalming, seward, helsing, mina, dracula):
    players[player.name] = player

locationIndex = 0
framesPassed = 0
startIndex = 0
while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit(0)
        if event.type == pygame.MOUSEBUTTONDOWN:
            startIndex += 1

    # move current player
    # if framesPassed * 10 // moveDelay < len(pastPlays):
    if startIndex < len(pastPlays):
        # find next move
        # startIndex = framesPassed * 10 // moveDelay
        currentPlayer = players[pastPlays[startIndex][0]]
        currentMove = pastPlays[startIndex][1]
        if currentMove == "HI":
            pass # no change
        elif currentMove[0] == "D" and currentMove[1].isdigit():
            dbIndex = int(currentMove[1])
            currentPlayer.currentCity = pastPlays[startIndex - (5 * dbIndex)][1]
            if currentPlayer.currentCity == "HI":
                currentPlayer.currentCity = pastPlays[startIndex - (5 * (dbIndex + 1))][1]
        elif currentMove == "TP":
            currentPlayer.currentCity = "CD"
        else:
            currentPlayer.currentCity = currentMove

    # draw background
    screen.blit(europeMap, backgroundRect)

    # draw players on screen
    if players["D"].currentCity != "??":
        players["D"].display()
    for player in players.values():
        if (player.name != "D"):
            if player.currentCity != "??":
                player.display()

    pygame.display.flip()
    pygame.time.delay(10)
    framesPassed += 1



