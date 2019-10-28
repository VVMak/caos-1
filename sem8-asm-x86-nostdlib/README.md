```python
# make magics here. Look at previous notebooks to see readable version
exec('\nget_ipython().run_cell_magic(\'javascript\', \'\', \'// setup cpp code highlighting\\nIPython.CodeCell.options_default.highlight_modes["text/x-c++src"] = {\\\'reg\\\':[/^%%cpp/]} ;\')\n\n# creating magics\nfrom IPython.core.magic import register_cell_magic, register_line_magic\nfrom IPython.display import display, Markdown\n\n@register_cell_magic\ndef save_file(fname, cell):\n    cell = cell if cell[-1] == \'\\n\' else cell + "\\n"\n    cmds = []\n    with open(fname, "w") as f:\n        for line in cell.split("\\n"):\n            if line.startswith("%"):\n                run_prefix = "%run "\n                assert line.startswith(run_prefix)\n                cmds.append(line[len(run_prefix):].strip())\n            else:\n                f.write(line + "\\n")\n    for cmd in cmds:\n        display(Markdown("Run: `%s`" % cmd))\n        get_ipython().system(cmd)\n\n@register_cell_magic\ndef cpp(fname, cell):\n    save_file(fname, cell)\n\n@register_cell_magic\ndef asm(fname, cell):\n    save_file(fname, cell)\n    \n@register_cell_magic\ndef makefile(fname, cell):\n    assert not fname\n    save_file("makefile", cell.replace(" " * 4, "\\t"))\n        \n@register_line_magic\ndef p(line):\n    print("{} = {}".format(line, eval(line)))\n')
```


    <IPython.core.display.Javascript object>


# Жизнь без стандартной библиотеки

Что это значит? Значит, что функции взаимодействия с внещним миром (чтение, запись файлов и т. д.) будут реализованы в самом бинаре программы. Возможно вы даже лично напишите их код.



## Компилим как обычно


```python
%%cpp main.c
%run gcc -m32 -masm=intel -fno-asynchronous-unwind-tables -O3 main.c -S -o main.S
%run gcc -m32 -masm=intel -O3 main.c -o main.exe
%run ls -la main.exe
%run ldd main.exe  # Выводим зависимости по динамическим библиотекам
%run cat main.S
%run objdump -M intel -d main.exe

int main() {
    return 0;
}
```


Run: `gcc -m32 -masm=intel -fno-asynchronous-unwind-tables -O3 main.c -S -o main.S`



Run: `gcc -m32 -masm=intel -O3 main.c -o main.exe`



Run: `ls -la main.exe`


    -rwxrwxr-x 1 pechatnov pechatnov 7312 Oct 22 22:50 main.exe



Run: `ldd main.exe  # Выводим зависимости по библиотекам`


    	linux-gate.so.1 =>  (0xf7f2e000)
    	libc.so.6 => /lib/i386-linux-gnu/libc.so.6 (0xf7d52000)
    	/lib/ld-linux.so.2 (0xf7f30000)



Run: `cat main.S`


    	.file	"main.c"
    	.intel_syntax noprefix
    	.section	.text.unlikely,"ax",@progbits
    .LCOLDB0:
    	.section	.text.startup,"ax",@progbits
    .LHOTB0:
    	.p2align 4,,15
    	.globl	main
    	.type	main, @function
    main:
    	xor	eax, eax
    	ret
    	.size	main, .-main
    	.section	.text.unlikely
    .LCOLDE0:
    	.section	.text.startup
    .LHOTE0:
    	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.10) 5.4.0 20160609"
    	.section	.note.GNU-stack,"",@progbits



