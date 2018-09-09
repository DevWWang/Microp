;ECSE 426 	LAB-1			Group18
;Description:	Testbench for Assembly file
;Author: Chuan Qin, chuan.qin2@mail.mcgill.ca
;				 Wei Wang, wei.wang18@mail.mcgill.ca
;Date: 2016-02-05

	AREA testa, CODE, READONLY
		EXPORT test
		IMPORT filteredArray
		IMPORT kfstate
		IMPORT Kalmanfilter
test
	PUSH {R14}								;store the original link address
	ADR	R0, inputArray 				;pointer to the current element in input array, starting at the first element
	LDR	R1, =filteredArray 		;pointer to the current elment in filtered data array, starting at the first element
	LDR	R2, length 						;load the lengh value of array into register
	LDR	R3, =kfstate					;pointer to the current element of kalman filter state
	PUSH {R0, R1, R2, R3, R5}	;put all 4 original parameters into stack
  LDR	R5, =Kalmanfilter
  BLX	R5										;link to the Assembly.s file
	
	;return
	POP {R0, R1, R2, R3, R5, R14}	;pop 4 original paramters, the assembly file address, and the original link address from the stack
	BX	LR
			
				
	ALIGN

;inputArray	DCFS	0.001, 0.001, 0.003, 0.004, 0.002
inputArray	DCFS	-0.665365, -0.329988, 0.164465, 0.043962, 0.295885
length	DCD	5

	END