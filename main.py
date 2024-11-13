# Wilson Diep
# diepw50@iastate.edu
# COMS 321 Programming Assignment 2
# 11/13/12024
from BinaryToASM import BinaryToASM

def main():
    Converter:BinaryToASM = BinaryToASM()
    print("Output: " + Converter.get_instruction_from_opcode(Converter.INSTRUCTIONS, 0b1001000100))

if __name__ == '__main__':
    main()