Run: `objdump -M intel -d main.exe`


    
    main.exe:     file format elf32-i386
    
    
    Disassembly of section .init:
    
    0804828c <_init>:
     804828c:	53                   	push   ebx
     804828d:	83 ec 08             	sub    esp,0x8
     8048290:	e8 7b 00 00 00       	call   8048310 <__x86.get_pc_thunk.bx>
     8048295:	81 c3 6b 1d 00 00    	add    ebx,0x1d6b
     804829b:	8b 83 fc ff ff ff    	mov    eax,DWORD PTR [ebx-0x4]
     80482a1:	85 c0                	test   eax,eax
     80482a3:	74 05                	je     80482aa <_init+0x1e>
     80482a5:	e8 26 00 00 00       	call   80482d0 <__libc_start_main@plt+0x10>
     80482aa:	83 c4 08             	add    esp,0x8
     80482ad:	5b                   	pop    ebx
     80482ae:	c3                   	ret    
    
    Disassembly of section .plt:
    
    080482b0 <__libc_start_main@plt-0x10>:
     80482b0:	ff 35 04 a0 04 08    	push   DWORD PTR ds:0x804a004
     80482b6:	ff 25 08 a0 04 08    	jmp    DWORD PTR ds:0x804a008
     80482bc:	00 00                	add    BYTE PTR [eax],al
    	...
    
    080482c0 <__libc_start_main@plt>:
     80482c0:	ff 25 0c a0 04 08    	jmp    DWORD PTR ds:0x804a00c
     80482c6:	68 00 00 00 00       	push   0x0
     80482cb:	e9 e0 ff ff ff       	jmp    80482b0 <_init+0x24>
    
    Disassembly of section .plt.got:
    
    080482d0 <.plt.got>:
     80482d0:	ff 25 fc 9f 04 08    	jmp    DWORD PTR ds:0x8049ffc
     80482d6:	66 90                	xchg   ax,ax
    
    Disassembly of section .text:
    
    080482e0 <main>:
     80482e0:	31 c0                	xor    eax,eax
     80482e2:	c3                   	ret    
    
    080482e3 <_start>:
     80482e3:	31 ed                	xor    ebp,ebp
     80482e5:	5e                   	pop    esi
     80482e6:	89 e1                	mov    ecx,esp
     80482e8:	83 e4 f0             	and    esp,0xfffffff0
     80482eb:	50                   	push   eax
     80482ec:	54                   	push   esp
     80482ed:	52                   	push   edx
     80482ee:	68 40 84 04 08       	push   0x8048440
     80482f3:	68 e0 83 04 08       	push   0x80483e0
     80482f8:	51                   	push   ecx
     80482f9:	56                   	push   esi
     80482fa:	68 e0 82 04 08       	push   0x80482e0
     80482ff:	e8 bc ff ff ff       	call   80482c0 <__libc_start_main@plt>
     8048304:	f4                   	hlt    
     8048305:	66 90                	xchg   ax,ax
     8048307:	66 90                	xchg   ax,ax
     8048309:	66 90                	xchg   ax,ax
     804830b:	66 90                	xchg   ax,ax
     804830d:	66 90                	xchg   ax,ax
     804830f:	90                   	nop
    
    08048310 <__x86.get_pc_thunk.bx>:
     8048310:	8b 1c 24             	mov    ebx,DWORD PTR [esp]
     8048313:	c3                   	ret    
     8048314:	66 90                	xchg   ax,ax
     8048316:	66 90                	xchg   ax,ax
     8048318:	66 90                	xchg   ax,ax
     804831a:	66 90                	xchg   ax,ax
     804831c:	66 90                	xchg   ax,ax
     804831e:	66 90                	xchg   ax,ax
    
    08048320 <deregister_tm_clones>:
     8048320:	b8 1b a0 04 08       	mov    eax,0x804a01b
     8048325:	2d 18 a0 04 08       	sub    eax,0x804a018
     804832a:	83 f8 06             	cmp    eax,0x6
     804832d:	76 1a                	jbe    8048349 <deregister_tm_clones+0x29>
     804832f:	b8 00 00 00 00       	mov    eax,0x0
     8048334:	85 c0                	test   eax,eax
     8048336:	74 11                	je     8048349 <deregister_tm_clones+0x29>
     8048338:	55                   	push   ebp
     8048339:	89 e5                	mov    ebp,esp
     804833b:	83 ec 14             	sub    esp,0x14
     804833e:	68 18 a0 04 08       	push   0x804a018
     8048343:	ff d0                	call   eax
     8048345:	83 c4 10             	add    esp,0x10
     8048348:	c9                   	leave  
     8048349:	f3 c3                	repz ret 
     804834b:	90                   	nop
     804834c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    
    08048350 <register_tm_clones>:
     8048350:	b8 18 a0 04 08       	mov    eax,0x804a018
     8048355:	2d 18 a0 04 08       	sub    eax,0x804a018
     804835a:	c1 f8 02             	sar    eax,0x2
     804835d:	89 c2                	mov    edx,eax
     804835f:	c1 ea 1f             	shr    edx,0x1f
     8048362:	01 d0                	add    eax,edx
     8048364:	d1 f8                	sar    eax,1
     8048366:	74 1b                	je     8048383 <register_tm_clones+0x33>
     8048368:	ba 00 00 00 00       	mov    edx,0x0
     804836d:	85 d2                	test   edx,edx
     804836f:	74 12                	je     8048383 <register_tm_clones+0x33>
     8048371:	55                   	push   ebp
     8048372:	89 e5                	mov    ebp,esp
     8048374:	83 ec 10             	sub    esp,0x10
     8048377:	50                   	push   eax
     8048378:	68 18 a0 04 08       	push   0x804a018
     804837d:	ff d2                	call   edx
     804837f:	83 c4 10             	add    esp,0x10
     8048382:	c9                   	leave  
     8048383:	f3 c3                	repz ret 
     8048385:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
     8048389:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
    
    08048390 <__do_global_dtors_aux>:
     8048390:	80 3d 18 a0 04 08 00 	cmp    BYTE PTR ds:0x804a018,0x0
     8048397:	75 13                	jne    80483ac <__do_global_dtors_aux+0x1c>
     8048399:	55                   	push   ebp
     804839a:	89 e5                	mov    ebp,esp
     804839c:	83 ec 08             	sub    esp,0x8
     804839f:	e8 7c ff ff ff       	call   8048320 <deregister_tm_clones>
     80483a4:	c6 05 18 a0 04 08 01 	mov    BYTE PTR ds:0x804a018,0x1
     80483ab:	c9                   	leave  
     80483ac:	f3 c3                	repz ret 
     80483ae:	66 90                	xchg   ax,ax
    
    080483b0 <frame_dummy>:
     80483b0:	b8 10 9f 04 08       	mov    eax,0x8049f10
     80483b5:	8b 10                	mov    edx,DWORD PTR [eax]
     80483b7:	85 d2                	test   edx,edx
     80483b9:	75 05                	jne    80483c0 <frame_dummy+0x10>
     80483bb:	eb 93                	jmp    8048350 <register_tm_clones>
     80483bd:	8d 76 00             	lea    esi,[esi+0x0]
     80483c0:	ba 00 00 00 00       	mov    edx,0x0
     80483c5:	85 d2                	test   edx,edx
     80483c7:	74 f2                	je     80483bb <frame_dummy+0xb>
     80483c9:	55                   	push   ebp
     80483ca:	89 e5                	mov    ebp,esp
     80483cc:	83 ec 14             	sub    esp,0x14
     80483cf:	50                   	push   eax
     80483d0:	ff d2                	call   edx
     80483d2:	83 c4 10             	add    esp,0x10
     80483d5:	c9                   	leave  
     80483d6:	e9 75 ff ff ff       	jmp    8048350 <register_tm_clones>
     80483db:	66 90                	xchg   ax,ax
     80483dd:	66 90                	xchg   ax,ax
     80483df:	90                   	nop
    
    080483e0 <__libc_csu_init>:
     80483e0:	55                   	push   ebp
     80483e1:	57                   	push   edi
     80483e2:	56                   	push   esi
     80483e3:	53                   	push   ebx
     80483e4:	e8 27 ff ff ff       	call   8048310 <__x86.get_pc_thunk.bx>
     80483e9:	81 c3 17 1c 00 00    	add    ebx,0x1c17
     80483ef:	83 ec 0c             	sub    esp,0xc
     80483f2:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
     80483f6:	8d b3 0c ff ff ff    	lea    esi,[ebx-0xf4]
     80483fc:	e8 8b fe ff ff       	call   804828c <_init>
     8048401:	8d 83 08 ff ff ff    	lea    eax,[ebx-0xf8]
     8048407:	29 c6                	sub    esi,eax
     8048409:	c1 fe 02             	sar    esi,0x2
     804840c:	85 f6                	test   esi,esi
     804840e:	74 25                	je     8048435 <__libc_csu_init+0x55>
     8048410:	31 ff                	xor    edi,edi
     8048412:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
     8048418:	83 ec 04             	sub    esp,0x4
     804841b:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
     804841f:	ff 74 24 2c          	push   DWORD PTR [esp+0x2c]
     8048423:	55                   	push   ebp
     8048424:	ff 94 bb 08 ff ff ff 	call   DWORD PTR [ebx+edi*4-0xf8]
     804842b:	83 c7 01             	add    edi,0x1
     804842e:	83 c4 10             	add    esp,0x10
     8048431:	39 f7                	cmp    edi,esi
     8048433:	75 e3                	jne    8048418 <__libc_csu_init+0x38>
     8048435:	83 c4 0c             	add    esp,0xc
     8048438:	5b                   	pop    ebx
     8048439:	5e                   	pop    esi
     804843a:	5f                   	pop    edi
     804843b:	5d                   	pop    ebp
     804843c:	c3                   	ret    
     804843d:	8d 76 00             	lea    esi,[esi+0x0]
    
    08048440 <__libc_csu_fini>:
     8048440:	f3 c3                	repz ret 
    
    Disassembly of section .fini:
    
    08048444 <_fini>:
     8048444:	53                   	push   ebx
     8048445:	83 ec 08             	sub    esp,0x8
     8048448:	e8 c3 fe ff ff       	call   8048310 <__x86.get_pc_thunk.bx>
     804844d:	81 c3 b3 1b 00 00    	add    ebx,0x1bb3
     8048453:	83 c4 08             	add    esp,0x8
     8048456:	5b                   	pop    ebx
     8048457:	c3                   	ret    


