from BinaryToASM import BinaryToASM

b = BinaryToASM()
# SUB X12, X10, XZR = 11001011000111110000000101001100
out = b.determine_instruction_type(0b11001011000111110000000101001100)
print(out)