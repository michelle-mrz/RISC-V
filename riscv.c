#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "hashtable.h"
#include "riscv.h"

/************** BEGIN HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ***************/
const int R_TYPE = 0;
const int I_TYPE = 1;
const int MEM_TYPE = 2;
const int U_TYPE = 3;
const int UNKNOWN_TYPE = 4;

/**
 * Return the type of instruction for the given operation
 * Available options are R_TYPE, I_TYPE, MEM_TYPE, UNKNOWN_TYPE
 */
static int get_op_type(char *op)
{
    const char *r_type_op[] = {"add", "sub", "and", "or", "xor", "slt", "sll", "sra"};
    const char *i_type_op[] = {"addi", "andi", "ori", "xori", "slti"};
    const char *mem_type_op[] = {"lw", "lb", "sw", "sb"};
    const char *u_type_op[] = {"lui"};
    for (int i = 0; i < (int)(sizeof(r_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(r_type_op[i], op) == 0)
        {
            return R_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(i_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(i_type_op[i], op) == 0)
        {
            return I_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(mem_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(mem_type_op[i], op) == 0)
        {
            return MEM_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(u_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(u_type_op[i], op) == 0)
        {
            return U_TYPE;
        }
    }
    return UNKNOWN_TYPE;
}
/*************** END HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ****************/

registers_t *registers;
// TODO: create any additional variables to store the state of the interpreter
hashtable_t *memory;

void init(registers_t *starting_registers)
{
    registers = starting_registers;
    // TODO: initialize any additional variables needed for state
    memory = ht_init(100000);

}

void end()
{
    // TODO: Free everything from memory
    free(registers);
    ht_free(memory);

}

// TODO: create any necessary helper functions

/**
 * Return a pointer to the first non-space character in the string,
 * copied from riscv_interpreter.c file
 */
char *ltrim_spaces_1(char *s)
{
    while (isspace(*s))
        s++;
    return s;
}

/**
 * Sign extends 12-bit immediates.
 */
int sign_extend(int val) {
    if (val > 0x7ff) {
        val = val - 0x1000;
    }
    return val;
}
/**
 * Return a the register number,
 * slices 'x' from register to return an int index
 */
int reg_num(char* str) 
{
    char *reg = ltrim_spaces_1(str);
    char *reg_final = reg + 1;
    return atoi(reg_final);
}

/**
 * Return the immediate, 
 * converts hex into int if necessary and sign extends 
 */
int imm_num(char* str) 
{
    char *imm = ltrim_spaces_1(str);
    if (imm[0] == '0' && imm[1] == 'x') {
        char *temp;
        long num = strtol(imm, &temp, 16);
        return sign_extend(num);
    }

    return sign_extend(atoi(str));
}

void step(char *instruction)
{
    // Extracts and returns the substring before the first space character,
    // by replacing the space character with a null-terminator.
    // `instruction` now points to the next character after the space
    // See `man strsep` for how this library function works
    char *op = strsep(&instruction, " ");
    // Uses the provided helper function to determine the type of instruction
    int op_type = get_op_type(op); 
    char *sliced1 = strsep(&instruction, ",");
    int rd = reg_num(sliced1);

    if (op_type == UNKNOWN_TYPE)
    {
        return;
    }
    if (rd == 0) // cannot change value of 0 register 
    {
        return;
    }
    if (op_type == I_TYPE) {
        char *sliced2 = strsep(&instruction, ","); // reg 1
        int r1 = reg_num(sliced2);
        int imm = imm_num(instruction);
        int val = registers->r[r1];
        if (strcmp(op, "addi") == 0) {
            registers->r[rd] = val + imm;
        }
        else if (strcmp(op, "andi") == 0) {
            registers->r[rd] = val & imm;
        }
    }
    else if (op_type == R_TYPE) {
        char *sliced2 = strsep(&instruction, ",");
        int r1 = reg_num(sliced2);
        int r2 = reg_num(instruction); 
        int val1 = registers->r[r1];
        int val2 = registers->r[r2];
        if (strcmp(op, "add") == 0) {
            registers->r[rd] = val1 + val2;
        }
        else if (strcmp(op, "sub") == 0) {
            registers->r[rd] = val1 - val2;
        }  
        else if (strcmp(op, "and") == 0) {
            registers->r[rd] = val1 & val2;
        }  
        else if (strcmp(op, "or") == 0) {
            registers->r[rd] = val1 | val2;
        } 
        else if (strcmp(op, "xor") == 0) {
            registers->r[rd] = val1 ^ val2;
        } 
        else if (strcmp(op, "sll") == 0) {
            registers->r[rd] = val1 << val2;
        } 
        else if (strcmp(op, "sra") == 0) {
            registers->r[rd] = val1 >> val2;
        } 
        else if (strcmp(op, "slt") == 0) {
            int num;
            if (val1 < val2) {
                num = 1;
            }
            else {
                num = 0;
            }
            registers->r[rd] = num;
        }
    }
    else if (op_type == U_TYPE) {
        int imm = (int)(strtol(instruction,NULL,0));
        if (imm > 0x7ffff)
        {
            imm = imm - 0x100000;
        }
        registers->r[rd] = imm << 12;
    }
    else if (op_type == MEM_TYPE) {
        char *sliced2 = strsep(&instruction, "(");
        char *sliced3 = strsep(&instruction, ")");

        int imm = imm_num(sliced2); // immediate to add 
        int r1 = reg_num(sliced3); // x0
        int r1_val = registers->r[r1]; // value inside x0
        int mem_key = imm + r1_val; // INDEX of where to store 

        if (strcmp(op, "sw") == 0) {
            int val = registers->r[rd];
            ht_add(memory, mem_key + 3, (val & 0xff000000) >> 24);
            ht_add(memory, mem_key + 2, (val & 0x00ff0000) >> 16);
            ht_add(memory, mem_key + 1, (val & 0x0000ff00) >> 8);
            ht_add(memory, mem_key, val & 0x000000ff);
        }
        else if (strcmp(op, "lw") == 0) {
            int b1 = ht_get(memory, mem_key + 3) << 24;
            int b2 = ht_get(memory, mem_key + 2) << 16;
            int b3 = ht_get(memory, mem_key + 1) << 8;
            int b4 = ht_get(memory, mem_key);
            int word = b1 | b2 | b3 | b4;
            registers->r[rd] = word;
        }
        if (strcmp(op, "sb") == 0) {
            int val = registers->r[rd]; // change this???
            ht_add(memory, mem_key, val & 0xff);
        }
        else if (strcmp(op, "lb") == 0) {
            int val = ht_get(memory, mem_key);
            if (val > 0x7f) {
                val = (val << 24) >> 24;
            }
            registers->r[rd] =val;
        }
    }
}

    // TODO: write logic for evaluating instruction on current interpreter state

