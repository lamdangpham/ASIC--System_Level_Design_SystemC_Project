;;  Returned value
;;         |  a<0  |  a>=0 |
;;  ------ +-------+-------+
;;   b<0   |  a-b  |   a   |
;;  ------ +-------+-------+
;;   b>=0  |   a   |  a-b  |
;;  ------ +-------+-------+

       .ORIG    x0000
cint    LD      R1,a
        BRn     aNeg

;; a>=0, if here
        LD      R2,b
        BRn     retA
        BR      cmp

;; a<0, if here
aNeg    LD      R2,b
        BRn     cmp
;;      BR      retA

;; a and b have different signs
retA    STI	R1, RESULT
        BR      leave

;; a and b have same sign
cmp     NOT     R2,R2
        ADD     R2,R2,#1
        ADD     R2,R1,R2
        STI	R2, RESULT

leave   HALT
a	.FILL	#15
b	.FILL	#6
RESULT	.FILL	#31
       .END