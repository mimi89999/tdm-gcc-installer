; -- setup_full_tdm64.nsi --
; Created: JohnE, 2010-05-31


; DISCLAIMER:
; The author(s) of this file's contents release it into the public domain,
; without express or implied warranty of any kind. You may use, modify, and
; redistribute this file freely.


!searchparse /file GCC-VERSION.txt "" TDM_GCC_VER

!define INNER_COMPONENTS "inner-manifest-tdm64.txt"
!define INNER_COMPONENTS_SYS "tdm64"
OutFile "output\tdm64-gcc-${TDM_GCC_VER}.exe"
!packhdr "exehead.tmp" 'upx --best exehead.tmp'

!include "main.nsh"