## Компилим, статически линкуя libc


```python
%%cpp main2.c
%run gcc -m32 -masm=intel -fno-asynchronous-unwind-tables -static -flto -O3  main2.c -S -o main2.S
%run gcc -m32 -masm=intel -static -flto -O3 main2.c -o main2.exe
%run ls -la main2.exe  # Заметьте, что размер стал сильно больше
%run ldd main2.exe
//%run objdump -M intel -d main2.exe
%run ./main2.exe

int main() {
    return 0;
}
```


Run: `gcc -m32 -masm=intel -fno-asynchronous-unwind-tables -static -flto -O3  main2.c -S -o main2.S`



Run: `gcc -m32 -masm=intel -static -flto -O3 main2.c -o main2.exe`



Run: `ls -la main2.exe  # Заметьте, что размер стал сильно больше`


    -rwxrwxr-x 1 pechatnov pechatnov 725164 Oct 23 09:24 main2.exe



Run: `ldd main2.exe`


    	not a dynamic executable



Run: `./main2.exe`



```python
!objdump -M intel -d main2.exe | grep -A 30 "_main>:"
#!objdump -M intel -d main2.exe | grep -A 30 "s80ea9f0"
```

    08048890 <generic_start_main>:
     8048890:	56                   	push   esi
     8048891:	53                   	push   ebx
     8048892:	83 ec 44             	sub    esp,0x44
     8048895:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
     8048899:	b8 00 00 00 00       	mov    eax,0x0
     804889e:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
     80488a2:	85 c0                	test   eax,eax
     80488a4:	89 4c 24 08          	mov    DWORD PTR [esp+0x8],ecx
     80488a8:	0f 84 1a 01 00 00    	je     80489c8 <generic_start_main+0x138>
     80488ae:	8b 0d 00 00 00 00    	mov    ecx,DWORD PTR ds:0x0
     80488b4:	31 c0                	xor    eax,eax
     80488b6:	85 c9                	test   ecx,ecx
     80488b8:	0f 94 c0             	sete   al
     80488bb:	a3 10 ad 0e 08       	mov    ds:0x80ead10,eax
     80488c0:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
     80488c4:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
     80488c8:	8b 54 24 5c          	mov    edx,DWORD PTR [esp+0x5c]
     80488cc:	8d 44 88 04          	lea    eax,[eax+ecx*4+0x4]
     80488d0:	89 15 c8 9f 0e 08    	mov    DWORD PTR ds:0x80e9fc8,edx
     80488d6:	a3 24 b5 0e 08       	mov    ds:0x80eb524,eax
     80488db:	90                   	nop
     80488dc:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
     80488e0:	83 c0 04             	add    eax,0x4
     80488e3:	8b 50 fc             	mov    edx,DWORD PTR [eax-0x4]
     80488e6:	85 d2                	test   edx,edx
     80488e8:	75 f6                	jne    80488e0 <generic_start_main+0x50>
     80488ea:	e8 c1 67 02 00       	call   806f0b0 <_dl_aux_init>
     80488ef:	a1 90 bc 0e 08       	mov    eax,ds:0x80ebc90
     80488f4:	85 c0                	test   eax,eax
     80488f6:	75 47                	jne    804893f <generic_start_main+0xaf>
    --
    08048b90 <__libc_start_main>:
     8048b90:	55                   	push   ebp
     8048b91:	57                   	push   edi
     8048b92:	31 c0                	xor    eax,eax
     8048b94:	56                   	push   esi
     8048b95:	53                   	push   ebx
     8048b96:	0f a2                	cpuid  
     8048b98:	83 ec 1c             	sub    esp,0x1c
     8048b9b:	81 fb 47 65 6e 75    	cmp    ebx,0x756e6547
     8048ba1:	a3 64 b5 0e 08       	mov    ds:0x80eb564,eax
     8048ba6:	0f 94 c0             	sete   al
     8048ba9:	81 f9 6e 74 65 6c    	cmp    ecx,0x6c65746e
     8048baf:	89 d7                	mov    edi,edx
     8048bb1:	89 c6                	mov    esi,eax
     8048bb3:	0f 94 c0             	sete   al
     8048bb6:	c7 04 24 00 00 00 00 	mov    DWORD PTR [esp],0x0
     8048bbd:	89 f2                	mov    edx,esi
     8048bbf:	c7 44 24 04 00 00 00 	mov    DWORD PTR [esp+0x4],0x0
     8048bc6:	00 
     8048bc7:	c7 44 24 08 00 00 00 	mov    DWORD PTR [esp+0x8],0x0
     8048bce:	00 
     8048bcf:	84 c2                	test   dl,al
     8048bd1:	74 0c                	je     8048bdf <__libc_start_main+0x4f>
     8048bd3:	81 ff 69 6e 65 49    	cmp    edi,0x49656e69
     8048bd9:	0f 84 6a 01 00 00    	je     8048d49 <__libc_start_main+0x1b9>
     8048bdf:	81 fb 41 75 74 68    	cmp    ebx,0x68747541
     8048be5:	0f 94 c3             	sete   bl
     8048be8:	81 f9 63 41 4d 44    	cmp    ecx,0x444d4163
     8048bee:	0f 94 c0             	sete   al
     8048bf1:	84 c3                	test   bl,al
     8048bf3:	74 0c                	je     8048c01 <__libc_start_main+0x71>


