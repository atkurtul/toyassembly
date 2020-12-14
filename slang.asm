#code

main:
	mov r1, r0
	mov r0, 0x4
	call fact
	halt

fact:
	mul r1, r0
	inc r1
	mul	r0, r1
	div r1, 0x2
	ret

#data

x: w = 1024