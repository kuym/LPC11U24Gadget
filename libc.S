.text
.align 0
.force_thumb
.syntax unified
.global __gnu_thumb1_case_uhi
.type __gnu_thumb1_case_uhi, function
__gnu_thumb1_case_uhi:
push	{r0, r1}
mov	r1, lr
lsrs	r1, r1, #1
lsls	r0, r0, #1
lsls	r1, r1, #1
ldrh	r1, [r1, r0]
lsls	r1, r1, #1
add	lr, lr, r1
pop	{r0, r1}
bx	lr