# Пишем сами без libc


```python
%%cpp minimal.c
%run gcc -m32 -masm=intel -nostdlib -O3 minimal.c -o minimal.exe
%run gcc -m32 -masm=intel -nostdlib -fno-asynchronous-unwind-tables -O3 minimal.c -S -o minimal.S
%run ls -la minimal.exe  # Заметьте, что размер стал очень маленьким :)
//%run ldd minimal.exe

%run cat minimal.S
//%run objdump -d minimal.exe

%run ./minimal.exe ; echo $? 

#include <sys/syscall.h>

    
// Универсальная функция для совершения системных вызовов
int syscall(int code, ...);
__asm__(R"(
syscall:
    push ebx
    push ebp
    push esi
    push edi
    mov eax, DWORD PTR [esp + 20] 
    mov ebx, DWORD PTR [esp + 24] 
    mov ecx, DWORD PTR [esp + 28] 
    mov edx, DWORD PTR [esp + 32]
    mov esi, DWORD PTR [esp + 36]
    mov edi, DWORD PTR [esp + 40]
    int 0x80
    pop edi
    pop esi
    pop ebp
    pop ebx
    ret
)");


void int_to_s(unsigned int i, char* s, int* len) {
    int clen = 0;
    for (int ic = i; ic; ic /= 10, ++clen);
    clen = clen ?: 1;
    s[clen] = '\0';
    for (int j = 0; j < clen; ++j, i /= 10) {
        s[clen - j - 1] = '0' + i % 10;
    }
    *len = clen;
}

unsigned int s_to_int(char* s) {
    unsigned int res = 0;
    while ('0' <= *s && *s <= '9') {
        res *= 10;
        res += *s - '0';
        ++s;
    }
    return res;
}

int print_int(int fd, unsigned int i) {
    char s[20];
    int len;
    int_to_s(i, s, &len);
    return syscall(SYS_write, fd, s, len);
}

int print_s(int fd, const char* s) {
    int len = 0;
    while (s[len]) ++len;
    return syscall(SYS_write, fd, s, len);
}


// Пример использования системного вызова для завершения работы программы
void _exit(int code);
__asm__(R"(
_exit:
    mov   eax, 1
    mov   ebx, [esp + 4]
    int   0x80
)");


const char hello_s[] = "Hello world from function 'write'!\n";
const int hello_s_size = sizeof(hello_s);

// Пример использования системного вызова для вывода в stdout
int write();
__asm__(R"(
write:
    push ebx
    mov eax, 4 
    mov ebx, 1
    lea ecx, [hello_s]
    mov edx, hello_s_size
    int 0x80
    pop ebx
    ret
)");


// Именно с этой функции всегда начинается выполнение программы
void _start() {
     const char hello_s_2[] = "Hello world from 'syscall'!\n";
     write();
     syscall(SYS_write, 1, hello_s_2, sizeof(hello_s_2));
     print_s(1, "Look at this value: "); print_int(1, 10050042); print_s(1, "\n");
     print_s(1, "Look at this value: "); print_int(1, s_to_int("123456")); print_s(1, "\n");
    
//    syscall(SYS_exit, 0);
    _exit(-1);
}

```


Run: `gcc -m32 -masm=intel -nostdlib -O3 minimal.c -o minimal.exe`



Run: `gcc -m32 -masm=intel -nostdlib -fno-asynchronous-unwind-tables -O3 minimal.c -S -o minimal.S`



Run: `ls -la minimal.exe  # Заметьте, что размер стал очень маленьким :)`


    -rwxrwxr-x 1 pechatnov pechatnov 2440 Oct 23 09:37 minimal.exe



Run: `cat minimal.S`


    	.file	"minimal.c"
    	.intel_syntax noprefix
    #APP
    	
    syscall:
        push ebx
        push ebp
        push esi
        push edi
        mov eax, DWORD PTR [esp + 20] 
        mov ebx, DWORD PTR [esp + 24] 
        mov ecx, DWORD PTR [esp + 28] 
        mov edx, DWORD PTR [esp + 32]
        mov esi, DWORD PTR [esp + 36]
        mov edi, DWORD PTR [esp + 40]
        int 0x80
        pop edi
        pop esi
        pop ebp
        pop ebx
        ret
    
    	
    _exit:
        mov   eax, 1
        mov   ebx, [esp + 4]
        int   0x80
    
    	
    write:
        push ebx
        mov eax, 4 
        mov ebx, 1
        lea ecx, [hello_s]
        mov edx, hello_s_size
        int 0x80
        pop ebx
        ret
    
    #NO_APP
    	.section	.text.unlikely,"ax",@progbits
    .LCOLDB0:
    	.text
    .LHOTB0:
    	.p2align 4,,15
    	.globl	int_to_s
    	.type	int_to_s, @function
    int_to_s:
    	push	ebp
    	push	edi
    	push	esi
    	push	ebx
    	mov	ecx, DWORD PTR [esp+20]
    	mov	edi, DWORD PTR [esp+24]
    	test	ecx, ecx
    	je	.L5
    	mov	ebx, ecx
    	xor	esi, esi
    	mov	ebp, 1717986919
    	.p2align 4,,10
    	.p2align 3
    .L3:
    	mov	eax, ebx
    	sar	ebx, 31
    	add	esi, 1
    	imul	ebp
    	sar	edx, 2
    	sub	edx, ebx
    	mov	ebx, edx
    	jne	.L3
    	mov	eax, esi
    .L2:
    	lea	ebx, [edi+esi]
    	mov	BYTE PTR [edi+eax], 0
    	mov	ebp, -858993459
    	.p2align 4,,10
    	.p2align 3
    .L4:
    	mov	eax, ecx
    	sub	ebx, 1
    	mul	ebp
    	shr	edx, 3
    	lea	eax, [edx+edx*4]
    	add	eax, eax
    	sub	ecx, eax
    	add	ecx, 48
    	mov	BYTE PTR [ebx], cl
    	cmp	ebx, edi
    	mov	ecx, edx
    	jne	.L4
    	mov	eax, DWORD PTR [esp+28]
    	mov	DWORD PTR [eax], esi
    	pop	ebx
    	pop	esi
    	pop	edi
    	pop	ebp
    	ret
    .L5:
    	mov	eax, 1
    	mov	esi, 1
    	jmp	.L2
    	.size	int_to_s, .-int_to_s
    	.section	.text.unlikely
    .LCOLDE0:
    	.text
    .LHOTE0:
    	.section	.text.unlikely
    .LCOLDB1:
    	.text
    .LHOTB1:
    	.p2align 4,,15
    	.globl	s_to_int
    	.type	s_to_int, @function
    s_to_int:
    	push	ebx
    	mov	ecx, DWORD PTR [esp+8]
    	movsx	edx, BYTE PTR [ecx]
    	lea	eax, [edx-48]
    	cmp	al, 9
    	mov	eax, 0
    	ja	.L11
    	.p2align 4,,10
    	.p2align 3
    .L12:
    	lea	eax, [eax+eax*4]
    	add	ecx, 1
    	lea	eax, [edx-48+eax*2]
    	movsx	edx, BYTE PTR [ecx]
    	lea	ebx, [edx-48]
    	cmp	bl, 9
    	jbe	.L12
    .L11:
    	pop	ebx
    	ret
    	.size	s_to_int, .-s_to_int
    	.section	.text.unlikely
    .LCOLDE1:
    	.text
    .LHOTE1:
    	.section	.text.unlikely
    .LCOLDB2:
    	.text
    .LHOTB2:
    	.p2align 4,,15
    	.globl	print_int
    	.type	print_int, @function
    print_int:
    	push	ebp
    	push	edi
    	push	esi
    	push	ebx
    	sub	esp, 44
    	mov	ecx, DWORD PTR [esp+68]
    	test	ecx, ecx
    	je	.L20
    	mov	ebx, ecx
    	xor	esi, esi
    	mov	edi, 1717986919
    	.p2align 4,,10
    	.p2align 3
    .L18:
    	mov	eax, ebx
    	sar	ebx, 31
    	add	esi, 1
    	imul	edi
    	sar	edx, 2
    	sub	edx, ebx
    	mov	ebx, edx
    	jne	.L18
    	mov	eax, esi
    .L17:
    	lea	edi, [esp+12]
    	mov	BYTE PTR [esp+12+eax], 0
    	mov	ebp, -858993459
    	lea	ebx, [edi+esi]
    	.p2align 4,,10
    	.p2align 3
    .L19:
    	mov	eax, ecx
    	sub	ebx, 1
    	mul	ebp
    	shr	edx, 3
    	lea	eax, [edx+edx*4]
    	add	eax, eax
    	sub	ecx, eax
    	add	ecx, 48
    	mov	BYTE PTR [ebx], cl
    	cmp	ebx, edi
    	mov	ecx, edx
    	jne	.L19
    	push	esi
    	push	ebx
    	push	DWORD PTR [esp+72]
    	push	4
    	call	syscall
    	add	esp, 60
    	pop	ebx
    	pop	esi
    	pop	edi
    	pop	ebp
    	ret
    .L20:
    	mov	eax, 1
    	mov	esi, 1
    	jmp	.L17
    	.size	print_int, .-print_int
    	.section	.text.unlikely
    .LCOLDE2:
    	.text
    .LHOTE2:
    	.section	.text.unlikely
    .LCOLDB3:
    	.text
    .LHOTB3:
    	.p2align 4,,15
    	.globl	print_s
    	.type	print_s, @function
    print_s:
    	sub	esp, 12
    	xor	eax, eax
    	mov	edx, DWORD PTR [esp+20]
    	cmp	BYTE PTR [edx], 0
    	je	.L25
    	.p2align 4,,10
    	.p2align 3
    .L26:
    	add	eax, 1
    	cmp	BYTE PTR [edx+eax], 0
    	jne	.L26
    .L25:
    	push	eax
    	push	edx
    	push	DWORD PTR [esp+24]
    	push	4
    	call	syscall
    	add	esp, 28
    	ret
    	.size	print_s, .-print_s
    	.section	.text.unlikely
    .LCOLDE3:
    	.text
    .LHOTE3:
    	.section	.rodata.str1.1,"aMS",@progbits,1
    .LC4:
    	.string	"123456"
    .LC5:
    	.string	"Look at this value: "
    .LC6:
    	.string	"\n"
    	.section	.text.unlikely
    .LCOLDB7:
    	.text
    .LHOTB7:
    	.p2align 4,,15
    	.globl	_start
    	.type	_start, @function
    _start:
    	push	ebx
    	sub	esp, 40
    	mov	DWORD PTR [esp+3], 1819043144
    	mov	DWORD PTR [esp+7], 1870078063
    	mov	DWORD PTR [esp+11], 543452274
    	mov	DWORD PTR [esp+15], 1836020326
    	mov	DWORD PTR [esp+19], 2037589792
    	mov	DWORD PTR [esp+23], 1818321779
    	mov	DWORD PTR [esp+27], 169944940
    	mov	BYTE PTR [esp+31], 0
    	call	write
    	push	29
    	lea	eax, [esp+7]
    	push	eax
    	push	1
    	push	4
    	call	syscall
    	pop	ecx
    	pop	ebx
    	push	OFFSET FLAT:.LC5
    	push	1
    	call	print_s
    	pop	eax
    	pop	edx
    	push	10050042
    	push	1
    	call	print_int
    	pop	ecx
    	pop	ebx
    	push	OFFSET FLAT:.LC6
    	push	1
    	call	print_s
    	pop	eax
    	pop	edx
    	push	OFFSET FLAT:.LC5
    	push	1
    	call	print_s
    	add	esp, 16
    	mov	ecx, OFFSET FLAT:.LC4
    	mov	eax, 49
    	xor	edx, edx
    	.p2align 4,,10
    	.p2align 3
    .L31:
    	lea	edx, [edx+edx*4]
    	add	ecx, 1
    	lea	edx, [eax-48+edx*2]
    	movsx	eax, BYTE PTR [ecx]
    	lea	ebx, [eax-48]
    	cmp	bl, 9
    	jbe	.L31
    	sub	esp, 8
    	push	edx
    	push	1
    	call	print_int
    	pop	eax
    	pop	edx
    	push	OFFSET FLAT:.LC6
    	push	1
    	call	print_s
    	mov	DWORD PTR [esp], -1
    	call	_exit
    	.size	_start, .-_start
    	.section	.text.unlikely
    .LCOLDE7:
    	.text
    .LHOTE7:
    	.globl	hello_s_size
    	.section	.rodata
    	.align 4
    	.type	hello_s_size, @object
    	.size	hello_s_size, 4
    hello_s_size:
    	.long	36
    	.globl	hello_s
    	.align 32
    	.type	hello_s, @object
    	.size	hello_s, 36
    hello_s:
    	.string	"Hello world from function 'write'!\n"
    	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.10) 5.4.0 20160609"
    	.section	.note.GNU-stack,"",@progbits



