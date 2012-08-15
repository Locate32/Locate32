/* functions for getting local drives and drive type



.text

/* void getlocaldrives(char* drives);
 *  gets local drives, drives is array of boolean values
 for example if drive C is local drives[2] is not zero
 */
.globl _getlocaldrives
	.align 4
_getlocaldrives:
	pushl %ebp
	movl %esp,%ebp
	pushl %eax
	pushl %ebx
	pushl %edx
	pushl %edi
	movl 8(%ebp),%edi
	movl $0x1,%ebx
loopstart:
	movl $0x4409,%eax	// testing if remote
	int $0x21
	jc nolocal
	test $0x1000,%edx
	jne nolocal
	movl $0x4408,%eax // testing if removable
	int $0x21
	jc nolocal
	cmp $0,%eax
	je nolocal	
local:
	movb $1,0(%edi)
	jmp loopcontinue;
nolocal:
	movb $0,0(%edi)
loopcontinue:
	incl %edi
	incl %ebx
	cmp $28,%ebx
	jb loopstart	
	popl %edi
	popl %edx
	popl %ebx
	popl %eax
	movl %ebp,%esp
	popl %ebp
	ret                           /* end of getlocaldrives() */

/* BYTE getdrivetype(char* root);
 *  gets drive type
 *  return values
 		0x00 = Unknown
		0x10 = Fixed
		0x20 = Removable (ex. Floppy)
		0x30 = CD-Rom
		0x40 = Remote (network)
		0x50 = Ramdisk
*/
 
.globl _getdrivetype
	.align 4
_getdrivetype:
	pushl %ebp
	movl %esp,%ebp
	
	pushl %ebx 
	pushl %ecx
	pushl %edx

	movl 8(%ebp),%ebx // root to ebx
	xorl %edx,%edx
	movb 0(%ebx),%dl  // root[0] to edx
	cmp $'a',%dl
	jb dtdriveuppercase
	subb $'a',%dl
	jmp dtdrivecontinue
dtdriveuppercase:
	subb $'A',%dl	
dtdrivecontinue:
	
	// testing whether drive is CDROM
	xorl %ebx,%ebx
	movl $0x150B,%eax
	movl %edx,%ecx	// now 1 = A:...
	int $0x2f
	cmp $0xadad,%ebx  // whether mscdex is installed
	jne dtcdromcontinue
	cmp $0,%eax		// drive is CDROM if eax is non zero
	jne dtcdrom
	
dtcdromcontinue:
	movl %edx,%ebx
	movl $0x4409,%eax // Checking remote
	incl %ebx	
	int $0x21	// now 1 = A:...
	jc dtnodrive
	test $0x1000,%edx
	jc dtremote // drive is remote
	
	movl $0x4408,%eax // is removable
	int $0x21
	jc dtnodrive
	cmp $0,%eax		// if eax=0, drive is removable
	je dtremovable
	
	// drive is fixed
	movl $0x10,%eax
	jmp dtend
	
dtcdrom:					
	movl $0x30,%eax
	jmp dtend
	
dtremovable:
	movl $0x20,%eax	
	jmp dtend
	
dtnodrive:
	xorl %eax,%eax
	jmp dtend
	
dtremote:
	movl $0x40,%eax
	jmp dtend
		
dtend:			
	popl %edx
	popl %ecx
	popl %ebx
	
	movl %ebp,%esp
	popl %ebp
	ret                           /* end of getdrivetype */

/* BOOL getvolumeinfo(char* root,DWORD* dwSerial,char* szLabel,char* szFS);
 *  returns volume serial
*/
 
.globl _getvolumeinfo
	.align 4
_getvolumeinfo:
	pushl %ebp
	movl %esp,%ebp 
	subl $100,%esp
	
	pushl %ebx
	pushl %edx
	pushl %esi
	pushl %edi
	
	movl 8(%ebp),%edx // root to ebx
	xorl %ebx,%ebx
	movb 0(%edx),%bl  // root[0] to ebx
	cmp $'a',%bl
	jb didriveuppercase
	subb $'a',%bl
	jmp didrivecontinue
didriveuppercase:
	subb $'A',%bl	
didrivecontinue:
	incl %ebx	             
	movl $0x6900,%eax
	movl %ebp,%edx
	subl $100,%edx
	int $0x21
	jc dierror
	         
//  setting	label to szLabel
	movl %ebp,%esi
	subl $94,%esi
	movl 16(%ebp),%edi
    movl $11,%ecx
    rep
    movsb
dilabelloop:
	cmpb $' ',-1(%edi)
	jne dilabelloopend
	decl %edi
	jmp dilabelloop
dilabelloopend:
	movl $0,(%edi)
                  
// setting fs to szFS
	movl %ebp,%esi
	subl $83,%esi
	movl 20(%ebp),%edi
    movl $8,%ecx
    rep
    movsb
difsloop:
	cmpb $' ',-1(%edi)
	jne difsloopend
	decl %edi
	jmp difsloop
difsloopend:
	movl $0,(%edi)
                  
// 	setting serial to dwSerial
	movl -98(%ebp),%eax
	movl 12(%ebp),%edi
	movl %eax,(%edi)
	
	movl $1,%eax
	jmp diend
	
dierror:
	movl $0,%eax

diend:
	popl %edi
    popl %esi
	popl %edx
    popl %ebx
    
	movl %ebp,%esp
	popl %ebp
	ret                           /* end of getvolumeinfo*/
