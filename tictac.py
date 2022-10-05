# author: Larissa Munishkina
# date: May 21, 2020
# file: tictac.py a Python program that implements a tic-tac-toe game
# input: user responses (strings)
# output: interactive text messages and a tic-tac-toe board

from board import Board
from player import Player

# main program
while True:
    print("Welcome to the TIC-TAC-TOE Game!\n")
    check = input("Would you like to play against an AI? [Y/N] ").upper()
    AI = False
    if check == "Y":
        AI = True
    name2 = "AI"
    if AI:
        name1 = input("What is your name: ")
    else:
        name1 = input("What is player 1's name: ")
        name2 = input("What is player 2's name: ")
        
    board = Board()
    player1 = Player(name1, "X")
    player2 = Player(name2, "O")
    turn = True
    while True:
        board.show()
        if turn:
            player1.choose(board)
            turn = False
        else:
            player2.choose(board)
            turn = True
        if board.isdone():
            break
    board.show()
    if board.get_winner() == player1.get_sign():
        print(f"{player1.get_name()} is a winner!")
    elif board.get_winner() == player2.get_sign():
        print(f"{player2.get_name()} is a winner!")
    else:
        print("It is a tie!")
    ans = input("Would you like to play again? [Y/N] ").upper()
    if (ans != "Y"):
        break
print("\nGoodbye!")