Run: `./minimal.exe ; echo $?`


    Hello world from function 'write'!
     Hello world from 'syscall'!
     Look at this value: 10050042
    Look at this value: 123456
    255



```python

```

# Смотрим на адреса различных переменных. Проверяем, что секции памяти расположены так, как мы ожидаем


```python
%%cpp look_at_addresses.c
%run gcc -m32 -masm=intel -O0 look_at_addresses.c -o look_at_addresses.exe
%run ./look_at_addresses.exe

#include <stdio.h>
#include <stdlib.h>

int func(int a) {
    return a;
}

int* func_s() {
    static int a;
    return &a;
}

int data[123] = {1, 2, 3};


int main2() {
   int local2 = 5;
   printf("Local 'local2' addr = %p\n", &local2); 
}


int main() {
    int local = 1;
    static int st = 2;
    int* all = malloc(12);
    
    printf("Func func addr = %p\n", (void*)func);
    printf("Func func_s addr = %p\n", (void*)func_s);
    printf("Global var addr = %p\n", data);
    
    printf("Static 'st' addr = %p\n", &st);
    printf("Static 'func_s.a' addr = %p\n", func_s());
    
    printf("Local 'local' addr = %p\n", &local);
    main2();
    
    printf("Heap 'all' addr = %p\n", all);
    
    return 0;
}
```


