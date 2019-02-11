/*
 * HW4.s
 *
 * Final Version
 *
 *  Created on: Apr 11, 2018
 *      Author: lcr0086
 */

       .data
       .type a, %object // making v which will hold array elements
       .size a,8 //v will have array size 8
       .type n, %object // making v which will hold array elements
       .size n,1 //v will have array size 8

n:     .xword 8

a:
       .float 17.0, 12.1, 3.2, 7.3, 31.4, 27.5, 5.6, 19.7 //placing 8 elements in v

       .text
       .global main
       .arch armv8-a+fp+simd
       .type main, %function
       .type average, %function
       .type sd, %function
main:
       ADRP X0, a //address of v
       ADD X0, X0, :lo12:a
       ADRP X1, n //address of n
       ADD X1, X1, :lo12:n

       LDUR X1, [X1, #0] // var n = 8

       FSUB S1, S1, S1 //var avg = 0
       FSUB S2, S2, S2 //var std_dev = 0

       BL average //calling average function

       BL sd  //calling standard dev. function

       B exit //exit program

average:
        MOV X3, X0 //copy address to X3
        SUB X2, X2, X2 //var i = 0
        FSUB S4, S4, S4 //var sum = 0.0
        loopSUM:
             LDUR S3, [X3, #0] //a[i]
             ADD X3, X3, #4 //move a to next element in array
             ADD X2, X2, #1 //i++
             FADD S4, S4, S3 //hold the sum of array
             CMP X1, X2 // i < n
             B.GT loopSUM
        SCVTF S5, X1 // 8 -> 8.0
        FDIV S1, S4, S5 // Return (Sum/n)

        BR X30


sd:
   MOV X3, X0 //copy address to X3
   SUB X2, X2, X2 // var i = 0
   FSUB S4, S4, S4 //var sum = 0.0
   FSUB S5, S5, S5 //Squared value
   FSUB S7, S7, S7 //sum + (a[i]-average) * (a[i]-average)/n)
   loopSumAvg:
        LDUR S3, [X3, #0] //a[i]
        ADD X3, X3, #4 //move a to next element in array
        ADD X2, X2, #1 //i++
        FSUB S5, S3, S1 //(a[i]-average)
        FSUB S5, S3, S1 //(a[i]-average)
        FMUL S5, S5, S5 //(a[i]-average) * (a[i]-average)
        FADD S4, S4, S5 //sum = sum + (a[i]-average) * (a[i]-average)
        CMP X1, X2 //i < n
        B.GT loopSumAvg
   SCVTF S6, X1 // 8 -> 8.0
   FDIV  S7, S4, S6//(sum + (a[i]-average) * (a[i]-average)/n)
   FSQRT S2, S7 //Return(Sqrt(sum + (a[i]-average) * (a[i]-average)/n))

   BR X30

exit:
