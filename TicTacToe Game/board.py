# author: Bruce Bai
# date: December 10, 2020
# file: board.py is a program of class Board that implements the update of board
#   and of how and when the game ends
# input: variables from the class player
# output: interactive text for the tic-tac-toe board

class Board:
    def __init__(self):
        # board is a list of cells that are represented 
        # by strings (" ", "O", and "X")
        # initially it is made of empty cells represented 
        # by " " strings
        self.sign = " "
        self.size = 3
        self.board = list(self.sign * self.size**2)
        # the winner's sign O or X
        self.winner = ""
    def get(self, ind):
        #retrieves content of cell
        return self.board[ind]
    def get_winner(self):
        # return the winner's sign O or X (an instance winner)
        return(self.winner)
    def set(self, cell, sign):
        # mark the cell on the board with the sign X or O
        idx = ("A1", "A2", "A3", "B1", "B2", "B3", "C1", "C2", "C3")
        x = idx.index(cell)
        self.board[x] = sign
    def isempty(self, cell):
        # return True if the cell is empty (not marked with X or O)
        idx = ("A1", "A2", "A3", "B1", "B2", "B3", "C1", "C2", "C3")
        x = idx.index(cell)
        if self.board[x] == " ":
            return True
    def isdone(self):
        done = False
        # check all game terminating conditions, if one of them is present, assign the var done to True
        # depending on conditions assign the instance var winner to O or X
        if self.sign not in self.board:
            done = True
        if(((self.board[0] == self.board[1] and self.board[1] == self.board[2]) or (self.board[0] == self.board[3] and self.board[3] == self.board[6])) and self.board[0] != " "):
            done = True
            self.winner = self.board[0]
        elif(((self.board[3] == self.board[4] and self.board[4] == self.board[5]) or (self.board[1] == self.board[4] and self.board[4] == self.board[7])) and self.board[4] != " "):
            done = True
            self.winner = self.board[4]
        elif(((self.board[2] == self.board[5] and self.board[5] == self.board[8]) or (self.board[6] == self.board[7] and self.board[7] == self.board[8])) and self.board[8] != " "):
            done = True
            self.winner = self.board[8]
        elif(((self.board[0] == self.board[4] and self.board[4] == self.board[8]) or (self.board[6] == self.board[4] and self.board[4] == self.board[2])) and self.board[4] != " "):
            done = True
            self.winner = self.board[4]
        return done
    def show(self):
        # draw the board
        print("\n   A   B   C   ")  
        print(" +---+---+---+ ")
        print("1| {} | {} | {} | ".format(self.board[0], self.board[3], self.board[6]))
        print(" +---+---+---+ ")
        print("2| {} | {} | {} | ".format(self.board[1], self.board[4], self.board[7]))
        print(" +---+---+---+ ")
        print("3| {} | {} | {} | ".format(self.board[2], self.board[5], self.board[8]))
        print(" +---+---+---+ \n")
