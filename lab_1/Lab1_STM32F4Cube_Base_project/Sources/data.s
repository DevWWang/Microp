;ECSE 426		LAB-1 	Group18
;Description:	Data File for storage of Assembly function
;Author: Chuan Qin, chuan.qin2@mail.mcgill.ca
;				 Wei Wang, wei.wang18@mail.mcgill.ca
;Date: 2016-02-05

	AREA data, DATA, READWRITE
		EXPORT  filteredArray
		EXPORT	kfstate

;declare outputArray and kalman state		  
filteredArray DCFS	0.0, 0.0, 0.0, 0.0, 0.0
kfstate DCFS 0.1, 0.1, 0.0, 0.1, 0.0
	END