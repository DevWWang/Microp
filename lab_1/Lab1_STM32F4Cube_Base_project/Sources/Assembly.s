;ECSE 426 	LAB-1 	Group18
;Desecription:	Assembly Kalman Filter
;Author: Chuan Qin, chuan.qin2@mail.mcgill.ca
;				 Wei Wang, wei.wang18@mail.mcgill.ca
;Date: 2016-02-05

	AREA Assembly, CODE, READONLY
		EXPORT	Kalmanfilter
		IMPORT  filteredArray
		IMPORT	kfstate
Kalmanfilter
	;initialize kalman state
	VLDM.f32	R3, {S0, S1, S2, S3, S4}
update
	VLDR.f32	S31, [R0] 		;store current input value from the input array to register S31
	
	;kalman state update function according to the formulas
	;p
	VADD.f32	S3, S3, S0		;p = p + q
	;k
	VADD.f32	S30, S3, S1		;temp1 = p + r
	VDIV.f32	S4, S3, S30		;k = p / temp1
	;x
	VSUB.f32	S30, S31, S2 	;temp1 = measurement - x
	VMUL.f32	S30, S4, S30	;temp1 = k * temp1
	VADD.f32	S2, S2, S30		;x = x + temp1
	;p
	VMLS.f32	S3, S4, S3 		;(1 - k) * p = p - k * p
	
	VSTR.f32	S2, [R1]			;return value x
	
	ADD	R0, R0, #4					;move pointer to next element of input array
	ADD	R1, R1, #4					;move the pointer of output array to the next element
	SUB	R2, R2, #1					;decrease the counter value by 1 for each element, note that counter value is equal to the length of the array
	CMP	R2, #0							;compare the counter value with 0
	BNE	update							;exit the for loop when the counter = 0
	
	;store the kalman state
	VSTM.f32	R3, {S0, S1, S2, S3, S4}		;kalman state{q, r, x, p, k}

	VMRS	R0, FPSCR				;detect the flag for differen exceptions nad store the result of detection into first argumment
	AND	R0, #0x0F					;retun 0 if there is no error occurred
	
	BX	LR
	NOP

	END