Run: `gcc -m32 -masm=intel -O0 look_at_addresses.c -o look_at_addresses.exe`



Run: `./look_at_addresses.exe`


    Func func addr = 0x804849b
    Func func_s addr = 0x80484a3
    Global var addr = 0x804a040
    Static 'st' addr = 0x804a22c
    Static 'func_s.a' addr = 0x804a234
    Local 'local' addr = 0xfffddd44
    Local 'local2' addr = 0xfffddd28
    Heap 'all' addr = 0x9868008


# Разбираемся в системным вызовом brk

`void *sbrk(intptr_t increment);`


```python
%%cpp minimal.c
%run gcc -m32 -masm=intel -nostdlib -O3 minimal.c -o minimal.exe
%run gcc -m32 -masm=intel -nostdlib -fno-asynchronous-unwind-tables -O3 minimal.c -S -o minimal.S

//%run cat minimal.S
//%run objdump -d minimal.exe

%run ./minimal.exe ; echo $? 

#include <sys/syscall.h>

    
// Универсальная функция для совершения системных вызовов
int syscall(int code, ...);
__asm__(R"(
syscall:
    push ebx
    push ebp
    push esi
    push edi
    mov eax, DWORD PTR [esp + 20] 
    mov ebx, DWORD PTR [esp + 24] 
    mov ecx, DWORD PTR [esp + 28] 
    mov edx, DWORD PTR [esp + 32]
    mov esi, DWORD PTR [esp + 36]
    mov edi, DWORD PTR [esp + 40]
    int 0x80
    pop edi
    pop esi
    pop ebp
    pop ebx
    ret
)");


void int_to_s(unsigned int i, char* s, int* len) {
    int clen = 0;
    for (int ic = i; ic; ic /= 10, ++clen);
    clen = clen ?: 1;
    s[clen] = '\0';
    for (int j = 0; j < clen; ++j, i /= 10) {
        s[clen - j - 1] = '0' + i % 10;
    }
    *len = clen;
}

unsigned int s_to_int(char* s) {
    unsigned int res = 0;
    while ('0' <= *s && *s <= '9') {
        res *= 10;
        res += *s - '0';
        ++s;
    }
    return res;
}

int print_int(int fd, unsigned int i) {
    char s[20];
    int len;
    int_to_s(i, s, &len);
    return syscall(SYS_write, fd, s, len);
}

int print_s(int fd, const char* s) {
    int len = 0;
    while (s[len]) ++len;
    return syscall(SYS_write, fd, s, len);
}


// Пример использования системного вызова для завершения работы программы
void _exit(int code);
__asm__(R"(
_exit:
    mov   eax, 1
    mov   ebx, [esp + 4]
    int   0x80
)");


const char hello_s[] = "Hello world from function 'write'!\n";
const int hello_s_size = sizeof(hello_s);

// Пример использования системного вызова для вывода в stdout
int write();
__asm__(R"(
write:
    push ebx
    mov eax, 4 
    mov ebx, 1
    lea ecx, [hello_s]
    mov edx, hello_s_size
    int 0x80
    pop ebx
    ret
)");


// Именно с этой функции всегда начинается выполнение программы
void _start() {
    const int size = 100 * 1000 * 1000;
    int* data_start = (void*)syscall(SYS_brk, 0);
    int* data_end = (void*)syscall(SYS_brk, (int)data_start + size);
    
    print_s(1, "Data begin: "); print_int(1, (int)(void*)data_start); print_s(1, "\n");
    print_s(1, "Data end: ");  print_int(1, (int)(void*)data_end); print_s(1, "\n");
    
    data_start[0] = 1;
    for (int i = 1; i < (data_end - data_start); ++i) {
        data_start[i] = data_start[i - 1] + 1;
        if (i % 10000000 == 0) {
            print_int(1, i); print_s(1, "\n");
        }
    }
    
    print_int(1, data_end[-1]); print_s(1, "\n");
    
    _exit(0);
}

```


Run: `gcc -m32 -masm=intel -nostdlib -O3 minimal.c -o minimal.exe`



Run: `gcc -m32 -masm=intel -nostdlib -fno-asynchronous-unwind-tables -O3 minimal.c -S -o minimal.S`



Run: `./minimal.exe ; echo $?`


    Data begin: 154443776
    Data end: 254443776
    10000000
    20000000
    25000000
    0



```python
hex(146067456)
```




    '0x8b4d000'




```python
hex(100500000)
```




    '0x5fd8220'




```python

```