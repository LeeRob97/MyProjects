/*
 * HW1.s
 *
 * Final Version
 *
 *  Created on: Feb 21, 2018
 *      Author: lcr0086
 */
       .data
       .type v, %object // making v which will hold array elements
       .type n, %object // n will iterate
       .size v,8 //v will have array size 8
       .size n,1

n:
       .xword 8
v:
       .xword 79, 55, 94, 48, 19, 13, 45, 21 //placing 8 elements in v

       .text
       .global main
       .arch armv8-a+fp+simd
       .type main, %function
main:
       ADRP X0, v //address of v
       ADD X0, X0, :lo12:v
       ADRP X4, n //address of n
       ADD X4, X4, :lo12:n
       LDUR X3, [X4, #0]
       LDUR X2, [X0, #0]
       MOV X9, X2 //Setting X9 to X2
       MOV X10, X2 //Setting X10 to X2
       SUB X8, X8, X8 //setting to zero
       SUB X3, X3, X3 //setting to zero
loop:
       LDUR X2, [X0, #0]
       ADD X3, X3, #1 //increment array
       ADD X0, X0, #8 //move a byte
       ADD X8, X8, X2 //hold the sum of array
       CMP X2, X10 //comparing value X2 to X10
       B.LT small //Branch to small label
       CMP X2, X9 //compare X2 to X9
       B.GT large //Branch large label
       CMP X4, X3
       B.GE loop //loop again
Exit:            //exit

small:             //store smallest value in X10
       MOV X10, X2
       B loop      //return to loop
large:             //store largest value to X9
       MOV X9, X2
       B loop
