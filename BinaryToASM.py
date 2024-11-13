
class BinaryToASM:

    """
    * create map of opcodes to ASM words
    * write separate functions to separate each of the fields depending on opcode
    * 

    """

    def __init__(self) -> None:
        """
        INSTRUCTIONS is the dict of instructions:opcode
        """
        self.INSTRUCTIONS = {
            "ADD":    0b10001011000,
            "ADDI":   0b1001000100,
            "AND":    0b10001010000,
            "ANDI":   0b1001001000,
            "B":      0b000101,
            "BL":     0b100101,
            "BR":     0b11010110000,
            "CBNZ":   0b10110101,
            "CBZ":    0b10110100,
            "EOR":    0b11001010000,
            "EORI":   0b1101001000,
            "LDUR":   0b11111000010,
            "LSL":    0b11010011011,
            "LSR":    0b11010011010,
            "ORR":    0b10101010000,
            "ORRI":   0b1011001000,
            "STUR":   0b11111000000,
            "SUB":    0b11001011000,
            "SUBI":   0b1101000100,
            "SUBIS":  0b1111000100,
            "SUBS":   0b11101011000,
            "MUL":    0b10011011000,
            "PRNT":   0b11111111101,
            "PRNL":   0b11111111100,
            "DUMP":   0b11111111110,
            "HALT":   0b11111111111
        }
    
        pass

    

    def determine_instruction_type(self, partial_binary_stream: int) -> str:
        """
        Given 32 bits of binary, determine the instruction type and return it as a string.
        """

        for i in range(12):
            if partial_binary_stream:

                pass


        # for i in range(31, -1, -1):
        #     print(f"Iteration {i} value is {bin(partial_binary_stream >> 32-i)}")
            # if partial_binary_stream >> i in self.INSTRUCTIONS.values():
            #     print("FOUND SOMETHING: " + partial_binary_stream >> i)


        return ""

    def determine_fields(self, instruction_type: str) -> list[int]:
        """
        Given an instruction type, return a list of the fields in order

        Args:
            instruction_type (str): Like "ADDI"

        Returns:
            list(int): Like [0b1001000100, 0b, 0b, 0b, etc] 
                where each is a field of binary of the instruction 

        """
        return []
    
    def convert_binary_to_asm(instruction_fields: list[int]) -> list[str]:
        pass

    def get_instruction_from_opcode(self, dictionary: dict[str, int], opcode:int) -> str:
        """
        Pass in an opcode and get the instruction.

        Args:
            dict: INSTRUCTIONS
            int: target_value is 

        Returns:
            str: the instruction like "ADDI"
        """
        for key, value in dictionary.items():
            if value == opcode:
                return key
        return None  # Return None if the value is not found

