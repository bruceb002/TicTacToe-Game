# author: Bruce Bai
# date: December 10, 2020
# file: player.py a program that implements the user info
#   in order to continue the game
# input: variables from both the main program
#   and the board file
# output: interactive text messages and updates the tictactoe board

from board import Board
from random import seed
from random import choice

class Player(Board):
    def __init__(self, name, sign):
        super().__init__()
        self.name = name  # player's name
        self.sign = sign  # player's sign O or X
    def get_sign(self):
        # return an instance sign
        return self.sign
    def get_name(self):
        # return an instance name
        return self.name

    ###AI FUNCTIONS###
    def finish(self, board):
        #finish and win the game if possible
        if(board.isempty("A1") and ((board.get(3) == self.sign and board.get(6) == self.sign) or (board.get(4) == self.sign and board.get(8) == self.sign) or (board.get(1) == self.sign and board.get(2) == self.sign))):
            return "A1"
        if(board.isempty("A2") and ((board.get(0) == self.sign and board.get(2) == self.sign) or (board.get(4) == self.sign and board.get(7) == self.sign))):
            return "A2"
        if(board.isempty("A3") and ((board.get(0) == self.sign and board.get(1) == self.sign) or (board.get(5) == self.sign and board.get(8) == self.sign) or (board.get(4) == self.sign and board.get(6) == self.sign))):
            return "A3"
        if(board.isempty("B1") and ((board.get(0) == self.sign and board.get(6) == self.sign) or (board.get(4) == self.sign and board.get(5) == self.sign))):
            return "B1"
        if(board.isempty("B2") and ((board.get(3) == self.sign and board.get(5) == self.sign) or (board.get(1) == self.sign and board.get(7) == self.sign) or (board.get(2) == self.sign and board.get(6) == self.sign) or (board.get(0) == self.sign and board.get(8) == self.sign))):
            return "B2"
        if(board.isempty("B3") and ((board.get(2) == self.sign and board.get(8) == self.sign) or (board.get(3) == self.sign and board.get(4) == self.sign))):
            return "B3"
        if(board.isempty("C1") and ((board.get(0) == self.sign and board.get(3) == self.sign) or (board.get(2) == self.sign and board.get(4) == self.sign) or (board.get(7) == self.sign and board.get(8) == self.sign))):
            return "C1"
        if(board.isempty("C2") and ((board.get(6) == self.sign and board.get(8) == self.sign) or (board.get(1) == self.sign and board.get(4) == self.sign))):
            return "C2"
        if(board.isempty("C3") and ((board.get(2) == self.sign and board.get(5) == self.sign) or (board.get(6) == self.sign and board.get(7) == self.sign) or (board.get(0) == self.sign and board.get(4) == self.sign))):
            return "C3"
        return "-1"
    
    def block(self, board, opp):
        # blocks opponents chance to win if possible
        if(board.isempty("A1") and ((board.get(3) == opp and board.get(6) == opp) or (board.get(4) == opp and board.get(8) == opp) or (board.get(1) == opp and board.get(2) == opp))):
            return "A1"
        if(board.isempty("A2") and ((board.get(0) == opp and board.get(2) == opp) or (board.get(4) == opp and board.get(7) == opp))):
            return "A2"
        if(board.isempty("A3") and ((board.get(0) == opp and board.get(1) == opp) or (board.get(5) == opp and board.get(8) == opp) or (board.get(4) == opp and board.get(6) == opp))):
            return "A3"
        if(board.isempty("B1") and ((board.get(0) == opp and board.get(6) == opp) or (board.get(4) == opp and board.get(5) == opp))):
            return "B1"
        if(board.isempty("B2") and ((board.get(3) == opp and board.get(5) == opp) or (board.get(1) == opp and board.get(7) == opp) or (board.get(2) == opp and board.get(6) == opp) or (board.get(0) == opp and board.get(8) == opp))):
            return "B2"
        if(board.isempty("B3") and ((board.get(2) == opp and board.get(8) == opp) or (board.get(3) == opp and board.get(4) == opp))):
            return "B3"
        if(board.isempty("C1") and ((board.get(0) == opp and board.get(3) == opp) or (board.get(2) == opp and board.get(4) == opp) or (board.get(7) == opp and board.get(8) == opp))):
            return "C1"
        if(board.isempty("C2") and ((board.get(6) == opp and board.get(8) == opp) or (board.get(1) == opp and board.get(4) == opp))):
            return "C2"
        if(board.isempty("C3") and ((board.get(2) == opp and board.get(5) == opp) or (board.get(6) == opp and board.get(7) == opp) or (board.get(0) == opp and board.get(4) == opp))):
            return "C3"
        return "-1"
    ######
        
    def choose(self, board):
        # prompt the user to choose a cell
        # if the user enters a valid string and the cell on the board is empty, update the board
        # otherwise print a message that the input is wrong and rewrite the prompt
        # use the methods board.isempty(cell), and board.set(cell, sign)
        # you need to convert A1, B1, …, C3 cells into index values from 1 to 9
        # you can use a tuple ("A1", "B1",...) to obtain indexes 
        # you can do the conversion here in the player.py or in the board.py
        # this implementation is up to you
        idx = ("A1", "A2", "A3", "B1", "B2", "B3", "C1", "C2", "C3")
        seed(184)
        first = True
        while True:
            cell = " "
            if self.name != "AI":
                cell = input(f"{self.name}, {self.sign}: Enter a cell [A-C][1-3]: ")
                cell = cell.upper()
            else:
                # print out only once
                if first:
                    print("The AI is choosing a cell...")
                first = False
                
                #Opponents sign
                opp = "X"
                
                cell = self.finish(board)
                if cell == "-1":
                    cell = self.block(board, opp)
                    
                #if unable to either block or finish, just randomly generate
                if cell == "-1":
                    cell = choice(idx)
                    
            wrong = False
            for letter in idx:
                if cell == letter:
                    break
                elif cell != letter:
                    if letter == "C3":
                        #AI shouldn't be 'choosing wrong'
                        if self.name != "AI":
                            print("\nYou did not choose correctly.")
                        wrong = True
                        break
                    else:
                        continue
                    
            if wrong:
                continue
            if board.isempty(cell):
                board.set(cell, self.sign)
                break
            else:
                #AI shouldn't be 'choosing wrong'
                if self.name != "AI":
                    print("\nYou did not choose correctly.")
                continue
        
                
                
        
