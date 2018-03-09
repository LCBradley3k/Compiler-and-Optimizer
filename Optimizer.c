/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

int main()
{
	Instruction *head;

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}
	/* YOUR CODE GOES HERE */
    //printf("HELLO!");
	if (head) {
        
        Instruction * instr = head;
        int writeCount = 0;

        while(instr){
            // set all instructions to not critical
            instr->critical = 'n';
            if(instr->opcode == WRITE){
                instr->critical = 'y';
                writeCount++;
            }
            if(instr->opcode == READ){
                instr->critical = 'y';
            }
            instr = instr->next;
        }


        // this is the main loop to go through each write statement
        while(writeCount != 0){
            int * importantRegs = (int*)malloc(10*sizeof(int));
            int * importantField1s = (int*)malloc(10*sizeof(int));
            int regUsed = 0;
            int field1Used = 0;
            int counter = writeCount;
            
            
            // get the primary register
            instr = head;

            // the variable written out
            char primaryVar;

            while(instr){
                if(instr->opcode == WRITE){
                    if(counter == 1){
                        // we've hit the correct WRITE statement (from bottom to top).
                        primaryVar = instr->field1;
                        //printf("THE PRIMARY VARIABLE IS %c\n", e
                        //rimaryVar);
                    }
                    counter--;
                }
                instr = instr->next;
            }

            // get the primary register
            //int primaryReg;
            instr = head;
            while(instr){
                if(instr->opcode == STORE && instr->field1 == primaryVar){
                    //primaryReg = instr->field2;
                    instr->critical = 'y';
                    importantRegs[regUsed] = instr->field2;
                    regUsed++;
                    //printf("The PRIMARY VARIABLE IS %c\n", primaryVar);
                }
                instr = instr->next;
            }

            // loop through the instructions backwards;
            instr = LastInstruction(head);
            int hasReachedCurrentWriteStmt = 0;
            //printf("THE LAST INSTRUCTION IS %c\n", instr->field1);
            while(instr){
                //PrintInstruction(stdout,instr);

                if(hasReachedCurrentWriteStmt != 0){
                    // run the algorithm 
                    //
                    //
                    // CASE 1 - 
                    int c2 = regUsed;
                    // loop through important regs
                    while(c2 != -1){
                        if(instr->opcode == ADD || instr->opcode  == MUL || instr->opcode == SUB){
                            if(instr->field1 == importantRegs[c2-1]){
                                // the first field is equal to one of the important Registers
                                // so we add the field2 and field3 to the importantRegs list
                                importantRegs[regUsed] = instr->field2;
                                regUsed++;
                                importantRegs[regUsed] = instr->field3;
                                regUsed++;

                                // see if the field 1 already exists, so we can remove duplicates
                                int c4 = field1Used;
                                int isExist = 0;
                                while(c4 != -1){
                                    if(instr->field1 == importantField1s[c4-1]){
                                        isExist = 1;
                                    }
                                    c4--;
                                }
                                if(isExist == 0){
                                    importantField1s[field1Used] = instr->field1;
                                    field1Used++;
                                    // only marked critical if the field1 does not yet exist in
                                    // importantField1s
                                    instr->critical = 'y';
                                }
                            }
                        }
                        if(instr->opcode == LOADI || instr->opcode == LOAD){
                            if(instr->field1 == importantRegs[c2-1]){
                                instr->critical = 'y';
                            }
                        }
                        c2--;
                    }
                
                } else {
                    // nothing matters yet, but we need to check if we're at correct write statement
                    if(instr->opcode == WRITE && instr->field1 == primaryVar){
                        hasReachedCurrentWriteStmt = 1;  
                    }
                }

                instr = instr->prev;
            }
 
            free(importantRegs);
            free(importantField1s);
            writeCount--;
        }



        // print out the instructions that are critical
        instr = head;
        while(instr){
            if(instr->critical == 'y'){
                PrintInstruction(stdout,instr);
            }
            instr = instr->next;
        }

        /*Instruction *instr = head;
        // Array of variables that come after any WRITE instruction
        char *writeVarArray = (char*)malloc(10*sizeof(char));
        // array of registers used in the STORE function
        char *regArray = (char*)malloc(10*sizeof(char));
        int arrayUsed = 0;
        int regUsed = 0;

        // first pass through - read and write set to critical, find important variables
        while(instr){
            PrintInstruction(stdout, instr);
            //printf("%c\n", instr->critical);
    
            // set each instruction critical to no
            instr->critical = 'n';

            if(instr->opcode == WRITE){
                instr->critical = 'y';
                writeVarArray[arrayUsed] = instr->field1;
                arrayUsed++;
                printf("%c\n", instr->field1);
            }

            if(instr->opcode == READ){
                instr->critical = 'y';
            }

            printf("%c\n", instr->critical);
            instr = instr->next;
        }   

        // second pass through - we know the important variables
        instr = head;
        while (instr){
            if(instr->opcode == STORE){
                // see if it is storing one of the values written out.
                int counter = arrayUsed;
                while (counter != 0){
                    // loop through all the eleends in writeVarArray
                    if(instr->field1 == writeVarArray[arrayUsed - 1]){
                        instr->critical = 'y';
                        // gets field2 and adds it to regArray
                        regArray[regUsed] = instr->field2;
                        regUsed++;
                        printf("field2 = %d\n", instr->field2);
                        printf("THIS STORES a\n");
                    }
                    counter --;
                }
            }

            //

            instr = instr->next;
        }

        // third pass through - we know the important variables and registers now. 
        instr = head;
        while(instr){

            if(instr->opcode == ADD || instr->opcode == MUL || instr->opcode == SUB){
                // check the registers of each one to see if the important register is found
                int counter = regUsed;
                while(counter!= 0){
                    if(instr->field1 == regArray[counter] || instr->field2 == regArray[counter] || instr->field3 == regArray[counter]){
                        // the important register was found
                        // does it exists in the list yet
                        int 1exists = 0;
                        int 2exists = 0;
                        int 3exists = 0;
                        int c2 = regUsed;
                        while(c2 != 0){
                            // check all 3 fields to see if they exists in the regArray array ( to avoid dupliates )
                            if(instr->field1 == regArray[c2]){ 1exists = 1 }
                            if(instr->field2 == regArray[c2]){ 2exists = 1 }
                            if(instr->field3 == regArray[c2]){ 3exists = 1 }
                            c2--;
                        }    
                        // add register if it doesn't exist
                        if(1exists == 0) { regArray[regUsed] = instr->field1 }
                        if(2exists == 0) { regArray[regUsed] = instr->field2 }
                        if(3exists == 0) { regArray[regUsed] = instr->field3 }
                    }
                    counter --;
                }
            }

            instr = instr->next;
        }

        // fourth pass thorugh - we now know all the important registers
        instr = head;
        while(instr){
            

            instr = instr->next;
        }
    


        // loops through instructions, first parameter is File, second is Instruction
		PrintInstructionList(stdout, head);

        // we have to go through each instruction and tag it as critical or not
        Instruction * instr = head;
        do{
            
        }
        while (instr != head);*/

        // test to tag one item
        
        

		// frees up memory from saved instructions
        DestroyInstructionList(head);
	}
	return EXIT_SUCCESS;
}

