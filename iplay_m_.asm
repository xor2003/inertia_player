; ---------------------------------------------------------------------------

;
;
; Input	MD5   :	3ADEF234086ACD44430B68CC52471B0D
; Input	CRC32 :	B294245C

; File Name   :	I:\IPLAY.EXE
; Format      :	MS-DOS executable (EXE)
; Base Address:	1000h Range: 10000h-35510h Loaded length: 16308h
; Entry	Point :	1905:42

;		.386
;		pmmx
;		.model large

; ===========================================================================

; Segment type:	Pure code
_text segment byte public 'CODE' use16
		assume cs:_text
		assume es:nothing, ss:nothing, ds:dseg,	fs:nothing, gs:nothing

; =============== S U B	R O U T	I N E =======================================


_moduleread proc far		; CODE XREF: _read_module+56P
					; DATA XREF: seg003:off_245C8o	...
		push	ds
		push	dx
		push	cs
		call	near ptr _snd_offx

loc_10006::
		push	cs
		call	near ptr _memfree_125DA
		pop	dx
		mov	ax, 3D00h
		int	21h		; DOS -	2+ - OPEN DISK FILE WITH HANDLE
					; DS:DX	-> ASCIZ filename
					; AL = access mode
					; 0 - read
		mov	bx, seg003
		mov	ds, bx
		assume ds:seg003
		mov	_savesp_245D0,	sp
		mov	_fhandle_module, ax
		mov	dx, offset _eModuleNotFound ; "Module not found\r\n"

loc_1001F::
		mov	ax, 0FFFFh
		jb	short _lfreaderr
		call	_ems_save_mapctx
		cld

loc_10028::
		mov	_byte_2461B, 0

loc_1002D::
		mov	_word_24662, 0

loc_10033::
		mov	dx, offset _chrin
		mov	cx, 1084
		call	_dosfread
		push	cs

loc_1003D::
		call	near ptr _clean_11C43

loc_10040::
		mov	bx, offset off_25326
		mov	dl, 23

loc_10045::				; CODE XREF: _moduleread+5Fj
		movzx	cx, byte ptr [bx+4]

loc_10049::
		mov	di, offset _chrin
		add	di, [bx+2]
		lea	si, [bx+5]

loc_10052::
		mov	ax, [bx]
		mov	bx, si
		add	bx, cx
		cld
		repe cmpsb
		jz	short loc_10064
		dec	dl
		jnz	short loc_10045
		mov	ax, offset _mod_n_t_module ; N.T.

loc_10064::				; CODE XREF: _moduleread+5Bj
		mov	_byte_24665, 1
		call	ax

loc_1006B::
		mov	bx, _fhandle_module
		mov	ah, 3Eh
		int	21h		; DOS -	2+ - CLOSE A FILE WITH HANDLE
					; BX = file handle
		adc	_word_24662, 0
		call	_ems_restore_mapctx

loc_1007B::
		movzx	ax, _byte_2461B

loc_10080::
		inc	ax
		cmp	ax, _mod_channels_number
		jbe	short loc_1008A
		mov	ax, _mod_channels_number

loc_1008A::				; CODE XREF: _moduleread+85j
		push	cs
		call	near ptr sub_12B83
		mov	si, offset _dword_27BC8
		push	cs

loc_10092::
		call	near ptr sub_12B18
		xor	ax, ax
		pop	ds
		retf
; ---------------------------------------------------------------------------

loc_10099::				; CODE XREF: _mod_n_t_module+6Dj
					; _mod_n_t_module+15Cj ...
		mov	dx, offset _aNotEnoughMemory ; "Not enough Memory available\r\n"
		mov	ax, 0FFFEh

_lfreaderr::				; CODE XREF: _moduleread+22j
					; _dosseek+18j ...
		push	ax
		push	dx
		mov	bx, _fhandle_module
		mov	ah, 3Eh
		int	21h		; DOS -	2+ - CLOSE A FILE WITH HANDLE
					; BX = file handle
		call	_ems_restore_mapctx
		push	cs
		call	near ptr _memfree_125DA
		mov	ax, ds
		mov	fs, ax
		assume fs:seg003
		pop	dx
		pop	ax
		mov	sp, _savesp_245D0
		pop	ds
		stc
		retf
_moduleread endp


; =============== S U B	R O U T	I N E =======================================

; N.T.

_mod_n_t_module proc near	; DATA XREF: _moduleread+61o
		mov	_module_type_text, 2E542E4Eh
		mov	_word_245D2, 0Fh
		mov	_mod_channels_number, 4
		mov	si, offset _byte_306DE
		call	_mod_1021E
		call	_mod_102F5
		mov	dx, 258h
		xor	cx, cx
		mov	bx, _fhandle_module
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file
		adc	_word_24662, 0
		jmp	loc_101B7
; ---------------------------------------------------------------------------

_mod_flt8_module::			; DATA XREF: seg003:0DDAo
		mov	_module_type_text, 38544C46h ;	FLT8
		mov	_moduleflag_246D0, 11b
		mov	_word_245D2, 1Fh
		mov	_mod_channels_number, 8
		mov	si, offset _byte_308BE
		call	_mod_1021E
		mov	si, offset _byte_27FE8
		mov	cx, 80h	; 'Ä'

loc_10118::				; CODE XREF: _mod_n_t_module+5Fj
		shr	byte ptr [si], 1
		inc	si
		dec	cx
		jnz	short loc_10118
		call	_mod_1024A
		call	_mod_102F5
		call	_mod_read_10311
		call	near ptr _mod_readfile_11F4E
		jb	loc_10099
		retn
; ---------------------------------------------------------------------------

_mod_tdz_module::				; DATA XREF: seg003:0E04o
		mov	al, _byte_30943
		jmp	short loc_10137
; ---------------------------------------------------------------------------

_mod_chn_module::				; DATA XREF: seg003:0DF5o
		mov	al, _byte_30940

loc_10137::				; CODE XREF: _mod_n_t_module+75j
		xor	ah, ah
		inc	_word_24662
		sub	al, 30h	; '0'
		jbe	short locret_10154
		cmp	al, 9
		ja	short locret_10154
		dec	_word_24662
		mov	_word_245D2, 1Fh
		mov	_mod_channels_number, ax

loc_10152::
		jmp	short loc_101A6
; ---------------------------------------------------------------------------

locret_10154::				; CODE XREF: _mod_n_t_module+82j
					; _mod_n_t_module+86j ...
		retn
; ---------------------------------------------------------------------------

_mod_ch_module::				; DATA XREF: seg003:0DFDo
		inc	_word_24662
		movzx	ax, _byte_30940
		sub	al, '0'
		jb	short locret_10154
		cmp	al, 9
		ja	short locret_10154
		imul	dx, ax,	10
		mov	al, byte ptr [unk_30941]
		sub	al, '0'
		jb	short locret_10154
		cmp	al, 9
		ja	short locret_10154
		add	ax, dx
		jz	short locret_10154
		cmp	ax, ' '
		ja	short locret_10154
		dec	_word_24662
		mov	_word_245D2, 1Fh
		mov	_mod_channels_number, ax
		jmp	short loc_101A6
; ---------------------------------------------------------------------------

_mod_cd81_module::			; DATA XREF: seg003:0DE3o seg003:0DECo
		mov	_word_245D2, 1Fh
		mov	_mod_channels_number, 8
		jmp	short loc_101A6
; ---------------------------------------------------------------------------

_mod_mk_module::				; DATA XREF: seg003:0D9Bo seg003:0DA4o ...
		mov	_word_245D2, 1Fh
		mov	_mod_channels_number, 4

loc_101A6::				; CODE XREF: _mod_n_t_module:loc_10152j
					; _mod_n_t_module+CDj ...
		mov	eax, [dword ptr	_byte_30940]
		mov	_module_type_text, eax
		mov	si, offset _byte_308BE
		call	_mod_1021E
		call	_mod_102F5

loc_101B7::				; CODE XREF: _mod_n_t_module+31j
		call	_mod_1024A
		cmp	_module_type_text, 2E4B2E4Dh ;	M.K.
		jnz	short loc_10213
		xor	dx, dx
		xor	cx, cx
		mov	bx, _fhandle_module
		mov	ax, 4202h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from end of file
		shl	edx, 10h
		mov	dx, ax
		push	edx
		mov	dx, 1084
		xor	cx, cx
		mov	bx, _fhandle_module
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file
		pop	edx
		movzx	eax, _word_245F2
		shl	eax, 0Bh

loc_101F4::
		add	eax, _dword_245C4
		add	eax, 1084
		cmp	eax, edx
		jnz	short loc_10213
		mov	_mod_channels_number, 8
		mov	_module_type_text, 20574F57h ;	WOW

loc_10213::				; CODE XREF: _mod_n_t_module+106j
					; _mod_n_t_module+145j
		call	_mod_read_10311
		call	near ptr _mod_readfile_11F4E
		jb	loc_10099
		retn
_mod_n_t_module endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_mod_1021E proc near		; CODE XREF: _mod_n_t_module+18p
					; _mod_n_t_module+52p ...
		mov	ax, ds
		mov	es, ax
		assume es:seg003
		cld
		lodsb
		xor	ah, ah
		mov	_word_245FA, ax
		lodsb
		cmp	al, 78h	; 'x'
		jb	short loc_10230
		xor	al, al

loc_10230::				; CODE XREF: _mod_1021E+Ej
		mov	_word_245F8, ax
		mov	di, offset _byte_27FE8
		mov	cx, 20h	; ' '
		cld
		rep movsd
		mov	si, offset _chrin ; in
		mov	di, offset asc_246B0 ; out
		mov	cx, 14h		; count
		call	_copy_printable
		retn
_mod_1021E endp


; =============== S U B	R O U T	I N E =======================================


_mod_1024A proc near		; CODE XREF: _mod_n_t_module+61p
					; _mod_n_t_module:loc_101B7p
		mov	si, offset _chrin
		mov	di, offset _myout ; out
		mov	cx, _word_245D2

loc_10254::				; CODE XREF: _mod_1024A+A6j
		push	cx
		add	si, 20		; in
		mov	cx, 16h		; count
		call	_copy_printable
		sub	si, 20
		pop	cx
		movzx	edx, word ptr [si+2Ah]
		xchg	dl, dh
		shl	edx, 1
		cmp	edx, 100000h
		cmc
		adc	_word_24662, 0
		mov	[di+20h], edx
		add	_dword_245C4, edx
		mov	al, [si+2Ch]
		and	al, 0Fh
		mov	[di+3Eh], al
		mov	ax, _freq_245DE
		mov	[di+36h], ax
		mov	al, [si+2Dh]
		mov	[di+3Dh], al
		movzx	ebx, word ptr [si+2Eh]
		xchg	bl, bh
		shl	ebx, 1
		movzx	eax, word ptr [si+30h]
		xchg	al, ah
		shl	eax, 1
		mov	[di+28h], eax
		cmp	eax, 2
		jbe	short loc_102DC
		cmp	ebx, edx
		jb	short loc_102C1
		shr	ebx, 1
		cmp	ebx, edx
		jnb	short loc_102DC

loc_102C1::				; CODE XREF: _mod_1024A+6Dj
		or	byte ptr [di+3Ch], 8
		add	eax, ebx
		cmp	eax, edx
		jbe	short loc_102DF
		mov	eax, [di+28h]
		shr	eax, 1
		add	eax, ebx
		cmp	eax, edx
		jbe	short loc_102DF

loc_102DC::				; CODE XREF: _mod_1024A+68j
					; _mod_1024A+75j
		mov	eax, edx

loc_102DF::				; CODE XREF: _mod_1024A+81j
					; _mod_1024A+90j
		dec	eax
		mov	[di+2Ch], eax
		mov	[di+24h], ebx
		add	si, 1Eh
		add	di, 40h	; '@'
		dec	cx
		jnz	loc_10254
		retn
_mod_1024A endp


; =============== S U B	R O U T	I N E =======================================


_mod_102F5 proc near		; CODE XREF: _mod_n_t_module+1Bp
					; _mod_n_t_module+64p ...
		mov	si, offset _byte_27FE8
		xor	bx, bx
		mov	cx, 80h	; 'Ä'
		cld

loc_102FE::				; CODE XREF: _mod_102F5+13j
		lodsb
		and	al, 7Fh
		cmp	al, bl
		jb	short loc_10307
		mov	bl, al

loc_10307::				; CODE XREF: _mod_102F5+Ej
		dec	cx
		jnz	short loc_102FE
		inc	bl
		mov	_word_245F2, bx
		retn
_mod_102F5 endp


; =============== S U B	R O U T	I N E =======================================


_mod_read_10311 proc near	; CODE XREF: _mod_n_t_module+67p
					; _mod_n_t_module:loc_10213p
		mov	cx, _word_245F2

loc_10315::				; CODE XREF: _mod_read_10311+D5j
		push	cx
		mov	dx, offset _word_31508
		mov	cx, _mod_channels_number
		shl	cx, 8
		call	_dosfread
		test	_moduleflag_246D0, 10b
		jz	short loc_1035C
		mov	ax, ds
		mov	es, ax
		mov	si, offset _word_31508
		mov	di, offset _byte_31D08
		mov	cx, 200h
		cld
		rep movsd
		mov	si, offset _byte_31D08
		mov	di, offset _word_31508
		mov	bx, 40h	; '@'

loc_10345::				; CODE XREF: _mod_read_10311+49j
		mov	cx, 4
		rep movsd
		add	si, 3F0h
		mov	cx, 4
		rep movsd
		sub	si, 400h
		dec	bx
		jnz	short loc_10345

loc_1035C::				; CODE XREF: _mod_read_10311+18j
		call	_memalloc12k
		mov	si, offset _word_31508
		mov	cx, 40h	; '@'

loc_10365::				; CODE XREF: _mod_read_10311+CEj
		push	cx
		mov	cx, _mod_channels_number
		xor	ch, ch

loc_1036C::				; CODE XREF: _mod_read_10311+C5j
		push	cx
		mov	eax, [si]
		add	si, 4
		mov	edx, eax
		xchg	al, ah
		xor	bl, bl
		and	ax, 0FFFh
		jz	short loc_103B9
		mov	bx, 72
		cmp	ax, 214
		jbe	short loc_10399
		mov	bx, 48
		cmp	ax, 428
		jbe	short loc_10399
		mov	bx, 24
		cmp	ax, 856
		jbe	short loc_10399
		xor	bx, bx

loc_10399::				; CODE XREF: _mod_read_10311+74j
					; _mod_read_10311+7Cj ...
		cmp	ax, [_table_25118+bx]
		jnb	short loc_103A8
		add	bx, 2
		cmp	bx, 166
		jb	short loc_10399

loc_103A8::				; CODE XREF: _mod_read_10311+8Cj
		mov	ax, bx
		shr	ax, 1
		mov	bl, 12
		div	bl
		inc	ah
		shl	al, 4
		or	al, ah
		mov	bl, al

loc_103B9::				; CODE XREF: _mod_read_10311+6Cj
		mov	bh, dl
		and	bh, 30h
		rol	edx, 10h
		mov	al, dl
		shr	al, 4
		or	bh, al
		and	dl, 0Fh
		mov	cl, 0FFh
		call	sub_11BA6
		pop	cx
		inc	ch
		cmp	ch, cl
		jb	short loc_1036C
		mov	byte ptr es:[di], 0
		inc	di
		pop	cx
		dec	cx
		jnz	short loc_10365
		call	_mem_reallocx
		pop	cx
		dec	cx
		jnz	loc_10315
		retn
_mod_read_10311 endp


; =============== S U B	R O U T	I N E =======================================

; 2STM

__2stm_module proc near	; DATA XREF: seg003:0E19o
		mov	_module_type_text, 4D545332h
		jmp	short loc_103FF
; ---------------------------------------------------------------------------

_stm_module::				; DATA XREF: seg003:0E0Co
		mov	_module_type_text, 204D5453h ;	STM

loc_103FF::				; CODE XREF: __2stm_module+9j
		mov	_moduleflag_246D0, 1000b
		mov	_mod_channels_number, 4
		mov	_word_245D2, 1Fh
		mov	_freq_245DE, 8448
		mov	al, 60h	; '`'
		call	sub_13E9B
		mov	_byte_24679, ah
		mov	_byte_2467A, al
		movzx	ax, _byte_30529
		mov	_word_245F2, ax
		mov	ax, ds
		mov	es, ax
		mov	si, offset _chrin ; in
		mov	di, offset asc_246B0 ; "				"
		mov	cx, 14h		; count
		call	_copy_printable
		mov	si, offset _my_in ; in
		mov	di, offset _myout ; out
		mov	cx, _word_245D2

loc_10445::				; CODE XREF: __2stm_module+E3j
		push	cx
		mov	cx, 0Ch		; count
		call	_copy_printable
		pop	cx

loc_1044D::
		movzx	eax, word ptr [si+10h]
		mov	edx, eax
		add	eax, 0Fh
		and	al, 0F0h
		cmp	eax, 100000h
		cmc
		adc	_word_24662, 0

loc_10467::
		mov	[di+20h], eax
		add	_dword_245C4, eax
		movzx	eax, word ptr [si+0Eh]
		shl	eax, 4
		mov	[di+38h], eax
		mov	ax, [si+18h]
		or	ax, ax
		jnz	short loc_10487
		mov	ax, _freq_245DE

loc_10487::				; CODE XREF: __2stm_module+97j
		mov	[di+36h], ax
		mov	al, [si+16h]
		mov	[di+3Dh], al
		movzx	ebx, word ptr [si+12h]
		mov	[di+24h], ebx
		movzx	eax, word ptr [si+14h]
		cmp	ax, 0FFFFh
		jnz	short loc_104B6
		xor	eax, eax
		mov	[di+24h], eax
		mov	[di+28h], eax
		dec	edx
		mov	[di+2Ch], edx
		jmp	short loc_104C7
; ---------------------------------------------------------------------------

loc_104B6::				; CODE XREF: __2stm_module+B6j
		mov	[di+2Ch], eax
		sub	eax, ebx
		inc	eax
		mov	[di+28h], eax
		or	byte ptr [di+3Ch], 8

loc_104C7::				; CODE XREF: __2stm_module+C9j
		add	si, 20h	; ' '
		add	di, 40h	; '@'
		dec	cx
		jnz	loc_10445
		mov	dx, offset _byte_27FE8
		mov	cx, 80h	; 'Ä'
		mov	eax, 410h
		call	_dosseek
		mov	si, offset _byte_27FE8
		xor	ax, ax

loc_104E6::				; CODE XREF: __2stm_module+105j
		cmp	byte ptr [si], 63h ; 'c'
		jnb	short loc_104F2
		inc	ax
		inc	si
		cmp	ax, 80h	; 'Ä'
		jb	short loc_104E6

loc_104F2::				; CODE XREF: __2stm_module+FEj
		mov	_word_245FA, ax
		mov	cx, _word_245F2

loc_104F9::				; CODE XREF: __2stm_module+195j
		push	cx
		mov	dx, offset _chrin
		mov	cx, 400h
		call	_dosfread
		call	_memalloc12k
		mov	si, offset _chrin
		mov	cx, 40h	; '@'

loc_1050C::				; CODE XREF: __2stm_module+18Ej
		push	cx
		mov	cx, _mod_channels_number
		xor	ch, ch

loc_10513::				; CODE XREF: __2stm_module+185j
		push	cx
		xor	bx, bx
		xor	dx, dx
		xor	cl, cl
		mov	al, [si]
		cmp	al, 0FEh ; '˛'
		jz	short loc_10565
		mov	cl, 0FFh
		cmp	al, 0FFh
		jz	short loc_1052E
		cmp	al, 0FBh ; '˚'
		jnb	short loc_10565
		inc	al
		mov	bl, al

loc_1052E::				; CODE XREF: __2stm_module+139j
		mov	bh, [si+1]
		shr	bh, 3
		mov	ax, [si+1]
		and	ax, 0F007h
		shr	ah, 1
		or	al, ah
		cmp	al, 40h	; '@'
		jbe	short loc_10544
		mov	al, 0FFh

loc_10544::				; CODE XREF: __2stm_module+155j
		mov	cl, al
		mov	ax, [si+2]
		and	al, 0Fh
		jz	short loc_10565
		cmp	al, 0Ah
		ja	short loc_10565
		mov	dh, ah
		rol	ebx, 10h
		mov	bl, al
		and	bx, 0Fh
		mov	dl, byte ptr cs:[asc_1058C+bx] ; "\x18\v\r\n\x02\x01\x03\x04\a"
		ror	ebx, 10h

loc_10565::				; CODE XREF: __2stm_module+133j
					; __2stm_module+13Dj ...
		call	sub_11BA6
		add	si, 4
		pop	cx
		inc	ch
		cmp	ch, cl
		jb	short loc_10513
		mov	byte ptr es:[di], 0
		inc	di
		pop	cx
		dec	cx
		jnz	short loc_1050C
		call	_mem_reallocx
		pop	cx
		dec	cx
		jnz	loc_104F9
		call	near ptr _mod_readfile_11F4E
		jb	loc_10099
		retn
__2stm_module endp ; sp-analysis failed

; ---------------------------------------------------------------------------
asc_1058C	db 0,18h,0Bh,0Dh,0Ah	; DATA XREF: __2stm_module+171r
		db 2,1,3,4,7,0

; =============== S U B	R O U T	I N E =======================================

; S3M

_s3m_module proc near		; DATA XREF: seg003:0E26o
		mov	_module_type_text, 204D3353h
		mov	_moduleflag_246D0, 10000b
		mov	_byte_2467E, 1
		mov	_byte_24673, 80h ; 'Ä'
		mov	_freq_245DE, 8363
		mov	_byte_2461A, 2
		cmp	_word_30532, 2
		jnb	short loc_105C7
		mov	_byte_24673, 0

loc_105C7::				; CODE XREF: _s3m_module+29j
		mov	ax, ds
		mov	es, ax
		mov	si, offset _chrin ; in
		mov	di, offset asc_246B0 ; "				"
		mov	cx, 1Ch		; count
		call	_copy_printable
		test	byte ptr [_config_word+1], 20h
		jz	short loc_1061E
		mov	dx, 64h	; 'd'
		mov	cl, _byte_3053B
		and	cx, 7Fh
		jz	short loc_10618
		test	_byte_3053B, 80h
		jz	short loc_105FF
		mov	ax, 0Bh
		mul	cx
		shrd	ax, dx,	3
		sub	ax, 2
		mov	cx, ax

loc_105FF::				; CODE XREF: _s3m_module+58j
		mov	ax, 100
		mul	cx
		mov	cx, '0'
		div	cx
		mov	dx, 100
		cmp	ax, 100
		jbe	short loc_10618
		cmp	ax, 2500
		ja	short loc_10618
		mov	dx, ax

loc_10618::				; CODE XREF: _s3m_module+51j
					; _s3m_module+78j ...
		mov	ax, dx
		push	cs
		call	near ptr _getset_amplif

loc_1061E::				; CODE XREF: _s3m_module+45j
		xor	si, si
		mov	di, offset _channels_25908
		xor	dx, dx
		mov	cx, 20h	; ' '

loc_10628::				; CODE XREF: _s3m_module+AEj
		mov	al, [_byte_30548+si]
		cmp	al, 0FFh
		jz	short loc_10640
		mov	dx, si
		shr	al, 4
		mov	ah, 1
		cmp	al, 1
		jz	short loc_1063D
		mov	ah, 0

loc_1063D::				; CODE XREF: _s3m_module+A2j
		mov	[di+1Dh], ah

loc_10640::				; CODE XREF: _s3m_module+97j
		inc	si
		add	di, 50h	; 'P'
		dec	cx
		jnz	short loc_10628
		inc	dx
		mov	_mod_channels_number, dx
		mov	cx, _mod_channels_number
		xor	si, si

loc_10652::				; CODE XREF: _s3m_module+CEj
		mov	al, _byte_2461E
		test	[_byte_30548+si], 8
		jz	short loc_1065F
		mov	al, _byte_2461F

loc_1065F::				; CODE XREF: _s3m_module+C3j
		mov	byte ptr [_dword_27BC8+si], al
		inc	si
		dec	cx
		jnz	short loc_10652
		mov	ax, _word_3052A
		cmp	ax, 63h	; 'c'
		jb	short loc_10672
		mov	ax, 63h	; 'c'

loc_10672::				; CODE XREF: _s3m_module+D6j
		mov	_word_245D2, ax
		mov	ax, _word_3052C
		cmp	ax, 100h
		jb	short loc_10680
		mov	ax, 100h

loc_10680::				; CODE XREF: _s3m_module+E4j
		mov	_word_245F2, ax
		mov	al, _byte_30539
		mov	_byte_24679, al
		mov	al, _byte_3053A
		mov	_byte_2467A, al
		mov	ax, ds
		mov	es, ax
		mov	di, offset _myout ; out
		mov	bx, (offset _dword_30566+2)
		add	bx, word ptr [unk_30528]
		movzx	ecx, _word_245D2

loc_106A3::				; CODE XREF: _s3m_module+1CAj
		push	bx
		push	cx
		mov	dx, offset _word_31508
		mov	cx, 50h	; 'P'
		movzx	eax, word ptr [bx]
		shl	eax, 4
		call	_dosseek
		mov	si, offset _word_31508
		xor	eax, eax
		xor	edx, edx
		cmp	byte ptr [si], 1
		jnz	short loc_106D8
		movzx	eax, word ptr [si+10h]
		mov	edx, eax
		cmp	eax, 100000h
		cmc
		adc	_word_24662, 0

loc_106D8::				; CODE XREF: _s3m_module+12Bj
		mov	[di+20h], eax
		add	_dword_245C4, eax
		movzx	eax, word ptr [si+0Eh]
		shl	eax, 4
		mov	[di+38h], eax
		mov	ax, [si+20h]
		or	ax, ax
		jnz	short loc_106F8
		mov	ax, _freq_245DE

loc_106F8::				; CODE XREF: _s3m_module+15Cj
		mov	[di+36h], ax
		mov	al, [si+1Ch]
		cmp	al, 3Fh	; '?'
		jb	short loc_10704
		mov	al, 3Fh	; '?'

loc_10704::				; CODE XREF: _s3m_module+169j
		mov	[di+3Dh], al
		test	byte ptr [si+1Fh], 1
		jnz	short loc_10720

loc_1070D::				; CODE XREF: _s3m_module+18Dj
					; _s3m_module+1A0j ...
		xor	eax, eax
		mov	[di+24h], eax
		mov	[di+28h], eax
		dec	edx
		mov	[di+2Ch], edx
		jmp	short loc_1074F
; ---------------------------------------------------------------------------

loc_10720::				; CODE XREF: _s3m_module+174j
		cmp	word ptr [si+14h], 0FFFFh
		jz	short loc_1070D
		movzx	edx, word ptr [si+14h]
		mov	[di+24h], edx
		movzx	eax, word ptr [si+18h]
		or	eax, eax
		jz	short loc_1070D
		dec	eax
		jz	short loc_1070D
		mov	[di+2Ch], eax
		inc	eax
		sub	eax, [di+24h]
		mov	[di+28h], eax
		or	byte ptr [di+3Ch], 8

loc_1074F::				; CODE XREF: _s3m_module+187j
		add	si, 30h	; '0'   ; in
		mov	cx, 1Ch		; count
		call	_copy_printable
		pop	cx
		pop	bx
		add	di, 40h	; '@'
		add	bx, 2
		dec	cx
		jnz	loc_106A3
		mov	si, (offset _dword_30566+2)
		xor	di, di
		xor	bx, bx
		mov	ax, word ptr [unk_30528]
		cmp	ax, 80h	; 'Ä'
		mov	ah, al
		ja	short loc_1079A
		xor	cl, cl

loc_10778::				; CODE XREF: _s3m_module+1FFj
		mov	al, [si]
		cmp	al, 0F0h ; ''
		jnb	short loc_1078F
		mov	[_byte_27FE8+di], al
		inc	bl
		inc	di
		cmp	cl, 0F0h ; ''
		jb	short loc_1078F
		mov	[_byte_280E7+di], 0FFh

loc_1078F::				; CODE XREF: _s3m_module+1E5j
					; _s3m_module+1F1j
		mov	cl, al
		inc	si
		inc	bh
		cmp	bh, ah
		jb	short loc_10778
		jmp	short loc_107AC
; ---------------------------------------------------------------------------

loc_1079A::				; CODE XREF: _s3m_module+1DDj
					; _s3m_module+213j
		mov	al, [si]
		cmp	al, 0FFh
		jz	short loc_107AC
		mov	[_byte_27FE8+di], al
		inc	bl
		inc	di
		inc	si
		cmp	bl, ah
		jb	short loc_1079A

loc_107AC::				; CODE XREF: _s3m_module+201j
					; _s3m_module+207j
		xor	bh, bh
		mov	_word_245FA, bx
		xor	ax, ax

loc_107B4::				; CODE XREF: _s3m_module+323j
		push	ax
		mov	bx, (offset _dword_30566+2)
		add	bx, word ptr [unk_30528]
		add	ax, _word_3052A
		shl	ax, 1
		add	bx, ax
		mov	dx, offset _word_31508
		mov	cx, 2
		movzx	eax, word ptr [bx]
		or	ax, ax
		jnz	short loc_107E0

loc_107D2::				; CODE XREF: _s3m_module+25Bj
		call	_memalloc12k
		mov	cx, 40h	; '@'
		xor	al, al
		cld
		rep stosb
		jmp	loc_108B1
; ---------------------------------------------------------------------------

loc_107E0::				; CODE XREF: _s3m_module+239j
		shl	eax, 4
		call	_dosseek
		mov	dx, offset _word_31508
		mov	cx, _word_31508
		cmp	cx, 308Fh
		jnb	short loc_107D2
		add	cx, 0Fh
		and	cl, 0F0h
		sub	cx, 2
		call	_dosfread
		call	_memalloc12k
		mov	si, offset _word_31508
		mov	cx, 40h	; '@'

loc_10809::				; CODE XREF: _s3m_module+316j
		push	cx
		lodsb
		or	al, al
		jz	loc_108A6

loc_10811::				; CODE XREF: _s3m_module+30Bj
		xor	bx, bx
		mov	ch, al
		test	ch, 20h
		jz	short loc_1082D
		mov	bx, [si]
		add	si, 2
		cmp	bl, 0FEh ; '˛'
		jnb	short loc_10826
		inc	bl

loc_10826::				; CODE XREF: _s3m_module+28Bj
		cmp	bh, 63h	; 'c'
		jbe	short loc_1082D
		xor	bh, bh

loc_1082D::				; CODE XREF: _s3m_module+281j
					; _s3m_module+292j
		mov	cl, 0FFh
		test	ch, 40h
		jz	short loc_1083E
		mov	cl, [si]
		inc	si
		cmp	cl, 40h	; '@'
		jbe	short loc_1083E
		mov	cl, 0FFh

loc_1083E::				; CODE XREF: _s3m_module+29Bj
					; _s3m_module+2A3j
		test	ch, 80h
		jz	short loc_1088D
		mov	dx, [si]
		add	si, 2
		cmp	dl, 19h
		ja	short loc_1088D
		rol	ebx, 10h
		movzx	bx, dl
		mov	dl, cs:[_s3mtable_108D6+bx]
		cmp	dl, 0FFh
		jz	short loc_10885
		cmp	dl, 0Fh
		jz	short loc_10880
		cmp	dl, 0Eh
		jnz	short loc_10887
		mov	bl, dh
		shr	bl, 4
		mov	al, cs:[_s3mtable_108F0+bx]
		cmp	al, 0FFh
		jz	short loc_10885
		shl	al, 4
		and	dh, 0Fh
		or	dh, al
		jmp	short loc_10887
; ---------------------------------------------------------------------------

loc_10880::				; CODE XREF: _s3m_module+2CAj
		cmp	dh, 20h	; ' '
		ja	short loc_10887

loc_10885::				; CODE XREF: _s3m_module+2C5j
					; _s3m_module+2DDj
		xor	dx, dx

loc_10887::				; CODE XREF: _s3m_module+2CFj
					; _s3m_module+2E7j ...
		ror	ebx, 10h
		jmp	short loc_1088F
; ---------------------------------------------------------------------------

loc_1088D::				; CODE XREF: _s3m_module+2AAj
					; _s3m_module+2B4j
		xor	dx, dx

loc_1088F::				; CODE XREF: _s3m_module+2F4j
		and	ch, 1Fh
		cmp	byte ptr [_mod_channels_number+1], ch
		jnb	short loc_1089C
		mov	byte ptr [_mod_channels_number+1], ch

loc_1089C::				; CODE XREF: _s3m_module+2FFj
		call	sub_11BA6
		lodsb
		or	al, al
		jnz	loc_10811

loc_108A6::				; CODE XREF: _s3m_module+276j
		mov	byte ptr es:[di], 0
		inc	di
		pop	cx
		dec	cx
		jnz	loc_10809

loc_108B1::				; CODE XREF: _s3m_module+246j
		call	_mem_reallocx
		pop	ax
		inc	ax
		cmp	ax, _word_245F2
		jb	loc_107B4
		mov	ax, _mod_channels_number
		inc	ah
		cmp	al, ah
		jb	short loc_108C9
		mov	al, ah

loc_108C9::				; CODE XREF: _s3m_module+32Ej
		xor	ah, ah
		mov	_mod_channels_number, ax
		call	near ptr _mod_readfile_11F4E
		jb	loc_10099
		retn
_s3m_module endp ; sp-analysis failed

; ---------------------------------------------------------------------------
_s3mtable_108D6	db 0FFh,10h,0Bh,0Dh,15h,12h,11h,13h,14h,1Bh,1Dh,17h,16h
					; DATA XREF: _s3m_module+2BDr
		db 0FFh,0FFh,9,0FFh,1Ch,7,0Eh,0Fh,0FFh,0FFh,0FFh,8,0FFh
_s3mtable_108F0	db 0,3,5,4,7,0FFh,0FFh,0FFh,8,0FFh,0FFh,6,0Ch,0Dh,0FFh
					; DATA XREF: _s3m_module+2D6r
		db 0FFh

; =============== S U B	R O U T	I N E =======================================

; E669

_e669_module proc near	; DATA XREF: seg003:0E61o
		mov	_module_type_text, 39363645h
		jmp	short loc_10914
; ---------------------------------------------------------------------------

__669_module::				; DATA XREF: seg003:0E5Ao
		mov	_module_type_text, 20393636h ;	669

loc_10914::				; CODE XREF: _e669_module+9j
		mov	_moduleflag_246D0, 100b
		mov	_byte_24673, 80h ; 'Ä'
		mov	_byte_2467E, 2
		mov	_mod_channels_number, 8
		movzx	ax, _byte_30576
		mov	_word_245D2, ax
		mov	al, _byte_30577
		mov	_word_245F2, ax
		mov	ah, _byte_2461F
		mov	al, _byte_2461E
		shl	eax, 10h
		mov	ah, _byte_2461F
		mov	al, _byte_2461E
		mov	_dword_27BC8, eax
		mov	_dword_27BCC, eax
		mov	ax, ds
		mov	es, ax
		mov	si, (offset _chrin+1)
		mov	cx, 4Ch	; 'L'

loc_1095C::				; CODE XREF: _e669_module+60j
		inc	si		; in
		cmp	byte ptr [si], 20h ; ' '
		loope	loc_1095C
		mov	di, offset asc_246B0 ; "				"
		mov	cx, 20h	; ' '   ; count
		call	_copy_printable
		xor	si, si
		xor	bh, bh

loc_1096F::				; CODE XREF: _e669_module+91j
		mov	bl, [_byte_30579+si]
		cmp	bl, 0FFh
		jz	short loc_10993
		mov	[_byte_27FE8+si], bl
		mov	al, [_byte_305F9+bx]
		mov	[_byte_280E8+si], al
		mov	al, [_byte_30679+bx]
		mov	[_byte_281E8+si], al
		inc	si
		cmp	si, 80h	; 'Ä'
		jb	short loc_1096F

loc_10993::				; CODE XREF: _e669_module+76j
		mov	_word_245FA, si
		mov	al, _byte_280E8
		mov	_byte_24679, al
		mov	_byte_2467A, 50h ; 'P'
		mov	dx, offset _chrin
		imul	cx, _word_245D2, 25
		mov	eax, 497
		call	_dosseek
		mov	si, offset _chrin ; in
		mov	di, offset _myout ; out
		mov	cx, _word_245D2

loc_109BD::				; CODE XREF: _e669_module+127j
		push	cx
		mov	cx, 0Dh		; count
		call	_copy_printable
		pop	cx
		mov	edx, [si+0Dh]
		cmp	edx, 100000h
		cmc
		adc	_word_24662, 0
		mov	[di+20h], edx
		add	_dword_245C4, edx
		mov	byte ptr [di+3Dh], 3Fh ; '?'
		mov	word ptr [di+36h], 2100h
		mov	ebx, [si+11h]
		mov	[di+24h], ebx
		mov	eax, [si+15h]
		cmp	eax, 0FFFFFh
		jb	short loc_10A0F
		xor	eax, eax
		mov	[di+24h], eax
		mov	[di+28h], eax
		dec	edx
		mov	[di+2Ch], edx
		jmp	short loc_10A20
; ---------------------------------------------------------------------------

loc_10A0F::				; CODE XREF: _e669_module+FAj
		mov	[di+2Ch], eax
		sub	eax, ebx
		inc	eax
		mov	[di+28h], eax
		or	byte ptr [di+3Ch], 8

loc_10A20::				; CODE XREF: _e669_module+10Dj
		add	si, 19h
		add	di, 40h	; '@'
		dec	cx
		jnz	short loc_109BD
		mov	cx, _word_245F2

loc_10A2D::				; CODE XREF: _e669_module+1C9j
		push	cx
		mov	dx, offset _word_31508
		mov	cx, 600h
		call	_dosfread
		call	_memalloc12k
		mov	si, offset _word_31508
		mov	cx, 40h	; '@'

loc_10A40::				; CODE XREF: _e669_module+1C0j
		push	cx
		mov	cx, _mod_channels_number
		xor	ch, ch

loc_10A47::				; CODE XREF: _e669_module+1B7j
		push	cx
		mov	al, [si]
		xor	bx, bx
		mov	cl, 0FFh
		cmp	al, 0FFh
		jz	short loc_10A83
		cmp	al, 0FEh ; '˛'
		jz	short loc_10A75
		shr	al, 2
		xor	ah, ah
		mov	dl, 0Ch
		div	dl
		inc	ah
		shl	al, 4
		or	al, ah
		mov	bl, al
		mov	ax, [si]
		xchg	al, ah
		shr	ax, 4
		and	al, 3Fh
		inc	al
		mov	bh, al

loc_10A75::				; CODE XREF: _e669_module+154j
		mov	al, [si+1]
		and	al, 0Fh
		mov	ah, 44h	; 'D'
		mul	ah
		shr	ax, 4
		mov	cl, al

loc_10A83::				; CODE XREF: _e669_module+150j
		mov	al, [si+2]
		mov	ah, al
		and	ah, 0Fh
		mov	dh, ah
		shr	al, 4
		cmp	al, 5
		ja	short loc_10AAA
		mov	dl, 0Fh
		jz	short loc_10AAC
		cmp	al, 4
		jz	short loc_10AAA
		cmp	al, 2
		jz	short loc_10AAA
		cmp	al, 3
		jnz	short loc_10AAA
		mov	dl, 1
		mov	dh, ah
		jmp	short loc_10AAC
; ---------------------------------------------------------------------------

loc_10AAA::				; CODE XREF: _e669_module+192j
					; _e669_module+19Aj ...
		xor	dx, dx

loc_10AAC::				; CODE XREF: _e669_module+196j
					; _e669_module+1A8j
		call	sub_11BA6
		add	si, 3
		pop	cx
		inc	ch
		cmp	ch, cl
		jb	short loc_10A47
		mov	byte ptr es:[di], 0
		inc	di
		pop	cx
		dec	cx
		jnz	loc_10A40
		call	_mem_reallocx
		pop	cx
		dec	cx
		jnz	loc_10A2D
		call	near ptr _mod_readfile_11F4E
		jb	loc_10099
		retn
_e669_module endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================

; MTM

_mtm_module proc near		; DATA XREF: seg003:0E2Fo
		mov	_module_type_text, 204D544Dh
		mov	_moduleflag_246D0, 100000b
		mov	_byte_24679, 6
		mov	_byte_2467A, 7Dh ; '}'
		mov	_byte_24673, 80h ; 'Ä'
		mov	ax, ds
		mov	es, ax
		mov	si, offset _myin	; in
		mov	di, offset asc_246B0 ; "				"
		mov	cx, 14h		; count
		call	_copy_printable
		cmp	_sndcard_type,	0
		jnz	short loc_10B25
		xor	si, si
		mov	cx, 10h

loc_10B0F::				; CODE XREF: _mtm_module+4Ej
		mov	al, byte ptr [_word_3052A+si]
		mov	di, ax
		and	di, 0Fh
		mov	al, cs:[_byte_13C54+di]
		mov	byte ptr [_dword_27BC8+si], al
		inc	si
		dec	cx
		jnz	short loc_10B0F

loc_10B25::				; CODE XREF: _mtm_module+33j
		movzx	ax, _byte_30526
		mov	_word_245D2, ax
		mov	al, _byte_30522
		inc	al
		mov	_word_245F2, ax
		movzx	ax, _byte_30523
		inc	ax
		mov	_word_245FA, ax
		mov	dx, offset _chrin
		imul	cx, _word_245D2, 25h
		add	cx, 0C2h ; '¬'
		xor	eax, eax
		call	_dosseek
		mov	si, offset unk_3054A
		mov	di, offset _myout
		mov	cx, _word_245D2
_mtm_module endp ; sp-analysis failed

_snd_off_chunk proc near

loc_10B5A::				; CODE XREF: _snd_off-3660j
		push	cx
		mov	cx, 16h		; count
		call	_copy_printable
		pop	cx
		mov	edx, [si+16h]

loc_10B66::				; CODE XREF: _snd_off+1Ej
		cmp	edx, 100000h
		cmc
		adc	_word_24662, 0
		mov	[di+20h], edx
		add	_dword_245C4, edx
		mov	al, [si+23h]
		mov	[di+3Dh], al
		mov	al, [si+22h]
		and	al, 0Fh
		mov	[di+3Eh], al
		mov	ax, _freq_245DE
		mov	[di+36h], ax
		mov	ebx, [si+1Ah]
		mov	[di+24h], ebx
		mov	eax, [si+1Eh]
		cmp	eax, 2
		ja	short loc_10BB5
		xor	eax, eax
		mov	[di+24h], eax
		mov	[di+28h], eax
		dec	edx
		mov	[di+2Ch], edx
		jmp	short loc_10BC6
; ---------------------------------------------------------------------------

loc_10BB5::				; CODE XREF: _snd_off-368Dj
		mov	[di+2Ch], eax
		sub	eax, ebx
		inc	eax
		mov	[di+28h], eax
		or	byte ptr [di+3Ch], 8

loc_10BC6::				; CODE XREF: _snd_off-367Aj
		add	si, 25h	; '%'
		add	di, 40h	; '@'
		dec	cx
		jnz	short loc_10B5A
		mov	di, offset _byte_27FE8
		mov	cx, 20h	; ' '
		cld
		rep movsd
		imul	ax, _word_245D2, 37
		add	ax, 0C2h ; '¬'
		movzx	eax, ax
		mov	_chrin, eax
		movzx	eax, _word_30520
		imul	eax, 192	; CODE XREF: _snd_deinit+16j
		add	eax, _chrin
		mov	dx, offset _byte_33508
		mov	cx, _word_245F2
		shl	cx, 6
		call	_dosseek
		mov	si, offset _byte_33508
		mov	cx, _word_245F2
		mov	ax, 4

loc_10C12::				; CODE XREF: _snd_off-3603j
		mov	bp, 1

loc_10C15::				; CODE XREF: _snd_off-3606j
		cmp	word ptr [si], 0
		jz	short loc_10C20
		cmp	bp, ax
		jb	short loc_10C20
		mov	ax, bp

loc_10C20::				; CODE XREF: _snd_off-3615j
					; _snd_off-3611j
		add	si, 2
		inc	bp
		cmp	bp, 20h	; ' '
		jbe	short loc_10C15
		dec	cx
		jnz	short loc_10C12
		mov	_mod_channels_number, ax
		mov	bx, offset _byte_33508
		mov	cx, _word_245F2

loc_10C36::				; CODE XREF: _snd_off-354Aj
		push	bx
		push	cx
		mov	si, offset _word_31508
		mov	cx, _mod_channels_number

loc_10C3F::				; CODE XREF: _snd_off-35AFj
		push	bx
		push	cx
		push	si
		mov	ax, [bx]
		or	ax, ax
		jnz	short loc_10C5A
		mov	ax, ds
		mov	es, ax
		mov	di, si
		mov	cx, 48
		xor	eax, eax
		cld
		rep stosd
		jmp	short loc_10C73
; ---------------------------------------------------------------------------

loc_10C5A::				; CODE XREF: _snd_off-35E7j
		dec	ax
		movzx	eax, ax
		imul	eax, 192
		add	eax, _chrin
		mov	dx, si
		mov	cx, 192
		call	_dosseek

loc_10C73::				; CODE XREF: _snd_off-35D5j
		pop	si
		pop	cx
		pop	bx
		add	bx, 2
		add	si, 192
		dec	cx
		jnz	short loc_10C3F
		call	_memalloc12k
		mov	si, offset _word_31508
		mov	cx, 40h	; '@'

loc_10C89::				; CODE XREF: _snd_off-3555j
		push	cx
		push	si
		mov	cx, _mod_channels_number
		xor	ch, ch

loc_10C91::				; CODE XREF: _snd_off-3562j
		push	cx

loc_10C92::
		xor	bx, bx
		mov	al, [si]
		shr	al, 2
		jz	short loc_10CAA
		xor	ah, ah
		mov	dl, 0Ch
		div	dl
		inc	ah
		shl	al, 4
		or	al, ah
		mov	bl, al

loc_10CAA::				; CODE XREF: _snd_off-3594j
		mov	ax, [si]
		mov	dl, ah
		xchg	al, ah
		shr	ax, 4
		and	al, 3Fh
		mov	bh, al
		and	dl, 0Fh
		mov	dh, [si+2]
		mov	cl, 0FFh
		call	sub_11BA6
		add	si, 192
		pop	cx
		inc	ch
		cmp	ch, cl
		jb	short loc_10C91
		mov	byte ptr es:[di], 0
		inc	di
		pop	si
		pop	cx
		add	si, 3
		dec	cx
		jnz	short loc_10C89
		call	_mem_reallocx
		pop	cx
		pop	bx
		add	bx, 40h	; '@'
		dec	cx
		jnz	loc_10C36
		mov	ax, 192
		mul	_word_30520
		mov	cx, dx
		imul	dx, _word_245D2, 37
		add	dx, 0C2h ; '¬'
		add	dx, _word_30524
		adc	cx, 0
		add	dx, ax
		adc	cx, 0
		mov	ax, _word_245F2
		shl	ax, 6
		add	dx, ax
		adc	cx, 0
		mov	bx, _fhandle_module
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file
		adc	_word_24662, 0
		call	near ptr _mod_readfile_11F4E
		jb	loc_10099
		retn
_snd_off_chunk endp

; =============== S U B	R O U T	I N E =======================================

; PSM

_psm_module proc near		; DATA XREF: seg003:0E37o
		mov	_module_type_text, 204D5350h
		mov	_moduleflag_246D0, 1000000b
		mov	ax, _word_30556
		mov	_mod_channels_number, ax
		mov	ax, _word_30554
		mov	_word_245D2, ax
		mov	_freq_245DE, 8448
		mov	al, _byte_3054B
		mov	_byte_24679, al
		mov	al, _byte_3054C
		mov	_byte_2467A, al
		movzx	ax, _byte_30550
		mov	_word_245FA, ax
		mov	ax, _word_30552
		mov	_word_245F2, ax
		mov	ax, ds
		mov	es, ax
		mov	si, offset _myin	; in
		mov	di, offset asc_246B0 ; "				"
		mov	cx, 30		; count
		call	_copy_printable
		mov	dx, offset _byte_3059A
		mov	cx, _word_245D2
		shl	cx, 6
		mov	eax, _dword_30566
		call	_dosseek
		mov	si, offset _byte_3059A
		mov	di, offset _myout ; out
		mov	cx, _word_245D2

loc_10D8C::				; CODE XREF: _psm_module+FAj
		push	cx
		push	si
		add	si, 0Dh		; in
		mov	cx, 16h		; count
		call	_copy_printable
		pop	si
		pop	cx
		mov	edx, [si+30h]
		cmp	edx, 100000h
		cmc
		adc	_word_24662, 0
		mov	[di+20h], edx
		add	_dword_245C4, edx
		mov	byte ptr [di+3Fh], 1
		mov	eax, [si+25h]
		mov	[di+38h], eax
		mov	ax, [si+3Eh]
		jnz	short loc_10DC7
		mov	ax, _freq_245DE

loc_10DC7::				; CODE XREF: _psm_module+9Cj
		mov	[di+36h], ax
		mov	al, [si+3Dh]
		mov	[di+3Dh], al
		mov	ebx, [si+34h]
		mov	[di+24h], ebx
		or	ebx, ebx
		jnz	short loc_10DF0
		xor	eax, eax
		mov	[di+24h], eax
		mov	[di+28h], eax
		dec	edx
		mov	[di+2Ch], edx
		jmp	short loc_10E19
; ---------------------------------------------------------------------------

loc_10DF0::				; CODE XREF: _psm_module+B5j
		mov	eax, [si+38h]
		mov	[di+28h], eax
		add	eax, ebx
		dec	eax
		mov	[di+2Ch], eax
		cmp	eax, edx
		jb	short loc_10E15
		dec	edx
		mov	[di+2Ch], edx
		sub	edx, ebx
		inc	edx
		mov	[di+28h], edx

loc_10E15::				; CODE XREF: _psm_module+DEj
		or	byte ptr [di+3Ch], 8

loc_10E19::				; CODE XREF: _psm_module+C8j
		add	si, 40h	; '@'
		add	di, 40h	; '@'
		dec	cx
		jnz	loc_10D8C
		mov	dx, offset _byte_27FE8
		mov	cx, _word_245FA
		mov	eax, _dword_3055A
		call	_dosseek
		mov	dx, _word_30562
		mov	cx, _word_30564
		mov	bx, _fhandle_module
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file
		adc	_word_24662, 0
		mov	cx, _word_245F2

loc_10E4C::				; CODE XREF: _psm_module+1DCj
		push	cx
		mov	dx, offset _word_31508
		mov	cx, 4
		call	_dosfread
		xor	si, si
		mov	cx, _word_245FA
		mov	ax, _my_seg_index
		mov	dl, _byte_3150A
		dec	dl
		and	dl, 3Fh

loc_10E68::				; CODE XREF: _psm_module+14Ej
		cmp	[_byte_27FE8+si], al
		jnz	short loc_10E72
		mov	[_byte_281E8+si], dl

loc_10E72::				; CODE XREF: _psm_module+146j
		inc	si
		dec	cx
		jnz	short loc_10E68
		mov	dx, offset _byte_3150C
		mov	cx, _word_31508
		sub	cx, 4
		call	_dosfread
		call	_memalloc12k
		mov	si, offset _byte_3150C
		mov	cx, 40h	; '@'

loc_10E8C::				; CODE XREF: _psm_module+1D5j
		push	cx
		lodsb
		or	al, al
		jz	short loc_10EF4

loc_10E92::				; CODE XREF: _psm_module+1CCj
		xor	bx, bx
		mov	ch, al
		test	ch, 80h
		jz	short loc_10EBD
		mov	bx, [si]
		add	si, 2
		or	bl, bl
		jz	short loc_10EB6
		dec	bl
		movzx	ax, bl
		mov	bl, 0Ch
		div	bl
		inc	ah
		shl	al, 4
		or	al, ah
		mov	bl, al

loc_10EB6::				; CODE XREF: _psm_module+17Cj
		cmp	bh, 63h	; 'c'
		jbe	short loc_10EBD
		xor	bh, bh

loc_10EBD::				; CODE XREF: _psm_module+173j
					; _psm_module+193j
		mov	cl, 0FFh
		test	ch, 40h
		jz	short loc_10ECE
		mov	cl, [si]
		inc	si
		cmp	cl, 40h	; '@'
		jbe	short loc_10ECE
		mov	cl, 0FFh

loc_10ECE::				; CODE XREF: _psm_module+19Cj
					; _psm_module+1A4j
		test	ch, 20h
		jz	short loc_10EDD
		mov	dx, [si]
		add	si, 2
		cmp	dl, 0Fh
		jbe	short loc_10EDF

loc_10EDD::				; CODE XREF: _psm_module+1ABj
		xor	dx, dx

loc_10EDF::				; CODE XREF: _psm_module+1B5j
		and	ch, 1Fh
		cmp	byte ptr [_mod_channels_number+1], ch
		jnb	short loc_10EEC
		mov	byte ptr [_mod_channels_number+1], ch

loc_10EEC::				; CODE XREF: _psm_module+1C0j
		call	sub_11BA6
		lodsb
		or	al, al
		jnz	short loc_10E92

loc_10EF4::				; CODE XREF: _psm_module+16Aj
		mov	byte ptr es:[di], 0
		inc	di
		pop	cx
		dec	cx
		jnz	short loc_10E8C
		call	_mem_reallocx
		pop	cx
		dec	cx
		jnz	loc_10E4C
		mov	ax, _mod_channels_number
		inc	ah
		cmp	al, ah
		jb	short loc_10F11
		mov	al, ah

loc_10F11::				; CODE XREF: _psm_module+1E7j
		xor	ah, ah
		mov	_mod_channels_number, ax
		call	near ptr _mod_readfile_11F4E
		jb	loc_10099
		retn
_psm_module endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================

; FAR

_far_module proc near		; DATA XREF: seg003:0E40o
		mov	_module_type_text, 20524146h
		mov	_moduleflag_246D0, 10000000b
		mov	_byte_24673, 0
		mov	_byte_2467E, 2
		mov	_mod_channels_number, 10h
		mov	al, byte ptr [_word_30552+1]
		and	ax, 0Fh
		mov	di, ax
		mov	al, cs:[_table_14057+di]
		mov	_byte_2467B, al
		mov	_byte_2467C, 0
		call	_calc_14043
		mov	_byte_2467A, al
		mov	_byte_24679, 4
		cmp	_sndcard_type,	0
		jnz	short loc_10F80
		xor	si, si
		mov	cx, _mod_channels_number

loc_10F6A::				; CODE XREF: _far_module+60j
		mov	al, byte ptr [_word_30554+si]
		mov	di, ax
		and	di, 0Fh
		mov	al, cs:[_byte_13C54+di]
		mov	byte ptr [_dword_27BC8+si], al
		inc	si
		dec	cx
		jnz	short loc_10F6A

loc_10F80::				; CODE XREF: _far_module+44j
		mov	ax, ds
		mov	es, ax
		mov	si, offset _myin	; in
		mov	di, offset asc_246B0 ; "				"
		mov	cx, 20h	; ' '   ; count
		call	_copy_printable
		mov	dx, (offset _dword_30566+2)
		mov	cx, 303h
		movzx	eax, word ptr [_dword_30566+2]
		add	eax, 62h ; 'b'
		call	_dosseek
		movzx	ax, _byte_30669
		cmp	ax, 100h
		jb	short loc_10FB0
		mov	ax, 100h

loc_10FB0::				; CODE XREF: _far_module+8Dj
		mov	_word_245FA, ax
		movzx	ax, _byte_3066A
		mov	_word_245F8, ax
		mov	si, (offset _dword_30566+2)
		mov	di, offset _byte_27FE8
		mov	cx, _word_245FA
		cld
		rep movsb
		mov	bx, offset _byte_3066B
		xor	ax, ax
		xor	dx, dx

loc_10FCF::				; CODE XREF: _far_module+C0j
		inc	dx
		cmp	word ptr [bx], 0
		jz	short loc_10FD7
		mov	ax, dx

loc_10FD7::				; CODE XREF: _far_module+B5j
		add	bx, 2
		cmp	dx, 100h
		jb	short loc_10FCF
		or	ax, ax
		stc
		jz	loc_10099
		cmp	ax, 100h
		jb	short loc_10FEF
		mov	ax, 100h

loc_10FEF::				; CODE XREF: _far_module+CCj
		mov	_word_245F2, ax
		mov	byte ptr [_chrin+3], 0
		mov	si, offset _byte_3066B
		mov	cx, _word_245F2

loc_10FFE::				; CODE XREF: _far_module+214j
		push	cx
		push	si
		mov	ax, [si]
		or	ax, ax
		jnz	short loc_1100F
		call	_memalloc12k
		mov	cx, 40h	; '@'
		jmp	loc_11120
; ---------------------------------------------------------------------------

loc_1100F::				; CODE XREF: _far_module+E6j
		sub	ax, 2
		shr	ax, 2
		xor	dx, dx
		div	_mod_channels_number
		push	ax
		dec	al
		and	al, 3Fh
		mov	byte ptr [_chrin], al
		xor	di, di
		mov	cx, _word_245FA
		mov	ah, byte ptr [_chrin+3]

loc_1102D::				; CODE XREF: _far_module+11Bj
		cmp	ah, [_byte_27FE8+di]
		jnz	short loc_11037
		mov	[_byte_281E8+di], al

loc_11037::				; CODE XREF: _far_module+113j
		inc	di
		dec	cx
		jnz	short loc_1102D
		mov	dx, offset _word_31508
		mov	cx, [si]
		call	_dosfread
		mov	byte ptr [_chrin+1], 0
		call	_memalloc12k
		pop	cx
		xor	ch, ch
		mov	si, offset _byte_3150A

loc_11051::				; CODE XREF: _far_module+1F7j
		push	cx
		mov	cx, _mod_channels_number
		xor	ch, ch

loc_11058::				; CODE XREF: _far_module+1ECj
		push	cx
		xor	bx, bx
		mov	cl, 0FFh
		mov	al, [si]
		or	al, al
		jz	short loc_11082
		dec	al
		xor	ah, ah
		mov	dl, 0Ch
		div	dl
		inc	al
		inc	ah
		shl	al, 4
		or	al, ah
		mov	bl, al
		mov	bh, [si+1]
		inc	bh
		cmp	bh, 63h	; 'c'
		jb	short loc_11082
		xor	bh, bh

loc_11082::				; CODE XREF: _far_module+143j
					; _far_module+160j
		mov	cl, 0FFh
		mov	al, [si+2]
		or	al, al
		jz	short loc_11094
		dec	al
		and	al, 0Fh
		shl	al, 2
		mov	cl, al

loc_11094::				; CODE XREF: _far_module+16Bj
		mov	dl, [si+3]
		mov	dh, dl
		shr	dl, 4
		and	dh, 0Fh
		cmp	dl, 3
		jz	short loc_110CB
		cmp	dl, 4
		jz	short loc_110E4
		cmp	dl, 5
		jz	short loc_110CF
		cmp	dl, 6
		jz	short loc_110D9
		cmp	dl, 0Bh
		jz	short loc_110FA

loc_110B8::
		cmp	dl, 0Dh
		jz	short loc_110EF
		cmp	dl, 0Eh
		jz	short loc_110F3
		cmp	dl, 0Fh
		jz	short loc_110EB
		xor	dx, dx
		jmp	short loc_110FF
; ---------------------------------------------------------------------------

loc_110CB::				; CODE XREF: _far_module+184j
		mov	dl, 19h
		jmp	short loc_110FF
; ---------------------------------------------------------------------------

loc_110CF::				; CODE XREF: _far_module+18Ej
		shr	dh, 1
		mov	byte ptr [_chrin+1], dh
		xor	dx, dx
		jmp	short loc_110FF
; ---------------------------------------------------------------------------

loc_110D9::				; CODE XREF: _far_module+193j
		shl	dh, 4
		or	dh, byte ptr [_chrin+1]
		mov	dl, 4
		jmp	short loc_110FF
; ---------------------------------------------------------------------------

loc_110E4::				; CODE XREF: _far_module+189j
		mov	dl, 0Eh

loc_110E6::
		or	dh, 90h
		jmp	short loc_110FF
; ---------------------------------------------------------------------------

loc_110EB::				; CODE XREF: _far_module+1A7j
		mov	dl, 1Fh
		jmp	short loc_110FF
; ---------------------------------------------------------------------------

loc_110EF::				; CODE XREF: _far_module+19Dj
		mov	dl, 20h	; ' '
		jmp	short loc_110FF
; ---------------------------------------------------------------------------

loc_110F3::				; CODE XREF: _far_module+1A2j
		mov	dl, 20h	; ' '
		shl	dh, 4
		jmp	short loc_110FF
; ---------------------------------------------------------------------------

loc_110FA::				; CODE XREF: _far_module+198j
		mov	dl, 0Eh

loc_110FC::
		or	dh, 80h

loc_110FF::				; CODE XREF: _far_module+1ABj
					; _far_module+1AFj ...
		call	sub_11BA6
		add	si, 4
		pop	cx
		inc	ch
		cmp	ch, cl
		jb	loc_11058
		mov	byte ptr es:[di], 0
		inc	di
		pop	cx
		dec	cx
		jnz	loc_11051
		mov	cx, 3Fh	; '?'
		sub	cl, byte ptr [_chrin]

loc_11120::				; CODE XREF: _far_module+EEj
		xor	al, al
		cld
		rep stosb
		call	_mem_reallocx
		pop	si
		pop	cx
		inc	byte ptr [_chrin+3]
		add	si, 2
		dec	cx
		jnz	loc_10FFE
		mov	ax, ds
		assume es:dseg
		mov	es, ax
		assume es:seg003

loc_1113A::
		mov	dx, offset _myin
		mov	cx, 8
		call	_dosfread
		mov	si, offset _myin
		mov	di, offset _myout

loc_11149::
		xor	ax, ax
		mov	_word_245D2, ax
		mov	ch, 8

loc_11150::				; CODE XREF: _far_module+305j
		mov	cl, 8

loc_11152::				; CODE XREF: _far_module+2FEj
		inc	ax
		shr	byte ptr [si], 1
		jnb	loc_11217
		push	ax
		push	cx
		push	si
		push	di
		mov	_word_245D2, ax
		push	di
		mov	dx, offset _word_31508
		mov	cx, 30h	; '0'
		call	_dosfread
		xor	dx, dx
		xor	cx, cx
		mov	bx, _fhandle_module
		mov	ax, 4201h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from present location
		pop	di		; out
		mov	[di+38h], ax
		mov	[di+3Ah], dx
		mov	si, offset _word_31508 ;	in

loc_11181::
		mov	edx, [si+20h]
		cmp	edx, 100000h
		cmc

loc_1118D::
		adc	_word_24662, 0
		mov	[di+20h], edx
		add	_dword_245C4, edx
		mov	al, [si+25h]
		ror	al, 4
		shr	al, 2
		mov	[di+3Dh], al
		mov	ax, _freq_245DE
		mov	[di+36h], ax

loc_111AD::
		test	byte ptr [si+2Fh], 8
		jnz	short loc_111C6

loc_111B3::				; CODE XREF: _far_module+2AFj
		xor	eax, eax
		mov	[di+24h], eax
		mov	[di+28h], eax
		dec	edx
		mov	[di+2Ch], edx
		jmp	short loc_111E8
; ---------------------------------------------------------------------------

loc_111C6::				; CODE XREF: _far_module+293j
		mov	eax, [si+2Ah]
		or	eax, eax
		jz	short loc_111B3
		mov	[di+2Ch], eax
		mov	ebx, [si+26h]
		mov	[di+24h], ebx

loc_111DB::
		sub	eax, ebx
		inc	eax
		mov	[di+28h], eax
		or	byte ptr [di+3Ch], 8

loc_111E8::				; CODE XREF: _far_module+2A6j
		mov	cx, 20h	; ' '   ; count
		call	_copy_printable
		test	byte ptr [si+2Eh], 1
		jz	short loc_11204
		or	byte ptr [di+3Ch], 4
		shr	dword ptr [di+24h], 1
		shr	dword ptr [di+2Ch], 1
		shr	dword ptr [di+28h], 1

loc_11204::				; CODE XREF: _far_module+2D4j
		mov	dx, [si+20h]
		mov	cx, [si+22h]
		mov	bx, _fhandle_module
		mov	ax, 4201h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from present location
		pop	di
		pop	si
		pop	cx
		pop	ax

loc_11217::				; CODE XREF: _far_module+237j
		add	di, 40h	; '@'
		dec	cl
		jnz	loc_11152
		inc	si
		dec	ch
		jnz	loc_11150
		cmp	_word_245D2, 0
		stc
		jz	loc_10099
		call	near ptr _mod_readfile_11F4E
		jb	loc_10099
		retn
_far_module endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================

; ULT

_ult_module proc near		; DATA XREF: seg003:0E49o
		mov	_module_type_text, 20544C55h
		mov	_moduleflag_246D0, 1000000000b
		mov	_byte_24673, 0
		mov	word ptr [_chrin], 40h ;	'@'
		mov	ax, _word_30515

loc_11256::
		xchg	al, ah
		mov	_word_30515, ax
		cmp	ax, 3034h
		jb	short loc_11265
		add	word ptr [_chrin], 2

loc_11265::				; CODE XREF: _ult_module+25j
		mov	_byte_24679, 6
		mov	_byte_2467A, 7Dh ; '}'

loc_1126F::
		mov	ax, ds
		mov	es, ax
		mov	si, offset _myin_0 ; in
		mov	di, offset asc_246B0 ; "				"
		mov	cx, 20h	; ' '   ; count
		call	_copy_printable
		mov	dx, offset _my_in
		mov	cx, 1
		movzx	eax, _byte_30537
		shl	eax, 5
		add	eax, 30h ; '0'
		call	_dosseek
		movzx	ax, _my_in
		mov	_word_245D2, ax
		mul	word ptr [_chrin]
		mov	cx, ax
		mov	dx, offset _byte_30539
		call	_dosfread
		mov	si, offset _byte_30539 ;	in
		mov	di, offset _myout ; out
		mov	cx, _word_245D2

loc_112B4::				; CODE XREF: _ult_module+131j
		push	cx
		push	si
		push	di
		mov	edx, [si+38h]
		sub	edx, [si+34h]
		jnb	short loc_112C4
		xor	edx, edx

loc_112C4::				; CODE XREF: _ult_module+86j
		cmp	edx, 100000h
		cmc
		adc	_word_24662, 0
		mov	[di+20h], edx
		add	_dword_245C4, edx
		mov	al, [si+3Ch]
		shr	al, 2
		mov	[di+3Dh], al
		mov	ax, _freq_245DE
		cmp	_word_30515, 3034h
		jb	short loc_112F1
		mov	ax, [si+3Eh]

loc_112F1::				; CODE XREF: _ult_module+B3j
		mov	[di+36h], ax
		mov	al, [si+3Dh]
		and	al, 1Ch
		mov	[di+3Ch], al
		test	al, 4
		jz	short loc_11316
		add	_dword_245C4, edx
		cmp	edx, 80000h
		cmc
		adc	_word_24662, 0
		shl	dword ptr [di+20h], 1

loc_11316::				; CODE XREF: _ult_module+C5j
		test	al, 8
		jnz	short loc_1132D

loc_1131A::				; CODE XREF: _ult_module+FBj
		xor	eax, eax
		mov	[di+24h], eax
		mov	[di+28h], eax
		dec	edx
		mov	[di+2Ch], edx
		jmp	short loc_11359
; ---------------------------------------------------------------------------

loc_1132D::				; CODE XREF: _ult_module+DFj
		mov	eax, [si+30h]
		or	eax, eax
		jz	short loc_1131A
		dec	eax
		mov	ebx, [si+2Ch]
		test	byte ptr [di+3Ch], 4
		jz	short loc_11348
		shr	ebx, 1
		shr	eax, 1

loc_11348::				; CODE XREF: _ult_module+107j
		mov	[di+24h], ebx
		mov	[di+2Ch], eax
		sub	eax, ebx
		inc	eax
		mov	[di+28h], eax

loc_11359::				; CODE XREF: _ult_module+F2j
		mov	cx, 20h	; ' '   ; count
		call	_copy_printable
		pop	di
		pop	si
		pop	cx
		add	di, 40h	; '@'

loc_11365::
		add	si, word ptr [_chrin]
		dec	cx
		jnz	loc_112B4
		mov	dx, offset _byte_30539
		mov	cx, 102h
		call	_dosfread
		mov	_word_245F8, 0
		mov	si, offset _byte_30539
		xor	ax, ax

loc_11382::				; CODE XREF: _ult_module+153j
		cmp	byte ptr [si], 0FFh
		jz	short loc_1138E
		inc	ax
		inc	si
		cmp	ax, 100h
		jb	short loc_11382

loc_1138E::				; CODE XREF: _ult_module+14Cj
		mov	_word_245FA, ax
		mov	ax, ds
		mov	es, ax
		mov	si, offset _byte_30539
		mov	di, offset _byte_27FE8
		mov	cx, _word_245FA
		cld
		rep movsb
		movzx	ax, _byte_30639
		inc	ax
		mov	_mod_channels_number, ax
		movzx	ax, _byte_3063A
		inc	ax
		mov	_word_245F2, ax
		mov	_byte_2467E, 0
		mov	ax, _word_30515
		cmp	ax, 3031h
		jz	short loc_113C6
		mov	_byte_2467E, 2

loc_113C6::				; CODE XREF: _ult_module+186j
		cmp	ax, 3033h
		jb	short loc_113F8
		mov	dx, offset _word_3063B
		mov	cx, _mod_channels_number
		call	_dosfread
		cmp	_sndcard_type,	0
		jnz	short loc_113F8
		xor	si, si
		mov	cx, _mod_channels_number

loc_113E2::				; CODE XREF: _ult_module+1BDj
		mov	al, byte ptr [_word_3063B+si]
		mov	di, ax
		and	di, 0Fh
		mov	al, cs:[_byte_13C54+di]
		mov	byte ptr [_dword_27BC8+si], al
		inc	si
		dec	cx
		jnz	short loc_113E2

loc_113F8::				; CODE XREF: _ult_module+190j
					; _ult_module+1A1j
		mov	si, offset _dword_30518
		mov	cx, _mod_channels_number

loc_113FF::				; CODE XREF: _ult_module+1F9j
		push	cx
		push	si
		xor	dx, dx
		xor	cx, cx
		mov	bx, _fhandle_module
		mov	ax, 4201h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from present location
		mov	[si], ax
		mov	[si+2],	dx
		mov	cx, _word_245F2

loc_11417::				; CODE XREF: _ult_module+1F1j
		push	cx
		mov	byte ptr [_word_3063B+1], 1
		mov	cx, 40h	; '@'

loc_11420::				; CODE XREF: _ult_module+1EDj
		push	cx
		call	_ult_read
		pop	cx
		dec	cx
		jnz	short loc_11420
		pop	cx
		dec	cx
		jnz	short loc_11417
		pop	si
		pop	cx
		add	si, 4
		dec	cx
		jnz	short loc_113FF
		mov	cx, _word_245F2

loc_11438::				; CODE XREF: _ult_module+2C6j
		push	cx
		mov	si, offset _dword_30518
		mov	di, offset _byte_30908
		mov	cx, _mod_channels_number

loc_11443::				; CODE XREF: _ult_module+250j
		push	cx
		mov	dx, [si]
		mov	cx, [si+2]
		mov	bx, _fhandle_module
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file
		mov	byte ptr [_word_3063B+1], 1
		mov	cx, 40h	; '@'

loc_1145A::				; CODE XREF: _ult_module+237j
		push	cx
		call	_ult_read
		mov	eax, _dword_3063D
		mov	[di], eax
		mov	al, _byte_30641
		mov	[di+4],	al
		add	di, 5
		pop	cx
		dec	cx
		jnz	short loc_1145A
		xor	dx, dx
		xor	cx, cx
		mov	bx, _fhandle_module
		mov	ax, 4201h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from present location
		mov	[si], ax
		mov	[si+2],	dx
		pop	cx
		add	si, 4
		dec	cx
		jnz	short loc_11443
		call	_memalloc12k
		mov	si, offset _byte_30908
		mov	cx, 40h	; '@'

loc_11494::				; CODE XREF: _ult_module+2BFj
		push	cx
		push	si
		mov	cx, _mod_channels_number
		xor	ch, ch

loc_1149C::				; CODE XREF: _ult_module+2B2j
		push	cx
		xor	bx, bx
		mov	al, [si]
		or	al, al
		jz	short loc_114C0
		dec	al
		xor	ah, ah
		mov	cl, 0Ch
		div	cl
		shl	al, 4
		inc	ah
		or	al, ah
		mov	bl, al
		mov	bh, [si+1]
		cmp	bh, 63h	; 'c'
		jb	short loc_114C0
		xor	bh, bh

loc_114C0::				; CODE XREF: _ult_module+26Aj
					; _ult_module+283j
		mov	cl, 0FFh
		mov	al, [si+2]
		mov	dl, al
		shr	al, 4
		mov	ah, [si+4]
		and	dl, 0Fh
		mov	dh, [si+3]
		call	_ult_1150B
		xchg	ax, dx
		call	_ult_1150B
		cmp	dl, al
		ja	short loc_114DF
		xchg	ax, dx

loc_114DF::				; CODE XREF: _ult_module+2A3j
		call	sub_11BA6
		pop	cx
		add	si, 140h
		inc	ch
		cmp	ch, cl
		jb	short loc_1149C
		mov	byte ptr es:[di], 0
		inc	di
		pop	si
		pop	cx
		add	si, 5
		dec	cx
		jnz	short loc_11494
		call	_mem_reallocx
		pop	cx
		dec	cx
		jnz	loc_11438
		call	near ptr _mod_readfile_11F4E
		jb	loc_10099
		retn
_ult_module endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_ult_1150B proc near		; CODE XREF: _ult_module+29Ap
					; _ult_module+29Ep
		cmp	al, 5
		jz	short loc_11520
		cmp	al, 0Ah
		jz	short loc_11523
		cmp	al, 0Bh
		jz	short loc_1152A
		cmp	al, 0Ch
		jz	short loc_11531
		cmp	al, 0Eh
		jz	short loc_11539
		retn
; ---------------------------------------------------------------------------

loc_11520::				; CODE XREF: _ult_1150B+2j
		xor	ax, ax
		retn
; ---------------------------------------------------------------------------

loc_11523::				; CODE XREF: _ult_1150B+6j
		shr	ah, 2
		and	ah, 33h
		retn
; ---------------------------------------------------------------------------

loc_1152A::				; CODE XREF: _ult_1150B+Aj
		and	ax, 0F00h
		or	ax, 800Eh
		retn
; ---------------------------------------------------------------------------

loc_11531::				; CODE XREF: _ult_1150B+Ej
		mov	cl, ah
		shr	cl, 2
		xor	ax, ax
		retn
; ---------------------------------------------------------------------------

loc_11539::				; CODE XREF: _ult_1150B+12j
		push	dx
		mov	dx, ax
		shr	dx, 4
		cmp	dl, 0EAh ; 'Í'
		jz	short loc_1154B
		cmp	dl, 0EBh ; 'Î'
		jz	short loc_1154B
		pop	dx
		retn
; ---------------------------------------------------------------------------

loc_1154B::				; CODE XREF: _ult_1150B+37j
					; _ult_1150B+3Cj
		mov	dh, ah
		and	dh, 0F0h
		and	ah, 0Fh
		shr	ah, 2
		or	ah, dh
		pop	dx
		retn
_ult_1150B endp


; =============== S U B	R O U T	I N E =======================================


_ult_read proc near		; CODE XREF: _ult_module+1E8p
					; _ult_module+222p
		dec	byte ptr [_word_3063B+1]
		jnz	short locret_11584
		mov	dx, offset _word_3063B
		mov	cx, 2
		call	_dosfread
		cmp	byte ptr [_word_3063B], 0FCh ; '¸'
		jz	short loc_11585
		mov	ax, _word_3063B
		mov	word ptr [_dword_3063D],	ax
		mov	byte ptr [_word_3063B+1], 1
		mov	dx, (offset _dword_3063D+2)
		mov	cx, 3
		call	_dosfread

locret_11584::				; CODE XREF: _ult_read+4j
		retn
; ---------------------------------------------------------------------------

loc_11585::				; CODE XREF: _ult_read+14j
		mov	dx, offset _dword_3063D
		mov	cx, 5
		call	_dosfread
		retn
_ult_read endp


; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


 ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


_inr_read_118B0 proc near	; CODE XREF: _inr_module+152p
		push	ds
		mov	ax, seg003
		mov	ds, ax
		mov	es, ax
		shl	dx, 6
		mov	ax, dx
		add	ax, offset _myout
		push	ax
		mov	cx, 96
		mov	bx, _fhandle_module
		mov	dx, offset _aInertiaSample ; "Inertia Sample: "
		mov	ah, 3Fh
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		pop	bx
		jb	loc_119B2
		lea	di, [bx]
		mov	si, offset asc_25856 ; "				\r\n\x1A"
		cld
		mov	cx, 8
		rep movsd
		mov	di, bx
		mov	ecx, _dword_25892
		mov	[di+24h], ecx
		mov	eax, _dword_25896
		mov	[di+2Ch], eax
		sub	eax, ecx
		inc	eax
		mov	[di+28h], eax
		mov	ax, _word_2588E
		mov	[di+36h], ax
		mov	al, _byte_2588C
		mov	[di+3Eh], al
		mov	al, _byte_2588B
		mov	[di+3Dh], al
		mov	al, _byte_2588D
		mov	[di+3Ch], al
		mov	eax, _dword_25886
		mov	[di+20h], eax

loc_1191C::				; CODE XREF: _inr_read_118B0+FCj
		mov	dx, offset _chrin
		mov	cx, 8
		mov	bx, _fhandle_module
		mov	ah, 3Fh
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		jb	loc_119B2
		mov	eax, _myin
		mov	_dword_257A0, eax
		xor	cx, cx
		xor	dx, dx
		mov	bx, _fhandle_module
		mov	ax, 4201h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from present location
		shl	edx, 10h
		mov	dx, ax
		add	_dword_257A0, edx
		mov	eax, _chrin
		cmp	eax, 54414453h	; SDAT
		jnz	short loc_11967	; SAMP
		mov	[di+38h], edx
		test	_sndflags_24622, 4
		jnz	short loc_11999
		jmp	short loc_11999
; ---------------------------------------------------------------------------

loc_11967::				; CODE XREF: _inr_read_118B0+A8j
		cmp	eax, 504D4153h	; SAMP
		jnz	short loc_11991	; ENDS
		xor	cx, cx
		xor	dx, dx
		mov	bx, _fhandle_module
		mov	ax, 4201h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from present location
		mov	dx, ax
		mov	cx, dx
		sub	dx, 8
		sub	cx, 0
		mov	bx, _fhandle_module
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file
		jmp	short loc_119AF
; ---------------------------------------------------------------------------

loc_11991::				; CODE XREF: _inr_read_118B0+BDj
		cmp	eax, 53444E45h	; ENDS
		jz	short loc_119AF

loc_11999::				; CODE XREF: _inr_read_118B0+B3j
					; _inr_read_118B0+B5j
		mov	dx, word ptr [_dword_257A0]
		mov	cx, word ptr [_dword_257A0+2]
		mov	bx, _fhandle_module
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file
		jb	short loc_119B2
		jmp	loc_1191C
; ---------------------------------------------------------------------------

loc_119AF::				; CODE XREF: _inr_read_118B0+DFj
					; _inr_read_118B0+E7j
		clc
		pop	ds
		retn
; ---------------------------------------------------------------------------

loc_119B2::				; CODE XREF: _inr_read_118B0+20j
					; _inr_read_118B0+7Aj ...
		mov	ax, 0FFFEh
		pop	ds
		retn
_inr_read_118B0 endp


; =============== S U B	R O U T	I N E =======================================


; void __usercall _inr_read_119B7(void *buffer<edi>)
_inr_read_119B7 proc near	; CODE XREF: _inr_module+B0p
					; _inr_module+C5p ...
		mov	ecx, _myin
		mov	bx, _fhandle_module
		mov	dx, di
		mov	ah, 3Fh
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		retn
_inr_read_119B7 endp


; =============== S U B	R O U T	I N E =======================================

; INR

_inr_module proc near		; DATA XREF: seg003:off_25326o
		mov	_module_type_text, 20524E49h
		mov	_moduleflag_246D0, 100000000b
		mov	_byte_24673, 0
		mov	_word_245F2, 0
		mov	_word_245D2, 0
		mov	ax, ds
		mov	es, ax
		cld
		mov	dx, offset _aInertiaModule ; "Inertia Module: "
		mov	cx, 50h	; 'P'
		xor	eax, eax
		call	_dosseek
		mov	si, (offset _aInertiaModule+10h)
		mov	di, offset asc_246B0 ; "				"
		mov	cx, 8
		cld
		rep movsd
		mov	al, _byte_257DB
		mov	_byte_2461A, al
		mov	al, _byte_257DC
		mov	_byte_2467E, al
		mov	ax, _word_257E6
		mov	_mod_channels_number, ax
		dec	ax
		mov	_byte_2461B, al
		mov	ax, _word_257EC
		mov	_freq_245DE, ax
		mov	ax, _word_257EE
		mov	_word_245FA, ax
		mov	ax, _word_257F0
		mov	_word_245F8, ax
		mov	al, _byte_257F2
		mov	_byte_2467A, al
		mov	al, _byte_257F3
		mov	_byte_24679, al

loc_11A39::				; CODE XREF: _inr_module+172j
		mov	cx, 8
		mov	bx, _fhandle_module
		mov	dx, offset _chrin
		mov	ah, 3Fh
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		jb	loc_11B3D
		mov	eax, _myin
		mov	_dword_257A0, eax
		xor	cx, cx
		xor	dx, dx
		mov	bx, _fhandle_module
		mov	ax, 4201h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from present location
		add	word ptr [_dword_257A0],	ax
		adc	word ptr [_dword_257A0+2], dx
		mov	eax, _chrin
		cmp	eax, 54534C54h	; TLST
		jnz	short loc_11A81	; BLST
		mov	di, offset _byte_280E8 ;	buffer
		call	_inr_read_119B7
		jb	loc_11B3D
		jmp	loc_11B28
; ---------------------------------------------------------------------------

loc_11A81::				; CODE XREF: _inr_module+ABj
		cmp	eax, 54534C42h	; BLST
		jnz	short loc_11A96	; PLST
		mov	di, offset _byte_281E8 ;	buffer
		call	_inr_read_119B7
		jb	loc_11B3D
		jmp	loc_11B28
; ---------------------------------------------------------------------------

loc_11A96::				; CODE XREF: _inr_module+C0j
		cmp	eax, 54534C50h	; PLST
		jnz	short loc_11AAA	; PATT
		mov	di, offset _byte_27FE8 ;	buffer
		call	_inr_read_119B7
		jb	loc_11B3D
		jmp	short loc_11B28
; ---------------------------------------------------------------------------

loc_11AAA::				; CODE XREF: _inr_module+D5j
		cmp	eax, 54544150h	; PATT
		jnz	short loc_11B09	; SAMP
		mov	ebx, _myin
		cmp	ebx, 40h ; '@'
		ja	short loc_11AC0
		mov	bx, 40h	; '@'   ; bytes

loc_11AC0::				; CODE XREF: _inr_module+F4j
		call	_memalloc
		jb	short loc_11B3D
		mov	ecx, _myin
		mov	di, _word_245F2
		inc	_word_245F2
		shl	di, 1
		mov	[_segs_table+di], ax
		cmp	cx, 40h	; '@'
		jbe	short loc_11AF3
		mov	[_myseg_size+di], cx
		xor	dx, dx
		mov	bx, _fhandle_module
		push	ds
		mov	ds, ax
		mov	ah, 3Fh
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		pop	ds
		jb	short loc_11B3D
		jmp	short loc_11B28
; ---------------------------------------------------------------------------

loc_11AF3::				; CODE XREF: _inr_module+114j
		mov	[_myseg_size+di], 40h ; '@'
		xor	di, di
		mov	es, ax
		assume es:nothing
		mov	cx, 10h
		xor	eax, eax
		cld
		rep stosd
		jmp	short loc_11B28
; ---------------------------------------------------------------------------

loc_11B09::				; CODE XREF: _inr_module+E9j
		cmp	eax, 504D4153h	; SAMP
		jnz	short loc_11B20	; ENDM
		mov	dx, _word_245D2
		inc	_word_245D2
		call	_inr_read_118B0
		jb	short loc_11B3D
		jmp	short loc_11B28
; ---------------------------------------------------------------------------

loc_11B20::				; CODE XREF: _inr_module+148j
		cmp	eax, 4D444E45h	; ENDM
		jz	short loc_11B41

loc_11B28::				; CODE XREF: _inr_module+B7j
					; _inr_module+CCj ...
		mov	dx, word ptr [_dword_257A0]
		mov	cx, word ptr [_dword_257A0+2]
		mov	bx, _fhandle_module
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file
		jnb	loc_11A39

loc_11B3D::				; CODE XREF: _inr_module+80j
					; _inr_module+B3j ...
		mov	ax, 0FFFEh
		retn
; ---------------------------------------------------------------------------

loc_11B41::				; CODE XREF: _inr_module+15Fj
		call	near ptr _mod_readfile_11F4E
		retn
_inr_module endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_dosseek proc	near		; CODE XREF: __2stm_module+F3p
					; _s3m_module+11Cp ...
		push	cx
		push	dx
		mov	dx, ax
		shr	eax, 10h
		mov	cx, ax
		mov	bx, _fhandle_module
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file
		pop	dx
		pop	cx
		mov	ax, 0FFFCh
		jb	_lfreaderr
_dosseek endp	; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_dosfread proc near		; CODE XREF: _moduleread+39p
					; _mod_read_10311+Fp ...
		mov	bx, _fhandle_module
		mov	ah, 3Fh
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		mov	cx, ax
		mov	ax, 0FFFCh
		jb	_lfreaderr	; 1 problem here
		retn
_dosfread endp


; =============== S U B	R O U T	I N E =======================================


_memalloc12k proc near	; CODE XREF: _mod_read_10311:loc_1035Cp
					; __2stm_module+118p ...
		mov	ebx, 12352	; bytes
		call	_memalloc
		jb	loc_10099
		mov	es, ax
		xor	di, di
		retn
_memalloc12k endp


; =============== S U B	R O U T	I N E =======================================


_mem_reallocx proc near	; CODE XREF: _mod_read_10311+D0p
					; __2stm_module+190p ...
		mov	bx, _my_seg_index
		shl	bx, 1
		mov	[_segs_table+bx], es
		mov	[_myseg_size+bx], di
		movzx	ebx, di
		mov	ax, es
		call	_memrealloc
		adc	_word_24662, 0
		inc	_my_seg_index
		retn
_mem_reallocx endp


; =============== S U B	R O U T	I N E =======================================


sub_11BA6 proc near		; CODE XREF: _mod_read_10311+BDp
					; __2stm_module:loc_10565p ...
		and	ch, 1Fh
		or	bl, bl
		jz	short loc_11BB2
		cmp	bl, 0FFh
		jnz	short loc_11BBB

loc_11BB2::				; CODE XREF: sub_11BA6+5j
		or	bh, bh
		jz	short loc_11BBE
		cmp	bh, 0FFh
		jz	short loc_11BBE

loc_11BBB::				; CODE XREF: sub_11BA6+Aj
		or	ch, 20h

loc_11BBE::				; CODE XREF: sub_11BA6+Ej
					; sub_11BA6+13j
		cmp	cl, 40h	; '@'
		ja	short loc_11BC6
		or	ch, 40h

loc_11BC6::				; CODE XREF: sub_11BA6+1Bj
		or	dx, dx
		jz	short loc_11BD1
		or	dl, dl
		jz	short loc_11C08

loc_11BCE::				; CODE XREF: sub_11BA6+64j
		or	ch, 80h

loc_11BD1::				; CODE XREF: sub_11BA6+22j
		test	ch, 0E0h
		jz	short locret_11C03
		mov	es:[di], ch
		inc	di
		test	ch, 80h
		jz	short loc_11BE5
		mov	es:[di], dx
		add	di, 2

loc_11BE5::				; CODE XREF: sub_11BA6+37j
		test	ch, 40h
		jz	short loc_11BEE
		mov	es:[di], cl
		inc	di

loc_11BEE::				; CODE XREF: sub_11BA6+42j
		test	ch, 20h
		jz	short loc_11BF9
		mov	es:[di], bx
		add	di, 2

loc_11BF9::				; CODE XREF: sub_11BA6+4Bj
		mov	al, ch
		and	al, 1Fh
		cmp	al, _byte_2461B
		ja	short loc_11C04

locret_11C03::				; CODE XREF: sub_11BA6+2Ej
		retn
; ---------------------------------------------------------------------------

loc_11C04::				; CODE XREF: sub_11BA6+5Bj
		mov	_byte_2461B, al
		retn
; ---------------------------------------------------------------------------

loc_11C08::				; CODE XREF: sub_11BA6+26j
		mov	dl, 1Dh
		jmp	short loc_11BCE
sub_11BA6 endp


; =============== S U B	R O U T	I N E =======================================


sub_11C0C proc near		; CODE XREF: sub_1415E+65p
		xor	si, si
		or	al, al
		jz	short locret_11C28
		xor	bx, bx

loc_11C14::				; CODE XREF: sub_11C0C+16j
					; sub_11C0C+1Aj
		mov	bl, cs:[_byte_11C29+bx]
		add	si, bx
		mov	bl, es:[si]
		inc	si
		shr	bl, 5
		jnz	short loc_11C14
		dec	al
		jnz	short loc_11C14

locret_11C28::				; CODE XREF: sub_11C0C+4j
		retn
sub_11C0C endp

; ---------------------------------------------------------------------------
_byte_11C29	db 0			; DATA XREF: sub_11C0C:loc_11C14r
					; sub_13623+1A1r
		db 2, 1, 3, 2, 4, 3, 5

; =============== S U B	R O U T	I N E =======================================


; int __usercall _copy_printable<eax>(char *in<esi>, char *out<edi>, int	count<ecx>)
_copy_printable proc near	; CODE XREF: _mod_1021E+28p
					; _mod_1024A+11p ...
		push	si
		push	di

loc_11C33::				; CODE XREF: _copy_printable+Dj
		mov	al, [si]
		inc	si
		cmp	al, 20h	; ' '
		jb	short loc_11C40
		mov	[di], al
		inc	di
		dec	cx
		jnz	short loc_11C33

loc_11C40::				; CODE XREF: _copy_printable+7j
		pop	di
		pop	si
		retn
_copy_printable endp


; =============== S U B	R O U T	I N E =======================================


_clean_11C43 proc far		; CODE XREF: _moduleread:loc_1003Dp
					; sub_12DA8+75p ...
		push	ds
		mov	ax, seg003
		mov	ds, ax
		mov	_byte_24679, 6
		mov	_byte_2467A, 125
		mov	_byte_2467E, 0
		mov	_moduleflag_246D0, 1
		mov	_my_seg_index,	0
		mov	_word_245F0, 0
		mov	_word_245F6, 0
		mov	_byte_24673, 0
		mov	_word_24630, 2
		mov	_word_245FA, 0
		mov	_word_245F8, 0
		mov	_mod_channels_number, 4
		mov	_word_245D6, 4
		mov	_word_245D8, 0
		mov	_word_245DA, 0
		mov	_word_245D2, 0
		mov	_freq_245DE, 8287
		test	_flag_playsetttings, 8
		jnz	short loc_11CB8
		mov	_freq_245DE, 8363

loc_11CB8::				; CODE XREF: _clean_11C43+6Dj
		mov	_byte_2461A, 0
		mov	_dword_245C4, 0
		mov	_amplification, 100
		mov	_byte_24625, 0
		mov	ax, ds
		mov	es, ax
		assume es:seg003
		cld
		mov	di, offset asc_246B0 ; "				"
		mov	cx, 8
		mov	eax, '    '
		rep stosd
		mov	di, offset _channels_25908
		xor	eax, eax
		mov	cx, 280h
		rep stosd
		mov	di, offset _byte_282E8
		mov	cx, 8
		rep stosd
		mov	di, offset _dword_27BC8
		mov	ah, _byte_2461E
		mov	al, _byte_2461F
		shl	eax, 10h
		mov	ah, _byte_2461F
		mov	al, _byte_2461E
		mov	cx, 8
		rep stosd
		mov	di, offset _myout
		xor	eax, eax
		mov	cx, 630h
		rep stosd
		mov	dx, 63h	; 'c'
		mov	di, offset _myout
		mov	eax, '    '

loc_11D2D::				; CODE XREF: _clean_11C43+FCj
		mov	cx, 8
		rep stosd
		sub	di, 20h	; ' '
		mov	word ptr [di+32h], 0FFFFh
		add	di, 40h	; '@'
		dec	dx
		jnz	short loc_11D2D
		mov	di, offset _segs_table
		mov	cx, 80h	; 'Ä'
		xor	eax, eax
		rep stosd
		mov	di, offset _byte_280E8
		mov	cx, 40h	; '@'
		rep stosd
		mov	di, offset _byte_27FE8
		mov	cx, 40h	; '@'
		rep stosd
		mov	di, offset _byte_282E8
		mov	cx, 8
		rep stosd
		mov	di, offset _byte_281E8
		mov	cx, 40h	; '@'
		mov	eax, 3F3F3F3Fh
		rep stosd
		pop	ds
		retf
_clean_11C43 endp


; =============== S U B	R O U T	I N E =======================================


_ems_init proc near		; CODE XREF: sub_12DA8+103p
		mov	_ems_enabled, 0
		mov	ax, 1
		test	byte ptr [_config_word],	2
		jz	loc_11E00
		xor	ax, ax
		mov	es, ax
		assume es:nothing
		mov	ax, 1
		mov	es, word ptr es:[19Eh]
		assume es:nothing
		cmp	dword ptr es:[0Ah], 584D4D45h ;	EMMX
		jnz	short loc_11E00
		cmp	dword ptr es:[0Eh], 30585858h ;	XXX0
		jnz	short loc_11E00
		mov	ah, 40h	; '@'
		int	67h		;  - LIM EMS - GET MANAGER STATUS
					; Return: AH = status
		mov	bx, ax
		mov	ax, 2
		or	bh, bh
		jnz	short loc_11E00
		mov	ah, 46h	; 'F'
		int	67h		;  - LIM EMS - GET EMM VERSION
					; Return: AH = status
		mov	bx, ax
		mov	ax, 2
		or	bh, bh
		jnz	short loc_11E00
		mov	ax, 3
		cmp	bl, 40h	; '@'
		jb	short loc_11E00
		mov	ah, 41h	; 'A'
		int	67h		;  - LIM EMS - GET PAGE	FRAME SEGMENT
					; Return: AH = 00h function successful,	BX = segment of	page frame
					; AH = error code
		cmp	ah, 0
		mov	ax, 2
		jnz	short loc_11E00
		mov	_ems_pageframe, bx
		mov	ah, 43h	; 'C'
		mov	bx, 1
		int	67h		;  - LIM EMS - GET HANDLE AND ALLOCATE MEMORY
					; BX = number of logical pages to allocate
					; Return: AH = status
		cmp	ah, 0
		mov	ax, 8
		jnz	short loc_11E00
		mov	_ems_handle, dx
		mov	_ems_enabled, 1
		mov	_ems_log_pagenum, 0
		xor	ax, ax
		clc
		retn
; ---------------------------------------------------------------------------

loc_11E00::				; CODE XREF: _ems_init+Dj _ems_init+25j	...
		stc
		retn
_ems_init endp


; =============== S U B	R O U T	I N E =======================================


_ems_release proc near	; CODE XREF: _ems_deinit+7p
		cmp	_ems_enabled, 1
		jnz	short locret_11E1D
		mov	bx, 8000h
		call	_ems_mapmem
		mov	dx, _ems_handle
		mov	ah, 45h	; 'E'
		int	67h		;  - LIM EMS - RELEASE HANDLE AND MEMORY
					; DX = EMM handle
					; Return: AH = status
		mov	_ems_log_pagenum, 0

locret_11E1D::				; CODE XREF: _ems_release+5j
		retn
_ems_release endp


; =============== S U B	R O U T	I N E =======================================


_ems_realloc proc near	; CODE XREF: _memfree_125DA+6p
		cmp	_ems_enabled, 1
		jnz	short locret_11E36
		mov	dx, _ems_handle
		mov	bx, 1
		mov	ah, 51h	; 'Q'
		int	67h		;  - LIM EMS 4.0 - REALLOCATE PAGES
					; DX = handle
					; BX = number of pages to be allocated to handle
					; Return: BX = actual number of	pages allocated	to handle
					; AH = status
		mov	_ems_log_pagenum, 0

locret_11E36::				; CODE XREF: _ems_realloc+5j
		retn
_ems_realloc endp


; =============== S U B	R O U T	I N E =======================================


_ems_deinit proc near		; CODE XREF: _deinit_125B9+Fp
		cmp	_ems_enabled, 1
		jnz	short locret_11E46
		call	_ems_release
		mov	_ems_enabled, 0

locret_11E46::				; CODE XREF: _ems_deinit+5j
		retn
_ems_deinit endp


; =============== S U B	R O U T	I N E =======================================


_ems_save_mapctx proc	near	; CODE XREF: _moduleread+24p
					; _volume_prep+16p ...
		cmp	_ems_enabled, 1
		jnz	short locret_11E67
		mov	cx, 32h	; '2'

loc_11E51::				; CODE XREF: _ems_save_mapctx+19j
		mov	dx, _ems_handle
		mov	ax, 4700h
		int	67h		;  - LIM EMS - SAVE MAPPING CONTEXT
					; DX = handle
					; Return: AH = status
		cmp	ah, 0
		jz	short locret_11E67
		dec	cx
		jnz	short loc_11E51
		mov	_byte_246A5, 1

locret_11E67::				; CODE XREF: _ems_save_mapctx+5j
					; _ems_save_mapctx+16j
		retn
_ems_save_mapctx endp


; =============== S U B	R O U T	I N E =======================================


_ems_restore_mapctx proc near	; CODE XREF: _moduleread+78p
					; _moduleread+A9p ...
		cmp	_ems_enabled, 1
		jnz	short locret_11E8A
		cmp	_byte_246A5, 1
		jnz	short locret_11E8A
		mov	cx, 32h	; '2'

loc_11E79::				; CODE XREF: _ems_restore_mapctx+20j
		mov	dx, _ems_handle
		mov	ax, 4800h
		int	67h		;  - LIM EMS - RESTORE MAPPING CONTEXT
					; DX = handle
					; Return: AH = status
		cmp	ah, 0
		jz	short locret_11E8A
		dec	cx
		jnz	short loc_11E79

locret_11E8A::				; CODE XREF: _ems_restore_mapctx+5j
					; _ems_restore_mapctx+Cj ...
		retn
_ems_restore_mapctx endp


; =============== S U B	R O U T	I N E =======================================


_ems_mapmem proc near		; CODE XREF: _useless_11787+34p
					; _ems_release+Ap ...
		cmp	_ems_enabled, 1
		jnz	short locret_11EC4
		mov	cx, 32h	; '2'
		cmp	bx, _ems_log_pagenum
		jb	short loc_11E9E
		mov	bx, 0FFFFh	; EMS UNMAP

loc_11E9E::				; CODE XREF: _ems_mapmem+Ej
					; _ems_mapmem+37j
		push	bx
		mov	dx, _ems_handle
		push	bx
		mov	ax, 4400h
		int	67h		;  - LIM EMS - MAP MEMORY
					; AL = physical	page number (0-3)
					; BX = logical page number, DX = handle
					; Return: AH = status
		pop	bx
		inc	bx
		jz	short loc_11EB3
		cmp	bx, _ems_log_pagenum
		jb	short loc_11EB6

loc_11EB3::				; CODE XREF: _ems_mapmem+20j
		mov	bx, 0FFFFh

loc_11EB6::				; CODE XREF: _ems_mapmem+26j
		mov	ax, 4401h
		int	67h		;  - LIM EMS - MAP MEMORY
					; AL = physical	page number (0-3)
					; BX = logical page number, DX = handle
					; Return: AH = status
		cmp	ah, 0
		pop	bx
		jz	short locret_11EC4
		dec	cx
		jnz	short loc_11E9E

locret_11EC4::				; CODE XREF: _ems_mapmem+5j
					; _ems_mapmem+34j
		retn
_ems_mapmem endp


; =============== S U B	R O U T	I N E =======================================


_ems_mapmem2 proc near	; CODE XREF: _ems_mapmemx+48p
					; _ems_mapmemx+F4p ...
		cmp	_ems_enabled, 1
		jnz	short locret_11EFE
		mov	cx, 32h	; '2'
		cmp	bx, _ems_log_pagenum
		jb	short loc_11ED8
		mov	bx, 0FFFFh

loc_11ED8::				; CODE XREF: _ems_mapmem2+Ej
					; _ems_mapmem2+37j
		push	bx
		mov	dx, _ems_handle
		push	bx
		mov	ax, 4402h
		int	67h		;  - LIM EMS - MAP MEMORY
					; AL = physical	page number (0-3)
					; BX = logical page number, DX = handle
					; Return: AH = status
		pop	bx
		inc	bx
		jz	short loc_11EED
		cmp	bx, _ems_log_pagenum
		jb	short loc_11EF0

loc_11EED::				; CODE XREF: _ems_mapmem2+20j
		mov	bx, 0FFFFh

loc_11EF0::				; CODE XREF: _ems_mapmem2+26j
		mov	ax, 4403h
		int	67h		;  - LIM EMS - MAP MEMORY
					; AL = physical	page number (0-3)
					; BX = logical page number, DX = handle
					; Return: AH = status
		cmp	ah, 0
		pop	bx
		jz	short locret_11EFE
		dec	cx
		jnz	short loc_11ED8

locret_11EFE::				; CODE XREF: _ems_mapmem2+5j
					; _ems_mapmem2+34j
		retn
_ems_mapmem2 endp


; =============== S U B	R O U T	I N E =======================================


_ems_realloc2 proc near	; CODE XREF: _mod_readfile_11F4E+36p
		inc	_byte_24617
		cmp	_ems_enabled, 1
		jnz	short loc_11F3C
		mov	ebx, [di+20h]
		shr	ebx, 4
		add	bx, 102h
		dec	bx
		shr	bx, 0Ah
		inc	bx
		mov	ah, 51h	; 'Q'
		mov	dx, _ems_handle
		add	bx, _ems_log_pagenum
		push	bx
		int	67h		;  - LIM EMS 4.0 - REALLOCATE PAGES
					; DX = handle
					; BX = number of pages to be allocated to handle
					; Return: BX = actual number of	pages allocated	to handle
					; AH = status
		pop	bx
		cmp	ah, 0
		jnz	short loc_11F3C
		mov	cx, _ems_log_pagenum
		mov	_ems_log_pagenum, bx
		mov	bx, cx
		mov	ax, _ems_pageframe
		retn
; ---------------------------------------------------------------------------

loc_11F3C::				; CODE XREF: _ems_realloc2+9j
					; _ems_realloc2+2Dj
		mov	ebx, [di+20h]
		add	ebx, 1020h	; bytes
		call	_memalloc
		mov	cx, 0FFFFh
		retn
_ems_realloc2 endp


; =============== S U B	R O U T	I N E =======================================


_mod_readfile_11F4E proc far	; CODE XREF: _mod_n_t_module+6Ap
					; _mod_n_t_module+159p ...
		mov	_byte_24617, 0
		cmp	_word_24662, 0
		stc
		jnz	short locret_11FD3
		test	_sndflags_24622, 4
		jnz	short loc_11FD6
		test	_sndflags_24622, 10h
		jnz	short loc_11FD2
		mov	cx, _word_245D2
		mov	di, offset _myout

loc_11F70::				; CODE XREF: _mod_readfile_11F4E+82j
		push	cx
		cmp	dword ptr [di+20h], 0
		jz	short loc_11FCB
		mov	_byte_24675, 1
		mov	al, [di+3Fh]
		mov	_byte_24674, al
		push	di
		call	_ems_realloc2
		pop	di
		jb	short loc_11FD4
		mov	[di+30h], ax
		mov	[di+32h], cx
		mov	es, ax
		test	_moduleflag_246D0, 10111011000b
		jz	short loc_11FA9
		mov	dx, [di+38h]
		mov	cx, [di+3Ah]
		mov	bx, _fhandle_module
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file

loc_11FA9::				; CODE XREF: _mod_readfile_11F4E+4Aj
		mov	bx, _fhandle_module
		mov	ecx, [di+20h]
		push	di
		push	es
		mov	dx, [di+32h]
		call	_mod_readfile_12247
		adc	_word_24662, 0
		pop	es
		assume es:nothing
		pop	di
		mov	ax, es
		push	di
		call	_ems_mapmemx
		pop	di
		or	byte ptr [di+3Ch], 1

loc_11FCB::				; CODE XREF: _mod_readfile_11F4E+28j
		add	di, 40h	; '@'
		pop	cx
		dec	cx
		jnz	short loc_11F70

loc_11FD2::				; CODE XREF: _mod_readfile_11F4E+19j
		clc

locret_11FD3::				; CODE XREF: _mod_readfile_11F4E+Bj
		retn
; ---------------------------------------------------------------------------

loc_11FD4::				; CODE XREF: _mod_readfile_11F4E+3Aj
		pop	cx
		retn
; ---------------------------------------------------------------------------

loc_11FD6::				; CODE XREF: _mod_readfile_11F4E+12j
		mov	eax, 10000h
		mov	cl, _dma_channel_0
		call	_alloc_dma_buf
		jb	locret_1221F
		mov	word ptr [_dma_buf_pointer+2], ax
		mov	word ptr [_dma_buf_pointer], 0
		mov	di, offset _myout
		mov	cx, _word_245D2

loc_11FF7::				; CODE XREF: _mod_readfile_11F4E+1D9j
		push	cx
		cmp	dword ptr [di+20h], 0
		jz	loc_12106
		inc	_byte_24617
		mov	_byte_24675, 1
		mov	al, [di+3Fh]
		mov	_byte_24674, al
		test	_moduleflag_246D0, 10111011000b
		jz	short loc_12027
		mov	dx, [di+38h]
		mov	cx, [di+3Ah]
		mov	bx, _fhandle_module
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file

loc_12027::				; CODE XREF: _mod_readfile_11F4E+C8j
		test	byte ptr [di+3Ch], 4
		jz	short loc_1206B
		mov	eax, [di+20h]
		add	eax, 1Fh
		and	al, 0E0h
		shr	eax, 2
		mov	bx, _word_24630
		shl	bx, 2
		add	ax, bx
		jnb	short loc_12056
		and	_word_24630, 0C000h
		add	_word_24630, 4000h
		jb	loc_12117

loc_12056::				; CODE XREF: _mod_readfile_11F4E+F6j
		mov	ax, _word_24630
		mov	bx, ax
		and	bx, 0C000h
		and	ax, 3FFFh
		shr	ax, 1
		or	ax, bx		; CODE XREF: _snd_initialze+13j

loc_12066::				; CODE XREF: _snd_initialze+13j
		mov	[di+34h], ax
		jmp	short loc_12071
; ---------------------------------------------------------------------------

loc_1206B::				; CODE XREF: _mod_readfile_11F4E+DDj
		mov	ax, _word_24630
		mov	[di+34h], ax

loc_12071::				; CODE XREF: _mod_readfile_11F4E+11Bj
		mov	ecx, [di+20h]

loc_12075::				; CODE XREF: _mod_readfile_11F4E+174j
		cmp	ecx, 8000h
		jbe	short loc_120C4
		sub	ecx, 8000h
		push	ecx
		mov	cx, 8000h
		mov	bx, _fhandle_module
		mov	ah, 3Fh	; '?'
		push	di
		push	ds
		lds	dx, _dma_buf_pointer
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		pop	ds
		pop	di
		adc	_word_24662, 0
		les	si, _dma_buf_pointer
		assume es:nothing
		mov	cx, 8000h
		call	_mod_sub_12220
		push	di

loc_120AA::
		mov	cx, 8000h
		mov	ax, _word_24630
		call	_nongravis_182E7
		xor	word ptr [_dma_buf_pointer], 8000h
		add	_word_24630, 800h
		pop	di
		pop	ecx
		jmp	short loc_12075
; ---------------------------------------------------------------------------

loc_120C4::				; CODE XREF: _mod_readfile_11F4E+12Ej
		jcxz	short loc_120E7
		mov	bx, _fhandle_module
		mov	ah, 3Fh	; '?'
		push	di
		push	cx
		push	ds
		lds	dx, _dma_buf_pointer
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		pop	ds
		pop	cx
		pop	di
		push	di
		adc	_word_24662, 0
		les	si, _dma_buf_pointer
		push	cx
		call	_mod_sub_12220
		pop	cx

loc_120E7::				; CODE XREF: _mod_readfile_11F4E:loc_120C4j
		push	cx
		mov	ax, _word_24630
		call	_nongravis_182E7
		xor	word ptr [_dma_buf_pointer], 8000h
		pop	ax
		add	ax, 21h	; '!'
		and	al, 0E0h
		shr	ax, 4

loc_120FD::
		add	_word_24630, ax
		pop	di
		or	byte ptr [di+3Ch], 1

loc_12106::				; CODE XREF: _mod_readfile_11F4E+AFj
		pop	cx
		mov	dx, _word_24630
		shr	dx, 1
		mov	al, _byte_24628
		shl	ax, 0Dh
		cmp	dx, ax
		jbe	short loc_12123

loc_12117::				; CODE XREF: _mod_readfile_11F4E+104j
		call	_memfree_18A28
		mov	dx, offset _aNotEnoughDramOn ; "Not enough DRAM on UltraSound\r\n"
		mov	ax, 0FFFDh
		jmp	_lfreaderr
; ---------------------------------------------------------------------------

loc_12123::				; CODE XREF: _mod_readfile_11F4E+1C7j
		add	di, 40h	; '@'
		dec	cx
		jnz	loc_11FF7

loc_1212B::				; CODE XREF: _mod_readfile_11F4E+1E2j
		cmp	_byte_2466E, 1
		jz	short loc_1212B
		call	_memfree_18A28
		mov	di, offset _myout
		mov	cx, _word_245D2

loc_1213C::				; CODE XREF: _mod_readfile_11F4E+2CCj
		test	byte ptr [di+3Ch], 1
		jz	loc_12216
		push	cx
		xor	ax, ax
		test	byte ptr [di+3Ch], 4
		jnz	short loc_121B9
		test	byte ptr [di+3Ch], 8
		jz	short loc_1219E
		mov	edx, [di+24h]
		push	edi
		movzx	edi, word ptr [di+34h]
		shl	edi, 4
		add	edi, edx
		inc	edi
		pop	edi
		mov	edx, [di+2Ch]
		push	edi
		movzx	edi, word ptr [di+34h]
		shl	edi, 4
		add	edi, edx
		add	edi, 2
		pop	edi
		mov	edx, [di+24h]
		push	edi
		movzx	edi, word ptr [di+34h]
		shl	edi, 4
		add	edi, edx
		pop	edi

loc_1219E::				; CODE XREF: _mod_readfile_11F4E+203j
		mov	edx, [di+2Ch]
		push	edi
		movzx	edi, word ptr [di+34h]
		shl	edi, 4
		add	edi, edx
		inc	edi
		pop	edi
		jmp	short loc_12215
; ---------------------------------------------------------------------------

loc_121B9::				; CODE XREF: _mod_readfile_11F4E+1FDj
		test	byte ptr [di+3Ch], 8
		jz	short loc_121EE
		mov	edx, [di+24h]
		test	byte ptr [di+3Ch], 10h
		jz	short loc_121CD
		mov	edx, [di+2Ch]

loc_121CD::				; CODE XREF: _mod_readfile_11F4E+279j
		push	edi
		mov	bx, [di+34h]
		movzx	edi, bx
		and	di, 1FFFh
		and	bx, 0C000h
		shr	bx, 1
		or	di, bx
		shl	edi, 4
		add	edi, edx
		pop	edi

loc_121EE::				; CODE XREF: _mod_readfile_11F4E+26Fj
		mov	edx, [di+2Ch]
		push	edi
		mov	bx, [di+34h]
		movzx	edi, bx
		and	di, 1FFFh
		and	bx, 0C000h
		shr	bx, 1
		or	di, bx
		shl	edi, 4
		add	edi, edx
		inc	edi
		pop	edi

loc_12215::				; CODE XREF: _mod_readfile_11F4E+269j
		pop	cx

loc_12216::				; CODE XREF: _mod_readfile_11F4E+1F2j
		add	di, 40h	; '@'
		dec	cx
		jnz	loc_1213C
		clc

locret_1221F::				; CODE XREF: _mod_readfile_11F4E+95j
		retn
_mod_readfile_11F4E endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_mod_sub_12220 proc near	; CODE XREF: _mod_readfile_11F4E+158p
					; _mod_readfile_11F4E+195p ...
		cmp	_byte_24674, 1
		jz	short loc_12228
		retn
; ---------------------------------------------------------------------------

loc_12228::				; CODE XREF: _mod_sub_12220+5j
		mov	al, _byte_24676
		cmp	_byte_24675, 0
		jz	short loc_12239
		mov	_byte_24675, 0
		xor	al, al

loc_12239::				; CODE XREF: _mod_sub_12220+10j
					; _mod_sub_12220+21j
		add	al, es:[si]
		mov	es:[si], al
		inc	si
		dec	cx
		jnz	short loc_12239
		mov	_byte_24676, al
		retn
_mod_sub_12220 endp


; =============== S U B	R O U T	I N E =======================================


_mod_readfile_12247 proc near	; CODE XREF: _mod_readfile_11F4E+68p
		mov	edi, ecx
		xor	esi, esi
		mov	bp, es

loc_1224F::				; CODE XREF: _mod_readfile_12247+98j
		push	dx
		cmp	dx, 0FFFFh
		jz	short loc_12262
		push	dx
		push	cx
		push	bx
		push	ax
		mov	bx, dx
		call	_ems_mapmem
		pop	ax
		pop	bx
		pop	cx
		pop	dx

loc_12262::				; CODE XREF: _mod_readfile_12247+Cj
		xor	dx, dx
		mov	ecx, 8000h
		cmp	edi, ecx
		ja	short loc_12271
		mov	cx, di

loc_12271::				; CODE XREF: _mod_readfile_12247+26j
		push	bx
		push	esi
		push	edi
		push	bp
		push	es
		push	ds
		mov	ds, bp
		assume ds:nothing
		mov	ah, 3Fh
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		pop	ds
		assume ds:seg003
		pushf
		push	ax
		or	ax, ax
		jz	short loc_122B8
		push	ax
		push	bp
		mov	es, bp
		assume es:nothing
		xor	si, si
		mov	cx, ax
		call	_mod_sub_12220
		pop	bp
		pop	ax
		cmp	_byte_24673, 0
		jz	short loc_122B8
		mov	fs, bp
		assume fs:nothing
		mov	cx, ax
		add	cx, 3
		shr	cx, 2
		jz	short loc_122B8
		mov	edx, 80808080h
		xor	si, si

loc_122AE::				; CODE XREF: _mod_readfile_12247+6Fj
		xor	fs:[si], edx
		add	si, 4
		dec	cx
		jnz	short loc_122AE

loc_122B8::				; CODE XREF: _mod_readfile_12247+3Dj
					; _mod_readfile_12247+51j ...
		pop	ax
		popf
		pop	es
		pop	bp
		pop	edi
		pop	esi
		pop	bx
		pop	dx
		jb	short locret_122E7
		or	ax, ax
		jz	short loc_122E3
		movzx	eax, ax
		add	esi, eax
		cmp	dx, 0FFFFh
		jnz	short loc_122DA
		add	bp, 800h
		jmp	short loc_122DC
; ---------------------------------------------------------------------------

loc_122DA::				; CODE XREF: _mod_readfile_12247+8Bj
		inc	dx
		inc	dx

loc_122DC::				; CODE XREF: _mod_readfile_12247+91j
		sub	edi, eax
		ja	loc_1224F

loc_122E3::				; CODE XREF: _mod_readfile_12247+7Fj
		mov	ecx, esi
		clc

locret_122E7::				; CODE XREF: _mod_readfile_12247+7Bj
		retn
_mod_readfile_12247 endp


; =============== S U B	R O U T	I N E =======================================


_ems_mapmemx proc near	; CODE XREF: _mod_readfile_11F4E+75p
		test	byte ptr [di+3Ch], 8
		jnz	loc_12386
		mov	ecx, [di+2Ch]
		inc	ecx
		mov	ebx, ecx
		mov	edx, [di+20h]
		add	edx, 800h
		cmp	word ptr [di+32h], 0FFFFh
		jz	short loc_1234E
		push	eax
		push	ecx
		push	edx
		shr	ebx, 0Eh
		add	bx, [di+32h]
		call	_ems_mapmem
		pop	edx
		pop	ecx
		pop	eax
		push	eax
		push	ecx
		push	edx
		mov	ebx, edx
		shr	ebx, 0Eh
		add	bx, [di+32h]
		call	_ems_mapmem2
		pop	edx
		pop	ecx
		pop	eax
		and	ecx, 3FFFh
		and	edx, 3FFFh
		add	edx, 8000h

loc_1234E::				; CODE XREF: _ems_mapmemx+20j
		mov	si, dx
		and	si, 0Fh
		shr	edx, 4
		add	dx, ax
		mov	fs, dx
		mov	di, cx
		and	di, 0Fh
		shr	ecx, 4
		add	cx, ax
		mov	es, cx
		assume es:nothing
		mov	cx, 200h
		cld

loc_1236C::				; CODE XREF: _ems_mapmemx+9Bj
		mov	eax, es:[di]
		mov	fs:[si], eax
		mov	dword ptr es:[di], 0
		add	si, 4
		add	di, 4
		dec	cx
		jnz	short loc_1236C
		retn
; ---------------------------------------------------------------------------

loc_12386::				; CODE XREF: _ems_mapmemx+4j
		push	ax
		push	di
		mov	ecx, [di+2Ch]
		inc	ecx
		mov	ebx, ecx
		cmp	word ptr [di+32h], 0FFFFh
		jz	short loc_123B0
		push	ecx
		push	eax
		shr	ebx, 0Eh
		add	bx, [di+32h]
		call	_ems_mapmem
		pop	eax
		pop	ecx
		and	ecx, 3FFFh

loc_123B0::				; CODE XREF: _ems_mapmemx+ADj
		mov	si, cx
		and	si, 0Fh
		shr	ecx, 4
		add	cx, ax
		mov	fs, cx
		mov	ecx, [di+20h]
		add	ecx, 800h
		mov	ebx, ecx
		cmp	word ptr [di+32h], 0FFFFh
		jz	short loc_123EE
		push	ecx
		push	eax
		shr	ebx, 0Eh
		add	bx, [di+32h]
		call	_ems_mapmem2
		pop	eax
		pop	ecx
		and	ecx, 3FFFh
		add	cx, 8000h

loc_123EE::				; CODE XREF: _ems_mapmemx+E7j
		mov	bx, cx
		and	bx, 0Fh
		shr	ecx, 4
		add	cx, ax
		mov	es, cx
		mov	di, bx
		mov	cx, 200h
		cld
		rep movs dword	ptr es:[di], dword ptr fs:[si]
		pop	di
		pop	ax
		mov	ecx, [di+24h]
		mov	ebx, ecx
		cmp	word ptr [di+32h], 0FFFFh
		jz	short loc_1242D
		push	ecx
		push	eax
		shr	ebx, 0Eh
		add	bx, [di+32h]
		call	_ems_mapmem
		pop	eax
		pop	ecx
		and	ecx, 3FFFh

loc_1242D::				; CODE XREF: _ems_mapmemx+12Aj
		mov	si, cx
		and	si, 0Fh
		shr	ecx, 4
		add	cx, ax
		mov	fs, cx
		mov	ecx, [di+2Ch]
		inc	ecx
		mov	ebx, ecx
		cmp	word ptr [di+32h], 0FFFFh
		jz	short loc_12466
		push	ecx
		push	eax
		shr	ebx, 0Eh
		add	bx, [di+32h]
		call	_ems_mapmem2
		pop	eax
		pop	ecx
		and	ecx, 3FFFh
		add	cx, 8000h

loc_12466::				; CODE XREF: _ems_mapmemx+15Fj
		mov	bx, cx
		and	bx, 0Fh
		shr	ecx, 4
		add	cx, ax
		mov	es, cx
		cmp	dword ptr [di+28h], 800h
		ja	short loc_12497
		mov	edx, [di+28h]
		add	dx, si
		mov	di, bx
		mov	bp, si
		mov	cx, 800h
		cld

loc_1248B::				; CODE XREF: _ems_mapmemx+1ACj
		movs	byte ptr es:[di], byte	ptr fs:[si]
		cmp	si, dx
		jb	short loc_12493
		mov	si, bp

loc_12493::				; CODE XREF: _ems_mapmemx+1A7j
		dec	cx
		jnz	short loc_1248B
		retn
; ---------------------------------------------------------------------------

loc_12497::				; CODE XREF: _ems_mapmemx+193j
		mov	di, bx
		mov	cx, 200h
		cld
		rep movs dword	ptr es:[di], dword ptr fs:[si]
		retn
_ems_mapmemx endp


; =============== S U B	R O U T	I N E =======================================


_ems_mapmemy proc near
		test	byte ptr [di+3Ch], 8
		jnz	loc_1253B
		mov	ecx, [di+2Ch]
		inc	ecx
		mov	ebx, ecx
		mov	edx, [di+20h]
		add	edx, 800h
		cmp	word ptr [di+32h], 0FFFFh
		jz	short loc_12508
		push	eax
		push	ecx
		push	edx
		shr	ebx, 0Eh
		add	bx, [di+32h]
		call	_ems_mapmem
		pop	edx
		pop	ecx
		pop	eax
		push	eax
		push	ecx
		push	edx
		mov	ebx, edx
		shr	ebx, 0Eh
		add	bx, [di+32h]
		call	_ems_mapmem2
		pop	edx
		pop	ecx
		pop	eax
		and	ecx, 3FFFh
		and	edx, 3FFFh
		add	edx, 8000h

loc_12508::				; CODE XREF: _ems_mapmemy+20j
		mov	si, dx
		and	si, 0Fh
		shr	edx, 4
		add	dx, ax
		mov	fs, dx
		mov	di, cx
		and	di, 0Fh
		shr	ecx, 4
		add	cx, ax
		mov	es, cx
		mov	cx, 200h
		xor	eax, eax
		cld

loc_12529::				; CODE XREF: _ems_mapmemy+96j
		mov	eax, fs:[si]
		mov	es:[di], eax
		add	si, 4
		add	di, 4
		dec	cx
		jnz	short loc_12529
		retn
; ---------------------------------------------------------------------------

loc_1253B::				; CODE XREF: _ems_mapmemy+4j
		mov	ecx, [di+20h]
		add	ecx, 800h
		mov	ebx, ecx
		cmp	word ptr [di+32h], 0FFFFh
		jz	short loc_12568
		push	ecx
		push	eax
		shr	ebx, 0Eh
		add	bx, [di+32h]
		call	_ems_mapmem
		pop	eax
		pop	ecx
		and	ecx, 3FFFh

loc_12568::				; CODE XREF: _ems_mapmemy+ABj
		mov	si, cx
		and	si, 0Fh
		shr	ecx, 4
		add	cx, ax
		mov	fs, cx
		mov	ecx, [di+2Ch]
		inc	ecx
		mov	ebx, ecx
		cmp	word ptr [di+32h], 0FFFFh
		jz	short loc_125A1
		push	ecx
		push	eax
		shr	ebx, 0Eh
		add	bx, [di+32h]
		call	_ems_mapmem2
		pop	eax
		pop	ecx
		and	ecx, 3FFFh
		add	cx, 8000h

loc_125A1::				; CODE XREF: _ems_mapmemy+E0j
		mov	bx, cx
		and	bx, 0Fh
		shr	ecx, 4
		add	cx, ax
		mov	es, cx
		mov	di, bx
		mov	cx, 200h
		cld
		rep movs dword	ptr es:[di], dword ptr fs:[si]
		retn
_ems_mapmemy endp


; =============== S U B	R O U T	I N E =======================================


_deinit_125B9 proc far	; CODE XREF: _start:loc_1907CP
					; _start+1BFP
		pushf
		push	ds
		mov	ax, seg003
		mov	ds, ax
		push	cs
		call	near ptr _snd_offx
		push	cs
		call	near ptr _memfree_125DA
		call	_ems_deinit
		mov	ax, _word_2460C
		call	_setmemallocstrat
		call	_snd_deinit
		call	_initclockfromrtc
		pop	ds
		popf
		retf
_deinit_125B9 endp

		assume ds:dseg

; =============== S U B	R O U T	I N E =======================================


_memfree_125DA proc far	; CODE XREF: _moduleread+7p
					; _moduleread+ADp ...
		push	ds
		mov	ax, seg003
		mov	ds, ax
		assume ds:seg003
		call	_ems_realloc
		cmp	word ptr [_dword_24640+2], 0
		jz	short loc_125F6
		call	_memfree_18A28
		mov	_dword_24640, 0

loc_125F6::				; CODE XREF: _memfree_125DA+Ej
		cmp	_byte_24665, 1
		jnz	short loc_1265B
		test	_sndflags_24622, 4
		jnz	short loc_1263D
		test	_sndflags_24622, 10h
		jnz	short loc_1263D
		mov	di, offset _myout
		mov	cx, _word_245D2

loc_12612::				; CODE XREF: _memfree_125DA+61j
		push	cx
		test	byte ptr [di+3Ch], 1
		jz	short loc_12636
		cmp	word ptr [di+32h], 0FFFFh
		jnz	short loc_12636
		cmp	word ptr [di+30h], 0
		jz	short loc_12636
		mov	ax, [di+30h]
		push	di
		call	_memfree
		pop	di
		and	byte ptr [di+3Ch], 0FEh
		mov	word ptr [di+30h], 0

loc_12636::				; CODE XREF: _memfree_125DA+3Dj
					; _memfree_125DA+43j ...
		pop	cx
		add	di, 40h	; '@'
		dec	cx
		jnz	short loc_12612

loc_1263D::				; CODE XREF: _memfree_125DA+28j
					; _memfree_125DA+2Fj
		mov	di, offset _segs_table
		mov	cx, _word_245F2

loc_12644::				; CODE XREF: _memfree_125DA+7Fj
		mov	ax, [di]
		or	ax, ax
		jz	short loc_12655
		push	cx
		push	di
		call	_memfree
		pop	di
		pop	cx
		mov	word ptr [di], 0

loc_12655::				; CODE XREF: _memfree_125DA+6Ej
		add	di, 2
		dec	cx
		jnz	short loc_12644

loc_1265B::				; CODE XREF: _memfree_125DA+21j
		pop	ds
		retf
_memfree_125DA endp

		assume ds:dseg

; =============== S U B	R O U T	I N E =======================================


sub_1265D proc far		; CODE XREF: _read_module+86P
					; _keyb_screen_loopP ...
		mov	ax, seg003
		mov	es, ax
		assume es:seg003
		mov	ax, es:[_volume_245FC]
		dec	ax
		mov	cl, al
		mov	si, offset _channels_25908
		mov	di, offset asc_246B0 ; "				"
		movzx	bp, es:[_sndcard_type]
		mov	ch, es:[_byte_24666]
		mov	bh, es:[_byte_24667]
		mov	dl, es:[_sndflags_24622]
		mov	dh, es:[_byte_24628]
		dec	dh
		and	dh, 3
		shl	dh, 1
		or	dh, es:[_is_stereo]
		shl	dh, 1
		or	dh, es:[_byte_24671]
		shl	dh, 3
		mov	al, byte ptr es:[_word_245F6]
		mov	ah, byte ptr es:[_word_245F0]
		retf
sub_1265D endp


; =============== S U B	R O U T	I N E =======================================


sub_126A9 proc far		; CODE XREF: _read_module+6AP
					; _text_init2+225P
		mov	ax, seg003
		mov	es, ax
		mov	di, offset asc_246B0 ; "				"
		mov	si, offset _myout
		mov	bl, byte ptr es:[_word_245FA]
		mov	bh, byte ptr es:[_word_245D2]
		mov	cl, byte ptr es:[_mod_channels_number]
		mov	ch, es:[_byte_24617]
		mov	eax, es:[_module_type_text]
		retf
sub_126A9 endp


; =============== S U B	R O U T	I N E =======================================


_volume_prep proc far		; CODE XREF: seg001:18BEP
					; _f2_draw_waves+CP ...
		push	ds
		mov	bx, seg003
		mov	ds, bx
		assume ds:seg003
		mov	_word_24610, ax
		mov	_my_size, cx
		test	_sndflags_24622, 4
		jnz	short loc_12702
		push	di
		push	es
		call	_ems_save_mapctx
		pop	es
		assume es:nothing
		pop	di
		mov	si, offset _channels_25908
		mov	dx, _mod_channels_number

loc_126F0::				; CODE XREF: _volume_prep+2Dj
		push	dx
		push	si
		call	_volume_prepare_waves
		pop	si
		pop	dx
		add	si, 50h	; 'P'
		dec	dx
		jnz	short loc_126F0
		call	_ems_restore_mapctx
		pop	ds
		retf
; ---------------------------------------------------------------------------

loc_12702::				; CODE XREF: _volume_prep+12j
		push	di
		push	es
		cmp	word ptr [_dword_24640+2], 0
		jnz	short loc_12721
		mov	eax, 800h
		mov	cl, _dma_channel_0
		call	_alloc_dma_buf
		mov	word ptr [_dword_24640+2], ax
		mov	word ptr [_dword_24640],	0

loc_12721::				; CODE XREF: _volume_prep+3Bj
		mov	ax, ds
		mov	es, ax
		assume es:seg003
		cld
		mov	si, offset _channels_25908
		mov	cx, _mod_channels_number

loc_1272D::				; CODE XREF: _volume_prep+87j
		pushf
		cli
		mov	dx, _gravis_port
		dec	dx
		mov	al, [si+18h]
		out	dx, al
		inc	dx
		mov	al, 8Ah	; 'ä'
		out	dx, al
		inc	dx
		in	ax, dx
		dec	dx
		and	ah, 1Fh
		shl	eax, 10h
		mov	al, 8Bh	; 'ã'
		out	dx, al
		inc	dx
		in	ax, dx
		dec	dx
		popf
		mov	[si+4],	eax
		add	si, 50h	; 'P'
		dec	cx
		jnz	short loc_1272D
		pop	es
		assume es:nothing
		pop	di
		mov	si, offset _channels_25908
		mov	ax, _mod_channels_number

loc_1275F::				; CODE XREF: _volume_prep+C8j
		push	ax
		push	si
		test	byte ptr [si+17h], 1
		jnz	short loc_1276C
		call	_memclean
		jmp	short loc_1278F
; ---------------------------------------------------------------------------

loc_1276C::				; CODE XREF: _volume_prep+97j
					; _volume_prep+A3j
		cmp	_byte_2466E, 1
		jz	short loc_1276C
		push	si
		push	di
		push	es
		mov	eax, _dword_24640
		call	sub_1279A
		pop	es
		pop	di
		pop	si

loc_12780::				; CODE XREF: _volume_prep+B7j
		cmp	_byte_2466E, 1
		jz	short loc_12780
		lfs	ax, _dword_24640
		call	sub_1281A

loc_1278F::				; CODE XREF: _volume_prep+9Cj
		pop	si
		pop	ax
		add	si, 50h	; 'P'
		dec	al
		jnz	short loc_1275F
		pop	ds
		retf
_volume_prep endp


; =============== S U B	R O U T	I N E =======================================


sub_1279A proc near		; CODE XREF: _volume_prep+ACp
		mov	_dma_buf_pointer, eax
		mov	ax, _word_24610
		xor	ah, ah
		imul	ax, [si+20h]
		mul	_my_size
		shrd	ax, dx,	8
		add	ax, 30h	; '0'
		test	_word_24610, 8000h
		jz	short loc_127BD
		add	ax, 100h

loc_127BD::				; CODE XREF: sub_1279A+1Ej
		test	byte ptr [si+19h], 4
		setnz	cl
		shl	ax, cl
		cmp	ax, 800h
		jb	short loc_127CE
		mov	ax, 800h

loc_127CE::				; CODE XREF: sub_1279A+2Fj
		mov	cx, ax
		mov	eax, [si+4]
		shr	eax, 0Dh
		and	al, 0FEh
		test	byte ptr [si+19h], 4
		jz	sub_182DB
		mov	eax, [si+4]
		shr	eax, 0Dh
		mov	dx, ax
		and	dx, 0C000h
		and	ax, 1FFFh
		shl	ax, 1
		or	ax, dx
		push	cx
		call	sub_182DB
		pop	cx

loc_127FC::				; CODE XREF: sub_1279A+67j
		cmp	_byte_2466E, 1
		jz	short loc_127FC
		shr	cx, 1
		push	ds
		lds	di, _dma_buf_pointer
		lea	bx, [di+1]

loc_1280D::				; CODE XREF: sub_1279A+7Cj
		mov	al, [bx]
		mov	[di], al
		add	bx, 2
		inc	di
		dec	cx
		jnz	short loc_1280D
		pop	ds
		retn
sub_1279A endp


; =============== S U B	R O U T	I N E =======================================


sub_1281A proc near		; CODE XREF: _volume_prep+BEp
		shl	eax, 10h
		mov	ax, _word_24610
		xor	ah, ah
		mul	word ptr [si+20h]
		mov	bp, ax
		shr	bp, 8
		mov	dh, al
		xor	dl, dl
		shr	eax, 10h
		jmp	short loc_12898
sub_1281A endp


; =============== S U B	R O U T	I N E =======================================


_volume_prepare_waves proc near ; CODE XREF: _volume_prep+24p
		test	byte ptr [si+17h], 1
		jz	_memclean
		test	_sndflags_24622, 1
		jz	_memclean
		push	di
		push	es
		mov	bx, [si+26h]
		mov	eax, [si+4]
		shr	eax, 22
		add	bx, ax
		push	si
		call	_ems_mapmem
		pop	si
		mov	eax, [si+4]
		mov	bx, ax
		shr	eax, 12
		cmp	word ptr [si+26h], 0FFFFh
		jz	short loc_12870
		and	eax, 3FFh

loc_12870::				; CODE XREF: _volume_prepare_waves+33j
		add	ax, [si+24h]
		mov	fs, ax
		mov	ax, _word_24610
		xor	ah, ah
		mul	word ptr [si+20h]
		mul	_freq1
		mov	bp, 22050
		div	bp
		mov	dx, ax
		movzx	ax, bh
		and	al, 0Fh
		mov	bp, dx
		shr	bp, 8
		mov	dh, dl
		mov	dl, bl
		pop	es
		pop	di

loc_12898::				; CODE XREF: sub_1281A+19j
		movzx	ebx, byte ptr [si+23h]
		mov	si, ax
		test	_word_24610, 4000h
		jz	short loc_128BB
		cmp	_amplification, 120
		jbe	short loc_128BB
		mov	ax, 100
		push	dx
		mul	bx
		div	_amplification
		pop	dx
		mov	bx, ax

loc_128BB::				; CODE XREF: _volume_prepare_waves+70j
					; _volume_prepare_waves+77j
		shl	ebx, 9
		add	bx, offset _vlm_byte_table
		inc	bx
		mov	cx, _my_size
		test	_word_24610, 8000h
		jz	short loc_1291E
		shl	ecx, 16
		shl	esi, 16
		mov	si, ax
		mov	cx, 100h

loc_128DD::				; CODE XREF: _volume_prepare_waves+DFj
		mov	eax, fs:[si]
		inc	si
		or	al, al
		jns	short loc_12913
		or	ah, ah
		js	short loc_12913
		ror	eax, 8
		cmp	al, ah
		jg	short loc_12913
		ror	eax, 8
		cmp	al, ah
		jg	short loc_12913
		rol	eax, 16
		mov	ax, fs:[si+3]
		rol	eax, 8
		cmp	al, ah
		jg	short loc_12913
		ror	eax, 8
		inc	si
		cmp	al, ah
		jle	short loc_1291A

loc_12913::				; CODE XREF: _volume_prepare_waves+AFj
					; _volume_prepare_waves+B3j ...
		dec	cx
		jnz	short loc_128DD
		shr	esi, 16

loc_1291A::				; CODE XREF: _volume_prepare_waves+DCj
		shr	ecx, 16

loc_1291E::				; CODE XREF: _volume_prepare_waves+99j
		xor	eax, eax

loc_12921::				; CODE XREF: _volume_prepare_waves+21Cj
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	short locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	short locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	short locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	short locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	short locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jz	short locret_12A55
		mov	al, fs:[si]
		mov	al, [ebx+eax*2]
		mov	es:[di], al
		add	dl, dh
		inc	di
		adc	si, bp
		dec	cx
		jnz	loc_12921

locret_12A55::				; CODE XREF: _volume_prepare_waves+FCj
					; _volume_prepare_waves+110j ...
		retn
_volume_prepare_waves endp


; =============== S U B	R O U T	I N E =======================================


_memclean proc near		; CODE XREF: _volume_prep+99p
					; _volume_prepare_waves+4j ...
		cld
		mov	cx, _my_size
		xor	ax, ax
		shr	cx, 1
		rep stosw
		adc	cx, cx
		rep stosb
		retn
_memclean endp


; =============== S U B	R O U T	I N E =======================================


_volume_12A66 proc far	; CODE XREF: _vlm_141DF+1p _snd_off+14p
		push	ds
		mov	ax, seg003
		mov	ds, ax
		mov	cx, _mod_channels_number
		mov	bx, offset _channels_25908

loc_12A73::				; CODE XREF: _volume_12A66+19j
		push	bx
		push	cx
		call	[off_245CE]
		pop	cx
		pop	bx
		add	bx, 50h	; 'P'
		dec	cx
		jnz	short loc_12A73
		pop	ds
		retf
_volume_12A66 endp

		assume ds:dseg

; =============== S U B	R O U T	I N E =======================================


_change_volume proc far	; CODE XREF: _keyb_screen_loop+17P
					; _keyb_screen_loop+23AP ...
		push	ds
		mov	cx, seg003
		mov	ds, cx
		assume ds:seg003
		cmp	ax, -1
		jz	short loc_12AA9
		mov	_volume_245FC,	ax
		mov	cx, _mod_channels_number
		mov	bx, offset _channels_25908

loc_12A98::				; CODE XREF: _change_volume+24j
		push	bx
		push	cx
		mov	al, [bx+8]
		call	[off_245CC]
		pop	cx
		pop	bx
		add	bx, 50h	; 'P'
		dec	cx
		jnz	short loc_12A98

loc_12AA9::				; CODE XREF: _change_volume+9j
		mov	ax, _volume_245FC
		pop	ds
		assume ds:dseg
		retf
_change_volume endp


; =============== S U B	R O U T	I N E =======================================


_getset_amplif proc far	; CODE XREF: _s3m_module+84p
					; _eff_14020+9p	...
		push	ds
		mov	cx, seg003
		mov	ds, cx
		assume ds:seg003
		cmp	ax, -1
		jz	short loc_12ACE
		mov	_amplification, ax
		mov	_byte_24625, 0
		cmp	ax, 100
		jbe	short loc_12ACB
		mov	_byte_24625, 1

loc_12ACB::				; CODE XREF: _getset_amplif+16j
		call	sub_13044

loc_12ACE::				; CODE XREF: _getset_amplif+9j
		mov	ax, _amplification
		pop	ds
		assume ds:dseg
		retf
_getset_amplif endp


; =============== S U B	R O U T	I N E =======================================


_get_playsettings proc far	; CODE XREF: _keyb_screen_loop+2AP
					; _keyb_screen_loop+350P ...
		push	ds
		mov	ax, seg003
		mov	ds, ax
		assume ds:seg003
		mov	al, _flag_playsetttings
		pop	ds
		assume ds:dseg
		retf
_get_playsettings endp


; =============== S U B	R O U T	I N E =======================================


_set_playsettings proc far	; CODE XREF: _keyb_screen_loop+357P
					; _keyb_screen_loop+36FP ...
		push	ds
		mov	bx, seg003
		mov	ds, bx
		assume ds:seg003
		mov	_flag_playsetttings, al
		call	_someplaymode
		and	byte ptr [_config_word+1], 0FEh
		test	_flag_playsetttings, 10h
		jz	short loc_12AFB
		or	byte ptr [_config_word+1], 1

loc_12AFB::				; CODE XREF: _set_playsettings+16j
		pop	ds
		retf
_set_playsettings endp


; =============== S U B	R O U T	I N E =======================================


sub_12AFD proc far		; CODE XREF: _keyb_screen_loop+1F9P
		push	ds
		mov	bx, seg003
		mov	ds, bx
		movzx	bx, ch
		cmp	bx, _mod_channels_number
		jnb	short loc_12B16
		imul	bx, 80
		add	bx, offset _channels_25908
		call	_eff_13A43

loc_12B16::				; CODE XREF: sub_12AFD+Dj
		pop	ds
		retf
sub_12AFD endp


; =============== S U B	R O U T	I N E =======================================


sub_12B18 proc far		; CODE XREF: _moduleread:loc_10092p
					; sub_12EBA+5Fp
		push	ax
		push	ds
		push	es
		mov	ax, seg003
		mov	es, ax
		assume es:seg003
		mov	di, offset _dword_27BC8
		mov	cx, 8
		cld
		rep movsd
		mov	ds, ax
		mov	_byte_2461C, 0
		mov	_byte_2461D, 0
		mov	si, offset _dword_27BC8
		mov	bx, offset _channels_25908
		mov	cx, _mod_channels_number
		xor	al, al

loc_12B42::				; CODE XREF: sub_12B18+65j
		push	ax
		mov	[bx+18h], al
		mov	al, [si]
		mov	[bx+3Ah], al
		test	_sndflags_24622, 4
		jnz	short loc_12B5F
		cmp	al, 40h	; '@'
		mov	al, 0
		jb	short loc_12B5A
		mov	al, 80h	; 'Ä'

loc_12B5A::				; CODE XREF: sub_12B18+3Ej
		mov	[bx+3Ah], al
		jmp	short loc_12B62
; ---------------------------------------------------------------------------

loc_12B5F::				; CODE XREF: sub_12B18+38j

loc_12B62::				; CODE XREF: sub_12B18+45j
		mov	al, [bx+3Ah]
		cmp	al, 0
		jz	short loc_12B71
		inc	_byte_2461D
		cmp	al, 80h	; 'Ä'
		jz	short loc_12B75

loc_12B71::				; CODE XREF: sub_12B18+4Fj
		inc	_byte_2461C

loc_12B75::				; CODE XREF: sub_12B18+57j
		pop	ax
		add	bx, 80
		inc	si
		inc	al
		dec	cx
		jnz	short loc_12B42
		pop	es
		assume es:nothing
		pop	ds
		pop	ax
		retf
sub_12B18 endp


; =============== S U B	R O U T	I N E =======================================


sub_12B83 proc far		; CODE XREF: _moduleread+8Bp
		push	ax
		push	ds
		mov	bx, seg003
		mov	ds, bx
		xor	ah, ah
		cmp	al, 20h	; ' '
		jb	short loc_12B92
		mov	al, 20h	; ' '

loc_12B92::				; CODE XREF: sub_12B83+Bj
		cmp	al, 2
		ja	short loc_12B98
		mov	al, 2

loc_12B98::				; CODE XREF: sub_12B83+11j
		mov	_mod_channels_number, ax
		mov	di, offset _channels_25908
		mov	cx, _mod_channels_number
		xor	dx, dx
		xor	bx, bx

loc_12BA6::				; CODE XREF: sub_12B83+4Cj
		cmp	byte ptr [di+1Dh], 0
		jnz	short loc_12BB3
		mov	[di+18h], dl
		inc	dl
		jmp	short loc_12BCB
; ---------------------------------------------------------------------------

loc_12BB3::				; CODE XREF: sub_12B83+27j
		cmp	byte ptr [di+1Dh], 1
		jnz	short loc_12BC0
		mov	[di+18h], dh
		inc	dh
		jmp	short loc_12BCB
; ---------------------------------------------------------------------------

loc_12BC0::				; CODE XREF: sub_12B83+34j
		cmp	byte ptr [di+1Dh], 2
		jnz	short loc_12BCB
		mov	[di+18h], bl
		inc	bl

loc_12BCB::				; CODE XREF: sub_12B83+2Ej
					; sub_12B83+3Bj ...
		add	di, 80
		dec	cx
		jnz	short loc_12BA6
		xor	ah, ah
		mov	al, dl
		mov	_word_245D6, ax
		mov	al, dh
		mov	_word_245D8, ax
		mov	al, bl
		mov	_word_245DA, ax
		test	_sndflags_24622, 4
		jz	short loc_12BEF
		mov	ax, _word_245D6

loc_12BEF::				; CODE XREF: sub_12B83+64j
		call	sub_13044
		call	_someplaymode
		pop	ds
		pop	ax
		retf
sub_12B83 endp


; =============== S U B	R O U T	I N E =======================================


_someplaymode proc near	; CODE XREF: _set_playsettings+9p
					; sub_12B83+6Fp
		mov	edx, 3
		mov	eax, 1775763456
		mov	ecx, 369D800h
		cmp	_byte_2461A, 0
		jnz	short loc_12C3C
		mov	edx, 3
		mov	eax, 1643177984
		mov	ecx, 361F0F0h
		test	_flag_playsetttings, 8
		jnz	short loc_12C3C
		mov	edx, 3
		mov	eax, 1776914432
		mov	ecx, 369E990h

loc_12C3C::				; CODE XREF: _someplaymode+17j
					; _someplaymode+30j
		mov	_dword_245C0, ecx
		movzx	edi, _freq1
		mov	cl, _byte_2461A
		shl	edi, cl
		div	edi
		mov	_dword_245BC, eax
		test	_sndflags_24622, 4
		jz	short loc_12C86
		movzx	ecx, _byte_24629
		mov	eax, 385532977
		test	_flag_playsetttings, 8
		jnz	short loc_12C75
		mov	eax, 389081954

loc_12C75::				; CODE XREF: _someplaymode+75j
		mul	ecx
		mov	cl, 12
		add	cl, _byte_2461A
		shrd	eax, edx, cl
		mov	_dword_2463C, eax

loc_12C86::				; CODE XREF: _someplaymode+62j
		mov	di, offset _channels_25908
		mov	cx, _mod_channels_number
		xor	ax, ax

loc_12C8F::				; CODE XREF: _someplaymode+9Ej
		mov	[di+3Eh], ax
		add	di, 50h	; 'P'
		dec	cx
		jnz	short loc_12C8F
		retn
_someplaymode endp

		assume ds:dseg

; =============== S U B	R O U T	I N E =======================================


_getset_playstate proc far	; CODE XREF: _keyb_screen_loop+401P
					; _keyb_screen_loop:loc_1A30DP ...
		push	bx
		push	ds
		mov	bx, seg003
		mov	ds, bx
		assume ds:seg003
		cmp	al, 0FFh
		jz	short loc_12CA7
		mov	_play_state, al

loc_12CA7::				; CODE XREF: _getset_playstate+9j
		mov	al, _play_state
		pop	ds
		assume ds:dseg
		pop	bx
		retf
_getset_playstate endp


; =============== S U B	R O U T	I N E =======================================


sub_12CAD proc far		; CODE XREF: _keyb_screen_loop+3CCP
					; _keyb_screen_loop+3DCP ...
		push	ds
		push	es
		mov	ax, seg003
		mov	ds, ax
		assume ds:seg003
		mov	es, ax
		assume es:seg003
		mov	si, offset _word_246A6
		mov	al, ch
		or	al, 0E0h
		mov	_word_246A9, bx
		mov	_byte_246A8, cl
		mov	_word_246A6, dx
		call	sub_13623
		pop	es
		assume es:nothing
		pop	ds
		retf
sub_12CAD endp

		assume ds:dseg

; =============== S U B	R O U T	I N E =======================================


_read_sndsettings proc far	; CODE XREF: _read_module+7DP
					; _callsubx+3DP
		push	ds
		mov	ax, seg003
		mov	ds, ax
		assume ds:seg003
		mov	al, _sndcard_type
		mov	dx, _snd_base_port
		mov	cl, _irq_number
		mov	ch, _dma_channel
		mov	ah, _freq_246D7
		mov	bl, _byte_246D8
		mov	bh, _byte_246D9
		mov	bp, _freq1
		test	_sndflags_24622, 4
		jz	short loc_12CFF
		mov	bp, _freq2

loc_12CFF::				; CODE XREF: _read_sndsettings+2Aj
		mov	si, _config_word
		pop	ds
		assume ds:dseg
		retf
_read_sndsettings endp


; =============== S U B	R O U T	I N E =======================================


sub_12D05 proc far		; CODE XREF: _start-2DP	_start+285P
		push	ds
		push	di
		push	es
		mov	ax, seg003
		mov	ds, ax
		assume ds:seg003
		mov	si, offset _aDeviceNotIniti ; "Device not initialised!"
		cmp	_snd_init, 1
		jnz	short loc_12D2E
		movzx	si, _sndcard_type
		shl	si, 1
		mov	si, offset _covox_txt
		mov	di, offset _chrin
		call	_myasmsprintf
		mov	byte ptr [di], 0
		mov	si, offset _chrin

loc_12D2E::				; CODE XREF: sub_12D05+10j
		pop	es
		assume es:dseg
		pop	di

loc_12D30::
		call	_strcpy_count_0
		pop	ds
		assume ds:dseg
		retf
sub_12D05 endp


; =============== S U B	R O U T	I N E =======================================


sub_12D35 proc far		; CODE XREF: _dosexec+5CP _dosexec+8BP
		push	ax
		push	bx
		push	ds
		mov	bx, seg003
		mov	ds, bx
		assume ds:seg003
		cmp	al, 1
		jz	short loc_12D4E

loc_12D41::
		mov	cs:[_byte_14F71], 0
		call	_setmemalloc1
		pop	ds
		pop	bx
		pop	ax
		retf
; ---------------------------------------------------------------------------

loc_12D4E::				; CODE XREF: sub_12D35+Aj
		mov	cs:[_byte_14F71], 1
		mov	ax, _word_2460C
		call	_setmemallocstrat
		call	_initclockfromrtc
		pop	ds
		pop	bx
		pop	ax
		retf
sub_12D35 endp


; =============== S U B	R O U T	I N E =======================================




		assume ds:dseg

; =============== S U B	R O U T	I N E =======================================


sub_12DA8 proc far		; CODE XREF: _callsubx+24P
		pushf
		cli
		push	ds
		mov	bp, seg003
		mov	ds, bp
		assume ds:seg003
		mov	_sndcard_type,	al
		mov	_snd_base_port, dx
		mov	_irq_number, cl
		mov	_dma_channel, ch
		mov	_freq_246D7, ah
		mov	_byte_246D8, bl
		mov	_byte_246D9, bh
		movzx	ax, ah
		imul	ax, 1000
		mov	_freq1, ax
		mov	_config_word, si
		mov	ax, di
		mov	_byte_246DC, 4Bh ; 'K'
		mov	[off_245CA], offset sub_13177
		mov	[off_245C8], offset sub_13429
		mov	[off_245CC], offset sub_131EF
		mov	[off_245CE], offset sub_131DA
		mov	_is_stereo, 0
		mov	_bit_mode, 8
		mov	_word_245E8, 400h
		mov	_snd_set_flag,	0
		mov	al, 8
		call	_getint_vect
		mov	word ptr cs:[_int8addr],	bx
		mov	word ptr cs:[_int8addr+2], dx
		push	cs
		call	near ptr _clean_11C43
		call	_snd_initialze
		jb	loc_12EB2
		call	_getmemallocstrat
		mov	_word_2460C, ax
		call	_setmemalloc1
		mov	al, _byte_246DC
		mov	ah, al
		and	ax, 0F00Fh
		shr	ah, 4
		movzx	di, al
		mov	al, cs:[_byte_13C54+di]
		movzx	di, ah
		mov	ah, cs:[_byte_13C54+di]
		test	_sndflags_24622, 4
		jnz	short loc_12E55
		mov	ax, 80h	; 'Ä'

loc_12E55::				; CODE XREF: sub_12DA8+A8j
		mov	_byte_2461E, ah
		mov	_byte_2461F, al
		push	cs
		call	near ptr _clean_11C43
		mov	al, 0
		test	byte ptr [_config_word+1], 1
		jz	short loc_12E6B
		or	al, 10h

loc_12E6B::				; CODE XREF: sub_12DA8+BFj
		test	byte ptr [_config_word],	4
		jz	short loc_12E74
		or	al, 4

loc_12E74::				; CODE XREF: sub_12DA8+C8j
		test	byte ptr [_config_word],	80h
		jz	short loc_12E7D
		or	al, 8

loc_12E7D::				; CODE XREF: sub_12DA8+D1j
		mov	_flag_playsetttings, al
		mov	ax, 400h
		mov	cl, _is_stereo
		and	cl, 1
		cmp	_bit_mode, 16
		jnz	short loc_12E9F
		mov	[off_245E0], offset _myin
		mov	[off_245E2], offset _chrin
		inc	cl

loc_12E9F::				; CODE XREF: sub_12DA8+E7j
		shr	ax, cl
		mov	_word_245E8, ax
		test	_sndflags_24622, 1
		jz	short loc_12EAE
		call	_ems_init

loc_12EAE::				; CODE XREF: sub_12DA8+101j
		pop	ds
		assume ds:dseg
		popf
		clc
		retf
; ---------------------------------------------------------------------------

loc_12EB2::				; CODE XREF: sub_12DA8+7Bj
		mov	bx, ds
		mov	fs, bx
		assume fs:dseg
		pop	ds
		assume ds:seg003
		popf
		stc
		retf
sub_12DA8 endp

		assume ds:dseg

; =============== S U B	R O U T	I N E =======================================


sub_12EBA proc far		; CODE XREF: _read_module+E3P
		pushf
		cli
		push	ds
		push	es
		mov	ax, seg003
		mov	ds, ax
		assume ds:seg003
		mov	_byte_24669, 0
		mov	_byte_2466A, 0
		mov	_byte_2466B, 0
		mov	_byte_2466C, 0
		mov	_byte_2466D, 0
		mov	_byte_24671, 0
		mov	_play_state, 0
		mov	_samples_outoffs_24600, 0
		mov	_word_24602, 0
		mov	_byte_24620, 0
		mov	_byte_24621, 0
		mov	ax, ds
		mov	es, ax
		assume es:seg003
		mov	di, offset _channels_25908
		xor	eax, eax
		mov	cx, 280h
		cld
		rep stosd
		xor	ax, ax
		xor	bx, bx
		push	cs
		call	near ptr sub_12F56
		mov	si, offset _dword_27BC8
		push	cs
		call	near ptr sub_12B18
		mov	al, _byte_2467A
		call	sub_13CF6
		mov	al, _byte_24679
		call	_eff_13CE8
		mov	al, _byte_24679
		mov	_byte_24668, al
		movzx	ax, _byte_2467A
		shl	ax, 1
		mov	dl, 5
		div	dl
		mov	_byte_2467B, al
		mov	_byte_2467C, 0
		call	_snd_on
		pop	es
		pop	ds
		assume ds:dseg
		popf
		retf
sub_12EBA endp


; =============== S U B	R O U T	I N E =======================================


_snd_offx proc far		; CODE XREF: _moduleread+3p
					; _deinit_125B9+8p ...
		pushf
		cli
		push	ds
		mov	ax, seg003
		mov	ds, ax
		assume ds:seg003
		call	_snd_off
		pop	ds
		assume ds:dseg
		popf
		retf
_snd_offx endp

		assume ds:seg003

; =============== S U B	R O U T	I N E =======================================


sub_12F56 proc far		; CODE XREF: sub_12EBA+58p
					; _keyb_screen_loop+167P ...
		pushf
		cli
		push	ds
		push	es
		mov	cx, seg003
		mov	ds, cx
		mov	_word_245F0, ax
		mov	_byte_24669, bl
		push	bx
		call	sub_1415E
		pop	bx
		cmp	bh, 1
		jnz	short loc_12F78
		mov	_byte_24668, 0
		call	sub_135CA

loc_12F78::				; CODE XREF: sub_12F56+18j
		pop	es
		pop	ds
		popf
		retf
sub_12F56 endp


; =============== S U B	R O U T	I N E =======================================


_get_12F7C proc far		; CODE XREF: _keyb_screen_loop+148P
					; _keyb_screen_loop+174P
		pushf
		cli
		push	ds
		mov	ax, seg003
		mov	ds, ax
		mov	ax, _word_245F0
		mov	bx, _word_245F6
		pop	ds
		popf
		retf
_get_12F7C endp


; =============== S U B	R O U T	I N E =======================================


_set_timer_int proc near	; CODE XREF: _covox_init+2Dp
					; _stereo_init+30p ...
		mov	ebx, 1000h	; bytes
		push	dx		; dx = subrouting offset
		call	_memalloc
		pop	dx
		jb	short locret_12FB3
		mov	word ptr [_dma_buf_pointer], 0
		mov	word ptr [_dma_buf_pointer+2], ax
		push	ax
		push	dx
		call	_memfill8080
		pop	bx
		mov	dx, cs
		mov	al, 8
		call	_setint_vect
		pop	ax
		clc

locret_12FB3::				; CODE XREF: _set_timer_int+Bj
		retn
_set_timer_int endp


; =============== S U B	R O U T	I N E =======================================


_clean_int8_mem_timr proc near ; CODE	XREF: _covox_deinitp
					; _stereo_cleanp ...
		mov	dx, word ptr cs:[_int8addr+2]
		mov	bx, word ptr cs:[_int8addr]
		mov	al, 8
		call	_setint_vect
		call	_clean_timer
		mov	ax, word ptr [_dma_buf_pointer+2]
		call	_memfree
		retn
_clean_int8_mem_timr endp


; =============== S U B	R O U T	I N E =======================================


_configure_timer proc	near	; CODE XREF: _covox_onp _stereo_setp ...
		call	sub_13017
		pushf
		cli
		mov	dx, 12h
		mov	ax, 34DCh
		div	_freq1
		call	_set_timer
		mov	cs:[_byte_14F70], 1
		mov	ax, _word_245E4
		mov	cs:[audio_len], ax
		popf
		retn
_configure_timer endp


; =============== S U B	R O U T	I N E =======================================


_memfill8080 proc near	; CODE XREF: _set_timer_int+18p
					; _covox_offp ...
		pushf
		cli
		xor	ax, ax
		call	_set_timer
		mov	cs:[_byte_14F70], 0
		mov	cs:[audio_len], 1
		mov	es, word ptr [_dma_buf_pointer+2]
		assume es:nothing
		xor	di, di
		mov	cx, 400h
		mov	eax, 80808080h
		cld
		rep stosd
		popf
		retn
_memfill8080 endp


; =============== S U B	R O U T	I N E =======================================


sub_13017 proc near		; CODE XREF: _configure_timerp
					; _proaud_setp ...
		mov	di, offset _myout
		mov	cx, _word_245D2

loc_1301E::				; CODE XREF: sub_13017+19j
		test	byte ptr [di+3Ch], 8
		jnz	short loc_1302C
		mov	eax, [di+2Ch]
		mov	[di+24h], eax

loc_1302C::				; CODE XREF: sub_13017+Bj
		add	di, 40h	; '@'
		dec	cx
		jnz	short loc_1301E
		mov	_samples_outoffs_24600, 0

loc_13038::				; CODE XREF: sub_13017+2Aj
		call	prepare_samples
		cmp	_samples_outoffs_24600, 800h
		jbe	short loc_13038
		retn
sub_13017 endp


; =============== S U B	R O U T	I N E =======================================


sub_13044 proc near		; CODE XREF: _getset_amplif:loc_12ACBp
					; sub_12B83:loc_12BEFp
		mov	al, _byte_2467E
		cmp	al, 0
		jz	short loc_13080
		cmp	al, 1
		jz	short loc_1305A
		cmp	al, 2
		jz	short loc_1306D
		mov	_byte_2467E, 0
		jmp	short loc_13080
; ---------------------------------------------------------------------------

loc_1305A::				; CODE XREF: sub_13044+9j
		mov	_byte_2467D, 3Fh ; '?'
		mov	[off_2462E], offset _table_24798
		mov	[off_24656], offset _table_25221
		jmp	short loc_13091
; ---------------------------------------------------------------------------

loc_1306D::				; CODE XREF: sub_13044+Dj
		mov	_byte_2467D, 3Fh ; '?'
		mov	[off_2462E], offset _table_24818
		mov	[off_24656], offset _table_25261
		jmp	short loc_13091
; ---------------------------------------------------------------------------

loc_13080::				; CODE XREF: sub_13044+5j
					; sub_13044+14j
		mov	_byte_2467D, 40h ; '@'
		mov	[off_2462E], offset _table_24716
		mov	[off_24656], offset _table_251E0

loc_13091::				; CODE XREF: sub_13044+27j
					; sub_13044+3Aj
		mov	di, offset _vlm_byte_table
		movzx	eax, _word_245D6
		cmp	ax, 2
		ja	short loc_130A2
		mov	ax, 2

loc_130A2::				; CODE XREF: sub_13044+59j
		cmp	_is_stereo, 1
		jnz	short loc_130AE
		shr	ax, 1
		adc	ax, 0

loc_130AE::				; CODE XREF: sub_13044+63j
		movzx	ebp, ax
		mov	si, [off_24656]
		movzx	cx, _byte_2467D
		inc	cx

loc_130BC::				; CODE XREF: sub_13044+122j
		push	cx
		push	ebp
		movzx	eax, byte ptr [si]
		inc	si
		movzx	edx, _amplification
		shl	edx, 16
		mul	edx
		mov	ecx, 100
		div	ecx
		xor	edx, edx
		div	ebp
		mov	bp, ax
		shr	eax, 16
		mov	ecx, eax
		cmp	_byte_24625, 1
		jz	short loc_13120
		xor	ax, ax
		xor	dx, dx
		mov	bl, 80h	; 'Ä'

loc_130F6::				; CODE XREF: sub_13044+BDj
		mov	[di], ax
		add	dx, bp
		adc	ax, cx
		add	di, 2
		dec	bl
		jnz	short loc_130F6
		add	di, 100h
		xor	ax, ax
		xor	dx, dx
		mov	bl, 80h	; 'Ä'

loc_1310D::				; CODE XREF: sub_13044+D4j
		sub	di, 2
		sub	dx, bp
		sbb	ax, cx
		mov	[di], ax
		dec	bl
		jnz	short loc_1310D
		add	di, 100h
		jmp	short loc_13162
; ---------------------------------------------------------------------------

loc_13120::				; CODE XREF: sub_13044+AAj
		xor	eax, eax
		xor	dx, dx
		mov	bl, 80h	; 'Ä'

loc_13127::				; CODE XREF: sub_13044+F7j
		cmp	eax, 7FFFh
		jg	short loc_1316B
		mov	[di], ax

loc_13131::				; CODE XREF: sub_13044+12Bj
		add	dx, bp
		adc	eax, ecx
		add	di, 2
		dec	bl
		jnz	short loc_13127
		add	di, 100h
		xor	eax, eax
		xor	dx, dx
		mov	bl, 80h	; 'Ä'

loc_13148::				; CODE XREF: sub_13044+118j
		sub	di, 2
		sub	dx, bp
		sbb	eax, ecx
		cmp	eax, 0FFFF8000h
		jl	short loc_13171
		mov	[di], ax

loc_1315A::				; CODE XREF: sub_13044+131j
		dec	bl
		jnz	short loc_13148
		add	di, 100h

loc_13162::				; CODE XREF: sub_13044+DAj
		pop	ebp
		pop	cx
		dec	cx
		jnz	loc_130BC
		retn
; ---------------------------------------------------------------------------

loc_1316B::				; CODE XREF: sub_13044+E9j
		mov	word ptr [di], 7FFFh
		jmp	short loc_13131
; ---------------------------------------------------------------------------

loc_13171::				; CODE XREF: sub_13044+112j
		mov	word ptr [di], 8000h
		jmp	short loc_1315A
sub_13044 endp


; =============== S U B	R O U T	I N E =======================================


sub_13177 proc near		; CODE XREF: sub_13429+4Ap
					; DATA XREF: sub_12DA8+38o
		or	ax, ax
		jz	short locret_131B2
		or	byte ptr [bx+3Dh], 4
		cmp	byte ptr [bx+1Dh], 1
		jz	short loc_131B3
		cmp	ax, [bx+3Eh]
		jz	short locret_131B2
		mov	[bx+3Eh], ax
		movzx	edi, ax
		xor	edx, edx
		mov	eax, _dword_245BC
		div	edi
		mov	[bx+20h], ax
		mov	cl, _byte_2461A
		shl	edi, cl
		xor	edx, edx
		mov	eax, _dword_245C0
		div	edi
		mov	[bx+1Eh], ax

locret_131B2::				; CODE XREF: sub_13177+2j
					; sub_13177+11j
		retn
; ---------------------------------------------------------------------------

loc_131B3::				; CODE XREF: sub_13177+Cj
					; _gravis_13215+Cj
		movzx	edi, ax
		xor	edx, edx
		mov	cl, _byte_2461A
		shl	edi, cl
		xor	edx, edx
		mov	eax, _dword_245C0
		div	edi
		mov	[bx+1Eh], ax
		retn
sub_13177 endp


; =============== S U B	R O U T	I N E =======================================


_nullsub_5 proc near		; CODE XREF: sub_131DA+4j
					; _gravis_13272+4j
		retn
_nullsub_5 endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_131EF

loc_131D0::				; CODE XREF: sub_131EF+4j
					; _gravis_132A9+4j ...
		xor	ah, ah
		imul	ax, _volume_245FC
		xor	al, al
		retn
; END OF FUNCTION CHUNK	FOR sub_131EF

; =============== S U B	R O U T	I N E =======================================


sub_131DA proc near		; CODE XREF: sub_13429+74j
					; DATA XREF: sub_12DA8+4Ao
		cmp	byte ptr [bx+1Dh], 1
		jz	short _nullsub_5
		test	byte ptr [bx+17h], 1
		jz	short locret_131EE
		and	byte ptr [bx+17h], 0FEh
		mov	byte ptr [bx+35h], 0

locret_131EE::				; CODE XREF: sub_131DA+Aj
		retn
sub_131DA endp


; =============== S U B	R O U T	I N E =======================================


sub_131EF proc near		; CODE XREF: sub_13429+4Fp
					; sub_13429+55p
					; DATA XREF: ...

; FUNCTION CHUNK AT 31D0 SIZE 0000000A BYTES

		cmp	byte ptr [bx+1Dh], 1
		jz	short loc_131D0
		and	byte ptr [bx+3Dh], 0BFh
		cmp	al, _byte_2467D
		jbe	short loc_13202
		mov	al, _byte_2467D

loc_13202::				; CODE XREF: sub_131EF+Ej
		xor	ah, ah
		mov	[bx+22h], al
		mul	_volume_245FC
		mov	al, [bx+23h]
		mov	[bx+36h], ax
		mov	[bx+23h], ah
		retn
sub_131EF endp


; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


sub_13429 proc near		; DATA XREF: sub_12DA8+3Eo
		test	byte ptr [bx+17h], 4
		jz	short locret_13498
		mov	al, [bx+2]
		cmp	al, [bx+3]
		jz	short loc_13471
		mov	[bx+3],	al
		dec	al
		xor	ah, ah
		shl	ax, 6
		mov	di, ax
		add	di, offset _myout
		mov	eax, [bx+28h]
		mov	[bx+40h], eax
		mov	eax, [bx+2Ch]
		mov	[bx+44h], eax
		mov	eax, [bx+30h]
		mov	[bx+48h], eax
		mov	al, [di+3Ch]
		mov	[bx+19h], al
		mov	ax, [di+32h]
		mov	[bx+26h], ax
		mov	ax, [di+30h]
		mov	[bx+24h], ax

loc_13471::				; CODE XREF: sub_13429+Cj
		mov	ax, [bx]
		call	sub_13177
		xor	al, al
		call	sub_131EF
		mov	al, [bx+8]
		call	sub_131EF
		test	byte ptr [bx+17h], 2
		jnz	short loc_13499
		or	byte ptr [bx+17h], 1
		movzx	eax, word ptr [bx+4Ch]
		shl	eax, 8
		mov	[bx+4],	eax

locret_13498::				; CODE XREF: sub_13429+4j
		retn
; ---------------------------------------------------------------------------

loc_13499::				; CODE XREF: sub_13429+5Cj
		test	byte ptr [bx+17h], 1
		jnz	sub_131DA
		retn
sub_13429 endp


; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


sub_135CA proc near		; CODE XREF: sub_12F56+1Fp
					; sub_140B6+3Cp ...
		mov	bx, offset _channels_25908
		mov	cx, _mod_channels_number
		xor	ax, ax

loc_135D3::				; CODE XREF: sub_135CA+1Aj
		mov	byte ptr [bx+3Dh], 0
		test	byte ptr [bx+17h], 10h
		jnz	short loc_135E0
		mov	[bx+0Ah], ax

loc_135E0::				; CODE XREF: sub_135CA+11j
		add	bx, 50h	; 'P'
		dec	cx
		jnz	short loc_135D3
		les	si, _pointer_245B4
		mov	al, es:[si]
		inc	si
		or	al, al
		jz	short loc_135FD

loc_135F2::				; CODE XREF: sub_135CA+31j
		call	sub_13623
		mov	al, es:[si]
		inc	si
		or	al, al
		jnz	short loc_135F2

loc_135FD::				; CODE XREF: sub_135CA+26j
		mov	word ptr [_pointer_245B4], si
		mov	bx, offset _channels_25908
		mov	cx, _mod_channels_number

loc_13608::				; CODE XREF: sub_135CA+56j
		test	byte ptr [bx+17h], 1
		jz	short loc_1361C
		test	byte ptr [bx+3Dh], 0Ch
		jnz	short loc_1361C
		mov	ax, [bx]
		push	cx
		call	[off_245CA]
		pop	cx

loc_1361C::				; CODE XREF: sub_135CA+42j
					; sub_135CA+48j
		add	bx, 50h	; 'P'
		dec	cx
		jnz	short loc_13608
		retn
sub_135CA endp


; =============== S U B	R O U T	I N E =======================================


sub_13623 proc near		; CODE XREF: sub_12CAD+1Cp
					; sub_135CA:loc_135F2p

; FUNCTION CHUNK AT 38BD SIZE 00000015 BYTES
; FUNCTION CHUNK AT 3ACE SIZE 00000009 BYTES
; FUNCTION CHUNK AT 3B66 SIZE 00000012 BYTES
; FUNCTION CHUNK AT 3BF1 SIZE 00000011 BYTES
; FUNCTION CHUNK AT 3CAE SIZE 00000005 BYTES
; FUNCTION CHUNK AT 3DF9 SIZE 00000025 BYTES
; FUNCTION CHUNK AT 3ED3 SIZE 00000031 BYTES

		mov	dh, al
		and	dh, 0E0h
		and	ax, 1Fh
		cmp	ax, _mod_channels_number
		jnb	loc_137BE
		shl	ax, 4
		mov	bx, ax
		shl	ax, 2
		add	bx, ax
		add	bx, offset _channels_25908
		test	dh, 80h
		jz	short loc_13661

loc_13646::
		and	dh, 7Fh
		mov	ax, es:[si]
		add	si, 2
		or	ax, ax
		jz	short loc_13661
		and	byte ptr [bx+17h], 0EFh
		cmp	al, 20h	; ' '
		ja	short loc_13661
		or	dh, 80h
		mov	[bx+0Ah], ax

loc_13661::				; CODE XREF: sub_13623+21j
					; sub_13623+2Ej ...
		test	dh, 40h
		jz	short loc_13677
		and	dh, 0BFh
		mov	al, es:[si]
		inc	si
		cmp	al, 40h	; '@'
		ja	short loc_13677
		or	dh, 40h
		mov	[bx+8],	al

loc_13677::				; CODE XREF: sub_13623+41j
					; sub_13623+4Cj
		mov	[bx+3Dh], dh
		test	dh, 20h
		jz	sub_137D5
		mov	dx, es:[si]
		add	si, 2
		or	dh, dh
		jz	loc_13718
		movzx	ax, dh
		cmp	ax, _word_245D2
		ja	loc_13718
		dec	ax
		shl	ax, 6
		mov	di, ax
		add	di, offset _myout
		and	byte ptr [bx+17h], 0FBh
		mov	al, [di+3Ch]
		and	al, 1
		shl	al, 2
		or	[bx+17h], al
		or	byte ptr [bx+3Dh], 10h
		mov	[bx+2],	dh
		mov	word ptr [bx+4Ch], 0
		test	byte ptr [bx+17h], 40h
		jz	short loc_136CB
		and	byte ptr [bx+17h], 0BFh
		mov	byte ptr [bx+3], 0

loc_136CB::				; CODE XREF: sub_13623+9Ej
		mov	eax, [di+24h]
		mov	[bx+28h], eax
		mov	eax, [di+28h]
		mov	[bx+2Ch], eax
		mov	eax, [di+2Ch]
		mov	[bx+30h], eax
		mov	cl, [di+3Eh]
		and	cx, 0Fh
		shl	cx, 3
		mov	ax, cx
		neg	ax
		shl	cx, 4
		add	ax, cx
		add	ax, offset _word_24998
		mov	[bx+38h], ax
		mov	ax, _freq_245DE
		mov	cx, [di+36h]
		jcxz	short loc_13705
		mov	ax, cx

loc_13705::				; CODE XREF: sub_13623+DEj
		mov	[bx+14h], ax
		test	byte ptr [bx+3Dh], 40h
		jnz	short loc_13718
		mov	al, [di+3Dh]
		mov	[bx+8],	al
		or	byte ptr [bx+3Dh], 40h

loc_13718::				; CODE XREF: sub_13623+66j
					; sub_13623+71j ...
		or	dl, dl
		jz	sub_137D5
		cmp	dl, 0FEh ; '˛'
		jz	loc_137CE
		cmp	dl, 0FFh
		jz	sub_137D5
		mov	[bx+35h], dl
		or	byte ptr [bx+3Dh], 8
		test	_sndflags_24622, 10h
		jnz	short loc_13742
		test	byte ptr [bx+17h], 4
		jz	loc_137CE

loc_13742::				; CODE XREF: sub_13623+115j
		mov	al, [bx+35h]
		call	sub_13826
		xchg	ax, [bx]
		test	byte ptr [bx+3Dh], 80h
		jz	short loc_13791
		mov	_word_245DC, ax
		mov	ax, [bx+0Ah]
		and	ah, 0F0h
		cmp	al, 3
		jz	loc_138BD
		cmp	al, 5
		jz	loc_138BD
		cmp	al, 9
		jz	loc_13ACE
		cmp	al, 0Ch
		jz	loc_13B66
		cmp	ax, 500Eh
		jz	loc_13BF1
		cmp	ax, 0D00Eh
		jz	loc_13CAE
		cmp	al, 13h
		jz	loc_13DF9
		cmp	al, 16h
		jz	loc_13DF9
		cmp	al, 19h
		jz	loc_13ED3

loc_13791::				; CODE XREF: sub_13623+12Bj
					; sub_13623+4B1j ...
		call	[off_245C8]
		test	byte ptr [bx+9], 4
		jnz	short loc_1379F
		mov	byte ptr [bx+0Dh], 0

loc_1379F::				; CODE XREF: sub_13623+176j
		test	byte ptr [bx+9], 40h
		jnz	short loc_137A9
		mov	byte ptr [bx+0Fh], 0

loc_137A9::				; CODE XREF: sub_13623+180j
		movzx	di, byte ptr [bx+0Ah]
		cmp	di, 32
		ja	_eff_nullsub
		shl	di, 1
		mov	al, [bx+0Bh]
		jmp	cs:[_effoff_18FA2+di]
; ---------------------------------------------------------------------------

loc_137BE::				; CODE XREF: sub_13623+Cj
		movzx	di, dh
		shr	di, 5
		mov	al, cs:[_byte_11C29+di]
		xor	ah, ah
		add	si, ax
		retn
; ---------------------------------------------------------------------------

loc_137CE::				; CODE XREF: sub_13623+FEj
					; sub_13623+11Bj
		call	sub_137D5
		jmp	[off_245CE]
sub_13623 endp ; sp-analysis	failed


; =============== S U B	R O U T	I N E =======================================


sub_137D5 proc near		; CODE XREF: sub_13623+5Aj
					; sub_13623+F7j ...
		test	byte ptr [bx+3Dh], 40h
		jnz	short loc_137F0
		movzx	di, byte ptr [bx+0Ah]
		cmp	di, 32
		ja	_eff_nullsub
		shl	di, 1
		mov	al, [bx+0Bh]
		jmp	cs:[_effoff_18F60+di]
; ---------------------------------------------------------------------------

loc_137F0::				; CODE XREF: sub_137D5+4j
		movzx	di, byte ptr [bx+0Ah]
		cmp	di, 32
		ja	_eff_nullsub
		shl	di, 1
		mov	al, [bx+0Bh]
		call	cs:[_effoff_18F60+di]
		test	byte ptr [bx+3Dh], 40h
		jz	short locret_13812
		mov	al, [bx+8]
		jmp	[off_245CC]
; ---------------------------------------------------------------------------

locret_13812::				; CODE XREF: sub_137D5+34j
		retn
sub_137D5 endp


; =============== S U B	R O U T	I N E =======================================


chanl_2_eff_13813 proc near		; CODE XREF: sub_140B6+24p
					; sub_140B6+4Ap ...
		movzx	di, byte ptr [bx+0Ah]
		cmp	di, 32
		ja	short _eff_nullsub
		shl	di, 1
		mov	al, [bx+0Bh]
		jmp	cs:[_effoff_18FE4+di]
chanl_2_eff_13813 endp


; =============== S U B	R O U T	I N E =======================================


sub_13826 proc near		; CODE XREF: sub_13623+122p
					; sub_13623+5D7p ...
		mov	cl, al
		movzx	di, cl
		dec	di
		and	di, 0Fh
		shl	di, 1
		shr	cl, 4
		cmp	_byte_2461A, 0
		jnz	short loc_1386C
		mov	ch, cl
		xor	cl, cl
		xor	ax, ax
		or	ch, ch
		jz	short loc_13863
		mov	ax, 24

loc_13848::
		dec	ch
		jz	short loc_13863
		mov	ax, 48
		dec	ch
		jz	short loc_13863
		mov	ax, 72
		dec	ch
		jz	short loc_13863
		mov	ax, 96
		dec	ch
		jz	short loc_13863
		mov	cl, ch

loc_13863::				; CODE XREF: sub_13826+1Dj
					; sub_13826+24j ...
		add	di, ax
		add	di, [bx+38h]
		sub	di, offset _word_246DE

loc_1386C::				; CODE XREF: sub_13826+13j
		mov	ax, [_word_246DE+di]
		shr	ax, cl
		mov	cx, [bx+14h]
		jcxz	short locret_1387D
		mul	_freq_245DE
		div	cx

locret_1387D::				; CODE XREF: sub_13826+4Fj
		retn
sub_13826 endp


; =============== S U B	R O U T	I N E =======================================


_eff_nullsub proc near	; CODE XREF: sub_13623+18Dj
					; sub_13623+196j ...
		retn
_eff_nullsub endp


; =============== S U B	R O U T	I N E =======================================


_eff_1387F proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		cmp	_byte_24668, 0
		jnz	short _eff_nullsub
_eff_1387F endp ; sp-analysis	failed


; =============== S U B	R O U T	I N E =======================================


_eff_13886 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		xor	ah, ah
		shl	ax, 4

loc_1388B::				; CODE XREF: _eff_13DE5+5j
		sub	[bx], ax
		cmp	word ptr [bx], 0A0h ; '†'
		jge	short loc_13897
		mov	word ptr [bx], 0A0h ; '†'

loc_13897::				; CODE XREF: _eff_13886+Bj
		mov	ax, [bx]
		jmp	[off_245CA]
_eff_13886 endp


; =============== S U B	R O U T	I N E =======================================


_eff_1389D proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		cmp	_byte_24668, 0
		jnz	short _eff_nullsub
_eff_1389D endp ; sp-analysis	failed


; =============== S U B	R O U T	I N E =======================================


_eff_138A4 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		xor	ah, ah
		shl	ax, 4

loc_138A9::				; CODE XREF: _eff_13DEF+5j
		add	[bx], ax
		jb	short loc_138B3
		cmp	word ptr [bx], 13696
		jbe	short loc_138B7

loc_138B3::				; CODE XREF: _eff_138A4+7j
		mov	word ptr [bx], 13696

loc_138B7::				; CODE XREF: _eff_138A4+Dj
		mov	ax, [bx]
		jmp	[off_245CA]
_eff_138A4 endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_13623

loc_138BD::				; CODE XREF: sub_13623+138j
					; sub_13623+13Ej
		mov	ax, [bx]
		cmp	ax, _word_245DC
		jnz	short loc_138C7
		xor	ax, ax

loc_138C7::				; CODE XREF: sub_13623+2A0j
		mov	[bx+10h], ax
		mov	ax, _word_245DC
		mov	[bx], ax
		jmp	sub_137D5
; END OF FUNCTION CHUNK	FOR sub_13623

; =============== S U B	R O U T	I N E =======================================


_eff_138D2 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		or	al, al
		jz	short loc_138DE
		xor	ah, ah
		shl	ax, 4
		mov	[bx+12h], ax

loc_138DE::				; CODE XREF: _eff_138D2+2j _eff_139AC+3j ...
		mov	ax, [bx+10h]
		or	ax, ax
		jz	locret_13CF4
		mov	dx, [bx+12h]
		cmp	ax, [bx]
		jb	short loc_138F6
		add	[bx], dx
		cmp	[bx], ax
		jb	short loc_1390B
		jmp	short loc_138FC
; ---------------------------------------------------------------------------

loc_138F6::				; CODE XREF: _eff_138D2+1Aj
		sub	[bx], dx
		cmp	ax, [bx]
		jl	short loc_1390B

loc_138FC::				; CODE XREF: _eff_138D2+22j
		mov	[bx], ax
		mov	word ptr [bx+10h], 0
		and	byte ptr [bx+17h], 0EFh
		jmp	[off_245CA]
; ---------------------------------------------------------------------------

loc_1390B::				; CODE XREF: _eff_138D2+20j
					; _eff_138D2+28j
		test	byte ptr [bx+17h], 20h
		jnz	short loc_13917
		mov	ax, [bx]
		jmp	[off_245CA]
; ---------------------------------------------------------------------------

loc_13917::				; CODE XREF: _eff_138D2+3Dj
		mov	di, [bx+38h]
		mov	ax, [bx]
		mov	cx, 3Bh	; ';'

loc_1391F::				; CODE XREF: _eff_138D2+55j
		cmp	[di], ax
		jbe	short loc_13929
		add	di, 2
		dec	cx
		jnz	short loc_1391F

loc_13929::				; CODE XREF: _eff_138D2+4Fj
		mov	ax, [di]
		jmp	[off_245CA]
_eff_138D2 endp


; =============== S U B	R O U T	I N E =======================================


_eff_1392F proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		mov	cl, 3

loc_13931::				; CODE XREF: _eff_13E2D+2j
		or	al, al
		jz	short loc_13950
		mov	ch, al
		mov	dl, [bx+0Ch]
		and	al, 0Fh
		jz	short loc_13943
		and	dl, 0F0h
		or	dl, al

loc_13943::				; CODE XREF: _eff_1392F+Dj
		and	ch, 0F0h
		jz	short loc_1394D
		and	dl, 0Fh
		or	dl, ch

loc_1394D::				; CODE XREF: _eff_1392F+17j
		mov	[bx+0Ch], dl

loc_13950::				; CODE XREF: _eff_1392F+4j _eff_139B2+5j ...
		mov	al, [bx+0Dh]
		shr	al, 2
		and	ax, 1Fh
		mov	dl, [bx+9]
		and	dl, 3
		jz	short loc_1397B
		shl	al, 3
		cmp	dl, 1
		jz	short loc_1396D
		mov	dl, 0FFh
		jmp	short loc_13981
; ---------------------------------------------------------------------------

loc_1396D::				; CODE XREF: _eff_1392F+38j
		mov	dl, al
		test	byte ptr [bx+0Dh], 80h
		jz	short loc_13981
		mov	dl, 0FFh
		sub	dl, al
		jmp	short loc_13981
; ---------------------------------------------------------------------------

loc_1397B::				; CODE XREF: _eff_1392F+30j
		mov	di, ax
		mov	dl, [_table_251C0+di]

loc_13981::				; CODE XREF: _eff_1392F+3Cj
					; _eff_1392F+44j ...
		mov	al, [bx+0Ch]
		mov	dh, al
		and	al, 0Fh
		mul	dl
		mov	ch, _flag_playsetttings
		and	ch, 1
		add	cl, ch
		shr	ax, cl
		test	byte ptr [bx+0Dh], 80h
		jz	short loc_1399D
		neg	ax

loc_1399D::				; CODE XREF: _eff_1392F+6Aj
		add	ax, [bx]
		shr	dh, 2
		and	dh, 3Ch
		add	[bx+0Dh], dh
		jmp	[off_245CA]
_eff_1392F endp


; =============== S U B	R O U T	I N E =======================================


_eff_139AC proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		call	_eff_13AD7
		jmp	loc_138DE
_eff_139AC endp


; =============== S U B	R O U T	I N E =======================================


_eff_139B2 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		call	_eff_13AD7
		mov	cl, 3
		jmp	short loc_13950
_eff_139B2 endp


; =============== S U B	R O U T	I N E =======================================


_eff_139B9 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		or	al, al
		jz	short loc_139D8
		mov	cl, al
		mov	dl, [bx+0Eh]
		and	al, 0Fh
		jz	short loc_139CB
		and	dl, 0F0h
		or	dl, al

loc_139CB::				; CODE XREF: _eff_139B9+Bj
		and	cl, 0F0h
		jz	short loc_139D5
		and	dl, 0Fh
		or	dl, cl

loc_139D5::				; CODE XREF: _eff_139B9+15j
		mov	[bx+0Eh], dl

loc_139D8::				; CODE XREF: _eff_139B9+2j
		mov	al, [bx+0Fh]
		shr	al, 2
		and	ax, 1Fh
		mov	dl, [bx+9]
		shr	dl, 4
		and	dl, 3
		jz	short loc_13A06
		shl	al, 3
		cmp	dl, 1
		jz	short loc_139F8
		mov	dl, 0FFh
		jmp	short loc_13A0C
; ---------------------------------------------------------------------------

loc_139F8::				; CODE XREF: _eff_139B9+39j
		mov	dl, al
		test	byte ptr [bx+0Fh], 80h
		jz	short loc_13A0C
		mov	dl, 0FFh
		sub	dl, al
		jmp	short loc_13A0C
; ---------------------------------------------------------------------------

loc_13A06::				; CODE XREF: _eff_139B9+31j
		mov	di, ax
		mov	dl, [_table_251C0+di]

loc_13A0C::				; CODE XREF: _eff_139B9+3Dj
					; _eff_139B9+45j ...
		mov	al, [bx+0Eh]
		mov	dh, al
		and	al, 0Fh
		mul	dl
		shr	ax, 6
		mov	ah, al
		mov	al, [bx+8]
		test	byte ptr [bx+0Fh], 80h
		jnz	short loc_13A30
		add	al, ah
		cmp	al, _byte_2467D
		jbe	short loc_13A36
		mov	al, _byte_2467D
		jmp	short loc_13A36
; ---------------------------------------------------------------------------

loc_13A30::				; CODE XREF: _eff_139B9+68j
		sub	al, ah
		jns	short loc_13A36
		xor	al, al

loc_13A36::				; CODE XREF: _eff_139B9+70j
					; _eff_139B9+75j ...
		shr	dh, 2
		and	dh, 3Ch
		add	[bx+0Fh], dh
		jmp	[off_245CC]
_eff_139B9 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13A43 proc near		; CODE XREF: sub_12AFD+16p
					; sub_13623+196j ...
		cmp	al, 0A4h ; '§'
		jz	short loc_13A5B
		cmp	al, 0A5h ; '•'
		jz	short loc_13A60
		cmp	al, 0A6h ; '¶'
		jz	short loc_13A65
		cmp	al, 80h	; 'Ä'
		ja	short locret_13A5A
		test	_sndflags_24622, 4

locret_13A5A::				; CODE XREF: _eff_13A43+Ej
		retn
; ---------------------------------------------------------------------------

loc_13A5B::				; CODE XREF: _eff_13A43+2j
		or	byte ptr [bx+17h], 80h
		retn
; ---------------------------------------------------------------------------

loc_13A60::				; CODE XREF: _eff_13A43+6j
		and	byte ptr [bx+17h], 7Fh
		retn
; ---------------------------------------------------------------------------

loc_13A65::				; CODE XREF: _eff_13A43+Aj
		xor	byte ptr [bx+17h], 80h
		retn
_eff_13A43 endp


; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


_eff_13A94 proc near		; CODE XREF: sub_137D5+16j
					; sub_137D5+2Bp ...
		or	al, al
		jz	short loc_13A9B
		mov	[bx+16h], al

loc_13A9B::				; CODE XREF: _eff_13A94+2j
		movzx	eax, byte ptr [bx+16h]
		shl	eax, 8
		cmp	eax, [bx+30h]
		ja	short loc_13AAE
		mov	[bx+4Ch], ax
		retn
; ---------------------------------------------------------------------------

loc_13AAE::				; CODE XREF: _eff_13A94+14j
		cmp	_byte_2461A, 0
		jnz	short loc_13AC6
		call	[off_245CE]
		and	byte ptr [bx+17h], 0FBh
		or	byte ptr [bx+17h], 40h
		mov	byte ptr [bx+3], 0
		retn
; ---------------------------------------------------------------------------

loc_13AC6::				; CODE XREF: _eff_13A94+1Fj
		mov	eax, [bx+30h]
		mov	[bx+4Ch], ax
		retn
_eff_13A94 endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_13623

loc_13ACE::				; CODE XREF: sub_13623+144j
		mov	al, [bx+0Bh]
		call	_eff_13A94
		jmp	loc_13791
; END OF FUNCTION CHUNK	FOR sub_13623

; =============== S U B	R O U T	I N E =======================================


_eff_13AD7 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		mov	dl, [bx+8]
		test	al, 0F0h
		jnz	short loc_13AEF

loc_13ADE::				; CODE XREF: _eff_13E32+1Ej
					; _eff_13E32+36j ...
		and	al, 0Fh

loc_13AE0::				; CODE XREF: _eff_13C95+8j
		sub	dl, al
		mov	al, dl
		jnb	short loc_13AE8
		xor	al, al

loc_13AE8::				; CODE XREF: _eff_13AD7+Dj
		mov	[bx+8],	al
		jmp	[off_245CC]
; ---------------------------------------------------------------------------

loc_13AEF::				; CODE XREF: _eff_13AD7+5j
					; _eff_13E32+27j ...
		shr	al, 4

loc_13AF2::				; CODE XREF: _eff_13C88+8j
		add	dl, al
		mov	al, dl
		cmp	al, _byte_2467D
		jbe	short loc_13AFF
		mov	al, _byte_2467D

loc_13AFF::				; CODE XREF: _eff_13AD7+23j
		mov	[bx+8],	al
		jmp	[off_245CC]
_eff_13AD7 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13B06 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		xor	ah, ah
		dec	ax
		mov	_word_245F0, ax
		inc	ax
		test	_flag_playsetttings, 4
		jnz	short loc_13B5B
		bt	word ptr [_byte_282E8], ax
		jnb	short loc_13B5B
		mov	cx, _word_245FA
		add	cx, 7
		shr	cx, 3
		jz	short loc_13B34
		xor	di, di

loc_13B29::				; CODE XREF: _eff_13B06+2Cj
		cmp	[_byte_282E8+di], 0FFh
		jnz	short loc_13B3E
		inc	di
		dec	cx
		jnz	short loc_13B29

loc_13B34::				; CODE XREF: _eff_13B06+1Fj
					; _eff_13B06+4Ej
		push	bx
		push	si
		push	es
		call	_vlm_141DF
		pop	es
		pop	si
		pop	bx
		retn
; ---------------------------------------------------------------------------

loc_13B3E::				; CODE XREF: _eff_13B06+28j
		mov	al, [_byte_282E8+di]
		shl	di, 3
		mov	cx, 8

loc_13B48::				; CODE XREF: _eff_13B06+48j
		shr	al, 1
		jnb	short loc_13B50
		inc	di
		dec	cx
		jnz	short loc_13B48

loc_13B50::				; CODE XREF: _eff_13B06+44j
		cmp	di, _word_245FA
		jnb	short loc_13B34
		dec	di
		mov	_word_245F0, di

loc_13B5B::				; CODE XREF: _eff_13B06+Cj
					; _eff_13B06+13j ...
		mov	_byte_24669, 0
		mov	_byte_2466A, 1
		retn
_eff_13B06 endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_13623

loc_13B66::				; CODE XREF: sub_13623+14Aj
		mov	al, [bx+0Bh]
		cmp	al, _byte_2467D
		jbe	short loc_13B72
		mov	al, _byte_2467D

loc_13B72::				; CODE XREF: sub_13623+54Aj
		mov	[bx+8],	al
		jmp	loc_13791
; END OF FUNCTION CHUNK	FOR sub_13623

; =============== S U B	R O U T	I N E =======================================


_eff_13B78 proc near		; CODE XREF: sub_137D5+16j
					; sub_137D5+2Bp ...
		cmp	al, _byte_2467D
		jbe	short loc_13B81
		mov	al, _byte_2467D

loc_13B81::				; CODE XREF: _eff_13B78+4j
		mov	[bx+8],	al
		jmp	[off_245CC]
_eff_13B78 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13B88 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		mov	dl, al
		and	dl, 0Fh
		shr	al, 4
		mov	ah, 0Ah
		mul	ah
		add	al, dl
		cmp	al, 3Fh	; '?'
		ja	short loc_13B5B
		mov	_byte_24669, al
		mov	_byte_2466A, 1
		retn
_eff_13B88 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13BA3 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		mov	di, ax
		shr	di, 3
		and	di, 1Eh
		and	al, 0Fh
		jmp	cs:[_effoff_19026+di]
_eff_13BA3 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13BB2 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		or	al, al
		jz	short loc_13BBB
		or	byte ptr [bx+17h], 20h
		retn
; ---------------------------------------------------------------------------

loc_13BBB::				; CODE XREF: _eff_13BB2+2j
		and	byte ptr [bx+17h], 0DFh
		retn
_eff_13BB2 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13BC0 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		and	byte ptr [bx+9], 0F0h
		or	[bx+9],	al
		retn
_eff_13BC0 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13BC8 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		and	ax, 0Fh
		mov	di, ax
		cmp	_byte_2461A, 0
		jnz	short loc_13BE7
		shl	di, 3
		mov	ax, di
		neg	ax
		shl	di, 4
		add	ax, di
		add	ax, offset _word_24998
		mov	[bx+38h], ax
		retn
; ---------------------------------------------------------------------------

loc_13BE7::				; CODE XREF: _eff_13BC8+Aj
		shl	di, 1
		mov	ax, [_table_246F6+di]
		mov	[bx+14h], dx
		retn
_eff_13BC8 endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_13623

loc_13BF1::				; CODE XREF: sub_13623+151j
		mov	al, [bx+0Bh]
		call	_eff_13BC8
		mov	al, [bx+35h]
		call	sub_13826
		mov	[bx], ax
		jmp	loc_13791
; END OF FUNCTION CHUNK	FOR sub_13623

; =============== S U B	R O U T	I N E =======================================


_eff_13C02 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		cmp	_byte_24668, 0
		jnz	locret_13CF4
		or	al, al
		jz	short loc_13C2D
		cmp	byte ptr [bx+3Ch], 0
		jnz	short loc_13C1A
		inc	al
		mov	[bx+3Ch], al

loc_13C1A::				; CODE XREF: _eff_13C02+11j
		dec	byte ptr [bx+3Ch]
		jz	locret_13CF4
		mov	al, [bx+3Bh]
		mov	_byte_24669, al
		mov	_byte_2466B, 1
		retn
; ---------------------------------------------------------------------------

loc_13C2D::				; CODE XREF: _eff_13C02+Bj
		mov	ax, _word_245F6
		mov	[bx+3Bh], al
		retn
_eff_13C02 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13C34 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		and	byte ptr [bx+9], 0Fh
		shl	al, 4
		or	[bx+9],	al
		retn
_eff_13C34 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13C3F proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		cmp	_byte_24668, 0
		jz	short loc_13C47
		retn
; ---------------------------------------------------------------------------

loc_13C47::				; CODE XREF: _eff_13C3F+5j
		mov	di, ax
		and	di, 0Fh
		mov	al, cs:[_byte_13C54+di]
		jmp	_eff_13A43
_eff_13C3F endp

; ---------------------------------------------------------------------------
_byte_13C54	db 0,9,12h,1Bh,24h,2Dh,36h,40h,40h,4Ah,53h,5Ch,65h,6Eh
					; DATA XREF: _mtm_module+43r
					; _far_module+55r ...
		db 77h,80h

; =============== S U B	R O U T	I N E =======================================


_eff_13C64 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		or	al, al
		jz	locret_13CF4
		cmp	_byte_24668, 0
		jnz	short loc_13C77
		test	byte ptr [bx+3Dh], 8
		jnz	short locret_13CF4

loc_13C77::				; CODE XREF: _eff_13C64+Bj
		mov	dl, al
		movzx	ax, _byte_24668
		div	dl
		or	ah, ah
		jnz	short locret_13CF4
		jmp	[off_245C8]
_eff_13C64 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13C88 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		mov	dl, [bx+8]
		cmp	_byte_24668, 0
		jz	loc_13AF2
		retn
_eff_13C88 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13C95 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		mov	dl, [bx+8]
		cmp	_byte_24668, 0
		jz	loc_13AE0
		retn
_eff_13C95 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13CA2 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		cmp	al, _byte_24668
		jnz	short locret_13CF4
		xor	al, al
		jmp	[off_245CC]
_eff_13CA2 endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_13623

loc_13CAE::				; CODE XREF: sub_13623+158j
		mov	al, [bx+0Bh]
		and	al, 0Fh
; END OF FUNCTION CHUNK	FOR sub_13623

; =============== S U B	R O U T	I N E =======================================


_eff_13CB3 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		cmp	al, _byte_24668
		jnz	short locret_13CF4
		cmp	word ptr [bx], 0
		jz	short locret_13CF4
		mov	byte ptr [bx+0Ah], 0
		mov	byte ptr [bx+0Bh], 0
		jmp	loc_13791
_eff_13CB3 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13CC9 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		cmp	_byte_24668, 0
		jnz	short locret_13CF4
		cmp	_byte_2466D, 0
		jnz	short locret_13CF4
		inc	al
		mov	_byte_2466C, al
		retn
_eff_13CC9 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13CDD proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		test	_flag_playsetttings, 2
		jnz	short _eff_13CE8
		cmp	al, 20h	; ' '
		ja	short sub_13CF6
_eff_13CDD endp ; sp-analysis	failed


; =============== S U B	R O U T	I N E =======================================


_eff_13CE8 proc near		; CODE XREF: sub_12EBA+6Bp
					; sub_13623+196j ...
		or	al, al
		jz	short locret_13CF5
		mov	_byte_24667, al
		mov	_byte_24668, 0

locret_13CF4::				; CODE XREF: _eff_138D2+11j
					; _eff_13C02+5j	...
		retn
; ---------------------------------------------------------------------------

locret_13CF5::				; CODE XREF: _eff_13CE8+2j
		retn
_eff_13CE8 endp


; =============== S U B	R O U T	I N E =======================================


sub_13CF6 proc near		; CODE XREF: sub_12EBA+65p
					; _eff_13CDD+9j	...

; FUNCTION CHUNK AT 3DB0 SIZE 00000011 BYTES

		xor	ah, ah
		mov	_byte_24666, al
		mov	cx, ax
		mov	dl, 91
		div	dl
		inc	al
		mov	cs:[_byte_14F72], al
		mov	cs:[_byte_14F73], al
		test	_sndflags_24622, 4
		jnz	short loc_13D4B
		test	_sndflags_24622, 10h
		jnz	_settimer
		shl	cx, 1
		mov	ax, 5
		mul	_freq1
		div	cx
		xor	dx, dx
		div	_word_245E8
		inc	ax
		or	dx, dx
		jnz	short loc_13D36
		dec	ax
		mov	dx, _word_245E8

loc_13D36::				; CODE XREF: sub_13CF6+39j
		mov	_word_245EA, dx
		mov	_word_245EC, ax
		mov	_word_245EE, ax
		mov	ax, _word_245E8
		mov	_word_245E4, ax
		mov	cs:[audio_len], ax
		retn
; ---------------------------------------------------------------------------

loc_13D4B::				; CODE XREF: sub_13CF6+1Aj
		call	sub_13D95
		mov	ah, al
		pushf
		cli
		mov	dx, _gravis_port
		sub	dx, 0FBh ; '˚'
		mov	al, 4
		out	dx, al
		inc	dx
		mov	al, 80h	; 'Ä'
		out	dx, al
		xor	al, al
		out	dx, al
		add	dx, 0FAh ; '˙'
		mov	al, 46h	; 'F'
		out	dx, al
		add	dl, 2
		mov	al, ah
		out	dx, al
		sub	dl, 2
		mov	al, 45h	; 'E'
		out	dx, al
		add	dl, 2
		xor	al, al
		out	dx, al
		mov	al, 4
		out	dx, al
		sub	dx, 0FDh ; '˝'
		mov	al, 4
		out	dx, al
		inc	dx
		mov	al, 1
		out	dx, al
		popf
		retn
sub_13CF6 endp ; sp-analysis	failed

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_13D95

loc_13D8D::				; CODE XREF: sub_13D95+10j
		shl	cx, 1
		inc	_byte_24618
		jmp	short loc_13D9A
; END OF FUNCTION CHUNK	FOR sub_13D95

; =============== S U B	R O U T	I N E =======================================


sub_13D95 proc near		; CODE XREF: sub_13CF6:loc_13D4Bp

; FUNCTION CHUNK AT 3D8D SIZE 00000008 BYTES

		mov	_byte_24618, 1

loc_13D9A::				; CODE XREF: sub_13D95-2j
		xor	dx, dx
		mov	ax, 31250
		div	cx
		neg	al
		or	ah, ah
		jnz	short loc_13D8D
		mov	ah, _byte_24618
		mov	_byte_24619, ah
		retn
sub_13D95 endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_13CF6

_settimer::				; CODE XREF: sub_13CF6+21j
		xor	ax, ax
		cmp	cx, 45
		jbe	short _set_timer
		mov	dx, 2Dh	; '-'
		mov	ax, 8426h
		div	cx
		jmp	short $+2
; END OF FUNCTION CHUNK	FOR sub_13CF6

; =============== S U B	R O U T	I N E =======================================


_set_timer proc near		; CODE XREF: _configure_timer+Fp
					; _memfill8080+4p ...
		mov	cs:[_timer_word_14F6E], ax
		pushf
		cli
		push	ax
		mov	al, 34h	; '4'
		out	43h, al		; Timer	8253-5 (AT: 8254.2).
		pop	ax
		out	40h, al		; Timer	8253-5 (AT: 8254.2).
		mov	al, ah
		out	40h, al		; Timer	8253-5 (AT: 8254.2).
		popf
		retn
_set_timer endp


; =============== S U B	R O U T	I N E =======================================


_clean_timer proc near	; CODE XREF: _clean_int8_mem_timr+Fp
					; _midi_sndoff+Fp
		pushf
		cli
		mov	al, 36h	; '6'
		out	43h, al		; Timer	8253-5 (AT: 8254.2).
		xor	al, al
		out	40h, al		; Timer	8253-5 (AT: 8254.2).
		jmp	short $+2
		out	40h, al		; Timer	8253-5 (AT: 8254.2).
		popf
		retn
_clean_timer endp


; =============== S U B	R O U T	I N E =======================================


_eff_13DE5 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		call	sub_14087
		or	ax, ax
		jnz	loc_1388B
		retn
_eff_13DE5 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13DEF proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		call	sub_14087
		or	ax, ax
		jnz	loc_138A9
		retn
_eff_13DEF endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_13623

loc_13DF9::				; CODE XREF: sub_13623+15Ej
					; sub_13623+164j
		mov	ax, [bx]
		cmp	ax, _word_245DC
		jnz	short loc_13E03
		xor	ax, ax

loc_13E03::				; CODE XREF: sub_13623+7DCj
		mov	[bx+10h], ax
		mov	ax, _word_245DC
		mov	[bx], ax
		mov	al, [bx+0Bh]
		or	al, al
		jnz	short loc_13E18
		mov	al, [bx+34h]
		mov	[bx+0Bh], al

loc_13E18::				; CODE XREF: sub_13623+7EDj
		mov	[bx+34h], al
		jmp	sub_137D5
; END OF FUNCTION CHUNK	FOR sub_13623

; =============== S U B	R O U T	I N E =======================================


_eff_13E1E proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		or	al, al
		jz	short loc_13E2A
		xor	ah, ah
		shl	ax, 2
		mov	[bx+12h], ax

loc_13E2A::				; CODE XREF: _eff_13E1E+2j _eff_13E7F+3j
		jmp	loc_138DE
_eff_13E1E endp


; =============== S U B	R O U T	I N E =======================================


_eff_13E2D proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		mov	cl, 5
		jmp	loc_13931
_eff_13E2D endp


; =============== S U B	R O U T	I N E =======================================


_eff_13E32 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		or	al, al
		jz	short loc_13E39
		mov	[bx+34h], al

loc_13E39::				; CODE XREF: _eff_13E32+2j
		mov	al, [bx+34h]
		mov	dl, [bx+8]
		mov	cl, al
		and	cl, 0Fh
		mov	ch, al
		shr	ch, 4
		cmp	cl, 0Fh
		jnz	short loc_13E5E
		or	ch, ch
		jz	loc_13ADE
		cmp	_byte_24668, 0
		jz	loc_13AEF
		retn
; ---------------------------------------------------------------------------

loc_13E5E::				; CODE XREF: _eff_13E32+1Aj
		cmp	ch, 0Fh
		jz	short loc_13E6F
		mov	dl, [bx+8]
		test	al, 0Fh
		jnz	loc_13ADE
		jmp	loc_13AEF
; ---------------------------------------------------------------------------

loc_13E6F::				; CODE XREF: _eff_13E32+2Fj
		or	cl, cl
		jz	loc_13AEF
		cmp	_byte_24668, 0
		jz	loc_13ADE
		retn
_eff_13E32 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13E7F proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		call	_eff_13E32
		jmp	short loc_13E2A
_eff_13E7F endp


; =============== S U B	R O U T	I N E =======================================


_eff_13E84 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		call	_eff_13E32
		mov	cl, 5
		jmp	loc_13950
_eff_13E84 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13E8C proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		call	sub_13E9B
		mov	_byte_24667, ah
		mov	_byte_24668, 0
		jmp	sub_13CF6
_eff_13E8C endp


; =============== S U B	R O U T	I N E =======================================


sub_13E9B proc near		; CODE XREF: __2stm_module+2Ep
					; _eff_13E8Cp
		movzx	di, al
		mov	dx, di
		and	dl, 0Fh
		shr	di, 4
		mov	ax, dx
		mul	cs:[_table_13EC3+di]
		shr	ax, 4
		neg	ax
		add	ax, 31h	; '1'
		mov	dx, ax
		shl	ax, 2
		add	ax, dx
		shr	ax, 1
		mov	dx, di
		mov	ah, dl
		retn
sub_13E9B endp

; ---------------------------------------------------------------------------
_table_13EC3	db 140,50,25,15,10,7,6,4,3,3,2,2,2,2,1,1 ; DATA	XREF: sub_13E9B+Dr
; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_13623

loc_13ED3::				; CODE XREF: sub_13623+16Aj
		mov	cl, [bx+0Bh]
		and	cx, 0FFh
		jz	loc_13791
		mov	ax, [bx]
		sub	ax, _word_245DC
		jns	short loc_13EE8
		neg	ax

loc_13EE8::				; CODE XREF: sub_13623+8C1j
		xor	dx, dx
		div	cx
		mov	[bx+12h], ax
		mov	byte ptr [bx+0Bh], 0
		or	byte ptr [bx+17h], 10h
		mov	ax, [bx]
		mov	[bx+10h], ax
		mov	ax, _word_245DC
		mov	[bx], ax
		jmp	sub_137D5
; END OF FUNCTION CHUNK	FOR sub_13623

; =============== S U B	R O U T	I N E =======================================


_nullsub_2 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		retn
_nullsub_2 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13F05 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		or	al, al
		jz	short loc_13F0C
		mov	[bx+34h], al

loc_13F0C::				; CODE XREF: _eff_13F05+2j
		mov	al, [bx+34h]
		mov	dl, al
		shr	dl, 4
		and	al, 0Fh
		mov	dh, al
		and	ax, 0Fh
		add	al, dl
		jz	locret_13CF4
		mov	cl, al
		movzx	ax, _byte_24668
		div	cl
		cmp	ah, dl
		jb	short loc_13F34
		xor	al, al
		jmp	[off_245CC]
; ---------------------------------------------------------------------------

loc_13F34::				; CODE XREF: _eff_13F05+27j
		mov	al, [bx+8]
		jmp	[off_245CC]
_eff_13F05 endp


; =============== S U B	R O U T	I N E =======================================


_eff_13F3B proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		or	al, al
		jz	short loc_13F42
		mov	[bx+34h], al

loc_13F42::				; CODE XREF: _eff_13F3B+2j
		mov	al, [bx+34h]
		mov	ch, al
		shr	al, 4
		test	al, 7
		jz	short loc_13FB7
		test	al, 8
		jnz	short loc_13F96
		cmp	al, 6
		jz	short loc_13F6D
		cmp	al, 7
		jz	short loc_13F7C
		dec	al
		mov	cl, al
		mov	al, 1
		shl	al, cl
		sub	[bx+8],	al
		jnb	short loc_13FB7
		mov	byte ptr [bx+8], 0
		jmp	short loc_13FB7
; ---------------------------------------------------------------------------

loc_13F6D::				; CODE XREF: _eff_13F3B+19j
		movzx	ax, byte ptr [bx+8]
		shl	ax, 1
		mov	dl, 3
		div	dl
		mov	[bx+8],	al
		jmp	short loc_13FB7
; ---------------------------------------------------------------------------

loc_13F7C::				; CODE XREF: _eff_13F3B+1Dj
		shr	byte ptr [bx+8], 1
		jmp	short loc_13FB7
; ---------------------------------------------------------------------------

loc_13F81::				; CODE XREF: _eff_13F3B+5Fj
		movzx	ax, byte ptr [bx+8]
		mov	dx, ax
		add	ax, dx
		add	ax, dx
		shr	ax, 1
		jmp	short loc_13FAB
; ---------------------------------------------------------------------------

loc_13F8F::				; CODE XREF: _eff_13F3B+63j
		mov	al, [bx+8]
		add	al, al
		jmp	short loc_13FAB
; ---------------------------------------------------------------------------

loc_13F96::				; CODE XREF: _eff_13F3B+15j
		and	al, 7
		cmp	al, 6
		jz	short loc_13F81
		cmp	al, 7
		jz	short loc_13F8F
		dec	al
		mov	cl, al
		mov	al, 1
		shl	al, cl
		add	al, [bx+8]

loc_13FAB::				; CODE XREF: _eff_13F3B+52j
					; _eff_13F3B+59j
		cmp	al, _byte_2467D
		jbe	short loc_13FB4
		mov	al, _byte_2467D

loc_13FB4::				; CODE XREF: _eff_13F3B+74j
		mov	[bx+8],	al

loc_13FB7::				; CODE XREF: _eff_13F3B+11j
					; _eff_13F3B+2Aj ...
		mov	al, ch
		and	al, 0Fh
		jmp	_eff_13C64
_eff_13F3B endp


; =============== S U B	R O U T	I N E =======================================


_eff_13FBE proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		or	al, al
		jnz	short loc_13FCE
		mov	al, [bx+34h]
		or	al, al
		jz	locret_13CF4
		mov	[bx+0Bh], al

loc_13FCE::				; CODE XREF: _eff_13FBE+2j
		mov	[bx+34h], al
		mov	al, [bx+35h]
		mov	dl, al
		and	dl, 0Fh
		jz	locret_13CF4
		dec	dl
		shr	al, 4
		mov	ah, 0Ch
		mul	ah
		add	dl, al
		movzx	ax, _byte_24668
		mov	dh, 3
		div	dh
		or	ah, ah
		jz	short loc_1401A
		mov	dh, [bx+0Bh]
		cmp	ah, 2
		jz	short loc_14000
		shr	dh, 4

loc_14000::				; CODE XREF: _eff_13FBE+3Dj
		and	dh, 0Fh
		add	dl, dh
		movzx	ax, dl
		mov	dh, 0Ch
		div	dh
		inc	ah
		shl	al, 4
		or	al, ah
		call	sub_13826
		jmp	[off_245CA]
; ---------------------------------------------------------------------------

loc_1401A::				; CODE XREF: _eff_13FBE+35j
		mov	ax, [bx]
		jmp	[off_245CA]
_eff_13FBE endp


; =============== S U B	R O U T	I N E =======================================


_eff_14020 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		xor	ah, ah
		shl	ax, 2
		push	bx
		push	si
		push	es
		push	cs
		call	near ptr _getset_amplif
		pop	es
		pop	si
		pop	bx
		retn
_eff_14020 endp


; =============== S U B	R O U T	I N E =======================================


_eff_14030 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		and	ax, 0Fh
		mov	di, ax
		mov	al, cs:[_table_14057+di]
		mov	_byte_2467B, al

loc_1403D::				; CODE XREF: _eff_14067+Ej
					; _eff_14067+17j ...
		call	_calc_14043
		jmp	sub_13CF6
_eff_14030 endp


; =============== S U B	R O U T	I N E =======================================


_calc_14043 proc near		; CODE XREF: _far_module+34p
					; _eff_14030:loc_1403Dp
		mov	al, _byte_2467B
		add	al, _byte_2467C
		and	eax, 0FFh
		lea	ax, [eax+eax*4]
		shr	ax, 1
		retn
_calc_14043 endp

; ---------------------------------------------------------------------------
_table_14057	db 0FFh,80h,40h,2Ah,20h,19h,15h,12h,10h,0Eh,0Ch,0Bh,0Ah
					; DATA XREF: _far_module+27r
					; _eff_14030+5r
		db 9,9,8

; =============== S U B	R O U T	I N E =======================================


_eff_14067 proc near		; CODE XREF: sub_13623+196j
					; sub_137D5+16j ...
		or	al, al
		jz	short loc_14080
		test	al, 0Fh
		jz	short loc_14077
		and	al, 0Fh
		sub	_byte_2467C, al
		jmp	short loc_1403D
; ---------------------------------------------------------------------------

loc_14077::				; CODE XREF: _eff_14067+6j
		shr	al, 4
		add	_byte_2467C, al
		jmp	short loc_1403D
; ---------------------------------------------------------------------------

loc_14080::				; CODE XREF: _eff_14067+2j
		mov	_byte_2467C, 0
		jmp	short loc_1403D
_eff_14067 endp


; =============== S U B	R O U T	I N E =======================================


sub_14087 proc near		; CODE XREF: _eff_13DE5p _eff_13DEFp
		xor	ah, ah
		or	al, al
		jz	short loc_14090
		mov	[bx+34h], al

loc_14090::				; CODE XREF: sub_14087+4j
		mov	al, [bx+34h]
		cmp	_byte_24668, 0
		jz	short loc_140A2
		cmp	al, 0E0h ; '‡'
		jnb	short loc_140B3
		shl	ax, 2
		retn
; ---------------------------------------------------------------------------

loc_140A2::				; CODE XREF: sub_14087+11j
		cmp	al, 0E0h ; '‡'
		jbe	short loc_140B3
		mov	dl, al
		and	al, 0Fh
		cmp	dl, 0F0h ; ''
		jbe	short locret_140B2
		shl	ax, 2

locret_140B2::				; CODE XREF: sub_14087+26j
		retn
; ---------------------------------------------------------------------------

loc_140B3::				; CODE XREF: sub_14087+15j
					; sub_14087+1Dj
		xor	ax, ax
		retn
sub_14087 endp


; =============== S U B	R O U T	I N E =======================================


sub_140B6 proc near		; CODE XREF: _gravis_set+1Ep
					; _gravis_int+91p ...
		cmp	_byte_24671, 1
		jz	short locret_140E5
		cmp	_play_state, 1
		jz	short locret_140E5
		inc	_byte_24668
		mov	al, _byte_24668
		cmp	al, _byte_24667
		jnb	short loc_140E6
		mov	bx, offset _channels_25908
		mov	cx, _mod_channels_number

loc_140D8::				; CODE XREF: sub_140B6+2Dj
		push	bx
		push	cx
		call	chanl_2_eff_13813
		pop	cx
		pop	bx
		add	bx, 50h	; 'P'
		dec	cx
		jnz	short loc_140D8

locret_140E5::				; CODE XREF: sub_140B6+5j sub_140B6+Cj
		retn
; ---------------------------------------------------------------------------

loc_140E6::				; CODE XREF: sub_140B6+19j
		mov	_byte_24668, 0
		cmp	_byte_2466D, 0
		jnz	short loc_140F7
		call	sub_135CA
		jmp	short loc_14111
; ---------------------------------------------------------------------------

loc_140F7::				; CODE XREF: sub_140B6+3Aj
		mov	bx, offset _channels_25908
		mov	cx, _mod_channels_number

loc_140FE::				; CODE XREF: sub_140B6+53j
		push	bx
		push	cx
		call	chanl_2_eff_13813
		pop	cx
		pop	bx
		add	bx, 50h	; 'P'
		dec	cx
		jnz	short loc_140FE
		mov	si, word ptr [_pointer_245B4]
		jmp	short $+2

loc_14111::				; CODE XREF: sub_140B6+3Fj
					; _midi_int8p+43p
		cmp	_byte_2466B, 1
		jz	loc_141BA
		cmp	_byte_2466A, 1
		jz	short loc_14153
		cmp	_byte_2466C, 0
		jz	short loc_14131
		xor	al, al
		xchg	al, _byte_2466C
		mov	_byte_2466D, al

loc_14131::				; CODE XREF: sub_140B6+70j
		cmp	_byte_2466D, 0
		jz	short loc_1413E
		dec	_byte_2466D
		jnz	short loc_14142

loc_1413E::				; CODE XREF: sub_140B6+80j
		inc	_word_245F6

loc_14142::				; CODE XREF: sub_140B6+86j
		mov	bx, _word_245F0
		movzx	ax, [_byte_281E8+bx]
		cmp	_word_245F6, ax
		jbe	loc_141DA

loc_14153::				; CODE XREF: sub_140B6+69j
		cmp	_play_state, 2
		jz	short loc_14184
		inc	_word_245F0
sub_140B6 endp ; sp-analysis	failed


; =============== S U B	R O U T	I N E =======================================


sub_1415E proc near		; CODE XREF: sub_12F56+11p
		mov	ax, _word_245FA
		cmp	_word_245F0, ax
		jb	short loc_14184
		test	_flag_playsetttings, 4
		jz	short _vlm_141DF
		mov	ax, _word_245F8
		mov	_word_245F0, ax
		or	ax, ax
		jnz	short loc_14184
		mov	al, _byte_2467A
		call	sub_13CF6
		mov	al, _byte_24679
		call	_eff_13CE8

loc_14184::				; CODE XREF: sub_140B6+A2j
					; sub_1415E+7j	...
		mov	bx, _word_245F0
		mov	al, [_byte_280E8+bx]
		or	al, al
		jz	short loc_141A2
		push	bx
		cmp	al, 0FFh
		jnz	short loc_1419E
		mov	al, _byte_2467A
		call	sub_13CF6
		mov	al, _byte_24679

loc_1419E::				; CODE XREF: sub_1415E+35j
		call	_eff_13CE8
		pop	bx

loc_141A2::				; CODE XREF: sub_1415E+30j
		bts	word ptr [_byte_282E8], bx
		movzx	bx, [_byte_27FE8+bx]
		mov	_my_seg_index,	bx
		shl	bx, 1
		mov	es, [_segs_table+bx]
		mov	word ptr [_pointer_245B4+2], es

loc_141BA::				; CODE XREF: sub_140B6+60j
		xor	ax, ax
		xchg	al, _byte_24669
		mov	_word_245F6, ax
		call	sub_11C0C
		mov	_byte_2466A, 0
		mov	_byte_2466B, 0
		mov	_byte_2466C, 0
		mov	_byte_2466D, 0

loc_141DA::				; CODE XREF: sub_140B6+99j
		mov	word ptr [_pointer_245B4], si
		retn
sub_1415E endp


; =============== S U B	R O U T	I N E =======================================


_vlm_141DF proc near		; CODE XREF: _eff_13B06+31p
					; sub_1415E+Ej
		push	cs
		call	near ptr _volume_12A66
		mov	_byte_24671, 1
		mov	dl, 1
		mov	bx, 5344h	; DS
		mov	cx, 4D50h	; MP
		mov	ax, 60FFh
		int	2Fh		; IPLAY: get data seg
		retn
_vlm_141DF endp


; =============== S U B	R O U T	I N E =======================================


_snd_initialze proc near	; CODE XREF: sub_12DA8+78p

; FUNCTION CHUNK AT 526B SIZE 000000DB BYTES

		cmp	_snd_init, 1
		jz	short loc_1420D
		mov	_snd_init, 1
		movzx	bx, _sndcard_type
		shl	bx, 1
		jmp     _covox_init
; ---------------------------------------------------------------------------

loc_1420D::				; CODE XREF: _snd_initialze+5j
					; _snd_on+5j ...
		stc
		retn
_snd_initialze endp ;	sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_snd_on proc near		; CODE XREF: sub_12EBA+87p
		cmp	_snd_init, 1
		jnz	short loc_1420D
		cmp	_snd_set_flag,	1
		jz	short loc_1420D
		mov	_snd_set_flag,	1
		movzx	bx, _sndcard_type
		shl	bx, 1
		jmp     _covox_on
_snd_on endp


; =============== S U B	R O U T	I N E =======================================


_snd_off proc	near		; CODE XREF: _snd_offx+8p _snd_deinit+Cp

; FUNCTION CHUNK AT 0B5A SIZE 000001CC BYTES

		cmp	_snd_init, 1
		jnz	short loc_1420D
		cmp	_snd_set_flag,	0
		jz	short loc_1420D
		mov	_snd_set_flag,	0
		push	cs
		call	near ptr _volume_12A66
		movzx	bx, _sndcard_type
		shl	bx, 1
		jmp     _covox_off
_snd_off endp	; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_snd_deinit proc near		; CODE XREF: _deinit_125B9+18p
		cmp	_snd_init, 1
		jnz	short loc_1420D
		mov	_snd_init, 0
		call	_snd_off
		movzx	bx, _sndcard_type
		shl	bx, 1
		jmp     _covox_deinit
_snd_deinit endp


; =============== S U B	R O U T	I N E =======================================


 ; sp-analysis failed

; START	OF FUNCTION CHUNK FOR _wss_test

loc_14332::				; CODE XREF: _proaud_init+22j
					; _proaud_init+E2j ...
		stc
		retn
; END OF FUNCTION CHUNK	FOR _wss_test

; =============== S U B	R O U T	I N E =======================================




; ---------------------------------------------------------------------------
		db 87h,	0DBh

; =============== S U B	R O U T	I N E =======================================


; void __cdecl _gravis_int()



; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


; void __cdecl _proaud_14700()



; =============== S U B	R O U T	I N E =======================================


 ;	sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


 ;	sp-analysis failed


; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================




; ---------------------------------------------------------------------------
_wss_freq_table	dw 5513			; DATA XREF: _wss_test+3Er
					; _wss_test:loc_149DFr
_wss_freq_table2	dw  1,19D7h,0Fh,1F40h, 0,2580h,0Eh,2B11h, 3,3E80h, 2,49D4h
					; DATA XREF: _wss_test+51r
		dw  5,5622h, 7,6B25h, 4,7D00h, 6,8133h,0Dh,93A8h, 9,0AC44h
		dw 0Bh,0BB80h,0Ch

; =============== S U B	R O U T	I N E =======================================




; ---------------------------------------------------------------------------

_wss_int::				; DATA XREF: _wss_set+18o
		push	ax
		push	dx
; ---------------------------------------------------------------------------
		db 0BAh	; ∫
_word_14913	dw 536h			; DATA XREF: _wss_set+14w
; ---------------------------------------------------------------------------
		xor	al, al
		out	dx, al
		push	ds
		mov	ax, seg003
		mov	ds, ax
		jmp	loc_14E10

; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


_sb16_init proc near		; DATA XREF: seg003:0D08o
		mov	_sndflags_24622, 9
		mov	_is_stereo, 1
		mov	_bit_mode, 16
		call	_sb16_detect_port
		mov	dx, offset _aErrorSoundcardN ; "Error: Soundcard	not found!\r\n"
		jb	loc_14332
		mov	al, _irq_number
		mov	_sb_irq_number, al
		cmp	al, 0FFh
		jnz	short loc_14ABB
		mov	ah, 80h	; 'Ä'
		call	_ReadMixerSB
		cmp	al, 0FFh
		jz	short loc_14ABB
		mov	ah, 2
		shr	al, 1
		jb	short loc_14AB3
		mov	ah, 5
		shr	al, 1
		jb	short loc_14AB3
		mov	ah, 7
		shr	al, 1
		jb	short loc_14AB3
		mov	ah, 0Ah

loc_14AB3::				; CODE XREF: _sb16_init+30j
					; _sb16_init+36j ...
		mov	_sb_irq_number, ah
		mov	_irq_number, ah

loc_14ABB::				; CODE XREF: _sb16_init+21j
					; _sb16_init+2Aj
		mov	al, _dma_channel
		mov	_dma_chn_mask,	al
		cmp	al, 0FFh
		jnz	short loc_14AFD
		mov	ah, 81h	; 'Å'
		call	_ReadMixerSB
		cmp	al, 0FFh
		jz	short loc_14AFD
		cmp	_bit_mode, 8
		jz	short loc_14AE7
		mov	ah, 7
		test	al, 80h
		jnz	short loc_14AF5
		mov	ah, 6
		test	al, 40h
		jnz	short loc_14AF5
		mov	ah, 5
		test	al, 20h
		jnz	short loc_14AF5

loc_14AE7::				; CODE XREF: _sb16_init+60j
		mov	ah, 3
		test	al, 4
		jnz	short loc_14AF5
		mov	ah, 1
		test	al, 2
		jnz	short loc_14AF5
		mov	ah, 0

loc_14AF5::				; CODE XREF: _sb16_init+66j
					; _sb16_init+6Cj ...
		mov	_dma_chn_mask,	ah
		mov	_dma_channel, ah

loc_14AFD::				; CODE XREF: _sb16_init+50j
					; _sb16_init+59j
		call	_sb16_sound_on
		mov	eax, 1000h
		mov	cl, _dma_chn_mask
		call	_alloc_dma_buf
		mov	word ptr [_dma_buf_pointer], 0
		mov	word ptr [_dma_buf_pointer+2], ax
		clc
		retn
_sb16_init endp


; =============== S U B	R O U T	I N E =======================================


_sb16_on proc	near		; DATA XREF: seg003:0D1Eo
		call	sub_13017
		mov	_dma_mode, 58h	; 'X'
		mov	_word_2460E, 1000h
		mov	si, offset _sb_callback ; myfunc
		mov	al, _sb_irq_number
		call	_setsnd_handler
		mov	dx, _sb_base_port
		add	dl, 0Ch

loc_14B36::				; CODE XREF: _sb16_on+21j
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14B36
		mov	al, 41h	; 'A'
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14B3E::				; CODE XREF: _sb16_on+29j
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14B3E
		mov	al, byte ptr [_freq1+1]
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14B47::				; CODE XREF: _sb16_on+32j
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14B47
		mov	al, byte ptr [_freq1]
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14B50::				; CODE XREF: _sb16_on+3Bj
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14B50
		cmp	_bit_mode, 16
		jz	short loc_14B6A
		mov	ax, _sb_base_port
		add	al, 0Eh
		mov	cs:[_word_14BBB], ax
		mov	ax, 0C6h ; '∆'
		jmp	short loc_14B76
; ---------------------------------------------------------------------------

loc_14B6A::				; CODE XREF: _sb16_on+42j
		mov	ax, _sb_base_port
		add	al, 0Fh
		mov	cs:[_word_14BBB], ax
		mov	ax, 10B6h

loc_14B76::				; CODE XREF: _sb16_on+50j
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14B77::				; CODE XREF: _sb16_on+62j
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14B77
		mov	al, _is_stereo
		and	al, 1
		shl	al, 5
		or	al, ah
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14B87::				; CODE XREF: _sb16_on+72j
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14B87
		mov	ax, _word_2460E
		shr	ax, 2
		mov	cl, _bit_mode
		shr	cl, 4
		and	cl, 1
		shr	ax, cl
		dec	ax
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14BA0::				; CODE XREF: _sb16_on+8Bj
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14BA0
		mov	al, ah
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		mov	cl, _dma_chn_mask
		call	_dma_186E3
		mov	_byte_2466E, 1
		retn
_sb16_on endp

; ---------------------------------------------------------------------------

; void __cdecl _sb_callback()
_sb_callback::				; DATA XREF: _sb16_on+Eo
		push	ax
		push	dx
; ---------------------------------------------------------------------------
		db 0BAh	; ∫
_word_14BBB	dw 22Fh			; DATA XREF: _sb16_on+49w _sb16_on+57w
; ---------------------------------------------------------------------------
		in	al, dx
		push	ds
		mov	ax, seg003
		mov	ds, ax
		jmp	loc_14E10

; =============== S U B	R O U T	I N E =======================================


_sb16_off proc near		; DATA XREF: seg003:0D34o
		pushf
		cli
		cmp	_byte_2466E, 1
		jnz	short loc_14BFD
		cli
		mov	dx, _sb_base_port
		add	dl, 0Ch

loc_14BD8::				; CODE XREF: _sb16_off+14j
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14BD8
		mov	al, 0DAh ; '⁄'
		cmp	_bit_mode, 8
		jz	short loc_14BE8
		mov	al, 0D9h ; 'Ÿ'

loc_14BE8::				; CODE XREF: _sb16_off+1Dj
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14BE9::				; CODE XREF: _sb16_off+25j
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14BE9
		call	_restore_intvector
		mov	cl, _dma_chn_mask
		call	_set_dmachn_mask
		mov	_byte_2466E, 0

loc_14BFD::				; CODE XREF: _sb16_off+7j
		popf
		retn
_sb16_off endp


; =============== S U B	R O U T	I N E =======================================


_sb16_deinit proc near	; DATA XREF: seg003:0D4Ao
		call	_memfree_18A28
		call	_sb16_sound_off
		retn
_sb16_deinit endp


; =============== S U B	R O U T	I N E =======================================




; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR _sb_set

_sbpro_set::				; CODE XREF: _sb_set+6j
					; DATA XREF: seg003:0D20o
		call	sub_13017
		mov	_dma_mode, 58h	; 'X'
		mov	_word_2460E, 1000h
		mov	ax, _sb_base_port
		add	al, 0Eh
		mov	cs:[_word_14CEB], ax
		mov	ah, 0Eh
		call	_ReadMixerSB
		mov	_byte_24664, al
		and	al, 0FDh
		cmp	_is_stereo, 0
		jz	short loc_14C89
		call	_WriteMixerSB
		or	al, 22h

loc_14C89::				; CODE XREF: _sb_set-F5j
		call	_WriteMixerSB
		pushf
		cli
		mov	dx, _sb_base_port
		add	dl, 0Eh
		in	al, dx		; DMA controller, 8237A-5.
					; Clear	mask registers.
					; Any OUT enables all 4	channels.
		sub	dl, 2

loc_14C99::				; CODE XREF: _sb_set-DBj
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14C99
		mov	al, 40h	; '@'
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14CA1::				; CODE XREF: _sb_set-D3j
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14CA1
		mov	al, _sb_timeconst
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14CAA::				; CODE XREF: _sb_set-CAj
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14CAA
		mov	al, 48h	; 'H'
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14CB2::				; CODE XREF: _sb_set-C2j
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14CB2
		mov	ax, _word_2460E
		shr	ax, 2
		dec	ax
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14CBF::				; CODE XREF: _sb_set-B5j
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14CBF
		mov	al, ah
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.

loc_14CC7::				; CODE XREF: _sb_set-ADj
		in	al, dx		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		or	al, al
		js	short loc_14CC7
		mov	al, 90h	; 'ê'
		out	dx, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		mov	_byte_2466E, 1
		mov	si, offset loc_14CE8 ; myfunc
		mov	al, _sb_irq_number
		call	_setsnd_handler
		mov	cl, _dma_chn_mask
		call	_dma_186E3
		popf
		retn
; END OF FUNCTION CHUNK	FOR _sb_set
; ---------------------------------------------------------------------------

loc_14CE8::				; DATA XREF: _sb_set-A3o
		push	ax
		push	dx
; ---------------------------------------------------------------------------
		db 0BAh	; ∫
_word_14CEB	dw 22Eh			; DATA XREF: _sb_set-108w
; ---------------------------------------------------------------------------
		in	al, dx
		push	ds
		mov	ax, seg003
		mov	ds, ax
		jmp	loc_14E10

; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


	; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


 ; sp-analysis failed

; START	OF FUNCTION CHUNK FOR _proaud_14700

loc_14E10::				; CODE XREF: _proaud_14700+15j
					; _text:491Ej	...
		add	_word_24602, 400h
		and	_word_24602, 0FFFh
		inc	_byte_24620
		cmp	_byte_24620, 2
		ja	_lc_disable_interpol

loc_14E29::				; CODE XREF: _proaud_14700+7BCj
		pushad
		push	es
		push	fs
		push	gs
		mov	al, 20h	; ' '
		out	20h, al		; Interrupt controller,	8259A.
		test	byte ptr [_config_word+1], 10h
		jz	short loc_14E4D
		inc	_byte_24621
		and	_byte_24621, 3
		jnz	short loc_14E4D
		mov	cl, _dma_channel
		call	_dma_186E3

loc_14E4D::				; CODE XREF: _proaud_14700+739j
					; _proaud_14700+744j
		mov	bx, 1
		mov	cl, _irq_number
		shl	bx, cl
		mov	dx, 21h	; '!'
		or	bh, bh
		jz	short loc_14E66
		mov	dx, 0A1h ; '°'
		mov	al, 20h	; ' '
		out	0A0h, al	; PIC 2	 same as 0020 for PIC 1
		mov	bl, bh

loc_14E66::				; CODE XREF: _proaud_14700+75Bj
		in	al, dx		; Interrupt Controller #2, 8259A
		or	al, bl
		out	dx, al		; Interrupt Controller #2, 8259A
		sti
		mov	ax, _word_24602

loc_14E6E::				; CODE XREF: _proaud_14700+78Aj
		mov	dx, ax
		cmp	ax, _samples_outoffs_24600
		ja	short loc_14E79
		add	ax, 1000h

loc_14E79::				; CODE XREF: _proaud_14700+774j
		sub	ax, _samples_outoffs_24600
		cmp	ax, 800h
		jb	short loc_14E8C
		push	dx
		call	prepare_samples
		pop	ax
		add	ax, 10h
		jmp	short loc_14E6E
; ---------------------------------------------------------------------------

loc_14E8C::				; CODE XREF: _proaud_14700+780j
		mov	bx, 1
		mov	cl, _irq_number
		shl	bx, cl
		mov	dx, 21h	; '!'
		or	bh, bh
		jz	short loc_14EA1
		mov	dx, 0A1h ; '°'
		mov	bl, bh

loc_14EA1::				; CODE XREF: _proaud_14700+79Aj
		not	bl
		in	al, dx		; Interrupt Controller #2, 8259A
		and	al, bl
		out	dx, al		; Interrupt Controller #2, 8259A
		pop	gs
		pop	fs
		pop	es
		popad
		dec	_byte_24620
		pop	ds
		pop	dx
		pop	ax
		iret
; ---------------------------------------------------------------------------

_lc_disable_interpol::			; CODE XREF: _proaud_14700+725j
		and	_flag_playsetttings, 0EFh
		jmp	loc_14E29
; END OF FUNCTION CHUNK	FOR _proaud_14700
; ---------------------------------------------------------------------------
		mov	al, 20h	; ' '
		cmp	_irq_number, 7
		jbe	short loc_14ECC
		out	0A0h, al	; PIC 2	 same as 0020 for PIC 1
		jmp	short $+2

loc_14ECC::				; CODE XREF: _text:4EC6j
		out	20h, al		; Interrupt controller,	8259A.
		pop	ds
		pop	dx
		pop	ax
		iret

; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


_timer_int_end proc far	; CODE XREF: _covox_timer_int+22j
					; _covox_timer_int+33j ...
		cmp	cs:[_byte_14F70], 0
		jz	short loc_14F3C
		pushad
		push	ds
		push	es
		push	fs
		push	gs
		mov	ax, seg003
		mov	ds, ax
		mov	ax, _word_245E4
		mov	cs:[audio_len], ax
		sti
		call	prepare_samples
		pop	gs
		pop	fs
		pop	es
		pop	ds
		popad
		iret
; ---------------------------------------------------------------------------

loc_14F3C::				; CODE XREF: _timer_int_end+6j
		mov	cs:[audio_len], 1
		jmp	cs:[_int8addr]
_timer_int_end endp

; ---------------------------------------------------------------------------
		dec	cs:[_byte_14F73]
		jz	short loc_14F50
		iret
; ---------------------------------------------------------------------------

loc_14F50::				; CODE XREF: _text:4F4Dj
		push	ax
		mov	al, cs:[_byte_14F72]
		mov	cs:[_byte_14F73], al
		mov	ax, cs:[_timer_word_14F6E]
		call	_set_timer
		pop	ax
		jmp	cs:[_int8addr]
; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
_int8addr	dd 0			; DATA XREF: sub_12DA8+6Aw
					; _clean_int8_mem_timr+5r ...
audio_len	dw 0			; DATA XREF: _configure_timer+1Bw
					; _memfill8080+Dw ...
_timer_word_14F6E dw 0			; DATA XREF: _set_timerw _text:4F59r
_byte_14F70	db 0			; DATA XREF: _configure_timer+12w
					; _memfill8080+7w ...
_byte_14F71	db 0			; DATA XREF: sub_12D35:loc_12D41w
					; sub_12D35:loc_12D4Ew
_byte_14F72	db 0			; DATA XREF: sub_13CF6+Dw _text:4F51r
_byte_14F73	db 0			; DATA XREF: sub_13CF6+11w
					; _text:4F48w	...

; =============== S U B	R O U T	I N E =======================================


_covox_init proc near		; DATA XREF: seg003:0D0Eo
		mov	_sndflags_24622, 3
		mov	_is_stereo, 0
		mov	_bit_mode, 8
		cmp	_snd_base_port, 0FFFFh
		jnz	short loc_14F95
		xor	ax, ax
		mov	es, ax
		assume es:nothing
		mov	ax, es:[408h]
		mov	_snd_base_port, ax

loc_14F95::				; CODE XREF: _covox_init+14j
		mov	ax, _snd_base_port
		mov	cs:[_word_14FC8], ax
		pushf
		cli
		mov	dx, offset _covox_timer_int
		call	_set_timer_int
		sub	ax, 0F00h
		mov	cs:[_word_14FC0], ax
		mov	cs:[_word_14FC5], 0F000h
		popf
		clc
		retn
_covox_init endp


; =============== S U B	R O U T	I N E =======================================


_covox_on proc near		; DATA XREF: seg003:0D24o
		call	_configure_timer
		retn
_covox_on endp

; ---------------------------------------------------------------------------

; =============== S U B	R O U T	I N E =======================================


_covox_timer_int proc	far	; DATA XREF: _covox_init+2Ao
		push	ax
		push	dx
		push	ds
; ---------------------------------------------------------------------------
		db 0BAh	; ∫		; self moifying
_word_14FC0	dw 1000h		; DATA XREF: _covox_init+33w
; ---------------------------------------------------------------------------
		mov	ds, dx
; ---------------------------------------------------------------------------
		assume ds:nothing
		db 0A0h	; †
_word_14FC5	dw 1234h		; DATA XREF: _covox_init+37w
					; _covox_timer_int+16w ...
		db 0BAh	; ∫
_word_14FC8	dw 378h			; DATA XREF: _covox_init+24w
; ---------------------------------------------------------------------------
		out	dx, al		; Printer Data Latch:
					; send byte to printer
		mov	al, 20h	; ' '
		out	20h, al		; Interrupt controller,	8259A.
		pop	ds
		assume ds:seg003
		pop	dx
		pop	ax
		inc	cs:[_word_14FC5]
		jz	short loc_14FE3
		dec	cs:[audio_len]
		jz	near ptr _timer_int_end
		iret
; ---------------------------------------------------------------------------

loc_14FE3::				; CODE XREF: _covox_timer_int+1Bj
		mov	cs:[_word_14FC5], 0F000h
		dec	cs:[audio_len]
		jz	near ptr _timer_int_end
		iret
_covox_timer_int endp	; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_covox_off proc near	; DATA XREF: seg003:0D3Ao
		call	_memfill8080
		retn
_covox_off endp


; =============== S U B	R O U T	I N E =======================================


_covox_deinit proc near	; DATA XREF: seg003:0D50o
		call	_clean_int8_mem_timr
		retn
_covox_deinit endp


; =============== S U B	R O U T	I N E =======================================


_stereo_init proc near	; DATA XREF: seg003:0D10o
		mov	_sndflags_24622, 3
		mov	_is_stereo, 1
		mov	_bit_mode, 8
		cmp	_snd_base_port, -1
		jnz	short loc_1501D
		xor	ax, ax
		mov	es, ax
		mov	ax, es:[408h]
		mov	_snd_base_port, ax

loc_1501D::				; CODE XREF: _stereo_init+14j
		mov	ax, _snd_base_port
		add	ax, 2
		mov	cs:[_word_1504D], ax
		pushf
		cli
		mov	dx, offset _stereo_timer_int
		call	_set_timer_int
		sub	ax, 0F00h
		mov	word ptr cs:[loc_15047+1], ax
		mov	cs:[_word_15056], 0F000h
		popf
		clc
		retn
_stereo_init endp


; =============== S U B	R O U T	I N E =======================================


_stereo_set proc near		; DATA XREF: seg003:0D26o
		call	_configure_timer
		retn
_stereo_set endp


; =============== S U B	R O U T	I N E =======================================


_stereo_timer_int proc far	; DATA XREF: _stereo_init+2Do
		push	ax
		push	dx
		push	ds

loc_15047::				; DATA XREF: _stereo_init+36w
		mov	dx, _text
		mov	ds, dx
; ---------------------------------------------------------------------------
		assume ds:_text
		db 0BAh
_word_1504D	dw 37Ah			; DATA XREF: _stereo_init+27w
; ---------------------------------------------------------------------------
		mov	al, 2
		out	dx, al
		sub	dl, 2
; ---------------------------------------------------------------------------
		db 0A1h
_word_15056	dw 1234h		; DATA XREF: _stereo_init+3Aw
					; _stereo_timer_int+28w	...
; ---------------------------------------------------------------------------
		out	dx, al		; Printer Data Latch:
					; send byte to printer
		add	dl, 2
		mov	al, 1
		out	dx, al		; Printer Control Bits:
					; 0: 1 when sending byte
					; 1: 1 causes LF after CR
					; 2: 0 resets the printer
					; 3: 1 selects the printer
					; 4: +IRQ Enable
		sub	dl, 2
		mov	al, ah
		out	dx, al		; Printer Data Latch:
					; send byte to printer
		mov	al, 20h	; ' '
		out	20h, al		; Interrupt controller,	8259A.
		pop	ds
		assume ds:seg003
		pop	dx
		pop	ax
		add	cs:[_word_15056], 2
		jb	short loc_1507E
		dec	cs:[audio_len]
		jz	near ptr _timer_int_end
		iret
; ---------------------------------------------------------------------------

loc_1507E::				; CODE XREF: _stereo_timer_int+2Ej
		mov	cs:[_word_15056], 0F000h
		dec	cs:[audio_len]
		jz	near ptr _timer_int_end
		iret
_stereo_timer_int endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_stereo_sndoff proc near	; DATA XREF: seg003:0D3Co
		call	_memfill8080
		retn
_stereo_sndoff endp


; =============== S U B	R O U T	I N E =======================================


_stereo_clean proc near	; DATA XREF: seg003:0D52o
		call	_clean_int8_mem_timr
		retn
_stereo_clean endp


; =============== S U B	R O U T	I N E =======================================


_adlib_init proc near		; DATA XREF: seg003:0D12o
		mov	_sndflags_24622, 0Bh
		mov	_is_stereo, 0
		mov	_bit_mode, 8
		call	_adlib_18389
		mov	ax, 2120h
		call	_adlib_18395
		mov	ax, 0F060h
		call	_adlib_18395
		mov	ax, 0F080h
		call	_adlib_18395
		mov	ax, 1C0h
		call	_adlib_18395
		mov	ax, 0E0h ; '‡'
		call	_adlib_18395
		mov	ax, 3F43h
		call	_adlib_18395
		mov	ax, 0B0h ; '∞'
		call	_adlib_18395
		mov	ax, 0A0h ; '†'
		call	_adlib_18395
		mov	ax, 8FA0h
		call	_adlib_18395
		mov	ax, 2EB0h
		call	_adlib_18395
		mov	cx, 4000h

loc_150E8::				; CODE XREF: _adlib_init+52j
		dec	cx
		jnz	short loc_150E8
		mov	ax, 20B0h
		call	_adlib_18395
		mov	ax, 0A0h ; '†'
		call	_adlib_18395
		mov	ax, 40h	; '@'
		call	_adlib_18395
		pushf
		cli
		mov	dx, offset _adlib_timer_int
		call	_set_timer_int
		sub	ax, 0F00h
		mov	word ptr cs:[loc_15120+1], ax
		mov	cs:[_word_15126], 0F000h
		popf
		clc
		retn
_adlib_init endp


; =============== S U B	R O U T	I N E =======================================


_adlib_set proc near		; DATA XREF: seg003:0D28o
		call	_configure_timer
		retn
_adlib_set endp

; ---------------------------------------------------------------------------

_adlib_timer_int::			; DATA XREF: _adlib_init+68o
		push	ax
		push	bx
		push	dx
		push	ds

loc_15120::				; DATA XREF: _adlib_init+71w
		mov	ax, 1234h	; self modifying
		mov	ds, ax
; ---------------------------------------------------------------------------
		assume ds:nothing
		db 0A0h	; †		; self modifying
_word_15126	dw 1234h		; DATA XREF: _adlib_init+75w
					; _text:5135w	...
; ---------------------------------------------------------------------------
		mov	bx, seg003
		mov	ds, bx
		assume ds:seg003
		mov	bx, offset _table_24898
		xlat
		mov	dx, 389h
		out	dx, al
		inc	cs:[_word_15126]
		jz	short loc_1514E

loc_1513C::				; CODE XREF: _text:5155j
		pop	ds
		pop	dx
		pop	bx
		mov	al, 20h	; ' '
		out	20h, al		; Interrupt controller,	8259A.
		pop	ax
		dec	cs:[audio_len]
		jz	near ptr _timer_int_end
		iret
; ---------------------------------------------------------------------------

loc_1514E::				; CODE XREF: _text:513Aj
		mov	cs:[_word_15126], 0F000h
		jmp	short loc_1513C

; =============== S U B	R O U T	I N E =======================================


_adlib_sndoff proc near	; DATA XREF: seg003:0D3Eo
		call	_memfill8080
		retn
_adlib_sndoff endp


; =============== S U B	R O U T	I N E =======================================


_adlib_clean proc near	; DATA XREF: seg003:0D54o
		call	_clean_int8_mem_timr
		call	_adlib_18389
		retn
_adlib_clean endp


; =============== S U B	R O U T	I N E =======================================


_pcspeaker_init proc near	; DATA XREF: seg003:0D14o
		mov	_sndflags_24622, 3
		mov	_is_stereo, 0
		mov	_bit_mode, 8
		pushf
		cli
		mov	al, 90h	; 'ê'
		out	43h, al		; Timer	8253-5 (AT: 8254.2).
		mov	dx, offset _pcspeaker_interrupt
		call	_set_timer_int
		sub	ax, 0F00h
		mov	cs:[_word_1519B], ax
		mov	cs:[_word_151A3], 0F000h
		popf
		clc
		retn
_pcspeaker_init endp


; =============== S U B	R O U T	I N E =======================================


_pcspeaker_on proc near	; DATA XREF: seg003:0D2Ao
		in	al, 61h		; PC/XT	PPI port B bits:
					; 0: Tmr 2 gate	ÕÀÕ OR	03H=spkr ON
					; 1: Tmr 2 data	Õº  AND	0fcH=spkr OFF
					; 3: 1=read high switches
					; 4: 0=enable RAM parity checking
					; 5: 0=enable I/O channel check
					; 6: 0=hold keyboard clock low
					; 7: 0=enable kbrd
		or	al, 3
		out	61h, al		; PC/XT	PPI port B bits:
					; 0: Tmr 2 gate	ÕÀÕ OR	03H=spkr ON
					; 1: Tmr 2 data	Õº  AND	0fcH=spkr OFF
					; 3: 1=read high switches
					; 4: 0=enable RAM parity checking
					; 5: 0=enable I/O channel check
					; 6: 0=hold keyboard clock low
					; 7: 0=enable kbrd
		call	_configure_timer
		retn
_pcspeaker_on endp

; ---------------------------------------------------------------------------

_pcspeaker_interrupt::			; DATA XREF: _pcspeaker_init+15o
		push	bx
		push	ds
; ---------------------------------------------------------------------------
		db 0BBh	; ª
_word_1519B	dw 1000h		; DATA XREF: _pcspeaker_init+1Ew
; ---------------------------------------------------------------------------
		mov	ds, bx
		xor	bh, bh
; ---------------------------------------------------------------------------
		db  8Ah	; ä
		db  1Eh
_word_151A3	dw 1234h		; DATA XREF: _pcspeaker_init+22w
					; _text:51B8w	...
; ---------------------------------------------------------------------------
		mov	bl, cs:[_pc_timer_tbl+bx]
		mov	bh, al
		mov	al, bl
		out	42h, al		; Timer	8253-5 (AT: 8254.2).
		mov	al, 20h	; ' '
		out	20h, al		; Interrupt controller,	8259A.
		mov	al, bh
		pop	ds
		pop	bx
		inc	cs:[_word_151A3]
		jz	short loc_151C9
		dec	cs:[audio_len]
		jz	near ptr _timer_int_end
		iret
; ---------------------------------------------------------------------------

loc_151C9::				; CODE XREF: _text:51BDj
		mov	cs:[_word_151A3], 0F000h
		dec	cs:[audio_len]
		jz	near ptr _timer_int_end
		iret

; =============== S U B	R O U T	I N E =======================================


_pcspeaker_off proc near	; DATA XREF: seg003:0D40o
		call	_memfill8080
		in	al, 61h		; PC/XT	PPI port B bits:
					; 0: Tmr 2 gate	ÕÀÕ OR	03H=spkr ON
					; 1: Tmr 2 data	Õº  AND	0fcH=spkr OFF
					; 3: 1=read high switches
					; 4: 0=enable RAM parity checking
					; 5: 0=enable I/O channel check
					; 6: 0=hold keyboard clock low
					; 7: 0=enable kbrd
		and	al, 0FCh
		out	61h, al		; PC/XT	PPI port B bits:
					; 0: Tmr 2 gate	ÕÀÕ OR	03H=spkr ON
					; 1: Tmr 2 data	Õº  AND	0fcH=spkr OFF
					; 3: 1=read high switches
					; 4: 0=enable RAM parity checking
					; 5: 0=enable I/O channel check
					; 6: 0=hold keyboard clock low
					; 7: 0=enable kbrd
		retn
_pcspeaker_off endp


; =============== S U B	R O U T	I N E =======================================


_pcspeaker_deinit proc	near	; DATA XREF: seg003:0D56o
		call	_clean_int8_mem_timr
		retn
_pcspeaker_deinit endp

; ---------------------------------------------------------------------------
_pc_timer_tbl	db 40h,40h,40h,40h,40h,40h,40h,40h,40h,40h,3Fh,3Fh,3Fh
					; DATA XREF: _text:51A5r
		db 3Fh,3Fh,3Fh,3Fh,3Fh,3Fh,3Fh,3Fh,3Fh,3Eh,3Eh,3Eh,3Eh
		db 3Eh,3Eh,3Eh,3Eh,3Eh,3Eh,3Dh,3Dh,3Dh,3Dh,3Dh,3Dh,3Dh
		db 3Dh,3Dh,3Ch,3Ch,3Ch,3Ch,3Ch,3Ch,3Ch,3Ch,3Ch,3Ch,3Bh
		db 3Bh,3Bh,3Bh,3Bh,3Bh,3Bh,3Bh,3Bh,3Bh,3Ah,3Ah,3Ah,3Ah
		db 3Ah,3Ah,3Ah,3Ah,3Ah,3Ah,39h,39h,39h,39h,39h,39h,39h
		db 39h,39h,39h,38h,38h,38h,38h,38h,38h,38h,38h,37h,37h
		db 37h,37h,37h,36h,36h,36h,36h,35h,35h,35h,35h,34h,34h
		db 34h,33h,33h,32h,32h,31h,31h,30h,30h,2Fh,2Eh,2Dh,2Ch
		db 2Bh,2Ah,29h,28h,27h,26h,25h,24h,23h,22h,21h,20h,1Fh
		db 1Eh
; START	OF FUNCTION CHUNK FOR _snd_initialze
nn::					; CODE XREF: _snd_initialze+13j
		db 1Dh,1Ch,1Bh,1Ah,19h,18h,17h,16h,15h,14h,13h,12h,11h
		db 11h,10h,10h,0Fh,0Fh,0Eh,0Eh,0Dh,0Dh,0Dh,0Ch,0Ch,0Ch
		db 0Ch,0Bh,0Bh,0Bh,0Bh,0Ah,0Ah,0Ah,0Ah,0Ah,9,9,9,9,9,9
		db 9,9,9,8,8,8,8,8,8,8,8,8,8,8,8,7,7,7,7,7,7,7,6,6,6,6
		db 6,6,6,6,6,6,6,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4
		db 4,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1
		db 1,1,1,1,1
; ---------------------------------------------------------------------------

_midi_init::				; DATA XREF: seg003:0D16o
		mov	_sndflags_24622, 12h
		mov	_is_stereo, 1
		mov	_bit_mode, 8
		mov	ax, _snd_base_port
		cmp	ax, 0FFFFh
		jnz	short loc_15302
		mov	ax, 330h

loc_15302::				; CODE XREF: _snd_initialze+1107j
		mov	_word_2465C, ax
		mov	_snd_base_port, ax
		mov	[off_245CA], offset _nullsub_4
		mov	[off_245C8], offset _midi_15466
		mov	[off_245CC], offset _midi_154AC
		mov	[off_245CE], offset _midi_1544D
		mov	bx, offset _channels_25908
		mov	ah, 1

loc_15325::				; CODE XREF: _snd_initialze+1146j
		mov	al, ah
		and	al, 0Fh
		mov	[bx+18h], al
		and	byte ptr [bx+17h], 0FEh
		mov	byte ptr [bx+35h], 0
		add	bx, 50h	; 'P'
		inc	ah
		cmp	ah, 10h
		jbe	short loc_15325
		call	_midi_153C0
		call	_midi_153D6
		clc
		retn
; END OF FUNCTION CHUNK	FOR _snd_initialze

; =============== S U B	R O U T	I N E =======================================


_midi_set proc near		; DATA XREF: seg003:0D2Co
		mov	bx, offset _midi_int8p
		mov	dx, cs
		mov	al, 8
		call	_setint_vect
		retn
_midi_set endp

; ---------------------------------------------------------------------------

; =============== S U B	R O U T	I N E =======================================


_midi_int8p proc far		; DATA XREF: _midi_seto
		pushad
		mov	al, 20h	; ' '
		out	20h, al		; Interrupt controller,	8259A.
		push	ds
		push	es
		push	fs
		push	gs
		mov	ax, seg003
		mov	ds, ax
		assume es:nothing
		cmp	_byte_24671, 1
		jz	short loc_1539A
		inc	_byte_24668
		mov	al, _byte_24668
		cmp	al, _byte_24667
		jnb	short loc_1538F
		mov	bx, offset _channels_25908
		mov	cx, _mod_channels_number

loc_15380::				; CODE XREF: _midi_int8p+37j
		push	bx
		push	cx
		call	chanl_2_eff_13813
		pop	cx
		pop	bx
		add	bx, 50h	; 'P'
		dec	cx
		jnz	short loc_15380
		jmp	short loc_1539A
; ---------------------------------------------------------------------------

loc_1538F::				; CODE XREF: _midi_int8p+23j
		mov	_byte_24668, 0
		call	sub_135CA
		call	loc_14111

loc_1539A::				; CODE XREF: _midi_int8p+16j
					; _midi_int8p+39j
		pop	gs
		pop	fs
		pop	es
		pop	ds
		popad
		iret
_midi_int8p endp


; =============== S U B	R O U T	I N E =======================================


_midi_sndoff proc near	; DATA XREF: seg003:0D42o
		mov	dx, word ptr cs:[_int8addr+2]
		mov	bx, word ptr cs:[_int8addr]
		mov	al, 8
		call	_setint_vect
		call	_clean_timer
		call	_midi_153D6
		retn
_midi_sndoff endp


; =============== S U B	R O U T	I N E =======================================


_midi_clean proc near		; DATA XREF: seg003:0D58o
		mov	ah, 0FFh
		call	_midi_153F1
		retn
_midi_clean endp


; =============== S U B	R O U T	I N E =======================================


_midi_153C0 proc near		; CODE XREF: _snd_initialze+1148p
		mov	ah, 0FFh
		call	_midi_153F1
		mov	cx, 8000h
		call	_midi_15442
		mov	ah, 3Fh	; '?'
		call	_midi_153F1
		xor	cx, cx
		call	_midi_15442
		retn
_midi_153C0 endp


; =============== S U B	R O U T	I N E =======================================


_midi_153D6 proc near		; CODE XREF: _snd_initialze+114Bp
					; _midi_sndoff+12p
		xor	bl, bl

loc_153D8::				; CODE XREF: _midi_153D6+18j
		mov	ah, 0B0h ; '∞'
		or	ah, bl
		call	_midi_15413
		mov	ah, 7Bh	; '{'
		call	_midi_15413
		xor	ah, ah
		call	_midi_15413
		inc	bl
		cmp	bl, 10h
		jb	short loc_153D8
		retn
_midi_153D6 endp


; =============== S U B	R O U T	I N E =======================================


_midi_153F1 proc near		; CODE XREF: _midi_clean+2p
					; _midi_153C0+2p ...
		mov	dx, _word_2465C
		inc	dx
		xor	cx, cx

loc_153F8::				; CODE XREF: _midi_153F1+Dj
		in	al, dx
		test	al, 40h
		jz	short loc_15401
		dec	cx
		jnz	short loc_153F8
		retn
; ---------------------------------------------------------------------------

loc_15401::				; CODE XREF: _midi_153F1+Aj
		mov	al, ah
		out	dx, al
		xor	cx, cx

loc_15406::				; CODE XREF: _midi_153F1+1Bj
		in	al, dx
		shl	al, 1
		jnb	short loc_1540E
		dec	cx
		jnz	short loc_15406

loc_1540E::				; CODE XREF: _midi_153F1+18j
		dec	dx
		in	al, dx
		cmp	al, 0FEh ; '˛'
		retn
_midi_153F1 endp


; =============== S U B	R O U T	I N E =======================================


_midi_15413 proc near		; CODE XREF: _midi_153D6+6p
					; _midi_153D6+Bp ...
		or	ah, ah
		jns	short loc_15421
		cmp	ah, _byte_24677
		jz	short locret_15441
		mov	_byte_24677, ah

loc_15421::				; CODE XREF: _midi_15413+2j
		mov	dx, _word_2465C
		inc	dx
		mov	cl, 0FFh

loc_15428::				; CODE XREF: _midi_15413+23j
		in	al, dx
		test	al, 40h
		jz	short loc_15439
		shl	al, 1
		jb	short loc_15434
		dec	dx
		in	al, dx
		inc	dx

loc_15434::				; CODE XREF: _midi_15413+1Cj
		dec	cl
		jnz	short loc_15428
		retn
; ---------------------------------------------------------------------------

loc_15439::				; CODE XREF: _midi_15413+18j
		dec	dx
		mov	al, ah
		out	dx, al
		sub	_byte_24678, al

locret_15441::				; CODE XREF: _midi_15413+8j
		retn
_midi_15413 endp


; =============== S U B	R O U T	I N E =======================================


_midi_15442 proc near		; CODE XREF: _midi_153C0+8p
					; _midi_153C0+12p
		mov	dx, _word_2465C
		inc	dx

loc_15447::				; CODE XREF: _midi_15442+7j
		in	al, dx
		dec	cx
		jnz	short loc_15447
		retn
_midi_15442 endp


; =============== S U B	R O U T	I N E =======================================


_nullsub_4 proc near		; DATA XREF: _snd_initialze+1112o
		retn
_nullsub_4 endp


; =============== S U B	R O U T	I N E =======================================


_midi_1544D proc near		; CODE XREF: _midi_15466+6p
					; DATA XREF: _snd_initialze+1124o
		and	byte ptr [bx+17h], 0FEh
		call	_midi_154DA
		or	ah, 80h
		call	_midi_15413
		call	_midi_154DE
		call	_midi_15413
		mov	ah, 7Fh	; ''
		call	_midi_15413
		retn
_midi_1544D endp


; =============== S U B	R O U T	I N E =======================================


_midi_15466 proc near		; DATA XREF: _snd_initialze+1118o
		test	byte ptr [bx+17h], 0FEh
		jz	short loc_1546F
		call	_midi_1544D

loc_1546F::				; CODE XREF: _midi_15466+4j
		or	byte ptr [bx+17h], 1
		mov	al, [bx+2]
		cmp	al, [bx+3]
		jz	short loc_1548D
		mov	[bx+3],	al
		call	_midi_154DA
		or	ah, 0C0h
		call	_midi_15413
		mov	ah, [bx+2]
		call	_midi_15413

loc_1548D::				; CODE XREF: _midi_15466+13j
		mov	al, [bx+8]
		call	_midi_154AC
		call	_midi_154DA
		or	ah, 90h
		call	_midi_15413
		call	_midi_154DE
		call	_midi_15413
		mov	ah, 7Fh	; ''
		call	_midi_15413
		or	byte ptr [bx+17h], 1
		retn
_midi_15466 endp


; =============== S U B	R O U T	I N E =======================================


_midi_154AC proc near		; CODE XREF: _midi_15466+2Ap
					; DATA XREF: _snd_initialze+111Eo
		cmp	al, _byte_2467D
		jb	short loc_154B5
		mov	al, _byte_2467D

loc_154B5::				; CODE XREF: _midi_154AC+4j
		cmp	al, [bx+1Bh]
		jz	short locret_154D9
		mov	[bx+1Bh], al
		movzx	di, al
		call	_midi_154DA
		or	ah, 0B0h
		call	_midi_15413
		mov	ah, 7
		call	_midi_15413
		mov	al, 80h	; 'Ä'
		add	di, [off_24656]
		mul	byte ptr [di]
		call	_midi_15413

locret_154D9::				; CODE XREF: _midi_154AC+Cj
		retn
_midi_154AC endp


; =============== S U B	R O U T	I N E =======================================


_midi_154DA proc near		; CODE XREF: _midi_1544D+4p
					; _midi_15466+18p ...
		mov	ah, [bx+18h]
		retn
_midi_154DA endp


; =============== S U B	R O U T	I N E =======================================


_midi_154DE proc near		; CODE XREF: _midi_1544D+Dp
					; _midi_15466+36p
		mov	al, [bx+35h]
		mov	dl, al
		and	dl, 0Fh
		dec	dl
		shr	al, 4
		mov	ah, 0Ch
		mul	ah
		add	al, dl
		mov	ah, al
		retn
_midi_154DE endp


; =============== S U B	R O U T	I N E =======================================


sub_154F4 proc near		; CODE XREF: sub_15577+9p sub_1609F+9p
		mov	ax, _word_245E4
		shr	ax, 4
		mov	_byte_24683, al
		push	bx
		push	si
		mov	bx, [si+26h]
		mov	eax, [si+4]
		shr	eax, 16h
		add	bx, ax
		call	_ems_mapmem
		pop	si
		pop	bx
		mov	eax, [si+4]
		shr	eax, 0Ch
		cmp	word ptr [si+26h], 0FFFFh
		jz	short loc_15525
		and	eax, 3FFh

loc_15525::				; CODE XREF: sub_154F4+29j
		add	ax, [si+24h]
		mov	es, ax
		movzx	ebx, byte ptr [si+23h]
		mov	ax, [si+36h]
		mov	_word_24614, ax
		mov	_byte_24616, 0
		test	_flag_playsetttings, 10h
		jz	short _lc_inerpol_disabld
		cmp	al, ah
		setnz	ah		; dosbox:  setnz sp
		mov	_byte_24616, ah
		movzx	ebx, al

_lc_inerpol_disabld::			; CODE XREF: sub_154F4+4Bj
		shl	ebx, 9
		add	bx, offset _vlm_byte_table
		movzx	ebp, word ptr [si+20h]
		mov	ax, bp
		mov	ch, al
		shr	ebp, 8
		mov	esi, [si+4]
		mov	ax, si
		mov	cl, al
		and	esi, 0FFFh
		shr	esi, 8
		retn
sub_154F4 endp


; =============== S U B	R O U T	I N E =======================================


sub_15577 proc near		; CODE XREF: prepare_samples:loc_16CB9p
					; prepare_samples+58Ap ...

; FUNCTION CHUNK AT 57F2 SIZE 000000C0 BYTES
; FUNCTION CHUNK AT 58C0 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 58EF SIZE 00000021 BYTES
; FUNCTION CHUNK AT 591E SIZE 00000021 BYTES
; FUNCTION CHUNK AT 594D SIZE 00000021 BYTES
; FUNCTION CHUNK AT 597C SIZE 00000021 BYTES
; FUNCTION CHUNK AT 59AB SIZE 00000021 BYTES
; FUNCTION CHUNK AT 59DA SIZE 00000021 BYTES
; FUNCTION CHUNK AT 5A09 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 5A38 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 5A67 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 5A96 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 5AC5 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 5AF4 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 5B23 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 5B52 SIZE 0000002D BYTES
; FUNCTION CHUNK AT 5E48 SIZE 00000257 BYTES

		test	byte ptr [si+17h], 1
		jz	locret_157BC
		push	si
		call	sub_154F4
		test	_flag_playsetttings, 10h
		jnz	_lc_perfrm_interpol
		cmp	_byte_24625, 1
		jz	loc_15E48
		xor	edx, edx
		mov	ax, _word_245E4
		and	eax, 0Fh
		jmp	cs:[_offs_noninterp+eax*2]
sub_15577 endp ; sp-analysis	failed

; START	OF FUNCTION CHUNK FOR sub_1609F

loc_155A8::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_155B8::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_155C8::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_155D8::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_155E8::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_155F8::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_15608::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_15618::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]

loc_15621::
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_15628::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_15638::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_15648::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_15658::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_15668::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_15678::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_15688::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		add	di, 8

loc_15698::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		cmp	_byte_24683, 0
		jz	loc_1578C

loc_156A1::				; CODE XREF: sub_1609F-917j
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+8],	ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+10h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+18h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+20h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+28h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+30h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+38h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+40h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+48h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+50h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+58h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+60h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+68h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+70h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		add	[di+78h], ax
		add	di, 80h	; 'Ä'
		dec	_byte_24683
		jnz	loc_156A1

loc_1578C::				; CODE XREF: sub_1609F-A02j
					; sub_15577+5E0j ...
		mov	eax, esi
		shl	eax, 8
		mov	al, cl
		pop	si
		mov	cx, _word_24614
		mov	[si+36h], cx
		mov	[si+23h], ch
		mov	edx, [si+4]
		and	dx, 0F000h
		add	eax, edx
		mov	edx, eax
		shr	edx, 8
		cmp	edx, [si+48h]
		ja	short loc_157BD
		mov	[si+4],	eax

locret_157BC::				; CODE XREF: sub_15577+4j
		retn
; ---------------------------------------------------------------------------

loc_157BD::				; CODE XREF: sub_1609F-8E9j
		mov	edx, [si+40h]
		shl	edx, 8
		test	byte ptr [si+19h], 8
		jz	short loc_157E5
		mov	ebx, [si+44h]
		shl	ebx, 8

loc_157D3::				; CODE XREF: sub_1609F-8C4j
		sub	eax, ebx
		jb	short loc_157DD
		cmp	eax, edx
		ja	short loc_157D3

loc_157DD::				; CODE XREF: sub_1609F-8C9j
		add	eax, ebx
		mov	[si+4],	eax
		retn
; ---------------------------------------------------------------------------

loc_157E5::				; CODE XREF: sub_1609F-8D6j
		mov	[si+4],	edx
		and	byte ptr [si+17h], 0FEh
		mov	byte ptr [si+35h], 0
		retn
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_15577

_lc_perfrm_interpol::			; CODE XREF: sub_15577+11j
		mov	al, ch
		cmp	al, cs:[_byte_158B4]
		jz	short loc_15877
		mov	cs:[_byte_158B4], al
		mov	cs:[_byte_158E3], al
		mov	cs:[_byte_15912], al
		mov	cs:[_byte_15941], al
		mov	cs:[_byte_15970], al
		mov	cs:[_byte_1599F], al
		mov	cs:[_byte_159CE], al
		mov	cs:[_byte_159FD], al
		mov	cs:[_byte_15A2C], al
		mov	cs:[_byte_15A5B], al
		mov	cs:[_byte_15A8A], al
		mov	cs:[_byte_15AB9], al
		mov	cs:[_byte_15AE8], al
		mov	cs:[_byte_15B17], al
		mov	cs:[_byte_15B46], al
		mov	cs:[_byte_15B81], al
		mov	cs:[_byte_15BAD], al
		mov	cs:[_byte_15BDA], al
		mov	cs:[_byte_15C07], al
		mov	cs:[_byte_15C34], al
		mov	cs:[_byte_15C61], al
		mov	cs:[_byte_15C8E], al
		mov	cs:[_byte_15CBB], al
		mov	cs:[_byte_15CE8], al
		mov	cs:[_byte_15D15], al
		mov	cs:[_byte_15D42], al
		mov	cs:[_byte_15D6F], al
		mov	cs:[_byte_15D9C], al
		mov	cs:[_byte_15DC9], al
		mov	cs:[_byte_15DF6], al
		mov	cs:[_byte_15E23], al

loc_15877::				; CODE XREF: sub_15577+282j
		and	ecx, 0FFh
		mov	ax, _word_245E4
		and	eax, 0Fh
		xor	edx, edx
		jmp	cs:[_offs_interpol+eax*2]

loc_15891::				; CODE XREF: sub_15577+28j
					; sub_1609F+28j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_158B4	db 0			; DATA XREF: sub_15577+27Dr
					; sub_15577+284w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_158C0::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_158E3	db 0			; DATA XREF: sub_15577+288w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_158EF::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15912	db 0			; DATA XREF: sub_15577+28Cw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_1591E::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15941	db 0			; DATA XREF: sub_15577+290w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_1594D::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15970	db 0			; DATA XREF: sub_15577+294w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_1597C::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_1599F	db 0			; DATA XREF: sub_15577+298w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_159AB::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_159CE	db 0			; DATA XREF: sub_15577+29Cw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_159DA::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_159FD	db 0			; DATA XREF: sub_15577+2A0w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_15A09::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15A2C	db 0			; DATA XREF: sub_15577+2A4w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_15A38::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15A5B	db 0			; DATA XREF: sub_15577+2A8w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_15A67::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15A8A	db 0			; DATA XREF: sub_15577+2ACw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_15A96::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15AB9	db 0			; DATA XREF: sub_15577+2B0w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_15AC5::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15AE8	db 0			; DATA XREF: sub_15577+2B4w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_15AF4::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15B17	db 0			; DATA XREF: sub_15577+2B8w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_15B23::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15B46	db 0			; DATA XREF: sub_15577+2BCw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_15577

loc_15B52::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		cmp	_byte_24683, 0
		jz	loc_1578C

loc_15B5B::				; CODE XREF: _text:5E41j
		xor	edx, edx
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_15577
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15B81	db 0			; DATA XREF: sub_15577+2C0w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15BAD	db 0			; DATA XREF: sub_15577+2C4w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+8],	eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15BDA	db 0			; DATA XREF: sub_15577+2C8w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+10h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15C07	db 0			; DATA XREF: sub_15577+2CCw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+18h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15C34	db 0			; DATA XREF: sub_15577+2D0w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+20h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15C61	db 0			; DATA XREF: sub_15577+2D4w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+28h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15C8E	db 0			; DATA XREF: sub_15577+2D8w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+30h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15CBB	db 0			; DATA XREF: sub_15577+2DCw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+38h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15CE8	db 0			; DATA XREF: sub_15577+2E0w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+40h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15D15	db 0			; DATA XREF: sub_15577+2E4w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+48h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15D42	db 0			; DATA XREF: sub_15577+2E8w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+50h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15D6F	db 0			; DATA XREF: sub_15577+2ECw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+58h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15D9C	db 0			; DATA XREF: sub_15577+2F0w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+60h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15DC9	db 0			; DATA XREF: sub_15577+2F4w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+68h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15DF6	db 0			; DATA XREF: sub_15577+2F8w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+70h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_15E23	db 0			; DATA XREF: sub_15577+2FCw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		add	[di+78h], eax
		add	di, 80h	; 'Ä'
		mov	dx, offset loc_15E3D
		cmp	_byte_24616, 1
		jz	loc_1690B

loc_15E3D::				; DATA XREF: _text:5E31o
		dec	_byte_24683
		jnz	loc_15B5B
		jmp	loc_1578C
; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_15577

loc_15E48::				; CODE XREF: sub_15577+1Aj
		xor	edx, edx
		mov	ax, _word_245E4
		and	eax, 0Fh
		jmp	cs:[off_18E60+eax*2]

loc_15E5B::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15E6E::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15E81::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15E94::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15EA7::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15EBA::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15ECD::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15EE0::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15EF3::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15F06::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15F19::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15F2C::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15F3F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15F52::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15F65::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		add	di, 8

loc_15F78::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		cmp	_byte_24683, 0
		jz	loc_1578C

loc_15F81::				; CODE XREF: sub_15577+B21j
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+8],	eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+10h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+18h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+20h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+28h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+30h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+38h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+40h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+48h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+50h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+58h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+60h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+68h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+70h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		add	[di+78h], eax
		add	di, 80h	; 'Ä'
		dec	_byte_24683
		jnz	loc_15F81
		jmp	loc_1578C
; END OF FUNCTION CHUNK	FOR sub_15577

; =============== S U B	R O U T	I N E =======================================


sub_1609F proc near		; CODE XREF: prepare_samples+4Bp
					; prepare_samples+594p ...

; FUNCTION CHUNK AT 55A8 SIZE 0000024A BYTES
; FUNCTION CHUNK AT 60E0 SIZE 00000297 BYTES
; FUNCTION CHUNK AT 6385 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 63B4 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 63E3 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 6412 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 6441 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 649F SIZE 00000021 BYTES
; FUNCTION CHUNK AT 64CE SIZE 00000021 BYTES
; FUNCTION CHUNK AT 64FD SIZE 00000021 BYTES
; FUNCTION CHUNK AT 652C SIZE 00000021 BYTES
; FUNCTION CHUNK AT 655B SIZE 00000021 BYTES
; FUNCTION CHUNK AT 658A SIZE 00000021 BYTES
; FUNCTION CHUNK AT 65B9 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 65E8 SIZE 00000021 BYTES
; FUNCTION CHUNK AT 6617 SIZE 0000002D BYTES
; FUNCTION CHUNK AT 6959 SIZE 00000310 BYTES
; FUNCTION CHUNK AT 745C SIZE 00000393 BYTES
; FUNCTION CHUNK AT 7A72 SIZE 000002AE BYTES
; FUNCTION CHUNK AT 8DB0 SIZE 00000005 BYTES
; FUNCTION CHUNK AT 8DB8 SIZE 00000005 BYTES

		test	byte ptr [si+17h], 1
		jz	loc_16BB0
		push	si
		call	sub_154F4
		test	_flag_playsetttings, 10h
		jnz	_lc_perfrm_interpol2
		cmp	_byte_24625, 1
		jz	loc_16959
		xor	edx, edx
		mov	ax, _word_245E4
		and	eax, 0Fh
		jmp	cs:[_offs_noninterp2+eax*2]
sub_1609F endp ; sp-analysis	failed


loc_160D0::				; DATA XREF: _text:8DDEo
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_160E0::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DDCo
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_160F0::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DDAo
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_16100::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DD8o
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_16110::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DD6o
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_16120::				; CODE XREF: sub_137D5+39j
					; _eff_139B9+86j ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_16130::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DD2o
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_16140::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DD0o
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_16150::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DCEo
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_16160::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DCCo
		mov	dl, es:[si]
		add	cl, ch

loc_16165::				; CODE XREF: _snd_initialze+13j
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_16170::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DCAo
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_16180::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DC8o
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_16190::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DC6o
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_161A0::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DC4o
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_161B0::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DC2o
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		add	di, 8

loc_161C0::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:_offs_noninterp2o
		cmp	_byte_24683, 0
		jz	loc_1578C

loc_161C9::				; CODE XREF: sub_1609F+211j
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+8],	ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+10h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+18h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+20h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+28h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+30h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+38h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+40h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+48h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+50h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+58h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+60h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+68h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+70h], ax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		adc	si, bp
		mov	[di+78h], ax
		add	di, 80h	; 'Ä'
		dec	_byte_24683
		jnz	loc_161C9
		jmp	loc_1578C
; ---------------------------------------------------------------------------

_lc_perfrm_interpol2::			; CODE XREF: sub_1609F+11j
		mov	al, ch
		cmp	al, cs:[_byte_16379]
		jz	short loc_1633C
		mov	cs:[_byte_16379], al
		mov	cs:[_byte_163A8], al
		mov	cs:[_byte_163D7], al
		mov	cs:[_byte_16406], al
		mov	cs:[_byte_16435], al
		mov	byte ptr cs:[unk_16464], al
		mov	cs:[_byte_16493], al
		mov	cs:[_byte_164C2], al
		mov	cs:[_byte_164F1], al
		mov	cs:[_byte_16520], al
		mov	cs:[_byte_1654F], al
		mov	cs:[_byte_1657E], al
		mov	byte ptr cs:[unk_165AD], al
		mov	cs:[_byte_165DC], al
		mov	cs:[_byte_1660B], al
		mov	cs:[_byte_16646], al
		mov	cs:[_byte_16672], al
		mov	cs:[_byte_1669F], al
		mov	cs:[_byte_166CC], al
		mov	cs:[_byte_166F9], al
		mov	cs:[_byte_16726], al
		mov	cs:[_byte_16753], al
		mov	cs:[_byte_16780], al
		mov	cs:[_byte_167AD], al
		mov	cs:[_byte_167DA], al
		mov	cs:[_byte_16807], al
		mov	cs:[_byte_16834], al
		mov	cs:[_byte_16861], al
		mov	cs:[_byte_1688E], al
		mov	cs:[_byte_168BB], al
		mov	cs:[_byte_168E8], al

loc_1633C::				; CODE XREF: sub_1609F+21Fj
		and	ecx, 0FFh
		mov	ax, _word_245E4
		and	eax, 0Fh
		xor	edx, edx
		jmp	cs:[_offs_interpol2+eax*2]

loc_16356::				; CODE XREF: sub_1609F+28j
					; DATA XREF: _text:8DFEo
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]

loc_16369::				; CODE XREF: _snd_initialze+13j
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16379	db 0			; DATA XREF: sub_1609F+21Ar
					; sub_1609F+221w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_16385::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_163A8	db 0			; DATA XREF: sub_1609F+225w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_163B4::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_163D7	db 0			; DATA XREF: sub_1609F+229w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_163E3::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16406	db 0			; DATA XREF: sub_1609F+22Dw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_16412::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16435	db 0			; DATA XREF: sub_1609F+231w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_16441::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db 80h
		db 0C1h	; ¡
unk_16464	db    0			; DATA XREF: sub_1609F+235w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax

loc_1646D::
		add	di, 8

loc_16470::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16493	db 0			; DATA XREF: sub_1609F+239w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_1649F::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_164C2	db 0			; DATA XREF: sub_1609F+23Dw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_164CE::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_164F1	db 0			; DATA XREF: sub_1609F+241w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_164FD::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16520	db 0			; DATA XREF: sub_1609F+245w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_1652C::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_1654F	db 0			; DATA XREF: sub_1609F+249w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_1655B::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde

loc_16568::
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_1657E	db 0			; DATA XREF: sub_1609F+24Dw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_1658A::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db 80h
		db 0C1h	; ¡
unk_165AD	db    0			; DATA XREF: sub_1609F+251w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_165B9::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_165DC	db 0			; DATA XREF: sub_1609F+255w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_165E8::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_1660B	db 0			; DATA XREF: sub_1609F+259w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		add	di, 8
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_16617::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		cmp	_byte_24683, 0
		jz	loc_1578C

loc_16620::				; CODE XREF: _text:6904j
		xor	edx, edx
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16646	db 0			; DATA XREF: sub_1609F+25Dw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16672	db 0			; DATA XREF: sub_1609F+261w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+8],	eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde

loc_16689::
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_1669F	db 0			; DATA XREF: sub_1609F+265w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+10h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_166CC	db 0			; DATA XREF: sub_1609F+269w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+18h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_166F9	db 0			; DATA XREF: sub_1609F+26Dw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+20h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16726	db 0			; DATA XREF: sub_1609F+271w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+28h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16753	db 0			; DATA XREF: sub_1609F+275w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+30h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde

loc_1676A::				; CODE XREF: _snd_initialze+13j
					; _snd_initialze+13j
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16780	db 0			; DATA XREF: sub_1609F+279w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+38h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_167AD	db 0			; DATA XREF: sub_1609F+27Dw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+40h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_167DA	db 0			; DATA XREF: sub_1609F+281w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+48h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16807	db 0			; DATA XREF: sub_1609F+285w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+50h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16834	db 0			; DATA XREF: sub_1609F+289w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+58h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_16861	db 0			; DATA XREF: sub_1609F+28Dw
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+60h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_1688E	db 0			; DATA XREF: sub_1609F+291w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+68h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_168BB	db 0			; DATA XREF: sub_1609F+295w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+70h], eax
		mov	dl, es:[si]
		mov	ax, [ebx+edx*2]
		mov	dl, es:[si+1]
		cwde
		movsx	edx, word ptr [ebx+edx*2]
		sub	edx, eax
		imul	edx, ecx
		sar	edx, 8
		add	eax, edx
; ---------------------------------------------------------------------------
		db  80h	; Ä
		db 0C1h	; ¡
_byte_168E8	db 0			; DATA XREF: sub_1609F+299w
; ---------------------------------------------------------------------------
		adc	si, bp
		xor	edx, edx
		mov	[di+78h], eax
		add	di, 80h	; 'Ä'
		mov	dx, offset loc_16900
		cmp	_byte_24616, 1
		jz	short loc_1690B

loc_16900::				; DATA XREF: _text:68F6o
		dec	_byte_24683
		jnz	loc_16620
		jmp	loc_1578C
; ---------------------------------------------------------------------------

loc_1690B::				; CODE XREF: _text:5E39j _text:68FEj
		mov	ax, _word_24614
		cmp	al, ah
		ja	short loc_16929
		add	al, 4
		cmp	al, ah
		jnb	short loc_16942
		mov	byte ptr [_word_24614], al
		movzx	ebx, al
		shl	ebx, 9
		add	bx, offset _vlm_byte_table
		jmp	dx
; ---------------------------------------------------------------------------

loc_16929::				; CODE XREF: _text:6910j
		sub	al, 4
		jbe	short loc_16942
		cmp	al, ah
		jbe	short loc_16942
		mov	byte ptr [_word_24614], al
		movzx	ebx, al
		shl	ebx, 9
		add	bx, offset _vlm_byte_table
		jmp	dx
; ---------------------------------------------------------------------------

loc_16942::				; CODE XREF: _text:6916j _text:692Bj ...
		mov	byte ptr [_word_24614], ah
		mov	_byte_24616, 0
		movzx	ebx, ah
		shl	ebx, 9
		add	bx, offset _vlm_byte_table
		jmp	dx
; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_16959::				; CODE XREF: sub_1609F+1Aj
		xor	edx, edx
		mov	ax, _word_245E4
		and	eax, 0Fh

loc_16963::				; CODE XREF: _snd_initialze+13j
		jmp	cs:[off_18E00+eax*2]

loc_1696C::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj
					; DATA XREF: ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_1697F::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_16992::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_169A5::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_169B8::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_169CB::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_169DE::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_169F1::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_16A04::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_16A17::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_16A2A::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_16A3D::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_16A50::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_16A63::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_16A76::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		add	di, 8

loc_16A89::				; CODE XREF: sub_1609F+28j
					; sub_1609F+2AEj ...
		cmp	_byte_24683, 0
		jz	loc_1578C

loc_16A92::				; CODE XREF: sub_1609F+B0Aj
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+8],	eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+10h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+18h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+20h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+28h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+30h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+38h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+40h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+48h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+50h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+58h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+60h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+68h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+70h], eax
		mov	dl, es:[si]
		add	cl, ch
		mov	ax, [ebx+edx*2]
		cwde
		adc	si, bp
		mov	[di+78h], eax
		add	di, 80h	; 'Ä'
		dec	_byte_24683
		jnz	loc_16A92
		jmp	loc_1578C
; ---------------------------------------------------------------------------

loc_16BB0::				; CODE XREF: sub_1609F+4j
		mov	cx, _word_245E4
		mov	bx, cx
		shr	cx, 4
		xor	eax, eax
		and	bx, 0Fh
		shl	bx, 1
		jmp	cs:[off_18E80+bx]

loc_16BC6::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16BCC::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16BD2::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16BD8::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16BDE::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16BE4::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16BEA::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16BF0::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16BF6::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16BFC::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16C02::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16C08::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16C0E::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16C14::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16C1A::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	[di], eax
		add	di, 8

loc_16C20::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		jcxz	short locret_16C68

loc_16C22::				; CODE XREF: sub_1609F:loc_16C66j
		mov	[di], eax
		mov	[di+8],	eax
		mov	[di+10h], eax
		mov	[di+18h], eax
		mov	[di+20h], eax
		mov	[di+28h], eax
		mov	[di+30h], eax
		mov	[di+38h], eax
		mov	[di+40h], eax
		mov	[di+48h], eax
		mov	[di+50h], eax
		mov	[di+58h], eax
		mov	[di+60h], eax
		mov	[di+68h], eax
		mov	[di+70h], eax
		mov	[di+78h], eax
		add	di, 80h	; 'Ä'
		dec	cx

loc_16C66::				; CODE XREF: _snd_initialze+13j
		jnz	short loc_16C22

locret_16C68::				; CODE XREF: sub_1609F:loc_16C20j
		retn
; END OF FUNCTION CHUNK	FOR sub_1609F

; =============== S U B	R O U T	I N E =======================================


prepare_samples proc near		; CODE XREF: sub_13017:loc_13038p
					; _proaud_14700+783p ...

; FUNCTION CHUNK AT 71D3 SIZE 0000008C BYTES
; FUNCTION CHUNK AT 77EF SIZE 00000035 BYTES

		call	_ems_save_mapctx
		cld
		mov	ax, _word_245E8
		mov	_word_245E4, ax
		dec	_word_245EE
		jnz	short loc_16C88
		call	sub_140B6
		mov	ax, _word_245EA
		mov	_word_245E4, ax
		mov	ax, _word_245EC
		mov	_word_245EE, ax

loc_16C88::				; CODE XREF: prepare_samples+Ej
		mov	_byte_24682, 0
		cmp	_is_stereo, 1
		jz	loc_171D3
		mov	si, offset _channels_25908
		mov	cx, _mod_channels_number

loc_16C9D::				; CODE XREF: prepare_samples+59j
		cmp	byte ptr [si+1Dh], 0
		jnz	short loc_16CBE
		push	cx
		push	si
		mov	di, offset _chrin
		test	_byte_24682, 1
		jnz	short loc_16CB9
		or	_byte_24682, 1
		call	sub_1609F
		jmp	short loc_16CBC
; ---------------------------------------------------------------------------

loc_16CB9::				; CODE XREF: prepare_samples+44j
		call	sub_15577

loc_16CBC::				; CODE XREF: prepare_samples+4Ej
		pop	si
		pop	cx

loc_16CBE::				; CODE XREF: prepare_samples+38j
		add	si, 50h	; 'P'
		dec	cx
		jnz	short loc_16C9D
		mov	di, _samples_outoffs_24600
		mov	cx, _word_245E4
		mov	si, (offset _chrin+1)
		mov	es, word ptr [_dma_buf_pointer+2]
		assume es:nothing
		mov	ax, 1000h
		sub	ax, di
		cmp	ax, cx
		ja	short loc_16CEB
		mov	bx, cx
		sub	bx, ax
		mov	cx, ax
		push	bx
		call	sub_16CF6
		pop	cx
		xor	di, di
		jcxz	short loc_16CEE

loc_16CEB::				; CODE XREF: prepare_samples+71j
		call	sub_16CF6

loc_16CEE::				; CODE XREF: prepare_samples+80j
		mov	_samples_outoffs_24600, di
		call	_ems_restore_mapctx
		retn
prepare_samples endp


; =============== S U B	R O U T	I N E =======================================


sub_16CF6 proc near		; CODE XREF: prepare_samples+7Ap
					; prepare_samples:loc_16CEBp
		cmp	_byte_24625, 1
		jz	loc_16E24
		mov	bx, cx
		and	bx, 0Fh
		shl	bx, 1
		jmp	cs:[off_18EA0+bx]

loc_16D0B::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D16::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D21::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D2C::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D37::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D42::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D4D::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D58::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al

loc_16D5F::
		add	si, 8
		inc	di

loc_16D63::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D6E::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D79::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D84::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D8F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16D9A::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16DA5::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		xor	al, 80h
		mov	es:[di], al
		add	si, 8
		inc	di

loc_16DB0::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		shr	cx, 4
		jz	short locret_16E23
		mov	edx, 80808080h

loc_16DBB::				; CODE XREF: sub_16CF6+12Bj
		mov	al, [si+10h]
		mov	ah, [si+18h]
		shl	eax, 10h
		mov	al, [si]
		mov	ah, [si+8]
		xor	eax, edx
		mov	es:[di], eax
		mov	al, [si+30h]
		mov	ah, [si+38h]
		shl	eax, 10h
		mov	al, [si+20h]
		mov	ah, [si+28h]
		xor	eax, edx
		mov	es:[di+4], eax
		mov	al, [si+50h]
		mov	ah, [si+58h]
		shl	eax, 10h
		mov	al, [si+40h]
		mov	ah, [si+48h]
		xor	eax, edx
		mov	es:[di+8], eax
		mov	al, [si+70h]
		mov	ah, [si+78h]
		shl	eax, 10h
		mov	al, [si+60h]
		mov	ah, [si+68h]
		xor	eax, edx
		mov	es:[di+0Ch], eax
		add	si, 80h	; 'Ä'
		add	di, 10h
		dec	cx
		jnz	short loc_16DBB

locret_16E23::				; CODE XREF: sub_16CF6+BDj
		retn
; ---------------------------------------------------------------------------

loc_16E24::				; CODE XREF: sub_16CF6+5j
		and	si, 0FFFCh
		mov	edx, 7FFFh
		mov	ebp, 0FFFF8000h
		mov	bx, cx
		and	bx, 0Fh
		shl	bx, 1
		jmp	cs:[off_18EC0+bx]

loc_16E3F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16E56
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16E56::				; DATA XREF: sub_16CF6+14Fo
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16E5D::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8		; CODE XREF: sub_137D5+39j
					; _eff_139B9+86j ...
		mov	bx, offset loc_16E74
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16E74::				; DATA XREF: sub_16CF6+16Do
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16E7B::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16E92
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16E92::				; DATA XREF: sub_16CF6+18Bo
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16E99::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16EB0
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16EB0::				; DATA XREF: sub_16CF6+1A9o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16EB7::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16ECE
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16ECE::				; DATA XREF: sub_16CF6+1C7o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16ED5::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16EEC
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16EEC::				; DATA XREF: sub_16CF6+1E5o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16EF3::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16F0A
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16F0A::				; DATA XREF: sub_16CF6+203o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16F11::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16F28
		cmp	eax, ebp

loc_16F1D::				; CODE XREF: _snd_initialze+13j
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16F28::				; DATA XREF: sub_16CF6+221o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16F2F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16F46
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16F46::				; DATA XREF: sub_16CF6+23Fo
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16F4D::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16F64
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16F64::				; DATA XREF: sub_16CF6+25Do
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16F6B::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16F82
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16F82::				; DATA XREF: sub_16CF6+27Bo
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16F89::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16FA0
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16FA0::				; DATA XREF: sub_16CF6+299o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16FA7::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16FBE
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16FBE::				; DATA XREF: sub_16CF6+2B7o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16FC5::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16FDC
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16FDC::				; DATA XREF: sub_16CF6+2D5o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_16FE3::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 8
		mov	bx, offset loc_16FFA
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_16FFA::				; DATA XREF: sub_16CF6+2F3o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_17001::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		shr	cx, 4
		jz	locret_171D2

loc_17008::				; CODE XREF: sub_16CF6+4D8j
		mov	eax, [si]
		mov	bx, offset loc_1701C
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_1701C::				; DATA XREF: sub_16CF6+315o
		xor	ah, 80h
		mov	es:[di], ah
		mov	eax, [si+8]
		mov	bx, offset loc_17037
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17037::				; DATA XREF: sub_16CF6+330o
		xor	ah, 80h
		mov	es:[di+1], ah
		mov	eax, [si+10h]
		mov	bx, offset loc_17053
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17053::				; DATA XREF: sub_16CF6+34Co
		xor	ah, 80h
		mov	es:[di+2], ah
		mov	eax, [si+18h]
		mov	bx, offset loc_1706F
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_1706F::				; DATA XREF: sub_16CF6+368o
		xor	ah, 80h
		mov	es:[di+3], ah
		mov	eax, [si+20h]
		mov	bx, offset loc_1708B
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_1708B::				; DATA XREF: sub_16CF6+384o
		xor	ah, 80h
		mov	es:[di+4], ah
		mov	eax, [si+28h]
		mov	bx, offset loc_170A7
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_170A7::				; DATA XREF: sub_16CF6+3A0o
		xor	ah, 80h
		mov	es:[di+5], ah
		mov	eax, [si+30h]
		mov	bx, offset loc_170C3
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_170C3::				; DATA XREF: sub_16CF6+3BCo
		xor	ah, 80h
		mov	es:[di+6], ah
		mov	eax, [si+38h]
		mov	bx, offset loc_170DF
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_170DF::				; DATA XREF: sub_16CF6+3D8o
		xor	ah, 80h
		mov	es:[di+7], ah
		mov	eax, [si+40h]
		mov	bx, offset loc_170FB
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_170FB::				; DATA XREF: sub_16CF6+3F4o
		xor	ah, 80h
		mov	es:[di+8], ah
		mov	eax, [si+48h]
		mov	bx, offset loc_17117
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17117::				; DATA XREF: sub_16CF6+410o
		xor	ah, 80h
		mov	es:[di+9], ah
		mov	eax, [si+50h]
		mov	bx, offset loc_17133
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17133::				; DATA XREF: sub_16CF6+42Co
		xor	ah, 80h
		mov	es:[di+0Ah], ah
		mov	eax, [si+58h]
		mov	bx, offset loc_1714F
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_1714F::				; DATA XREF: sub_16CF6+448o
		xor	ah, 80h
		mov	es:[di+0Bh], ah
		mov	eax, [si+60h]
		mov	bx, offset loc_1716B
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_1716B::				; DATA XREF: sub_16CF6+464o
		xor	ah, 80h
		mov	es:[di+0Ch], ah
		mov	eax, [si+68h]
		mov	bx, offset loc_17187
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17187::				; DATA XREF: sub_16CF6+480o
		xor	ah, 80h
		mov	es:[di+0Dh], ah
		mov	eax, [si+70h]
		mov	bx, offset loc_171A3
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_171A3::				; DATA XREF: sub_16CF6+49Co
		xor	ah, 80h
		mov	es:[di+0Eh], ah
		mov	eax, [si+78h]
		mov	bx, offset loc_171BF
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_171BF::				; DATA XREF: sub_16CF6+4B8o
		xor	ah, 80h
		mov	es:[di+0Fh], ah
		add	si, 80h	; 'Ä'
		add	di, 10h
		dec	cx
		jnz	loc_17008

locret_171D2::				; CODE XREF: sub_16CF6+30Ej
		retn
sub_16CF6 endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR prepare_samples

loc_171D3::				; CODE XREF: prepare_samples+29j
		mov	cx, _mod_channels_number
		mov	si, offset _channels_25908

loc_171DA::				; CODE XREF: prepare_samples+5B7j
		push	cx
		push	si
		cmp	byte ptr [si+1Dh], 0
		jnz	short loc_1721A
		cmp	byte ptr [si+3Ah], 0
		jz	short loc_17202
		mov	di, [off_245E0]
		test	_byte_24682, 1
		jz	short loc_171F8
		call	sub_15577
		jmp	short loc_1721A
; ---------------------------------------------------------------------------

loc_171F8::				; CODE XREF: prepare_samples+588j
		or	_byte_24682, 1
		call	sub_1609F
		jmp	short loc_1721A
; ---------------------------------------------------------------------------

loc_17202::				; CODE XREF: prepare_samples+57Dj
		mov	di, [off_245E2]
		test	_byte_24682, 2
		jz	short loc_17212
		call	sub_15577
		jmp	short loc_1721A
; ---------------------------------------------------------------------------

loc_17212::				; CODE XREF: prepare_samples+5A2j
		or	_byte_24682, 2
		call	sub_1609F

loc_1721A::				; CODE XREF: prepare_samples+577j
					; prepare_samples+58Dj ...
		pop	si
		pop	cx
		add	si, 50h	; 'P'
		dec	cx
		jnz	short loc_171DA
		cmp	_bit_mode, 16
		jz	_lc_16bit
		mov	di, _samples_outoffs_24600
		mov	cx, _word_245E4
		mov	si, (offset _chrin+1)
		mov	es, word ptr [_dma_buf_pointer+2]
		assume es:nothing
		mov	ax, 1000h
		sub	ax, di
		shl	cx, 1
		cmp	ax, cx
		ja	short loc_17254
		mov	bx, cx
		sub	bx, ax
		mov	cx, ax
		push	bx
		call	sub_1725F
		pop	cx
		xor	di, di
		jcxz	short loc_17257

loc_17254::				; CODE XREF: prepare_samples+5DAj
		call	sub_1725F

loc_17257::				; CODE XREF: prepare_samples+5E9j
		mov	_samples_outoffs_24600, di
		call	_ems_restore_mapctx
		retn
; END OF FUNCTION CHUNK	FOR prepare_samples

; =============== S U B	R O U T	I N E =======================================


sub_1725F proc near		; CODE XREF: prepare_samples+5E3p
					; prepare_samples:loc_17254p
		cmp	_byte_24625, 1
		jz	loc_17441
		or	si, 1
		mov	edx, 80808080h
		shr	cx, 1
		mov	bx, cx
		and	bx, 0Fh
		shl	bx, 1
		jmp	cs:[off_18EE0+bx]

loc_1727F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_1728F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_1729F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_172AF::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_172BF::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_172CF::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_172DF::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_172EF::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_172FF::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_1730F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_1731F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_1732F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_1733F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_1734F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_1735F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	al, [si]
		mov	ah, [si+4]
		add	si, 8
		xor	ax, dx
		mov	es:[di], ax
		add	di, 2

loc_1736F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		shr	cx, 4
		jz	locret_17440

loc_17376::				; CODE XREF: sub_1725F+1DDj
		mov	al, [si+8]
		mov	bl, [si+18h]
		mov	ah, [si+0Ch]
		mov	bh, [si+1Ch]
		shl	eax, 10h
		shl	ebx, 10h
		mov	al, [si]
		mov	bl, [si+10h]
		mov	ah, [si+4]
		mov	bh, [si+14h]
		xor	eax, edx
		xor	ebx, edx
		mov	es:[di], eax
		mov	es:[di+4], ebx
		mov	al, [si+28h]
		mov	bl, [si+38h]
		mov	ah, [si+2Ch]
		mov	bh, [si+3Ch]
		shl	eax, 10h
		shl	ebx, 10h
		mov	al, [si+20h]
		mov	bl, [si+30h]
		mov	ah, [si+24h]
		mov	bh, [si+34h]
		xor	eax, edx
		xor	ebx, edx
		mov	es:[di+8], eax
		mov	es:[di+0Ch], ebx
		mov	al, [si+48h]
		mov	bl, [si+58h]
		mov	ah, [si+4Ch]
		mov	bh, [si+5Ch]
		shl	eax, 10h
		shl	ebx, 10h
		mov	al, [si+40h]
		mov	bl, [si+50h]
		mov	ah, [si+44h]
		mov	bh, [si+54h]
		xor	eax, edx
		xor	ebx, edx
		mov	es:[di+10h], eax
		mov	es:[di+14h], ebx
		mov	al, [si+68h]
		mov	bl, [si+78h]
		mov	ah, [si+6Ch]
		mov	bh, [si+7Ch]
		shl	eax, 10h
		shl	ebx, 10h
		mov	al, [si+60h]
		mov	bl, [si+70h]
		mov	ah, [si+64h]
		mov	bh, [si+74h]
		xor	eax, edx
		xor	ebx, edx
		mov	es:[di+18h], eax
		mov	es:[di+1Ch], ebx
		add	si, 80h	; 'Ä'
		add	di, 20h	; ' '
		dec	cx
		jnz	loc_17376

locret_17440::				; CODE XREF: sub_1725F+113j
		retn
; ---------------------------------------------------------------------------

loc_17441::				; CODE XREF: sub_1725F+5j
		and	si, 0FFFCh
		mov	edx, 7FFFh
		mov	ebp, 0FFFF8000h
		mov	bx, cx
		and	bx, 0Fh
		shl	bx, 1
		jmp	cs:[off_18F00+bx]
sub_1725F endp ; sp-analysis	failed

; START	OF FUNCTION CHUNK FOR sub_1609F

loc_1745C::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17473
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17473::				; DATA XREF: sub_1609F+13C3o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_1747A::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17491
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17491::				; DATA XREF: sub_1609F+13E1o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_17498::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_174AF
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_174AF::				; DATA XREF: sub_1609F+13FFo
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_174B6::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_174CD
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_174CD::				; DATA XREF: sub_1609F+141Do
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_174D4::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_174EB
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_174EB::				; DATA XREF: sub_1609F+143Bo
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_174F2::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17509
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17509::				; DATA XREF: sub_1609F+1459o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_17510::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17527
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17527::				; DATA XREF: sub_1609F+1477o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_1752E::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17545
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17545::				; DATA XREF: sub_1609F+1495o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_1754C::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]

loc_1754F::				; CODE XREF: _snd_initialze+13j
		add	si, 4
		mov	bx, offset loc_17563
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17563::				; DATA XREF: sub_1609F+14B3o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_1756A::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17581
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17581::				; DATA XREF: sub_1609F+14D1o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_17588::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_1759F
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_1759F::				; DATA XREF: sub_1609F+14EFo
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_175A6::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_175BD
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_175BD::				; DATA XREF: sub_1609F+150Do
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_175C4::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_175DB
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_175DB::				; DATA XREF: sub_1609F+152Bo
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_175E2::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_175F9
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_175F9::				; DATA XREF: sub_1609F+1549o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_17600::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17617
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17617::				; DATA XREF: sub_1609F+1567o
		xor	ah, 80h
		mov	es:[di], ah
		inc	di

loc_1761E::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		shr	cx, 4
		jz	locret_177EE

loc_17625::				; CODE XREF: sub_1609F+174Bj
		mov	eax, [si]
		mov	bx, offset loc_17639
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17639::				; DATA XREF: sub_1609F+1589o
		xor	ah, 80h
		mov	es:[di], ah
		mov	eax, [si+4]
		mov	bx, offset loc_17654
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17654::				; DATA XREF: sub_1609F+15A4o
		xor	ah, 80h
		mov	es:[di+1], ah
		mov	eax, [si+8]
		mov	bx, offset loc_17670
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17670::				; DATA XREF: sub_1609F+15C0o
		xor	ah, 80h
		mov	es:[di+2], ah
		mov	eax, [si+0Ch]
		mov	bx, offset loc_1768C
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_1768C::				; DATA XREF: sub_1609F+15DCo
		xor	ah, 80h
		mov	es:[di+3], ah
		mov	eax, [si+10h]
		mov	bx, offset loc_176A8
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_176A8::				; DATA XREF: sub_1609F+15F8o
		xor	ah, 80h
		mov	es:[di+4], ah
		mov	eax, [si+14h]
		mov	bx, offset loc_176C4
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_176C4::				; DATA XREF: sub_1609F+1614o
		xor	ah, 80h
		mov	es:[di+5], ah
		mov	eax, [si+18h]
		mov	bx, offset loc_176E0
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_176E0::				; DATA XREF: sub_1609F+1630o
		xor	ah, 80h
		mov	es:[di+6], ah
		mov	eax, [si+1Ch]
		mov	bx, offset loc_176FC
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_176FC::				; DATA XREF: sub_1609F+164Co
		xor	ah, 80h
		mov	es:[di+7], ah
		mov	eax, [si+20h]
		mov	bx, offset loc_17718
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17718::				; DATA XREF: sub_1609F+1668o
		xor	ah, 80h
		mov	es:[di+8], ah
		mov	eax, [si+24h]
		mov	bx, offset loc_17734
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17734::				; DATA XREF: sub_1609F+1684o
		xor	ah, 80h
		mov	es:[di+9], ah
		mov	eax, [si+28h]
		mov	bx, offset loc_17750
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17750::				; DATA XREF: sub_1609F+16A0o
		xor	ah, 80h
		mov	es:[di+0Ah], ah
		mov	eax, [si+2Ch]
		mov	bx, offset loc_1776C
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_1776C::				; DATA XREF: sub_1609F+16BCo
		xor	ah, 80h
		mov	es:[di+0Bh], ah
		mov	eax, [si+30h]
		mov	bx, offset loc_17788
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17788::				; DATA XREF: sub_1609F+16D8o
		xor	ah, 80h
		mov	es:[di+0Ch], ah
		mov	eax, [si+34h]
		mov	bx, offset loc_177A4
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_177A4::				; DATA XREF: sub_1609F+16F4o
		xor	ah, 80h
		mov	es:[di+0Dh], ah
		mov	eax, [si+38h]
		mov	bx, offset loc_177C0
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_177C0::				; DATA XREF: sub_1609F+1710o
		xor	ah, 80h
		mov	es:[di+0Eh], ah
		mov	eax, [si+3Ch]
		mov	bx, offset loc_177DC
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_177DC::				; DATA XREF: sub_1609F+172Co
		xor	ah, 80h
		mov	es:[di+0Fh], ah
		add	si, 40h	; '@'
		add	di, 10h
		dec	cx
		jnz	loc_17625

locret_177EE::				; CODE XREF: sub_1609F+1582j
		retn
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR prepare_samples

_lc_16bit::				; CODE XREF: prepare_samples+5BEj
		mov	di, _samples_outoffs_24600
		mov	cx, _word_245E4
		mov	si, offset _chrin
		mov	es, word ptr [_dma_buf_pointer+2]
		mov	ax, 1000h
		sub	ax, di
		shr	ax, 2
		cmp	cx, ax
		jbe	short loc_17819
		mov	bx, cx
		sub	bx, ax
		mov	cx, ax
		push	bx
		call	sub_17824
		pop	cx
		xor	di, di
		jcxz	short loc_1781C

loc_17819::				; CODE XREF: prepare_samples+B9Fj
		call	sub_17824

loc_1781C::				; CODE XREF: prepare_samples+BAEj
		mov	_samples_outoffs_24600, di
		call	_ems_restore_mapctx
		retn
; END OF FUNCTION CHUNK	FOR prepare_samples

; =============== S U B	R O U T	I N E =======================================


sub_17824 proc near		; CODE XREF: prepare_samples+BA8p
					; prepare_samples:loc_17819p
		cmp	_byte_24625, 1
		jz	loc_17A58
		mov	bx, cx
		and	bx, 0Fh
		shl	bx, 1
		jmp	cs:[off_18F20+bx]

loc_17839::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_1784C::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_1785F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_17872::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_17885::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_17898::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_178AB::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_178BE::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_178D1::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_178E4::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_178F7::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_1790A::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_1791D::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_17930::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_17943::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		add	si, 8
		mov	es:[di], eax
		add	di, 4

loc_17956::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		shr	cx, 4
		jz	locret_17A57

loc_1795D::				; CODE XREF: sub_17824+22Fj
		mov	ax, [si+4]
		shl	eax, 10h
		mov	ax, [si]
		mov	es:[di], eax
		mov	ax, [si+0Ch]
		shl	eax, 10h
		mov	ax, [si+8]
		mov	es:[di+4], eax
		mov	ax, [si+14h]
		shl	eax, 10h
		mov	ax, [si+10h]
		mov	es:[di+8], eax
		mov	ax, [si+1Ch]
		shl	eax, 10h
		mov	ax, [si+18h]
		mov	es:[di+0Ch], eax
		mov	ax, [si+24h]
		shl	eax, 10h
		mov	ax, [si+20h]
		mov	es:[di+10h], eax
		mov	ax, [si+2Ch]
		shl	eax, 10h
		mov	ax, [si+28h]
		mov	es:[di+14h], eax
		mov	ax, [si+34h]
		shl	eax, 10h
		mov	ax, [si+30h]
		mov	es:[di+18h], eax
		mov	ax, [si+3Ch]
		shl	eax, 10h
		mov	ax, [si+38h]
		mov	es:[di+1Ch], eax
		mov	ax, [si+44h]
		shl	eax, 10h
		mov	ax, [si+40h]
		mov	es:[di+20h], eax
		mov	ax, [si+4Ch]
		shl	eax, 10h
		mov	ax, [si+48h]
		mov	es:[di+24h], eax
		mov	ax, [si+54h]
		shl	eax, 10h
		mov	ax, [si+50h]
		mov	es:[di+28h], eax
		mov	ax, [si+5Ch]
		shl	eax, 10h
		mov	ax, [si+58h]
		mov	es:[di+2Ch], eax
		mov	ax, [si+64h]
		shl	eax, 10h
		mov	ax, [si+60h]
		mov	es:[di+30h], eax
		mov	ax, [si+6Ch]
		shl	eax, 10h
		mov	ax, [si+68h]
		mov	es:[di+34h], eax
		mov	ax, [si+74h]
		shl	eax, 10h
		mov	ax, [si+70h]
		mov	es:[di+38h], eax
		mov	ax, [si+7Ch]
		shl	eax, 10h
		mov	ax, [si+78h]
		mov	es:[di+3Ch], eax
		add	si, 80h	; 'Ä'
		add	di, 40h	; '@'
		dec	cx
		jnz	loc_1795D

locret_17A57::				; CODE XREF: sub_17824+135j
		retn
; ---------------------------------------------------------------------------

loc_17A58::				; CODE XREF: sub_17824+5j
		mov	edx, 7FFFh
		mov	ebp, 0FFFF8000h
		shl	cx, 1
		mov	bx, cx
		and	bx, 0Fh
		shl	bx, 1
		jmp	cs:[off_18F40+bx]
sub_17824 endp ; sp-analysis	failed

; START	OF FUNCTION CHUNK FOR sub_1609F

loc_17A72::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17A89
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17A89::				; DATA XREF: sub_1609F+19D9o
		mov	es:[di], ax
		add	di, 2

loc_17A8F::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17AA6
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17AA6::				; DATA XREF: sub_1609F+19F6o
		mov	es:[di], ax
		add	di, 2

loc_17AAC::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17AC3
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17AC3::				; DATA XREF: sub_1609F+1A13o
		mov	es:[di], ax
		add	di, 2

loc_17AC9::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17AE0
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17AE0::				; DATA XREF: sub_1609F+1A30o
		mov	es:[di], ax
		add	di, 2

loc_17AE6::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17AFD
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17AFD::				; DATA XREF: sub_1609F+1A4Do
		mov	es:[di], ax
		add	di, 2

loc_17B03::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17B1A
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17B1A::				; DATA XREF: sub_1609F+1A6Ao
		mov	es:[di], ax
		add	di, 2

loc_17B20::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17B37
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17B37::				; DATA XREF: sub_1609F+1A87o
		mov	es:[di], ax
		add	di, 2

loc_17B3D::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17B54
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17B54::				; DATA XREF: sub_1609F+1AA4o
		mov	es:[di], ax
		add	di, 2

loc_17B5A::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17B71
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17B71::				; DATA XREF: sub_1609F+1AC1o
		mov	es:[di], ax
		add	di, 2

loc_17B77::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17B8E
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17B8E::				; DATA XREF: sub_1609F+1ADEo
		mov	es:[di], ax
		add	di, 2

loc_17B94::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17BAB
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17BAB::				; DATA XREF: sub_1609F+1AFBo
		mov	es:[di], ax
		add	di, 2

loc_17BB1::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17BC8
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17BC8::				; DATA XREF: sub_1609F+1B18o
		mov	es:[di], ax
		add	di, 2

loc_17BCE::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17BE5
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17BE5::				; DATA XREF: sub_1609F+1B35o
		mov	es:[di], ax
		add	di, 2

loc_17BEB::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17C02
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17C02::				; DATA XREF: sub_1609F+1B52o
		mov	es:[di], ax
		add	di, 2

loc_17C08::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		mov	eax, [si]
		add	si, 4
		mov	bx, offset loc_17C1F
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17C1F::				; DATA XREF: sub_1609F+1B6Fo
		mov	es:[di], ax
		add	di, 2

loc_17C25::				; CODE XREF: sub_15577+28j
					; sub_15577+311j ...
		shr	cx, 4
		jz	_nullsub_3

loc_17C2C::				; CODE XREF: _text:7DC1j
		mov	eax, [si]
		mov	bx, offset loc_17C40
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17C40::				; DATA XREF: sub_1609F+1B90o
		mov	es:[di], ax
		mov	eax, [si+4]
		mov	bx, offset loc_17C58
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17C58::				; DATA XREF: sub_1609F+1BA8o
		mov	es:[di+2], ax
		mov	eax, [si+8]
		mov	bx, offset loc_17C71
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17C71::				; DATA XREF: sub_1609F+1BC1o
		mov	es:[di+4], ax
		mov	eax, [si+0Ch]
		mov	bx, offset loc_17C8A
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17C8A::				; DATA XREF: sub_1609F+1BDAo
		mov	es:[di+6], ax
		mov	eax, [si+10h]
		mov	bx, offset loc_17CA3
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17CA3::				; DATA XREF: sub_1609F+1BF3o
		mov	es:[di+8], ax
		mov	eax, [si+14h]
		mov	bx, offset loc_17CBC
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17CBC::				; DATA XREF: sub_1609F+1C0Co
		mov	es:[di+0Ah], ax
		mov	eax, [si+18h]
		mov	bx, offset loc_17CD5
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17CD5::				; DATA XREF: sub_1609F+1C25o
		mov	es:[di+0Ch], ax
		mov	eax, [si+1Ch]
		mov	bx, offset loc_17CEE
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17CEE::				; DATA XREF: sub_1609F+1C3Eo
		mov	es:[di+0Eh], ax
		mov	eax, [si+20h]
		mov	bx, offset loc_17D07
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17D07::				; DATA XREF: sub_1609F+1C57o
		mov	es:[di+10h], ax
		mov	eax, [si+24h]
		mov	bx, offset loc_17D20
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8
; END OF FUNCTION CHUNK	FOR sub_1609F

loc_17D20::				; DATA XREF: sub_1609F+1C70o
		mov	es:[di+12h], ax
		mov	eax, [si+28h]
		mov	bx, offset loc_17D39
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17D39::				; DATA XREF: _text:7D28o
		mov	es:[di+14h], ax
		mov	eax, [si+2Ch]
		mov	bx, offset loc_17D52
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17D52::				; DATA XREF: _text:7D41o
		mov	es:[di+16h], ax
		mov	eax, [si+30h]
		mov	bx, offset loc_17D6B
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17D6B::				; DATA XREF: _text:7D5Ao
		mov	es:[di+18h], ax
		mov	eax, [si+34h]
		mov	bx, offset loc_17D84
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17D84::				; DATA XREF: _text:7D73o
		mov	es:[di+1Ah], ax
		mov	eax, [si+38h]
		mov	bx, offset loc_17D9D
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17D9D::				; DATA XREF: _text:7D8Co
		mov	es:[di+1Ch], ax
		mov	eax, [si+3Ch]
		mov	bx, offset loc_17DB6
		cmp	eax, ebp
		jl	loc_18DB0
		cmp	eax, edx
		jg	loc_18DB8

loc_17DB6::				; DATA XREF: _text:7DA5o
		mov	es:[di+1Eh], ax
		add	si, 40h	; '@'
		add	di, 20h	; ' '
		dec	cx
		jnz	loc_17C2C

; =============== S U B	R O U T	I N E =======================================


_nullsub_3 proc near		; CODE XREF: sub_1609F+1B89j
		retn
_nullsub_3 endp


; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


 ; sp-analysis failed

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR _gravis_17F7D

loc_18077::				; CODE XREF: _gravis_17F7D+11j
		stc
		retn
; END OF FUNCTION CHUNK	FOR _gravis_17F7D

; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================




; ---------------------------------------------------------------------------
asc_182C3	db 0,0,1,3,0,2,0,4,0,0,0,5,6,0,0,7 ; DATA XREF:	_gravis_18216+5r
					; _gravis_18216+Fr
asc_182D3	db 0,1,0,2,0,3,4,5	; DATA XREF: _gravis_18216+19r
					; _gravis_18216+23r

; =============== S U B	R O U T	I N E =======================================


sub_182DB proc near		; CODE XREF: sub_1279A+44j
					; sub_1279A+5Ep
		mov	_dma_mode, 44h	; 'D'
		mov	_byte_24645, 2
		jmp	short loc_182F7
sub_182DB endp


; =============== S U B	R O U T	I N E =======================================


_nongravis_182E7 proc	near	; CODE XREF: _mod_readfile_11F4E+162p
					; _mod_readfile_11F4E+19Dp
		mov	_dma_mode, 48h	; 'H'
		mov	bl, _byte_24673
		and	bl, 80h
		mov	_byte_24645, bl

loc_182F7::				; CODE XREF: sub_182DB+Aj
					; _nongravis_182E7+15j
		cmp	_byte_2466E, 1
		jz	short loc_182F7
		mov	_word_24636, 0
		mov	bp, ax
		shl	ax, 2
		mov	_word_2460E, cx
		mov	bx, cx
		shr	cx, 2
		add	cx, ax
		jnb	short loc_18338
		jz	short loc_18338
		neg	ax
		shl	ax, 2
		mov	_word_2460E, ax
		mov	cx, ax
		sub	bx, ax
		mov	_word_24636, bx
		add	ax, word ptr [_dma_buf_pointer]
		mov	_word_24634, ax
		shr	cx, 4
		add	cx, bp
		mov	_word_24632, cx

loc_18338::				; CODE XREF: _gravis_int+E2p
					; _nongravis_182E7+2Dj ...
		pushf
		cli
		push	bp
		mov	cl, _dma_channel_0
		call	_dma_186E3
		pop	bp
		mov	bl, 21h	; '!'
		or	bl, _byte_24645
		cmp	_dma_channel_0, 4
		jb	short loc_18360
		or	bl, 4
		mov	ax, bp
		and	ax, 0C000h
		shr	bp, 1
		and	bp, 1FFFh
		or	bp, ax

loc_18360::				; CODE XREF: _nongravis_182E7+67j
		mov	dx, _gravis_port
		mov	al, 41h	; 'A'
		out	dx, al
		add	dl, 2
		in	al, dx
		sub	dl, 2
		mov	al, 42h	; 'B'
		out	dx, al
		inc	dx
		mov	ax, bp
		out	dx, ax
		dec	dx
		mov	_byte_2466E, 1
		mov	al, 41h	; 'A'
		out	dx, al
		add	dl, 2
		mov	al, bl
		out	dx, al
		sub	dl, 2
		popf
		retn
_nongravis_182E7 endp


; =============== S U B	R O U T	I N E =======================================


_adlib_18389 proc near	; CODE XREF: _adlib_init+Fp
					; _adlib_clean+3p
		xor	ax, ax

loc_1838B::				; CODE XREF: _adlib_18389+9j
		call	_adlib_18395
		inc	al
		cmp	al, 0E8h ; 'Ë'
		jbe	short loc_1838B
		retn
_adlib_18389 endp


; =============== S U B	R O U T	I N E =======================================


_adlib_18395 proc near	; CODE XREF: _adlib_init+15p
					; _adlib_init+1Bp ...
		push	ax
		push	dx
		mov	dx, 388h
		out	dx, al
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		inc	dx
		mov	al, ah
		out	dx, al
		dec	dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		in	al, dx
		pop	dx
		pop	ax
		retn
_adlib_18395 endp


; =============== S U B	R O U T	I N E =======================================


_sb16_detect_port proc near	; CODE XREF: _useless_12D61+22p
					; _sb16_init+Fp	...
		cmp	_snd_base_port, 0FFFFh
		jz	short loc_183DE
		mov	ax, _snd_base_port
		mov	_sb_base_port,	ax
		call	_CheckSB
		jnb	short loc_1842D

loc_183DE::				; CODE XREF: _sb16_detect_port+5j
		mov	_sb_base_port,	220h
		call	_CheckSB
		jnb	short loc_1842D
		mov	_sb_base_port,	240h
		call	_CheckSB
		jnb	short loc_1842D
		mov	_sb_base_port,	210h
		call	_CheckSB
		jnb	short loc_1842D
		mov	_sb_base_port,	230h
		call	_CheckSB
		jnb	short loc_1842D
		mov	_sb_base_port,	250h
		call	_CheckSB
		jnb	short loc_1842D
		mov	_sb_base_port,	260h
		call	_CheckSB
		jnb	short loc_1842D
		mov	_sb_base_port,	280h
		call	_CheckSB
		jnb	short loc_1842D
		stc
		retn
; ---------------------------------------------------------------------------

loc_1842D::				; CODE XREF: _sb16_detect_port+10j
					; _sb16_detect_port+1Bj	...
		mov	al, 10h
		call	_WriteSB
		mov	al, 80h	; 'Ä'
		call	_WriteSB
		mov	al, 0E1h ; '·'
		call	_WriteSB
		call	_ReadSB
		mov	ah, al
		call	_ReadSB
		mov	_word_24654, ax
		clc
		retn
_sb16_detect_port endp


; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================




; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR _sb_detect_irq

loc_184C3::				; CODE XREF: _sb_detect_irq+18j
					; _sb_detect_irq+22j ...
		cmp	_dma_channel, 0FFh
		jz	short loc_184DC
		mov	al, _dma_channel
		mov	_dma_chn_mask,	al
		cmp	_bit_mode, 16
		jz	short _sb16_sound_on
		call	_sb16_18540
		jnb	short _sb16_sound_on

loc_184DC::				; CODE XREF: _sb_detect_irq+7Fj
		cmp	_bit_mode, 8
		jz	short loc_18501
		mov	_dma_chn_mask,	5
		call	_sb16_18540
		jnb	short _sb16_sound_on
		mov	_dma_chn_mask,	6
		call	_sb16_18540
		jnb	short _sb16_sound_on
		mov	_dma_chn_mask,	7
		call	_sb16_18540
		jnb	short _sb16_sound_on

loc_18501::				; CODE XREF: _sb_detect_irq+98j
		mov	_dma_chn_mask,	1
		call	_sb16_18540
		jnb	short _sb16_sound_on
		mov	_dma_chn_mask,	3
		call	_sb16_18540
		jnb	short _sb16_sound_on
		mov	_dma_chn_mask,	0
		call	_sb16_18540
		jnb	short _sb16_sound_on
		mov	dx, offset _aErrorCouldNot_1 ; "Error: Could not	find DMA!\r\n"
		stc
		retn
; END OF FUNCTION CHUNK	FOR _sb_detect_irq

; =============== S U B	R O U T	I N E =======================================


_sb16_sound_on proc near	; CODE XREF: _sb16_init:loc_14AFDp
					; _sb_detect_irq+8Cj ...
		call	_CheckSB
		mov	al, 0D1h ; '—'
		call	_WriteSB
		mov	ax, _sb_base_port
		mov	_snd_base_port, ax
		mov	al, _sb_irq_number
		mov	_irq_number, al
		mov	al, _dma_chn_mask
		mov	_dma_channel, al
		clc
		retn
_sb16_sound_on endp


; =============== S U B	R O U T	I N E =======================================


_sb16_18540 proc near		; CODE XREF: _sb_detect_irq+8Ep
					; _sb_detect_irq+9Fp ...
		mov	_dma_mode, 48h	; 'H'
		cli
		call	_CheckSB
		mov	_word_2460E, 2
		mov	_dma_buf_pointer, 0
		mov	cl, _dma_chn_mask
		call	_dma_186E3
		mov	_sb_int_counter, 1
		mov	si, offset _sb16_handler_int ; myfunc
		mov	al, _sb_irq_number
		call	_setsnd_handler
		cmp	_dma_chn_mask,	4
		jnb	short loc_18591
		mov	al, 40h	; '@'
		call	_WriteSB
		mov	al, 0D3h ; '”'
		call	_WriteSB
		mov	al, 14h
		call	_WriteSB
		mov	ax, _word_2460E
		dec	ax
		call	_WriteSB
		mov	al, ah
		call	_WriteSB
		jmp	short loc_185B5
; ---------------------------------------------------------------------------

loc_18591::				; CODE XREF: _sb16_18540+32j
		mov	al, 42h	; 'B'
		call	_WriteSB
		mov	al, 0ACh ; '¨'
		call	_WriteSB
		mov	al, 44h	; 'D'
		call	_WriteSB
		mov	al, 0B6h ; '∂'
		call	_WriteSB
		mov	al, 30h	; '0'
		call	_WriteSB
		mov	ax, _word_2460E
		call	_WriteSB
		mov	al, ah
		call	_WriteSB

loc_185B5::				; CODE XREF: _sb16_18540+4Fj
		sti
		xor	cx, cx

loc_185B8::				; CODE XREF: _sb16_18540+7Fj
		cmp	_sb_int_counter, 1
		jnz	short loc_185CD
		loop	loc_185B8
		call	_restore_intvector
		mov	cl, _dma_chn_mask
		call	_set_dmachn_mask
		stc
		retn
; ---------------------------------------------------------------------------

loc_185CD::				; CODE XREF: _sb16_18540+7Dj
		call	_restore_intvector
		mov	cl, _dma_chn_mask
		call	_set_dmachn_mask
		clc
		retn
_sb16_18540 endp


; =============== S U B	R O U T	I N E =======================================


; int _sb16_handler_int
_sb16_handler_int proc far	; DATA XREF: _sb_test_interrupt+5o
					; _sb16_18540+24o
		push	ax
		push	dx
		push	ds
		mov	ax, seg003
		mov	ds, ax
		mov	dx, _sb_base_port
		add	dl, 0Eh
		in	al, dx		; DMA controller, 8237A-5.
					; Clear	mask registers.
					; Any OUT enables all 4	channels.
		inc	_sb_int_counter
		mov	al, 20h	; ' '
		out	20h, al		; Interrupt controller,	8259A.
		pop	ds
		pop	dx
		pop	ax
		iret
_sb16_handler_int endp

; ---------------------------------------------------------------------------
		cmp	cl, 4
		jnb	short loc_18631
		xor	al, al
		out	0Ch, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		cmp	cl, 1
		jz	short loc_18616
		cmp	cl, 2
		jz	short loc_1861F
		cmp	cl, 3
		jz	short loc_18628
		in	al, 1		; DMA controller, 8237A-5.
					; channel 0 current word count
		mov	ah, al
		in	al, 1		; DMA controller, 8237A-5.
					; channel 0 current word count
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------

loc_18616::				; CODE XREF: _text:8601j
		in	al, 3		; DMA controller, 8237A-5.
					; channel 1 current word count
		mov	ah, al
		in	al, 3		; DMA controller, 8237A-5.
					; channel 1 current word count
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------

loc_1861F::				; CODE XREF: _text:8606j
		in	al, 5		; DMA controller, 8237A-5.
					; channel 2 current word count
		mov	ah, al
		in	al, 5		; DMA controller, 8237A-5.
					; channel 2 current word count
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------

loc_18628::				; CODE XREF: _text:860Bj
		in	al, 7		; DMA controller, 8237A-5.
					; channel 3 current word count
		mov	ah, al
		in	al, 7		; DMA controller, 8237A-5.
					; channel 3 current word count
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------

loc_18631::				; CODE XREF: _text:85F8j
		xor	al, al
		out	0D8h, al
		cmp	cl, 5
		jz	short loc_1864D
		cmp	cl, 6
		jz	short loc_18656
		cmp	cl, 7
		jz	short loc_1865F
		in	al, 0C2h	; DMA controller, 8237A-5.
					; channel 1 current address
		mov	ah, al
		in	al, 0C2h	; DMA controller, 8237A-5.
					; channel 1 current address
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------

loc_1864D::				; CODE XREF: _text:8638j
		in	al, 0C6h	; DMA controller, 8237A-5.
					; channel 3 current address
		mov	ah, al
		in	al, 0C6h	; DMA controller, 8237A-5.
					; channel 3 current address
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------

loc_18656::				; CODE XREF: _text:863Dj
		in	al, 0CAh	; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		mov	ah, al
		in	al, 0CAh	; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------

loc_1865F::				; CODE XREF: _text:8642j
		in	al, 0CEh	; DMA controller, 8237A-5.
					; Clear	mask registers.
					; Any OUT enables all 4	channels.
		mov	ah, al
		in	al, 0CEh	; DMA controller, 8237A-5.
					; Clear	mask registers.
					; Any OUT enables all 4	channels.
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------
		cmp	cl, 4
		jnb	short loc_186A4
		xor	al, al
		out	0Ch, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		cmp	cl, 1
		jz	short loc_18689
		cmp	cl, 2
		jz	short loc_18692
		cmp	cl, 3
		jz	short loc_1869B
		in	al, 0
		mov	ah, al
		in	al, 0
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------

loc_18689::				; CODE XREF: _text:8674j
		in	al, 2		; DMA controller, 8237A-5.
					; channel 1 current address
		mov	ah, al
		in	al, 2		; DMA controller, 8237A-5.
					; channel 1 current address
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------

loc_18692::				; CODE XREF: _text:8679j
		in	al, 4		; DMA controller, 8237A-5.
					; channel 2 current address
		mov	ah, al
		in	al, 4		; DMA controller, 8237A-5.
					; channel 2 current address
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------

loc_1869B::				; CODE XREF: _text:867Ej
		in	al, 6		; DMA controller, 8237A-5.
					; channel 3 current address
		mov	ah, al
		in	al, 6		; DMA controller, 8237A-5.
					; channel 3 current address
		xchg	al, ah
		retn
; ---------------------------------------------------------------------------

loc_186A4::				; CODE XREF: _text:866Bj
		xor	al, al
		out	0D8h, al
		cmp	cl, 5
		jz	short loc_186C2
		cmp	cl, 6
		jz	short loc_186CD
		cmp	cl, 7
		jz	short loc_186D8
		in	al, 0C0h	; DMA controller, 8237A-5.
					; channel 0 current address
		mov	ah, al
		in	al, 0C0h	; DMA controller, 8237A-5.
					; channel 0 current address
		xchg	al, ah
		shl	ax, 1
		retn
; ---------------------------------------------------------------------------

loc_186C2::				; CODE XREF: _text:86ABj
		in	al, 0C4h	; DMA controller, 8237A-5.
					; channel 2 current address
		mov	ah, al
		in	al, 0C4h	; DMA controller, 8237A-5.
					; channel 2 current address
		xchg	al, ah
		shl	ax, 1
		retn
; ---------------------------------------------------------------------------

loc_186CD::				; CODE XREF: _text:86B0j
		in	al, 0C8h	; DMA 8237A-5. status register bits:
					; 0-3: channel 0-3 has reached terminal	count
					; 4-7: channel 0-3 has a request pending
		mov	ah, al
		in	al, 0C8h	; DMA 8237A-5. status register bits:
					; 0-3: channel 0-3 has reached terminal	count
					; 4-7: channel 0-3 has a request pending
		xchg	al, ah
		shl	ax, 1
		retn
; ---------------------------------------------------------------------------

loc_186D8::				; CODE XREF: _text:86B5j
		in	al, 0CCh	; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		mov	ah, al
		in	al, 0CCh	; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		xchg	al, ah
		shl	ax, 1
		retn

; =============== S U B	R O U T	I N E =======================================


_dma_186E3 proc near		; CODE XREF: _proaud_set+59p
					; _wss_set+28p ...
		test	byte ptr [_config_word+1], 10h
		jz	short loc_186EF
		and	_dma_mode, 0EFh

loc_186EF::				; CODE XREF: _dma_186E3+5j
		cmp	cl, 1
		jz	short loc_18761
		cmp	cl, 2
		jz	loc_187A6
		cmp	cl, 3
		jz	loc_187EB
		cmp	cl, 4
		jz	loc_18830
		cmp	cl, 5
		jz	loc_18878
		cmp	cl, 6
		jz	loc_188C2
		cmp	cl, 7
		jz	loc_1890C
		mov	al, 4
		out	0Ah, al		; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		xor	al, al
		out	0Ch, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		mov	al, _dma_mode
		out	0Bh, al		; DMA 8237A-5. mode register bits:
					; 0-1: channel (00=0; 01=1; 10=2; 11=3)
					; 2-3: transfer	type (00=verify=Nop; 01=write; 10=read)
					; 4: 1=enable auto-initialization
					; 5: 1=address increment; 0=address decrement
					; 6-7: 00=demand mode; 01=single; 10=block; 11=cascade
		mov	dx, word ptr [_dma_buf_pointer+2]
		rol	dx, 4
		mov	ax, dx
		and	al, 0F0h
		and	dx, 0Fh
		add	ax, word ptr [_dma_buf_pointer]
		adc	dx, 0
		add	ax, word ptr [_dword_24694]
		adc	dx, word ptr [_dword_24694+2]
		out	0, al
		mov	al, ah
		out	0, al
		mov	al, dl
		out	87h, al		; DMA page register 74LS612:
					; Channel 0 (address bits 16-23)
		mov	ax, _word_2460E
		dec	ax
		out	1, al		; DMA controller, 8237A-5.
					; channel 0 base address and word count
		mov	al, ah
		out	1, al		; DMA controller, 8237A-5.
					; channel 0 base address and word count
		xor	al, al
		out	0Ah, al		; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		retn
; ---------------------------------------------------------------------------

loc_18761::				; CODE XREF: _dma_186E3+Fj
		mov	al, 5
		out	0Ah, al		; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		xor	al, al
		out	0Ch, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		mov	al, _dma_mode
		or	al, 1
		out	0Bh, al		; DMA 8237A-5. mode register bits:
					; 0-1: channel (00=0; 01=1; 10=2; 11=3)
					; 2-3: transfer	type (00=verify=Nop; 01=write; 10=read)
					; 4: 1=enable auto-initialization
					; 5: 1=address increment; 0=address decrement
					; 6-7: 00=demand mode; 01=single; 10=block; 11=cascade
		mov	dx, word ptr [_dma_buf_pointer+2]
		rol	dx, 4
		mov	ax, dx
		and	al, 0F0h
		and	dx, 0Fh
		add	ax, word ptr [_dma_buf_pointer]
		adc	dx, 0
		add	ax, word ptr [_dword_24694]
		adc	dx, word ptr [_dword_24694+2]
		out	2, al		; DMA controller, 8237A-5.
					; channel 1 base address
					; (also	sets current address)
		mov	al, ah
		out	2, al		; DMA controller, 8237A-5.
					; channel 1 base address
					; (also	sets current address)
		mov	al, dl
		out	83h, al		; DMA page register 74LS612:
					; Channel 1 (address bits 16-23)
		mov	ax, _word_2460E
		dec	ax
		out	3, al		; DMA controller, 8237A-5.
					; channel 1 base address and word count
		mov	al, ah
		out	3, al		; DMA controller, 8237A-5.
					; channel 1 base address and word count
		mov	al, 1
		out	0Ah, al		; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		retn
; ---------------------------------------------------------------------------

loc_187A6::				; CODE XREF: _dma_186E3+14j
		mov	al, 6
		out	0Ah, al		; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		xor	al, al
		out	0Ch, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		mov	al, _dma_mode
		or	al, 2
		out	0Bh, al		; DMA 8237A-5. mode register bits:
					; 0-1: channel (00=0; 01=1; 10=2; 11=3)
					; 2-3: transfer	type (00=verify=Nop; 01=write; 10=read)
					; 4: 1=enable auto-initialization
					; 5: 1=address increment; 0=address decrement
					; 6-7: 00=demand mode; 01=single; 10=block; 11=cascade
		mov	dx, word ptr [_dma_buf_pointer+2]
		rol	dx, 4
		mov	ax, dx
		and	al, 0F0h
		and	dx, 0Fh
		add	ax, word ptr [_dma_buf_pointer]
		adc	dx, 0
		add	ax, word ptr [_dword_24694]
		adc	dx, word ptr [_dword_24694+2]
		out	4, al		; DMA controller, 8237A-5.
					; channel 2 base address
					; (also	sets current address)
		mov	al, ah
		out	4, al		; DMA controller, 8237A-5.
					; channel 2 base address
					; (also	sets current address)
		mov	al, dl
		out	81h, al		; DMA page register 74LS612:
					; Channel 2 (diskette DMA)  (address bits 16-23)
		mov	ax, _word_2460E
		dec	ax
		out	5, al		; DMA controller, 8237A-5.
					; channel 2 base address and word count
		mov	al, ah
		out	5, al		; DMA controller, 8237A-5.
					; channel 2 base address and word count
		mov	al, 2
		out	0Ah, al		; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		retn
; ---------------------------------------------------------------------------

loc_187EB::				; CODE XREF: _dma_186E3+1Bj
		mov	al, 7
		out	0Ah, al		; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		xor	al, al
		out	0Ch, al		; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		mov	al, _dma_mode
		or	al, 3
		out	0Bh, al		; DMA 8237A-5. mode register bits:
					; 0-1: channel (00=0; 01=1; 10=2; 11=3)
					; 2-3: transfer	type (00=verify=Nop; 01=write; 10=read)
					; 4: 1=enable auto-initialization
					; 5: 1=address increment; 0=address decrement
					; 6-7: 00=demand mode; 01=single; 10=block; 11=cascade
		mov	dx, word ptr [_dma_buf_pointer+2]
		rol	dx, 4
		mov	ax, dx
		and	al, 0F0h
		and	dx, 0Fh
		add	ax, word ptr [_dma_buf_pointer]
		adc	dx, 0
		add	ax, word ptr [_dword_24694]
		adc	dx, word ptr [_dword_24694+2]
		out	6, al		; DMA controller, 8237A-5.
					; channel 3 base address
					; (also	sets current address)
		mov	al, ah
		out	6, al		; DMA controller, 8237A-5.
					; channel 3 base address
					; (also	sets current address)
		mov	al, dl
		out	82h, al		; DMA page register 74LS612:
					; Channel 3 (hard disk DMA) (address bits 16-23)
		mov	ax, _word_2460E
		dec	ax
		out	7, al		; DMA controller, 8237A-5.
					; channel 3 base address and word count
		mov	al, ah
		out	7, al		; DMA controller, 8237A-5.
					; channel 3 base address and word count
		mov	al, 3
		out	0Ah, al		; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		retn
; ---------------------------------------------------------------------------

loc_18830::				; CODE XREF: _dma_186E3+22j
		mov	al, 4
		out	0D4h, al
		xor	al, al
		out	0D8h, al
		mov	al, _dma_mode
		out	0D6h, al
		movzx	edx, word ptr [_dma_buf_pointer+2]
		shl	edx, 4
		movzx	eax, word ptr [_dma_buf_pointer]
		add	eax, edx
		add	eax, _dword_24694
		shr	eax, 1
		out	0C0h, al	; DMA controller, 8237A-5.
					; channel 0 base address
					; (also	sets current address)
		mov	al, ah
		out	0C0h, al	; DMA controller, 8237A-5.
					; channel 0 base address
					; (also	sets current address)
		shr	eax, 0Fh
		out	8Fh, al		; DMA page register 74LS612:
					; refresh
		mov	ax, _word_2460E
		shr	ax, 1
		adc	ax, 0
		dec	ax
		out	0C2h, al	; DMA controller, 8237A-5.
					; channel 1 base address
					; (also	sets current address)
		mov	al, ah
		out	0C2h, al	; DMA controller, 8237A-5.
					; channel 1 base address
					; (also	sets current address)
		xor	al, al
		out	0D4h, al
		retn
; ---------------------------------------------------------------------------

loc_18878::				; CODE XREF: _dma_186E3+29j
		mov	al, 5
		out	0D4h, al
		xor	al, al
		out	0D8h, al
		mov	al, _dma_mode
		or	al, 1
		out	0D6h, al
		movzx	edx, word ptr [_dma_buf_pointer+2]
		shl	edx, 4
		movzx	eax, word ptr [_dma_buf_pointer]
		add	eax, edx
		add	eax, _dword_24694
		shr	eax, 1
		out	0C4h, al	; DMA controller, 8237A-5.
					; channel 2 base address
					; (also	sets current address)
		mov	al, ah
		out	0C4h, al	; DMA controller, 8237A-5.
					; channel 2 base address
					; (also	sets current address)
		shr	eax, 0Fh
		out	8Bh, al		; DMA page register 74LS612:
					; Channel 5 (address bits 17-23)
		mov	ax, _word_2460E
		shr	ax, 1
		adc	ax, 0
		dec	ax
		out	0C6h, al	; DMA controller, 8237A-5.
					; channel 3 base address
					; (also	sets current address)
		mov	al, ah
		out	0C6h, al	; DMA controller, 8237A-5.
					; channel 3 base address
					; (also	sets current address)
		mov	al, 1
		out	0D4h, al
		retn
; ---------------------------------------------------------------------------

loc_188C2::				; CODE XREF: _dma_186E3+30j
		mov	al, 6
		out	0D4h, al
		xor	al, al
		out	0D8h, al
		mov	al, _dma_mode
		or	al, 2
		out	0D6h, al
		movzx	edx, word ptr [_dma_buf_pointer+2]
		shl	edx, 4
		movzx	eax, word ptr [_dma_buf_pointer]
		add	eax, edx
		add	eax, _dword_24694
		shr	eax, 1
		out	0C8h, al	; DMA 8237A-5. cmd reg bits:
					; 0: enable mem-to-mem DMA
					; 1: enable Ch0	address	hold
					; 2: disable controller
					; 3: compressed	timing mode
					; 4: enable rotating priority
					; 5: extended write mode; 0=late write
					; 6: DRQ sensing - active high
					; 7: DACK sensing - active high
		mov	al, ah
		out	0C8h, al	; DMA 8237A-5. cmd reg bits:
					; 0: enable mem-to-mem DMA
					; 1: enable Ch0	address	hold
					; 2: disable controller
					; 3: compressed	timing mode
					; 4: enable rotating priority
					; 5: extended write mode; 0=late write
					; 6: DRQ sensing - active high
					; 7: DACK sensing - active high
		shr	eax, 0Fh
		out	89h, al		; DMA page register 74LS612:
					; Channel 6 (address bits 17-23)
		mov	ax, _word_2460E
		shr	ax, 1
		adc	ax, 0
		dec	ax
		out	0CAh, al	; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		mov	al, ah
		out	0CAh, al	; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		mov	al, 2
		out	0D4h, al
		retn
; ---------------------------------------------------------------------------

loc_1890C::				; CODE XREF: _dma_186E3+37j
		mov	al, 7
		out	0D4h, al
		mov	al, _dma_mode
		or	al, 3
		out	0D6h, al
		xor	al, al
		out	0D8h, al
		movzx	edx, word ptr [_dma_buf_pointer+2]
		shl	edx, 4
		movzx	eax, word ptr [_dma_buf_pointer]
		add	eax, edx
		add	eax, _dword_24694
		shr	eax, 1
		out	0CCh, al	; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		mov	al, ah
		out	0CCh, al	; DMA controller, 8237A-5.
					; clear	byte pointer flip-flop.
		shr	eax, 0Fh
		out	8Ah, al		; DMA page register 74LS612:
					; Channel 7 (address bits 17-23)
		mov	ax, _word_2460E
		shr	ax, 1
		adc	ax, 0
		dec	ax
		out	0CEh, al	; DMA controller, 8237A-5.
					; Clear	mask registers.
					; Any OUT enables all 4	channels.
		mov	al, ah
		out	0CEh, al	; DMA controller, 8237A-5.
					; Clear	mask registers.
					; Any OUT enables all 4	channels.
		mov	al, 3
		out	0D4h, al
		retn
_dma_186E3 endp


; =============== S U B	R O U T	I N E =======================================


_set_dmachn_mask proc	near	; CODE XREF: _proaud_sndoff+1Dp
					; _wss_sndoff+32p ...
		mov	al, cl
		cmp	al, 4
		jnb	short loc_18961
		or	al, 4
		out	0Ah, al		; DMA controller, 8237A-5.
					; single mask bit register
					; 0-1: select channel (00=0; 01=1; 10=2; 11=3)
					; 2: 1=set mask	for channel; 0=clear mask (enable)
		retn
; ---------------------------------------------------------------------------

loc_18961::				; CODE XREF: _set_dmachn_mask+4j
		and	al, 3
		or	al, 4
		out	0D4h, al
		retn
_set_dmachn_mask endp


; =============== S U B	R O U T	I N E =======================================


_alloc_dma_buf proc near	; CODE XREF: _mod_readfile_11F4E+92p
					; _volume_prep+47p ...
		mov	_dword_24684, eax
		mov	_byte_2469C, cl
		mov	_memflg_2469A,	0
		mov	_byte_2469B, 0
		mov	_dword_24694, 0
		call	_getmemallocstrat
		push	ax
		call	_setmemalloc2
		mov	ebx, _dword_24684
		shl	ebx, 1		; bytes
		call	_memalloc
		jb	loc_18A22
		mov	_myseg_24698, ax
		mov	dx, ax
		mov	ebx, _dword_24684
		cmp	_byte_2469C, 4
		jb	short loc_189DB
		movzx	eax, ax
		shl	eax, 4
		and	eax, 1FFFFh
		add	eax, ebx
		cmp	eax, 20000h
		jbe	short loc_18A0A
		sub	eax, ebx
		neg	eax
		add	eax, 20000h
		add	ebx, eax
		and	dx, 0E000h
		add	dh, 20h	; ' '
		jmp	short loc_18A0A
; ---------------------------------------------------------------------------

loc_189DB::				; CODE XREF: _alloc_dma_buf+40j
		movzx	eax, ax
		shl	eax, 4
		and	eax, 0FFFFh
		add	eax, ebx
		cmp	eax, 10000h
		jbe	short loc_18A0A
		sub	eax, ebx
		neg	eax
		add	eax, 10000h
		add	ebx, eax
		and	dx, 0F000h
		add	dh, 10h

loc_18A0A::				; CODE XREF: _alloc_dma_buf+59j
					; _alloc_dma_buf+71j ...
		mov	_word_2468C, dx
		mov	ax, _myseg_24698
		call	_memrealloc
		pop	ax
		call	_setmemallocstrat
		mov	_memflg_2469A,	1
		mov	ax, _word_2468C
		clc
		retn
; ---------------------------------------------------------------------------

loc_18A22::				; CODE XREF: _alloc_dma_buf+2Dj
		pop	ax
		call	_setmemallocstrat
		stc
		retn
_alloc_dma_buf endp


; =============== S U B	R O U T	I N E =======================================


_memfree_18A28 proc near	; CODE XREF: _mod_readfile_11F4E:loc_12117p
					; _mod_readfile_11F4E+1E4p ...
		cmp	_memflg_2469A,	1
		jnz	short loc_18A3B
		mov	_memflg_2469A,	0
		mov	ax, _myseg_24698
		call	_memfree
		retn
; ---------------------------------------------------------------------------

loc_18A3B::				; CODE XREF: _memfree_18A28+5j
		clc
		retn
_memfree_18A28 endp


; =============== S U B	R O U T	I N E =======================================


; int __usercall _setsnd_handler<eax>(void (__cdecl *myfunc)()<esi>)
_setsnd_handler proc near	; CODE XREF: _gravis_init+C1p
					; _proaud_set+14p ...
		pushf
		push	es
		cli
		movzx	cx, al
		in	al, 0A1h	; Interrupt Controller #2, 8259A
		mov	ah, al
		in	al, 21h		; Interrupt controller,	8259A.
		mov	_interrupt_mask, ax
		mov	bx, 0FFFEh
		rol	bx, cl
		cmp	cl, 8
		jb	short loc_18A5C
		and	bl, 0FBh
		add	cl, 60h	; '`'

loc_18A5C::				; CODE XREF: _setsnd_handler+17j
		add	cl, 8
		shl	cx, 2
		and	ax, bx
		out	21h, al		; Interrupt controller,	8259A.
		mov	al, ah
		out	0A1h, al	; Interrupt Controller #2, 8259A
		mov	_intvectoffset, cx
		mov	bx, cx
		xor	ax, ax
		mov	es, ax
		assume es:nothing
		les	cx, es:[bx]
		assume es:nothing
		mov	_old_intprocoffset, cx
		mov	_old_intprocseg, es
		mov	es, ax
		assume es:nothing
		mov	es:[bx], si
		mov	word ptr es:[bx+2], cs
		pop	es
		assume es:nothing
		popf
		retn
_setsnd_handler endp


; =============== S U B	R O U T	I N E =======================================


_restore_intvector proc near	; CODE XREF: _gravis_clean+16p
					; _useless_1474C+Fp ...
		pushf
		push	es
		cli
		mov	ax, _interrupt_mask
		out	21h, al		; Interrupt controller,	8259A.
		mov	al, ah
		out	0A1h, al	; Interrupt Controller #2, 8259A
		mov	bx, _intvectoffset
		xor	ax, ax
		mov	es, ax
		assume es:nothing
		mov	ax, _old_intprocoffset
		mov	es:[bx], ax
		mov	ax, _old_intprocseg
		mov	es:[bx+2], ax
		pop	es
		assume es:seg003
		popf
		retn
_restore_intvector endp


; =============== S U B	R O U T	I N E =======================================


_getint_vect proc near	; CODE XREF: sub_12DA8+67p
		push	es
		mov	ah, 35h
		int	21h		; DOS -	2+ - GET INTERRUPT VECTOR
					; AL = interrupt number
					; Return: ES:BX	= value	of interrupt vector
		mov	dx, es
		pop	es
		assume es:nothing
		retn
_getint_vect endp


; =============== S U B	R O U T	I N E =======================================


_setint_vect proc near	; CODE XREF: _set_timer_int+20p
					; _clean_int8_mem_timr+Cp ...
		push	ds
		mov	ds, dx
		mov	dx, bx
		mov	ah, 25h
		int	21h		; DOS -	SET INTERRUPT VECTOR
					; AL = interrupt number
					; DS:DX	= new vector to	be used	for specified interrupt
		pop	ds
		retn
_setint_vect endp


; =============== S U B	R O U T	I N E =======================================


; int __usercall _memalloc<eax>(int bytes<ebx>)
_memalloc proc near		; CODE XREF: _inr_module:loc_11AC0p
					; _memalloc12k+6p ...
		add	ebx, 0Fh
		shr	ebx, 4
		cmp	ebx, 10000h
		jnb	short loc_18AD9
		mov	ah, 48h
		int	21h		; DOS -	2+ - ALLOCATE MEMORY
					; BX = number of 16-byte paragraphs desired
		retn
; ---------------------------------------------------------------------------

loc_18AD9::				; CODE XREF: _memalloc+Fj
		mov	ax, 8
		stc
		retn
_memalloc endp


; =============== S U B	R O U T	I N E =======================================


_memfree proc	near		; CODE XREF: _memfree_125DA+4Fp
					; _memfree_125DA+72p ...
		push	es
		mov	es, ax
		assume es:nothing
		mov	ah, 49h
		int	21h		; DOS -	2+ - FREE MEMORY
					; ES = segment address of area to be freed
		pop	es
		retn
_memfree endp


; =============== S U B	R O U T	I N E =======================================


_memrealloc proc near		; CODE XREF: _mem_reallocx+14p
					; _alloc_dma_buf+A9p
		add	ebx, 0Fh
		shr	ebx, 4
		cmp	ebx, 10000h
		jnb	short loc_18AFF
		mov	es, ax
		mov	ah, 4Ah
		int	21h		; DOS -	2+ - ADJUST MEMORY BLOCK SIZE (SETBLOCK)
					; ES = segment address of block	to change
					; BX = new size	in paragraphs
		retn
; ---------------------------------------------------------------------------

loc_18AFF::				; CODE XREF: _memrealloc+Fj
		mov	ax, 8
		stc
		retn
_memrealloc endp


; =============== S U B	R O U T	I N E =======================================


_setmemallocstrat proc near	; CODE XREF: _deinit_125B9+15p
					; sub_12D35+22p ...
		push	ax
		movzx	bx, al
		mov	ax, 5801h
		int	21h		; DOS -	3+ - GET/SET MEMORY ALLOCATION STRATEGY
					; AL = function	code: set allocation strategy
		pop	bx
		shr	bx, 8
		mov	ax, 5803h
		int	21h		; DOS -	3+ - GET/SET MEMORY ALLOCATION STRATEGY
					; AL = function	code: (DOS 5beta) set UMB link state
		retn
_setmemallocstrat endp


; =============== S U B	R O U T	I N E =======================================


_getmemallocstrat proc near	; CODE XREF: sub_12DA8+7Fp
					; _alloc_dma_buf+1Bp
		mov	ax, 5800h
		int	21h		; DOS -	3+ - GET/SET MEMORY ALLOCATION STRATEGY
					; AL = function	code: get allocation strategy
		push	ax
		mov	ax, 5802h
		int	21h		; DOS -	3+ - GET/SET MEMORY ALLOCATION STRATEGY
					; AL = function	code: (DOS 5beta) get UMB link state
		pop	bx
		mov	ah, al
		mov	al, bl
		retn
_getmemallocstrat endp


; =============== S U B	R O U T	I N E =======================================


_setmemalloc1 proc near	; CODE XREF: sub_12D35+12p
					; sub_12DA8+85p
		test	byte ptr [_config_word],	1
		jz	short _setmemalloc2
		mov	ax, 181h
		jmp	short _setmemallocstrat
_setmemalloc1 endp


; =============== S U B	R O U T	I N E =======================================


_setmemalloc2 proc near	; CODE XREF: _alloc_dma_buf+1Fp
					; _setmemalloc1+5j
		mov	ax, 1
		jmp	short _setmemallocstrat
_setmemalloc2 endp


; =============== S U B	R O U T	I N E =======================================


_WriteMixerSB proc near	; CODE XREF: _sb_set-F3p
					; _sb_set:loc_14C89p ...
		push	ax
		push	dx
		mov	dx, _sb_base_port
		add	dl, 4
		xchg	al, ah
		out	dx, al
		jmp	short $+2
		jmp	short $+2
		inc	dx
		mov	al, ah
		out	dx, al
		pop	dx
		pop	ax
		retn
_WriteMixerSB endp


; =============== S U B	R O U T	I N E =======================================


_ReadMixerSB proc near	; CODE XREF: _sb16_init+25p
					; _sb16_init+54p ...
		push	dx
		mov	dx, _sb_base_port
		add	dl, 4
		mov	al, ah
		out	dx, al
		jmp	short $+2
		jmp	short $+2
		inc	dx
		in	al, dx
		pop	dx
		retn
_ReadMixerSB endp


; =============== S U B	R O U T	I N E =======================================


_WriteSB proc	near		; CODE XREF: _sb_set+32p _sb_set+38p ...
		push	dx
		push	cx
		push	ax
		mov	dx, _sb_base_port
		add	dl, 0Ch
		mov	cx, 1000h

loc_18B70::				; CODE XREF: _WriteSB+13j
		in	al, dx
		or	al, al
		jns	short loc_18B7C
		dec	cx
		jnz	short loc_18B70
		pop	ax
		pop	cx
		pop	dx
		retn
; ---------------------------------------------------------------------------

loc_18B7C::				; CODE XREF: _WriteSB+10j
		pop	ax
		out	dx, al
		pop	cx
		pop	dx
		retn
_WriteSB endp


; =============== S U B	R O U T	I N E =======================================


_ReadSB proc near		; CODE XREF: _sb16_detect_port+70p
					; _sb16_detect_port+75p	...
		push	dx
		push	cx
		push	ax
		mov	dx, _sb_base_port
		add	dl, 0Eh
		mov	cx, 1000h

loc_18B8E::				; CODE XREF: _ReadSB+13j
		in	al, dx
		or	al, al
		js	short loc_18B9C
		dec	cx
		jnz	short loc_18B8E
		pop	ax
		pop	cx
		pop	dx
		xor	al, al
		retn
; ---------------------------------------------------------------------------

loc_18B9C::				; CODE XREF: _ReadSB+10j
		pop	ax
		sub	dl, 4
		in	al, dx
		pop	cx
		pop	dx
		retn
_ReadSB endp


; =============== S U B	R O U T	I N E =======================================


_CheckSB proc	near		; CODE XREF: _sbpro_sndoff+9p
					; _sb16_detect_port+Dp ...
		mov	dx, _sb_base_port
		add	dl, 6
		mov	al, 1
		out	dx, al
		mov	ax, 400h

loc_18BB1::				; CODE XREF: _CheckSB+Ej
		dec	ax
		jnz	short loc_18BB1
		out	dx, al
		call	_ReadSB
		cmp	al, 0AAh ; '™'
		jnz	short loc_18BBE
		clc
		retn
; ---------------------------------------------------------------------------

loc_18BBE::				; CODE XREF: _CheckSB+16j
		stc
		retn
_CheckSB endp


; =============== S U B	R O U T	I N E =======================================


_sb16_sound_off proc near	; CODE XREF: _sb16_deinit+3p
					; _sbpro_clean+3p ...
		call	_CheckSB
		mov	al, 0D3h ; '”'
		call	_WriteSB
		retn
_sb16_sound_off endp


; =============== S U B	R O U T	I N E =======================================


_initclockfromrtc proc near	; CODE XREF: _deinit_125B9+1Bp
					; sub_12D35+25p
		mov	ah, 2
		int	1Ah		; CLOCK	- READ REAL TIME CLOCK (AT,XT286,CONV,PS)
					; Return: CH = hours in	BCD
					; CL = minutes in BCD
					; DH = seconds in BCD
		xor	eax, eax
		mov	al, dh
		mov	ah, al
		shr	ah, 4
		and	al, 0Fh
		aad
		mov	ebx, eax
		mov	al, ch
		mov	ah, al
		shr	ah, 4
		and	al, 0Fh
		aad
		imul	edx, eax, 60
		mov	al, cl
		mov	ah, al
		shr	ah, 4
		and	al, 0Fh
		aad
		add	dx, ax
		imul	eax, edx, 60
		add	eax, ebx
		mov	edx, 1193180
		mul	edx
		shrd	eax, edx, 10h
		xor	dx, dx
		mov	es, dx
		assume es:nothing
		mov	es:[46Ch], eax
		retn
_initclockfromrtc endp


; =============== S U B	R O U T	I N E =======================================


_u32tox proc near		; CODE XREF: _myasmsprintf+EBp
		ror	eax, 10h
		call	_u16tox
		ror	eax, 10h
_u32tox endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_u16tox proc near		; CODE XREF: _u32tox+4p
					; _myasmsprintf+D7p
		xchg	al, ah
		call	_u8tox
		mov	al, ah
_u16tox endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_u8tox proc near		; CODE XREF: _u16tox+2p
					; _myasmsprintf+C4p
		push	ax
		shr	al, 4
		call	_u4tox
		pop	ax
_u8tox endp ;	sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_u4tox proc near		; CODE XREF: _u8tox+4p
		and	al, 0Fh
		or	al, '0'
		cmp	al, '9'
		jbe	short loc_18C3D
		add	al, 7

loc_18C3D::				; CODE XREF: _u4tox+6j
		mov	[si], al
		inc	si
		retn
_u4tox endp


; =============== S U B	R O U T	I N E =======================================


_my_i8toa10_0 proc near	; CODE XREF: _myasmsprintf+8Ap
		cbw
_my_i8toa10_0 endp


; =============== S U B	R O U T	I N E =======================================


_my_i16toa10_0 proc near	; CODE XREF: _myasmsprintf+9Dp
		cwde
_my_i16toa10_0 endp ;	sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_my_i32toa10_0 proc near	; CODE XREF: _myasmsprintf+B1p
		xor	cx, cx
		or	eax, eax
		jns	short _my_i32toa10_1
		mov	dl, '-'
		call	_my_putdigit
		neg	eax
		jmp	short _my_i32toa10_1
_my_i32toa10_0 endp


; =============== S U B	R O U T	I N E =======================================


_my_u8toa_10 proc near	; CODE XREF: _myasmsprintf+53p
		xor	ah, ah
_my_u8toa_10 endp


; =============== S U B	R O U T	I N E =======================================


_my_u16toa_10 proc near	; CODE XREF: _myasmsprintf+65p
		movzx	eax, ax
_my_u16toa_10 endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_my_u32toa10_0 proc near	; CODE XREF: _myasmsprintf+78p
		xor	cx, cx

_my_i32toa10_1::				; CODE XREF: _my_i32toa10_0+5j
					; _my_i32toa10_0+Fj
		mov	ebx, 10
_my_u32toa10_0 endp ;	sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_my_u32toa_0 proc near	; CODE XREF: _my_u32toa_0+Dp
		xor	edx, edx
		div	ebx
		or	eax, eax
		jz	short loc_18C75
		push	edx
		call	_my_u32toa_0
		pop	edx

loc_18C75::				; CODE XREF: _my_u32toa_0+9j
		or	dl, '0'
_my_u32toa_0 endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_my_putdigit proc near	; CODE XREF: _my_i32toa10_0+9p
		mov	[si], dl
		inc	si
		inc	cx
		retn
_my_putdigit endp

; ---------------------------------------------------------------------------
_asmprintf_tbl	dw offset _mysprintf_0_nop ; DATA XREF: _myasmsprintf+1Cr
		dw offset _mysprintf_1_offstr
		dw offset _mysprintf_2_off8str
		dw offset _mysprintf_3_off16str
		dw offset _mysprintf_4_u8toa
		dw offset _mysprintf_5_u16toa
		dw offset _mysprintf_6_u32toa
		dw offset _mysprintf_7_i8toa
		dw offset _mysprintf_8_i16toa
		dw offset _mysprintf_9_i32toa
		dw offset _mysprintf_10_u8tox
		dw offset _mysprintf_11_u16tox
		dw offset _mysprintf_12_u32tox

; =============== S U B	R O U T	I N E =======================================


_myasmsprintf proc near	; CODE XREF: sub_12D05+20p
		push	es
		mov	ax, ds
		mov	es, ax
		assume es:seg003
		jmp	short loc_18CA2
; ---------------------------------------------------------------------------

loc_18C9F::				; CODE XREF: _myasmsprintf+Fj
		mov	[di], al
		inc	di

loc_18CA2::				; CODE XREF: _myasmsprintf+5j
					; _myasmsprintf+2Dj ...
		mov	al, [si]
		inc	si
		cmp	al, 20h	; ' '
		jnb	short loc_18C9F
		cmp	al, 0Ch
		ja	short _mysprintf_0_nop
		inc	si
		mov	bl, al
		xor	bh, bh
		shl	bx, 1
		jmp	cs:[_asmprintf_tbl+bx]

_mysprintf_0_nop::			; CODE XREF: _myasmsprintf+13j
					; DATA XREF: _text:_asmprintf_tblo
		pop	es
		assume es:dseg
		retn
; ---------------------------------------------------------------------------

_mysprintf_1_offstr::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C80o
		push	si
		mov	si, [si]
		call	_strcpy_count_0
		pop	si
		add	si, 2
		jmp	short loc_18CA2
; ---------------------------------------------------------------------------

_mysprintf_2_off8str::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C82o
		mov	bx, [si]
		mov	bl, [bx]
		xor	bh, bh
		jmp	short loc_18CD3
; ---------------------------------------------------------------------------

_mysprintf_3_off16str::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C84o
		mov	bx, [si]
		mov	bx, [bx]

loc_18CD3::				; CODE XREF: _myasmsprintf+35j
		push	si
		shl	bx, 1
		mov	si, [si+2]
		mov	si, [bx+si]
		call	_strcpy_count_0
		pop	si
		add	si, 4
		jmp	short loc_18CA2
; ---------------------------------------------------------------------------

_mysprintf_4_u8toa::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C86o
		push	si
		mov	si, [si]
		mov	al, [si]
		mov	si, di
		call	_my_u8toa_10
		mov	di, si
		pop	si
		add	si, 2
		jmp	short loc_18CA2
; ---------------------------------------------------------------------------

_mysprintf_5_u16toa::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C88o
		push	si
		mov	si, [si]
		mov	ax, [si]
		mov	si, di
		call	_my_u16toa_10
		mov	di, si
		pop	si
		add	si, 2
		jmp	short loc_18CA2
; ---------------------------------------------------------------------------

_mysprintf_6_u32toa::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C8Ao
		push	si
		mov	si, [si]
		mov	eax, [si]
		mov	si, di
		call	_my_u32toa10_0
		mov	di, si
		pop	si
		add	si, 2
		jmp	short loc_18CA2
; ---------------------------------------------------------------------------

_mysprintf_7_i8toa::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C8Co
		push	si
		mov	si, [si]
		mov	al, [si]
		mov	si, di
		call	_my_i8toa10_0
		mov	di, si
		pop	si
		add	si, 2
		jmp	loc_18CA2
; ---------------------------------------------------------------------------

_mysprintf_8_i16toa::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C8Eo
		push	si
		mov	si, [si]
		mov	ax, [si]
		mov	si, di
		call	_my_i16toa10_0
		mov	di, si
		pop	si
		add	si, 2
		jmp	loc_18CA2
; ---------------------------------------------------------------------------

_mysprintf_9_i32toa::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C90o
		push	si
		mov	si, [si]
		mov	eax, [si]
		mov	si, di
		call	_my_i32toa10_0
		mov	di, si
		pop	si
		add	si, 2
		jmp	loc_18CA2
; ---------------------------------------------------------------------------

_mysprintf_10_u8tox::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C92o
		push	si
		mov	si, [si]
		mov	al, [si]
		mov	si, di
		call	_u8tox
		mov	di, si
		pop	si
		add	si, 2
		jmp	loc_18CA2
; ---------------------------------------------------------------------------

_mysprintf_11_u16tox::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C94o
		push	si
		mov	si, [si]
		mov	ax, [si]
		mov	si, di
		call	_u16tox
		mov	di, si
		pop	si
		add	si, 2
		jmp	loc_18CA2
; ---------------------------------------------------------------------------

_mysprintf_12_u32tox::			; CODE XREF: _myasmsprintf+1Cj
					; DATA XREF: _text:8C96o
		push	si
		mov	si, [si]
		mov	eax, [si]
		mov	si, di
		call	_u32tox
		mov	di, si
		pop	si
		add	si, 2
		jmp	loc_18CA2
_myasmsprintf endp


; =============== S U B	R O U T	I N E =======================================


_mystrlen_0 proc near
		mov	ax, -1
		dec	si

loc_18D93::				; CODE XREF: _mystrlen_0+9j
		inc	ax
		inc	si
		cmp	byte ptr [si], 0
		jnz	short loc_18D93
		sub	si, ax
		retn
_mystrlen_0 endp


; =============== S U B	R O U T	I N E =======================================


_strcpy_count_0 proc near	; CODE XREF: sub_12D05:loc_12D30p
					; _myasmsprintf+26p ...
		xor	cx, cx
		jmp	short loc_18DA6
; ---------------------------------------------------------------------------

loc_18DA1::				; CODE XREF: _strcpy_count_0+Ej
		mov	es:[di], al
		inc	si
		inc	di

loc_18DA6::				; CODE XREF: _strcpy_count_0+2j
		mov	al, [si]
		inc	cx
		or	al, al
		jnz	short loc_18DA1
		retn
_strcpy_count_0 endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_18DB0::				; CODE XREF: sub_16CF6+155j
					; sub_16CF6+173j ...
		mov	eax, ebp
		jmp	bx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR sub_1609F

loc_18DB8::				; CODE XREF: sub_16CF6+15Cj
					; sub_16CF6+17Aj ...
		mov	eax, edx
		jmp	bx
; END OF FUNCTION CHUNK	FOR sub_1609F
; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
_offs_noninterp2	dw offset loc_161C0	; DATA XREF: sub_1609F+28r
		dw offset loc_161B0
		dw offset loc_161A0
		dw offset loc_16190
		dw offset loc_16180
		dw offset loc_16170
		dw offset loc_16160
		dw offset loc_16150
		dw offset loc_16140
		dw offset loc_16130
		dw offset loc_16120
		dw offset loc_16110
		dw offset loc_16100
		dw offset loc_160F0
		dw offset loc_160E0
		dw offset loc_160D0
_offs_interpol2	dw offset loc_16617	; DATA XREF: sub_1609F+2AEr
		dw offset loc_165E8
		dw offset loc_165B9
		dw offset loc_1658A
		dw offset loc_1655B
		dw offset loc_1652C
		dw offset loc_164FD
		dw offset loc_164CE
		dw offset loc_1649F
		dw offset loc_16470
		dw offset loc_16441
		dw offset loc_16412
		dw offset loc_163E3
		dw offset loc_163B4
		dw offset loc_16385
		dw offset loc_16356
off_18E00	dw offset loc_16A89	; DATA XREF: sub_1609F:loc_16963r
		dw offset loc_16A76
		dw offset loc_16A63
		dw offset loc_16A50
		dw offset loc_16A3D
		dw offset loc_16A2A
		dw offset loc_16A17
		dw offset loc_16A04
		dw offset loc_169F1
		dw offset loc_169DE
		dw offset loc_169CB
		dw offset loc_169B8
		dw offset loc_169A5
		dw offset loc_16992
		dw offset loc_1697F
		dw offset loc_1696C
_offs_noninterp	dw offset loc_15698	; DATA XREF: sub_15577+28r
		dw offset loc_15688
		dw offset loc_15678
		dw offset loc_15668
		dw offset loc_15658
		dw offset loc_15648
		dw offset loc_15638
		dw offset loc_15628
		dw offset loc_15618
		dw offset loc_15608
		dw offset loc_155F8
		dw offset loc_155E8
		dw offset loc_155D8
		dw offset loc_155C8
		dw offset loc_155B8
		dw offset loc_155A8
_offs_interpol	dw offset loc_15B52	; DATA XREF: sub_15577+311r
		dw offset loc_15B23
		dw offset loc_15AF4
		dw offset loc_15AC5
		dw offset loc_15A96
		dw offset loc_15A67
		dw offset loc_15A38
		dw offset loc_15A09
		dw offset loc_159DA
		dw offset loc_159AB
		dw offset loc_1597C
		dw offset loc_1594D
		dw offset loc_1591E
		dw offset loc_158EF
		dw offset loc_158C0
		dw offset loc_15891
off_18E60	dw offset loc_15F78	; DATA XREF: sub_15577+8DBr
		dw offset loc_15F65
		dw offset loc_15F52
		dw offset loc_15F3F
		dw offset loc_15F2C
		dw offset loc_15F19
		dw offset loc_15F06
		dw offset loc_15EF3
		dw offset loc_15EE0
		dw offset loc_15ECD
		dw offset loc_15EBA
		dw offset loc_15EA7
		dw offset loc_15E94
		dw offset loc_15E81
		dw offset loc_15E6E
		dw offset loc_15E5B
off_18E80	dw offset loc_16C20	; DATA XREF: sub_1609F+B22r
		dw offset loc_16C1A
		dw offset loc_16C14
		dw offset loc_16C0E
		dw offset loc_16C08
		dw offset loc_16C02
		dw offset loc_16BFC
		dw offset loc_16BF6
		dw offset loc_16BF0
		dw offset loc_16BEA
		dw offset loc_16BE4
		dw offset loc_16BDE
		dw offset loc_16BD8
		dw offset loc_16BD2
		dw offset loc_16BCC
		dw offset loc_16BC6
off_18EA0	dw offset loc_16DB0	; DATA XREF: sub_16CF6+10r
		dw offset loc_16DA5
		dw offset loc_16D9A
		dw offset loc_16D8F
		dw offset loc_16D84
		dw offset loc_16D79
		dw offset loc_16D6E
		dw offset loc_16D63
		dw offset loc_16D58
		dw offset loc_16D4D
		dw offset loc_16D42
		dw offset loc_16D37
		dw offset loc_16D2C
		dw offset loc_16D21
		dw offset loc_16D16
		dw offset loc_16D0B
off_18EC0	dw offset loc_17001	; DATA XREF: sub_16CF6+144r
		dw offset loc_16FE3
		dw offset loc_16FC5
		dw offset loc_16FA7
		dw offset loc_16F89
		dw offset loc_16F6B
		dw offset loc_16F4D
		dw offset loc_16F2F
		dw offset loc_16F11
		dw offset loc_16EF3
		dw offset loc_16ED5
		dw offset loc_16EB7
		dw offset loc_16E99
		dw offset loc_16E7B
		dw offset loc_16E5D
		dw offset loc_16E3F
off_18EE0	dw offset loc_1736F	; DATA XREF: sub_1725F+1Br
		dw offset loc_1735F
		dw offset loc_1734F
		dw offset loc_1733F
		dw offset loc_1732F
		dw offset loc_1731F
		dw offset loc_1730F
		dw offset loc_172FF
		dw offset loc_172EF
		dw offset loc_172DF
		dw offset loc_172CF
		dw offset loc_172BF
		dw offset loc_172AF
		dw offset loc_1729F
		dw offset loc_1728F
		dw offset loc_1727F
off_18F00	dw offset loc_1761E	; DATA XREF: sub_1725F+1F8r
		dw offset loc_17600
		dw offset loc_175E2
		dw offset loc_175C4
		dw offset loc_175A6
		dw offset loc_17588
		dw offset loc_1756A
		dw offset loc_1754C
		dw offset loc_1752E
		dw offset loc_17510
		dw offset loc_174F2
		dw offset loc_174D4
		dw offset loc_174B6
		dw offset loc_17498
		dw offset loc_1747A
		dw offset loc_1745C
off_18F20	dw offset loc_17956	; DATA XREF: sub_17824+10r
		dw offset loc_17943
		dw offset loc_17930
		dw offset loc_1791D
		dw offset loc_1790A
		dw offset loc_178F7
		dw offset loc_178E4
		dw offset loc_178D1
		dw offset loc_178BE
		dw offset loc_178AB
		dw offset loc_17898
		dw offset loc_17885
		dw offset loc_17872
		dw offset loc_1785F
		dw offset loc_1784C
		dw offset loc_17839
off_18F40	dw offset loc_17C25	; DATA XREF: sub_17824+249r
		dw offset loc_17C08
		dw offset loc_17BEB
		dw offset loc_17BCE
		dw offset loc_17BB1
		dw offset loc_17B94
		dw offset loc_17B77
		dw offset loc_17B5A
		dw offset loc_17B3D
		dw offset loc_17B20
		dw offset loc_17B03
		dw offset loc_17AE6
		dw offset loc_17AC9
		dw offset loc_17AAC
		dw offset loc_17A8F
		dw offset loc_17A72
_effoff_18F60	dw offset _eff_nullsub	; DATA XREF: sub_137D5+16r
					; sub_137D5+2Br
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_13A43
		dw offset _eff_13A94
		dw offset _eff_nullsub
		dw offset _eff_13B06
		dw offset _eff_13B78
		dw offset _eff_13B88
		dw offset _eff_13BA3
		dw offset _eff_13CDD
		dw offset _eff_13CE8
		dw offset _eff_13DE5
		dw offset _eff_13DEF
		dw offset _eff_nullsub
		dw offset _eff_13E2D
		dw offset _eff_13E32
		dw offset _eff_13E7F
		dw offset _eff_13E84
		dw offset _eff_13E8C
		dw offset _eff_nullsub
		dw offset _nullsub_2
		dw offset _eff_13F05
		dw offset _eff_13F3B
		dw offset _eff_nullsub
		dw offset _eff_14020
		dw offset _eff_14030
		dw offset _eff_14067
_effoff_18FA2	dw offset _eff_nullsub	; DATA XREF: sub_13623+196r
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_13A43
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_13B06
		dw offset _eff_nullsub
		dw offset _eff_13B88
		dw offset _eff_13BA3
		dw offset _eff_13CDD
		dw offset _eff_13CE8
		dw offset _eff_13DE5
		dw offset _eff_13DEF
		dw offset _eff_nullsub
		dw offset _eff_13E2D
		dw offset _eff_13E32
		dw offset _eff_13E7F
		dw offset _eff_13E84
		dw offset _eff_13E8C
		dw offset _eff_nullsub
		dw offset _nullsub_2
		dw offset _eff_13F05
		dw offset _eff_13F3B
		dw offset _eff_nullsub
		dw offset _eff_14020
		dw offset _eff_14030
		dw offset _eff_14067
_effoff_18FE4	dw offset _eff_nullsub	; DATA XREF: chanl_2_eff_13813+Er
		dw offset _eff_13886
		dw offset _eff_138A4
		dw offset _eff_138D2
		dw offset _eff_1392F
		dw offset _eff_139AC
		dw offset _eff_139B2
		dw offset _eff_139B9
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_13AD7
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_13BA3
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_13DE5
		dw offset _eff_13DEF
		dw offset _eff_13E1E
		dw offset _eff_13E2D
		dw offset _eff_13E32
		dw offset _eff_13E7F
		dw offset _eff_13E84
		dw offset _eff_nullsub
		dw offset _eff_138D2
		dw offset _nullsub_2
		dw offset _eff_13F05
		dw offset _eff_13F3B
		dw offset _eff_13FBE
		dw offset _eff_nullsub
		dw offset _eff_nullsub
		dw offset _eff_nullsub
_effoff_19026	dw offset _eff_nullsub	; DATA XREF: _eff_13BA3+Ar
		dw offset _eff_1387F
		dw offset _eff_1389D
		dw offset _eff_13BB2
		dw offset _eff_13BC0
		dw offset _eff_13BC8
		dw offset _eff_13C02
		dw offset _eff_13C34
		dw offset _eff_13C3F
		dw offset _eff_13C64
		dw offset _eff_13C88
		dw offset _eff_13C95
		dw offset _eff_13CA2
		dw offset _eff_13CB3
		dw offset _eff_13CC9
		dw offset _eff_nullsub
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
_text ends

; ---------------------------------------------------------------------------
; ===========================================================================

; Segment type:	Pure code
seg001 segment byte public 'CODE' use16
		assume cs:seg001
; START	OF FUNCTION CHUNK FOR _start
		assume es:nothing, ss:nothing, ds:dseg,	fs:nothing, gs:nothing

loc_19050::				; CODE XREF: _start+4Cj
					; DATA XREF: dseg:off_1DE3Co ...
		call	_callsubx

loc_19053::
		jb	loc_192B9

loc_19057::				; "\rCurrent Soundcard settings:\r\n\n$"
		mov	dx, offset _aCurrentSoundcard
		mov	ah, 9
		int	21h		; DOS -	PRINT STRING
					; DS:DX	-> string terminated by	"$"
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	di, offset _buffer_1 ; 2800h
		call	sub_12D05
		mov	byte ptr es:[di], '$'

loc_1906E::				; 2800h
		mov	dx, offset _buffer_1
		mov	ah, 9
		int	21h		; DOS -	PRINT STRING
					; DS:DX	-> string terminated by	"$"
		mov	dx, offset _myendl ; "\r\n$"
		mov	ah, 9
		int	21h		; DOS -	PRINT STRING
					; DS:DX	-> string terminated by	"$"

loc_1907C::
		call	_deinit_125B9
		mov	ax, 4C00h

loc_19084::				; DOS -	2+ - QUIT WITH EXIT CODE (EXIT)
		int	21h		; AL = exit code
; ---------------------------------------------------------------------------

loc_19086::				; CODE XREF: _start+57j
		mov	dx, 0
		mov	ah, 9
		int	21h		; DOS -	PRINT STRING
					; DS:DX	-> string terminated by	"$"

loc_1908D::
		mov	ax, 4C00h

loc_19090::				; DOS -	2+ - QUIT WITH EXIT CODE (EXIT)
		int	21h		; AL = exit code
; END OF FUNCTION CHUNK	FOR _start
		assume ss:seg004, ds:nothing

; =============== S U B	R O U T	I N E =======================================


_start proc near

; FUNCTION CHUNK AT 0000 SIZE 00000042 BYTES
; FUNCTION CHUNK AT 07D7 SIZE 00000059 BYTES
; FUNCTION CHUNK AT 0D44 SIZE 0000007D BYTES

		mov	ax, dseg

loc_19095::
		mov	ds, ax
		assume ds:dseg
		mov	_esseg_atstart, es
		mov	ax, sp
		add	ax, 13h
		mov	cl, 4

loc_190A2::
		shr	ax, cl
		mov	bx, ss
		add	bx, ax
		mov	ax, es
		sub	bx, ax
		mov	ah, 4Ah
		int	21h		; DOS -	2+ - ADJUST MEMORY BLOCK SIZE (SETBLOCK)
					; ES = segment address of block	to change
					; BX = new size	in paragraphs
		cld

loc_190B1::
		xor	dl, dl
		mov	cx, 4D50h	; PM
		mov	bx, 5344h	; DS
		mov	ax, 60FFh

loc_190BC::
		int	2Fh
		cmp	ax, 4F4Bh	; 'KO' check for single instance
		jnz	short loc_190D3
		mov	dx, offset _aCriticalErrorT ; "\r\n\nCritical error: The	player jumped to"...
		mov	ah, 9
		int	21h		; DOS -	PRINT STRING
					; DS:DX	-> string terminated by	"$"
		xor	ah, ah
		int	16h		; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
					; Return: AH = scan code, AL = character

loc_190CE::
		mov	ax, 4C02h
		int	21h		; DOS -	2+ - QUIT WITH EXIT CODE (EXIT)
					; AL = exit code
; ---------------------------------------------------------------------------

loc_190D3::				; CODE XREF: _start+2Fj
		call	_loadcfg
		call	_parse_cmdline
		bt	ebp, 8
		jb	loc_19050

loc_190E2::
		test	ebp, 80000080h
		jnz	short loc_19086
		bt	ebp, 0Bh
		jnb	short loc_190F7
		or	byte ptr [_configword], 4

loc_190F7::				; CODE XREF: _start+5Ej
		bt	ebp, 3
		jnb	short loc_19103
		and	byte ptr [_configword], 0FBh

loc_19103::				; CODE XREF: _start+6Aj
		bt	ebp, 6
		jnb	short loc_19114
		and	byte ptr [_configword+1], 0F1h
		or	byte ptr [_configword+1], 2

loc_19114::				; CODE XREF: _start+76j
		bt	ebp, 5
		jnb	short loc_19125
		and	byte ptr [_configword+1], 0F1h
		or	byte ptr [_configword+1], 4

loc_19125::				; CODE XREF: _start+87j
		bt	ebp, 13h
		jnb	short loc_19131
		and	byte ptr [_configword+1], 0F1h

loc_19131::				; CODE XREF: _start+98j
		bt	ebp, 4
		jnb	short loc_1913D
		and	byte ptr [_configword], 0FDh

loc_1913D::				; CODE XREF: _start+A4j
		bt	ebp, 14h
		jnb	short loc_19149
		and	byte ptr [_configword], 0FEh

loc_19149::				; CODE XREF: _start+B0j
		bt	ebp, 0Eh
		jnb	short loc_19155
		and	byte ptr [_configword], 0BFh

loc_19155::				; CODE XREF: _start+BCj
		bt	ebp, 2
		jnb	short loc_19161
		or	byte ptr [_configword], 40h

loc_19161::				; CODE XREF: _start+C8j
		bt	ebp, 15h
		setb	al
		mov	_byte_1DE86, al
		mov	al, _byte_1DCF8
		mov	ah, al
		and	al, 0Fh
		mov	_byte_1DE82, al
		shr	ah, 4
		mov	_byte_1DE83, ah

loc_1917D::
		mov	_videomempointer, 0B8000000h
		mov	ax, 3508h
		int	21h		; DOS -	2+ - GET INTERRUPT VECTOR
					; AL = interrupt number
					; Return: ES:BX	= value	of interrupt vector
		mov	_oint8off_1DE14, bx
		mov	_oint8seg_1DE16, es
		mov	ax, 3509h
		int	21h		; DOS -	2+ - GET INTERRUPT VECTOR
					; AL = interrupt number
					; Return: ES:BX	= value	of interrupt vector
		mov	word ptr cs:[_oint9_1C1A4], bx
		mov	word ptr cs:[_oint9_1C1A4+2], es

loc_191A2::
		mov	ax, 3524h

loc_191A5::				; DOS -	2+ - GET INTERRUPT VECTOR
		int	21h		; AL = interrupt number
					; Return: ES:BX	= value	of interrupt vector
		mov	word ptr cs:[_oint24_1C1AC], bx
		mov	word ptr cs:[_oint24_1C1AC+2], es
		mov	ax, 352Fh
		int	21h		; DOS -	2+ - GET INTERRUPT VECTOR
					; AL = interrupt number
					; Return: ES:BX	= value	of interrupt vector
		mov	word ptr cs:[_oint2f_1C1B4], bx
		mov	word ptr cs:[_oint2f_1C1B4+2], es
		push	ds
		mov	ax, cs
		mov	ds, ax
		assume ds:seg001
		mov	dx, offset _int9_keyb
		mov	ax, 2509h
		int	21h		; DOS -	SET INTERRUPT VECTOR
					; AL = interrupt number
					; DS:DX	= new vector to	be used	for specified interrupt
		mov	dx, offset _int24
		mov	ax, 2524h
		int	21h		; DOS -	SET INTERRUPT VECTOR
					; AL = interrupt number
					; DS:DX	= new vector to	be used	for specified interrupt
		mov	dx, offset _int2f_checkmyself
		mov	ax, 252Fh

loc_191DB::				; DOS -	SET INTERRUPT VECTOR
		int	21h		; AL = interrupt number
					; DS:DX	= new vector to	be used	for specified interrupt
		pop	ds
		assume ds:dseg
		mov	ah, 34h
		int	21h		; DOS -	2+ internal - RETURN CritSectFlag (InDOS) POINTER
		mov	_critsectpoint_off, bx
		mov	_critsectpoint_seg, es

loc_191EA::
		push	ds
		mov	ax, 5D06h
		int	21h		; DOS -	3.1+ internal -	GET ADDRESS OF DOS SWAPPABLE DATA AREA
					; Return: CF set on error, CF clear if successful
		mov	ax, ds
		pop	ds
		mov	_swapdata_off,	si
		mov	_swapdata_seg,	ax
		mov	_byte_1DE70, 0FFh
		call	_mouse_init
		mov	bl, byte ptr [_configword+1]
		shr	bl, 1
		and	bx, 7
		cmp	bl, 5
		jbe	short loc_19212
		xor	bl, bl

loc_19212::				; CODE XREF: _start+17Cj
		shl	bx, 1
		mov	ax, [off_1CA8E+bx]
		mov	[off_1DE3C], ax
		cmp	_buffer_1DB6C,	40h ; '@'
		jz	loc_19D94
		cmp	_buffer_1DB6C,	20h ; ' '
		jbe	loc_192CA
		mov	_word_1DE4E, 2
		call	_find_mods
		jb	short loc_19256
		call	_callsubx
		jb	short loc_19256
		call	_readallmoules
		jb	short loc_19250

loc_19242::
		cmp	byte ptr [_word_1DE50], 1Ch
		jz	loc_192E0
		mov	_byte_1DE7E, 0

loc_19250::				; CODE XREF: _start+1AEj _start+5C2j ...
		cli
		call	_deinit_125B9

loc_19256::				; CODE XREF: _start+1A4j _start+1A9j ...
		call	_mouse_deinit
		push	ds
		lds	dx, cs:[_oint2f_1C1B4]
		mov	ax, 252Fh
		int	21h		; DOS -	SET INTERRUPT VECTOR
					; AL = interrupt number
					; DS:DX	= new vector to	be used	for specified interrupt
		pop	ds
		push	ds
		lds	dx, cs:[_oint24_1C1AC]
		mov	ax, 2524h
		int	21h		; DOS -	SET INTERRUPT VECTOR
					; AL = interrupt number
					; DS:DX	= new vector to	be used	for specified interrupt
		pop	ds
		push	ds
		lds	dx, cs:[_oint9_1C1A4]
		mov	ax, 2509h
		int	21h		; DOS -	SET INTERRUPT VECTOR
					; AL = interrupt number
					; DS:DX	= new vector to	be used	for specified interrupt
		pop	ds
		mov	ax, 3
		int	10h		; - VIDEO - SET	VIDEO MODE
					; AL = mode
		call	_txt_enableblink
		mov	cx, 0
		mov	dx, 124Fh
		mov	bl, 78h	; 'x'
		mov	ax, 7F03h
		call	_draw_frame
		call	_txt_draw_top_title
		mov	si, offset _hopeyoulike
		les	di, _videomempointer
		assume es:nothing
		call	_write_scr
		mov	dx, 1300h
		xor	bh, bh
		mov	ah, 2
		int	10h		; - VIDEO - SET	CURSOR POSITION
					; DH,DL	= row, column (0,0 = upper left)
					; BH = page number
		cmp	_byte_1DE7E, 0
		jz	short loc_192C3
		mov	dx, 1400h
		xor	bh, bh
		mov	ah, 2
		int	10h		; - VIDEO - SET	CURSOR POSITION
					; DH,DL	= row, column (0,0 = upper left)
					; BH = page number

loc_192B9::				; CODE XREF: _start:loc_19053j
		push	ds
		lds	dx, _messagepointer
		mov	ah, 9
		int	21h		; DOS -	PRINT STRING
					; DS:DX	-> string terminated by	"$"
		pop	ds

loc_192C3::				; CODE XREF: _start+21Cj
		mov	ah, 4Ch	; 'L'
		mov	al, _byte_1DE7E
		int	21h		; DOS -	2+ - QUIT WITH EXIT CODE (EXIT)
					; AL = exit code

loc_192CA::				; CODE XREF: _start+197j
		mov	_byte_1DE7E, 5
		mov	word ptr [_messagepointer], offset _aNotEnoughMemor ; "Not enough	memory.\r\n$"
		mov	word ptr [_messagepointer+2], ds
		call	_callsubx
		jb	loc_19256

loc_192E0::				; CODE XREF: _start+1B5j
		mov	si, offset _mystr ; str
		call	_dosgetcurdir
		mov	_word_1DE62, 0
		mov	_word_1DE5E, 0
		mov	_byte_1DE7F, 1

loc_192F7::				; CODE XREF: _start+456j _start+6F3j ...
		call	_setvideomode
		mov	_byte_1DE70, 1

loc_192FF::				; CODE XREF: _start+471j _start+4A7j ...
		mov	cx, 0
		mov	dx, 1B4Fh
		mov	bl, 78h	; 'x'
		mov	ax, 7F03h
		call	_draw_frame
		call	_txt_draw_top_title
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	di, offset _buffer_1 ; 2800h
		call	sub_12D05
		mov	byte ptr es:[di], 0
		sub	di, 16EFh
		and	di, 0FFFEh
		mov	ax, 50h	; 'P'
		sub	ax, di
		add	ax, 320h
		les	di, _videomempointer
		assume es:nothing
		add	di, ax		; videoptr
		mov	si, offset _buffer_1 ; str
		mov	ah, 78h	; 'x'   ; color
		call	_put_message
		mov	cx, 604h
		mov	dx, 84Bh
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame
		mov	ah, 19h
		int	21h		; DOS -	GET DEFAULT DISK NUMBER
		mov	edx, 5C3A41h	; "A:\"
		add	dl, al
		mov	_buffer_1DC6C,	edx
		mov	si, (offset _buffer_1DC6C+3)
		xor	dl, dl
		mov	ah, 47h
		int	21h		; DOS -	2+ - GET CURRENT DIRECTORY
					; DL = drive (0=default, 1=A, etc.)
					; DS:SI	points to 64-byte buffer area
		mov	si, offset _buffer_1DC6C
		call	_mystrlen
		shr	ax, 1
		neg	ax
		add	ax, 257h
		shl	ax, 1
		les	di, _videomempointer
		add	di, ax		; videoptr
		mov	si, offset _buffer_1DC6C	; str
		mov	ah, 7Bh	; '{'   ; color
		call	_put_message
		cmp	_byte_1DE7F, 1
		jnz	short loc_19395
		mov	si, offset _msg	; "Searching directory for modules  "
		mov	ax, 7E0Dh
		call	_message_1BE77
		call	_modules_search

loc_19395::				; CODE XREF: _start+2F5j
		mov	_byte_1DE7E, 0
		mov	_word_1DE60, 0FFFFh
		mov	cx, 906h
		mov	dx, 1949h
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame

loc_193AE::				; CODE XREF: _start+4C8j _start+4EAj ...
		call	_filelist_198B8
		mov	ax, _word_1DE62
		mov	bl, 10h
		call	_recolortxt

loc_193BC::				; CODE XREF: _start+3CBj _start+481j ...
		mov	al, _byte_1DE7C
		xor	al, 1
		mov	_byte_1DE7D, al
		call	_mouse_show

loc_193C7::				; CODE XREF: _start+373j
		test	_byte_1DE90, 2
		jnz	loc_19848
		test	_byte_1DE90, 1
		jnz	loc_19827
		mov	al, _byte_1DE7C
		cmp	al, _byte_1DE7D
		jz	short loc_193FF	; keyboard message loop	here
		mov	_byte_1DE7D, al
		les	di, _videomempointer
		add	di, 104Ah	; videoptr
		mov	ah, 78h	; 'x'   ; color
		mov	si, offset _aHitBackspaceToRe ; "Hit backspace to return	to playmode, F-"...
		cmp	_byte_1DE7C, 0
		jz	short loc_193FC
		mov	si, offset _aPressF1ForHelpQu ; "		 Press F-1 for help, Qu"...

loc_193FC::				; CODE XREF: _start+365j
		call	_put_message

loc_193FF::				; CODE XREF: _start+34Ej
		mov	ax, cs:[_key_code] ; keyboard message loop here
		or	ax, ax
		jz	short loc_193C7
		push	ax
		call	_mouse_hide
		pop	ax
		mov	cs:[_key_code], 0
		cmp	al, 1
		jz	loc_1964E
		cmp	al, 48h	; 'H'
		jz	loc_1957F
		cmp	al, 50h	; 'P'
		jz	loc_1953C
		cmp	al, 47h	; 'G'
		jz	loc_195A7
		cmp	al, 4Fh	; 'O'
		jz	loc_195BE
		cmp	al, 49h	; 'I'
		jz	loc_195EA
		cmp	al, 51h	; 'Q'

loc_19439::
		jz	loc_19610
		cmp	al, 0Eh
		jz	loc_19762
		cmp	al, 53h	; 'S'

loc_19445::
		jz	loc_19657
		cmp	al, 3Bh	; ';'
		jz	loc_19788
		cmp	al, 42h	; 'B'
		jz	loc_197D6
		cmp	al, 43h	; 'C'
		jz	loc_197E7
		cmp	al, 1Ch
		jnz	loc_193BC
		mov	ax, _word_1DE62

loc_19464::				; CODE XREF: seg001:0839j
		add	ax, _word_1DE5E
		mov	dx, ax
		shl	ax, 1
		add	ax, dx
		add	ax, _word_1DE52
		mov	fs, ax
		mov	si, 0Ch
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	di, offset _buffer_1DB6C
		mov	dx, di
		cld
		movs	dword ptr es:[di], dword ptr fs:[si]
		movs	dword ptr es:[di], dword ptr fs:[si]
		movs	dword ptr es:[di], dword ptr fs:[si]
		mov	byte ptr [di], 0
		cmp	byte ptr fs:[2], 0
		jz	short loc_194EB
		cmp	byte ptr fs:[2], 1
		jz	short loc_19506
		push	dx

loc_1949E::
		mov	cx, 501h
		mov	dx, 1A4Eh
		mov	bl, 7Fh	; ''
		mov	ax, 7800h
		call	_draw_frame
		mov	si, offset _aLoadingModule ; _msg
		mov	ax, 7E0Dh
		call	_message_1BE77
		pop	dx
		call	_read_module

loc_194B9::
		jnb	short loc_194E3
		mov	si, offset _aNotEnoughMemo_0 ; "Not enough memory available to load all"...
		cmp	ax, 0FFFEh
		jz	short loc_194CE
		mov	si, offset _aNotEnoughDram_0 ; "Not enough DRAM on your UltraSound to l"...
		cmp	ax, 0FFFDh
		jz	short loc_194CE
		mov	si, offset _aModuleIsCorrupt ; _msg

loc_194CE::				; CODE XREF: _start+42Fj _start+437j
		mov	ax, 7E0Dh
		call	_message_1BE77
		xor	ax, ax
		mov	cs:[_key_code], ax

loc_194DA::				; CODE XREF: _start+44Fj
		xchg	ax, cs:[_key_code]
		or	ax, ax
		jz	short loc_194DA

loc_194E3::				; CODE XREF: _start:loc_194B9j
		mov	_byte_1DE7F, 0
		jmp	loc_192F7
; ---------------------------------------------------------------------------

loc_194EB::				; CODE XREF: _start+401j
		mov	_byte_1DE7F, 1
		mov	dx, offset _buffer_1DB6C
		mov	ah, 3Bh
		int	21h		; DOS -	2+ - CHANGE THE	CURRENT	DIRECTORY (CHDIR)
					; DS:DX	-> ASCIZ directory name	(may include drive)
		mov	_word_1DE62, 0
		mov	_word_1DE5E, 0
		jmp	loc_192FF
; ---------------------------------------------------------------------------

loc_19506::				; CODE XREF: _start+409j
		mov	_byte_1DE7F, 1
		mov	dl, fs:[0Dh]
		sub	dl, 'A'
		jb	loc_193BC
		cmp	dl, 'Z' - 'A' +1
		jnb	loc_193BC
		mov	bl, dl
		inc	bl
		push	dx
		mov	ax, 440Fh
		int	21h		; DOS -	2+ - IOCTL -
		pop	dx
		mov	ah, 0Eh
		int	21h		; DOS -	SELECT DISK
					; DL = new default drive number	(0 = A,	1 = B, etc.)
					; Return: AL = number of logical drives
		mov	_word_1DE62, 0
		mov	_word_1DE5E, 0
		jmp	loc_192FF
; ---------------------------------------------------------------------------

loc_1953C::				; CODE XREF: _start+38Fj _start+603j ...
		cmp	_word_1DE62, 0Eh
		jnb	short loc_1955D
		mov	bx, _word_1DE54
		dec	bx
		mov	ax, _word_1DE62
		cmp	ax, bx
		jnb	loc_193BC
		mov	bl, 70h	; 'p'
		call	_recolortxt
		inc	_word_1DE62
		jmp	loc_193AE
; ---------------------------------------------------------------------------

loc_1955D::				; CODE XREF: _start+4AFj
		cmp	_word_1DE54, 0Fh
		jb	loc_193BC
		mov	ax, _word_1DE54
		sub	ax, _word_1DE5E
		jb	loc_193BC
		cmp	ax, 10h
		jb	loc_193BC
		inc	_word_1DE5E
		jmp	loc_193AE
; ---------------------------------------------------------------------------

loc_1957F::				; CODE XREF: _start+389j
					; DATA XREF: dseg:_str_24461o ...
		cmp	_word_1DE62, 0
		jz	short loc_19595
		mov	ax, _word_1DE62
		mov	bl, 70h	; 'p'
		call	_recolortxt
		dec	_word_1DE62
		jmp	loc_193AE
; ---------------------------------------------------------------------------

loc_19595::				; CODE XREF: _start+4F2j
		sub	_word_1DE5E, 1
		jnb	loc_193AE
		mov	_word_1DE5E, 0
		jmp	loc_193AE
; ---------------------------------------------------------------------------

loc_195A7::				; CODE XREF: _start+395j
		mov	ax, _word_1DE62
		mov	bl, 70h	; 'p'
		call	_recolortxt
		mov	_word_1DE62, 0
		mov	_word_1DE5E, 0
		jmp	loc_193AE
; ---------------------------------------------------------------------------

loc_195BE::				; CODE XREF: _start+39Bj
		mov	ax, _word_1DE62
		mov	bl, 70h	; 'p'
		call	_recolortxt
		mov	ax, _word_1DE54
		dec	ax
		cmp	ax, 0Fh
		jb	short loc_195DE
		sub	ax, 0Eh
		mov	_word_1DE5E, ax
		mov	_word_1DE62, 0Eh
		jmp	loc_193AE
; ---------------------------------------------------------------------------

loc_195DE::				; CODE XREF: _start+53Bj
		mov	_word_1DE5E, 0
		mov	_word_1DE62, ax
		jmp	loc_193AE
; ---------------------------------------------------------------------------

loc_195EA::				; CODE XREF: _start+3A1j
		mov	ax, _word_1DE62
		mov	bl, 70h	; 'p'
		call	_recolortxt
		xor	ax, ax
		xchg	ax, _word_1DE62
		or	ax, ax
		jnz	loc_193AE
		sub	_word_1DE5E, 0Fh
		jnb	loc_193AE
		mov	_word_1DE5E, 0
		jmp	loc_193AE
; ---------------------------------------------------------------------------

loc_19610::				; CODE XREF: _start:loc_19439j
		mov	ax, _word_1DE62
		mov	bl, 70h	; 'p'
		call	_recolortxt
		mov	ax, _word_1DE54
		dec	ax
		cmp	ax, 0Fh
		jb	short loc_19648
		mov	ax, 0Eh
		xchg	ax, _word_1DE62
		cmp	ax, 0Eh
		jnz	loc_193AE
		add	_word_1DE5E, 0Fh
		mov	ax, _word_1DE54
		sub	ax, 0Fh
		cmp	_word_1DE5E, ax
		jbe	loc_193AE
		mov	_word_1DE5E, ax
		jmp	loc_193AE
; ---------------------------------------------------------------------------

loc_19648::				; CODE XREF: _start+58Dj
		mov	_word_1DE62, ax
		jmp	loc_193AE
; ---------------------------------------------------------------------------

loc_1964E::				; CODE XREF: _start+383j
					; DATA XREF: dseg:7CAFo
		mov	si, offset _mystr ; str
		call	_doschdir
		jmp	loc_19250
; ---------------------------------------------------------------------------

loc_19657::				; CODE XREF: _start:loc_19445j
		mov	ax, _word_1DE5E
		add	ax, _word_1DE62
		mov	dx, ax
		shl	ax, 1
		add	ax, dx
		add	ax, _word_1DE52
		mov	fs, ax
		test	cs:[_keyb_switches], 4
		jnz	short loc_196B0
		cmp	byte ptr fs:[2], 2
		jnz	loc_193BC
		mov	_word_1DE60, 0FFFFh
		test	byte ptr fs:[3], 40h
		jnz	short loc_19698
		or	byte ptr fs:[3], 40h
		inc	_word_1DE5C
		jmp	loc_1953C
; ---------------------------------------------------------------------------

loc_19698::				; CODE XREF: _start+5F7j
		and	byte ptr fs:[3], 0BFh
		sub	_word_1DE5C, 1
		jnb	loc_1953C
		mov	_word_1DE5C, 0
		jmp	loc_1953C
; ---------------------------------------------------------------------------

loc_196B0::				; CODE XREF: _start+5DFj
		cmp	_word_1DE5C, 0
		jz	loc_193BC
		mov	cx, 602h
		mov	dx, 1A4Eh
		mov	bl, 7Fh	; ''
		mov	ax, 7800h
		call	_draw_frame
		mov	si, offset _aDeleteMarkedFil ; "Delete marked files? [Y/N]"
		mov	ax, 7E0Dh
		call	_message_1BE77

loc_196D0::				; CODE XREF: _start+647j _start+649j
		xor	ax, ax
		xchg	ax, cs:[_key_code]
		or	ax, ax
		jz	short loc_196D0
		js	short loc_196D0
		mov	_byte_1DE7F, 0
		cmp	ax, 15h
		jnz	loc_192FF
		mov	fs, _word_1DE52
		mov	cx, _word_1DE54

loc_196F1::				; CODE XREF: _start+6BAj
		test	byte ptr fs:[3], 40h
		jz	short loc_19744
		cmp	byte ptr fs:[2], 2
		jnz	short loc_19744
		push	cx
		push	fs
		mov	cx, 602h
		mov	dx, 1A4Eh
		mov	bl, 7Fh	; ''
		mov	ax, 7800h
		call	_draw_frame
		pop	fs
		push	fs
		mov	eax, fs:[0Ch]
		mov	dword ptr [_aFile], eax ; "File"
		mov	eax, fs:[10h]
		mov	dword ptr [_aName], eax ; "name"
		mov	eax, fs:[14h]
		mov	dword ptr [_a_ext], eax ; ".Ext"
		mov	si, offset _aDeletingFile ; "Deleting file: "
		mov	ax, 7E0Dh
		call	_message_1BE77
		mov	dx, offset _aFile ; "File"
		mov	ah, 41h
		int	21h		; DOS -	2+ - DELETE A FILE (UNLINK)
					; DS:DX	-> ASCIZ pathname of file to delete (no	wildcards allowed)
		pop	fs
		pop	cx

loc_19744::				; CODE XREF: _start+665j _start+66Dj
		mov	ax, fs
		add	ax, 3
		mov	fs, ax
		assume fs:nothing
		dec	cx
		jnz	short loc_196F1
		mov	_word_1DE62, 0
		mov	_word_1DE5E, 0
		mov	_byte_1DE7F, 1
		jmp	loc_192FF
; ---------------------------------------------------------------------------

loc_19762::				; CODE XREF: _start+3ADj
					; DATA XREF: dseg:7C8Fo
		cmp	_byte_1DE7C, 1
		jz	loc_193BC
		mov	cx, 602h
		mov	dx, 1A4Eh
		mov	bl, 7Fh	; ''
		mov	ax, 7800h
		call	_draw_frame
		call	[off_1DE3C]
		call	_keyb_screen_loop
		mov	_byte_1DE7F, 0
		jmp	loc_192F7
; ---------------------------------------------------------------------------

loc_19788::				; CODE XREF: _start+3B9j
		mov	cx, 604h
		mov	dx, 84Bh
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame
		mov	cx, 906h
		mov	dx, 1949h
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame
		les	di, _videomempointer
		assume es:nothing
		add	di, 1042h
		mov	cx, 4Eh	; 'N'
		mov	ax, 7820h	; 'x'
		cld
		rep stosw
		mov	si, offset _word_1D3B0
		les	di, _videomempointer
		call	_write_scr

loc_197BF::				; CODE XREF: _start+733j
		cmp	byte ptr cs:[_key_code],	0
		jle	short loc_197BF
		mov	cs:[_key_code], 0
		mov	_byte_1DE7F, 0
		jmp	loc_192F7
; ---------------------------------------------------------------------------

loc_197D6::				; CODE XREF: _start+3BFj
		call	_mouse_deinit
		call	_dosexec
		call	_mouse_init
		mov	_byte_1DE7F, 0
		jmp	loc_192F7
; ---------------------------------------------------------------------------

loc_197E7::				; CODE XREF: _start+3C5j
		xor	byte ptr [_configword], 20h
		jmp	loc_193BC
_start endp


; =============== S U B	R O U T	I N E =======================================




; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR _start

loc_19827::				; CODE XREF: _start+343j
		call	_mouse_hide
		and	_byte_1DE90, 0FEh
;		mov	bx, offset _str_24461 ; _mystr
		mov	ax, _mousecolumn
		mov	bp, _mouserow
		shr	ax, 3
		shr	bp, 3
		call	_mouse_1C7CF
		jb	loc_193BC
		jmp	bx
; ---------------------------------------------------------------------------

loc_19848::				; CODE XREF: _start+33Aj
		call	_mouse_hide
;		mov	bx, offset stru_2448B ;	_mystr
		mov	ax, _mousecolumn
		mov	bp, _mouserow
		shr	ax, 3
		shr	bp, 3
		call	_mouse_1C7CF
		jb	loc_193BC
		push	es
		xor	dx, dx
		mov	es, dx
		assume es:nothing
		mov	edx, es:[46Ch]
		cmp	edx, _dword_1DE88
		jz	short loc_1987C
		mov	_dword_1DE88, edx
		pop	es
		assume es:nothing
		jmp	bx
; ---------------------------------------------------------------------------

loc_1987C::				; CODE XREF: _start+7E0j
		pop	es
		jmp	loc_193BC
; END OF FUNCTION CHUNK	FOR _start
; ---------------------------------------------------------------------------

loc_19880::				; DATA XREF: dseg:7C85o
		cmp	bp, 0Eh
		ja	loc_193BC
		mov	ax, bp
		jmp	loc_19464

; =============== S U B	R O U T	I N E =======================================


; void __usercall _dosgetcurdir(char *str<esi>)
_dosgetcurdir proc near	; CODE XREF: _start+251p _dosexec+53p
		push	si
		mov	ah, 19h
		int	21h		; DOS -	GET DEFAULT DISK NUMBER
		pop	si
		mov	[si], al
		mov	byte ptr [si+1], '\'
		add	si, 2
		xor	dl, dl
		mov	ah, 47h
		int	21h		; DOS -	2+ - GET CURRENT DIRECTORY
					; DL = drive (0=default, 1=A, etc.)
					; DS:SI	points to 64-byte buffer area
		retn
_dosgetcurdir endp


; =============== S U B	R O U T	I N E =======================================


; void __usercall _doschdir(char	*str<esi>)
_doschdir proc near		; CODE XREF: _start+5BFp _dosexec+A6p
		mov	dl, [si]
		inc	si
		push	si
		mov	ah, 0Eh
		int	21h		; DOS -	SELECT DISK
					; DL = new default drive number	(0 = A,	1 = B, etc.)
					; Return: AL = number of logical drives
		pop	dx
		mov	ah, 3Bh
		int	21h		; DOS -	2+ - CHANGE THE	CURRENT	DIRECTORY (CHDIR)
					; DS:DX	-> ASCIZ directory name	(may include drive)
		retn
_doschdir endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR _filelist_198B8

_recolortxtx::				; CODE XREF: _filelist_198B8+8j
		mov	ax, _word_1DE62
		mov	bl, 70h	; 'p'
		jmp	_recolortxt
; END OF FUNCTION CHUNK	FOR _filelist_198B8

; =============== S U B	R O U T	I N E =======================================


_filelist_198B8 proc near	; CODE XREF: _start:loc_193AEp

; FUNCTION CHUNK AT 0860 SIZE 00000008 BYTES

		cld
		mov	ax, _word_1DE5E
		cmp	ax, _word_1DE60
		jz	short _recolortxtx
		mov	_word_1DE60, ax
		mov	cx, _word_1DE54
		sub	cx, _word_1DE5E
		cmp	cx, 0Fh
		jb	short loc_198D5
		mov	cx, 0Fh

loc_198D5::				; CODE XREF: _filelist_198B8+18j
		mov	ax, _word_1DE5E
		mov	dx, ax
		shl	ax, 1
		add	ax, dx
		add	ax, _word_1DE52
		mov	fs, ax
		assume fs:nothing
		mov	ax, (80*10+10)*2

loc_198E7::				; CODE XREF: _filelist_198B8+13Cj
		push	ax
		push	cx
		les	di, _videomempointer
		add	di, ax
		mov	bp, di
		mov	ah, 7Eh	; '~'
		cmp	byte ptr fs:[2], 2
		jz	short loc_198FD
		mov	ah, 7Bh	; '{'

loc_198FD::				; CODE XREF: _filelist_198B8+41j
		mov	si, 0Ch
		mov	cx, 0Ch

loc_19903::				; CODE XREF: _filelist_198B8+5Aj
		mov	al, fs:[si]
		or	al, al
		jz	short loc_19914	; " " fill the space after file	names
		mov	es:[di], ax
		inc	si
		add	di, 2
		dec	cx
		jnz	short loc_19903

loc_19914::				; CODE XREF: _filelist_198B8+50j
		mov	ax, 7E20h	; " " fill the space after file	names
		cld
		rep stosw
		cmp	byte ptr fs:[2], 2
		jz	short loc_1992A
		mov	cx, 51

loc_19925::
		rep stosw
		jmp	loc_199E7
; ---------------------------------------------------------------------------

loc_1992A::				; CODE XREF: _filelist_198B8+68j
		push	bp
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	di, offset _buffer_1 ; 2800h
		mov	bp, 8		; count
		mov	eax, fs:[8]
		call	_my_u32toa_fill

loc_1993D::
		mov	ax, fs:[6]
		and	al, 1Fh
		movzx	eax, al
		mov	bp, 3		; count
		call	_my_u32toa_fill
		mov	byte ptr [di], '-'

loc_19950::
		inc	di
		mov	ax, fs:[6]
		shr	ax, 5

loc_19958::
		and	eax, 0Fh
		lea	si, [_aJanfebmaraprmayj+eax+eax*2] ; "	JanFebMarAprMayJunJulAugSepOctNovDec"
		cld
		movsw
		movsb
		mov	byte ptr [di], '-'
		inc	di
		movzx	eax, word ptr fs:[6]
		shr	ax, 9
		add	ax, 1980
		mov	bp, 4		; count
		call	_my_u32toa_fill
		mov	byte ptr [di], ' '
		inc	di
		mov	ax, fs:[4]
		shr	ax, 0Bh
		mov	dl, 10
		div	dl
		or	ax, '00'
		mov	[di], ax
		mov	byte ptr [di+2], ':'
		mov	ax, fs:[4]
		shr	ax, 5
		and	ax, 3Fh	; '?'
		div	dl
		or	ax, '00'
		mov	[di+3],	ax
		mov	word ptr [di+5], ' '
		pop	bp
		mov	si, offset _buffer_1 ; str
		mov	es, word ptr [_videomempointer+2]
		assume es:nothing
		lea	di, [bp+18h]
		mov	ah, 7Fh	; ''
		call	_text_1BF69
		test	byte ptr fs:[3], 40h
		jz	short loc_199CF
		mov	si, offset _aMarkedToDelete ; "<Marked to Delete>    "
		mov	ah, 7Fh	; ''
		call	_text_1BF69
		jmp	short loc_199E7
; ---------------------------------------------------------------------------

loc_199CF::				; CODE XREF: _filelist_198B8+10Bj
		mov	ah, 7Eh	; '~'
		mov	si, 1Ah

loc_199D4::				; CODE XREF: _filelist_198B8+12Dj
		mov	al, fs:[si]
		or	al, al
		jz	short loc_199E7
		mov	es:[di], ax
		inc	si
		add	di, 2
		cmp	si, 30h	; '0'
		jb	short loc_199D4

loc_199E7::				; CODE XREF: _filelist_198B8+6Fj
					; _filelist_198B8+115j ...
		mov	ax, fs
		add	ax, 3
		mov	fs, ax
		assume fs:nothing
		pop	cx
		pop	ax
		add	ax, 0A0h ; '†'
		dec	cx
		jnz	loc_198E7
		retn
_filelist_198B8 endp


; =============== S U B	R O U T	I N E =======================================


_recolortxt proc near		; CODE XREF: _start+324p _start+4C1p ...
		imul	di, ax,	80*2
		add	di, (80*2*10)+(8*2)+1
		mov	cx, 64

loc_19A04::				; CODE XREF: _recolortxt+19j
		mov	al, es:[di]
		and	al, 0Fh
		or	al, bl
		mov	es:[di], al
		add	di, 2
		dec	cx
		jnz	short loc_19A04
		retn
_recolortxt endp


; =============== S U B	R O U T	I N E =======================================


_cpy_printable proc near	; CODE XREF: _modules_search+230p
		push	si
		push	di

loc_19A17::				; CODE XREF: _cpy_printable+Ej
		mov	al, [si]
		inc	si
		cmp	al, ' '
		jb	short loc_19A25
		mov	es:[di], al
		inc	di
		dec	cx
		jnz	short loc_19A17

loc_19A25::				; CODE XREF: _cpy_printable+7j
		cld
		mov	al, ' '
		rep stosb
		pop	di
		pop	si
		retn
_cpy_printable endp

; ---------------------------------------------------------------------------
		dec	cx
		jz	short locret_19A3E
		mov	al, es:[si]
		inc	si

loc_19A34::				; CODE XREF: seg001:09ECj
		xor	es:[si], al
		mov	al, es:[si]
		inc	si
		dec	cx
		jnz	short loc_19A34

locret_19A3E::				; CODE XREF: seg001:09DEj
		retn
; ---------------------------------------------------------------------------
		dec	cx
		jz	short locret_19A52
		mov	al, es:[si]
		inc	si

loc_19A46::				; CODE XREF: seg001:0A00j
		mov	ah, es:[si]
		xor	es:[si], al
		mov	al, ah
		inc	si
		dec	cx
		jnz	short loc_19A46

locret_19A52::				; CODE XREF: seg001:09F0j
		retn

; =============== S U B	R O U T	I N E =======================================


_modules_search proc near	; CODE XREF: _start+300p
		mov	_word_1DE64, 2192
		mov	_word_1DE66, 0
		cmp	_word_1DE52, 0
		jz	short loc_19A6E
		mov	es, _word_1DE52
		assume es:nothing
		mov	ah, 49h
		int	21h		; DOS -	2+ - FREE MEMORY
					; ES = segment address of area to be freed

loc_19A6E::				; CODE XREF: _modules_search+11j
		mov	bx, 1000h
		mov	ah, 48h
		int	21h		; DOS -	2+ - ALLOCATE MEMORY
					; BX = number of 16-byte paragraphs desired
		jb	loc_19250
		mov	_word_1DE52, ax
		mov	es, ax
		assume es:nothing
		xor	di, di
		mov	cx, 4000h
		xor	eax, eax
		cld
		rep stosd
		mov	dword ptr [_buffer_1DB6C], 2A2E2Ah ; '*.*'
		mov	_word_1DE5C, 0
		mov	_word_1DE54, 0
		mov	_word_1DE56, 0
		mov	_word_1DE58, 0
		mov	_word_1DE5A, 0
		cld
		mov	_word_1DE4E, 12h
		call	_find_mods
		mov	es, _word_1DE52
		jb	loc_19CA2

loc_19AC3::				; CODE XREF: _modules_search+CFj
		lfs	di, _videomempointer
		assume fs:nothing
		add	di, _word_1DE64
		mov	bx, _word_1DE66
		mov	ah, 7Fh	; ''
		mov	al, byte ptr [_slider+bx] ; "ƒ\\|/ƒ\\|/"
		mov	fs:[di], ax
		inc	_word_1DE66
		and	_word_1DE66, 7
		test	byte ptr [unk_1DC01], 10h
		jz	short loc_19B1D
		cmp	word ptr [_buffer_1DB6C], '.'
		jz	short loc_19B1D
		mov	byte ptr es:[2], 0
		mov	si, offset _buffer_1DB6C
		mov	di, 0Ch
		cld
		movsd
		movsd
		movsd
		movsb
		inc	_word_1DE56
		mov	ax, es
		add	ax, 3
		mov	es, ax
		assume es:nothing
		inc	_word_1DE54
		cmp	_word_1DE54, 52Bh
		jnb	loc_19CA2

loc_19B1D::				; CODE XREF: _modules_search+94j
					; _modules_search+9Bj
		push	es
		call	_dosfindnext
		pop	es
		assume es:nothing
		jnb	short loc_19AC3

loc_19B24::				; '*.*'
		mov	dword ptr [_buffer_1DB6C], 2A2E2Ah
		mov	_word_1DE4E, 2
		push	es
		call	_find_mods
		pop	es
		assume es:nothing
		jb	loc_19CA2

loc_19B3C::				; CODE XREF: _modules_search+24Bj
		lfs	di, _videomempointer
		add	di, _word_1DE64
		mov	bx, _word_1DE66
		mov	ah, 7Fh	; ''
		mov	al, byte ptr [_slider+bx] ; "ƒ\\|/ƒ\\|/"
		mov	fs:[di], ax
		inc	_word_1DE66
		and	_word_1DE66, 7
		test	byte ptr [unk_1DC01], 10h
		jnz	loc_19C99
		mov	si, offset _buffer_1DB6C
		mov	cx, 8

loc_19B6A::				; CODE XREF: _modules_search+125j
		inc	si
		cmp	byte ptr [si], 0
		jz	loc_19C99
		cmp	byte ptr [si], '.'
		jz	short loc_19B7D
		dec	cx
		jnz	short loc_19B6A
		jmp	loc_19C99
; ---------------------------------------------------------------------------

loc_19B7D::				; CODE XREF: _modules_search+122j
		mov	edx, [si]
		mov	si, offset _a_mod_nst_669_s ; ".MOD.NST.669.STM.S3M.MTM.PSM.WOW.INR.FA"...

loc_19B83::				; CODE XREF: _modules_search+13Fj
		mov	eax, [si]
		or	al, al
		jz	loc_19C99
		add	si, 4
		cmp	eax, edx
		jnz	short loc_19B83
		mov	si, offset _buffer_1DB6C
		mov	dx, si
		mov	byte ptr es:[2], 2
		mov	word ptr es:[0], 0
		mov	di, 0Ch
		cld
		movsd
		movsd
		movsd
		movsb
		mov	si, offset unk_1DC01

loc_19BB4::
		mov	di, 3
		movsb
		and	byte ptr es:[3], 3Fh ; '?'
		movsd
		movsd
		mov	ebp, eax
		inc	_word_1DE58
		cmp	cs:[_key_code], 1
		jnz	short loc_19BDD
		mov	cs:[_key_code], 0
		or	byte ptr [_configword], 20h

loc_19BDD::				; CODE XREF: _modules_search+17Cj
		mov	si, offset asc_1DA00 ; "		      "
		mov	cx, 16h
		test	byte ptr [_configword], 20h
		jnz	loc_19C80
		mov	ax, 3D00h
		int	21h		; DOS -	2+ - OPEN DISK FILE WITH HANDLE
					; DS:DX	-> ASCIZ filename
					; AL = access mode
					; 0 - read
		jb	loc_19C86
		mov	bx, ax
		mov	dx, offset _buffer_1DC6C
		mov	bx, ax
		mov	cx, 80h	; 'Ä'
		mov	ah, 3Fh	; '?'
		push	bx
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		pop	bx
		pushf
		mov	ah, 3Eh
		int	21h		; DOS -	2+ - CLOSE A FILE WITH HANDLE
					; BX = file handle
		popf
		jb	short loc_19C86
		mov	si, offset _byte_1DC7C
		mov	cx, 16h
		cmp	ebp, 524E492Eh	; .INR
		jz	short loc_19C80
		mov	si, offset unk_1DC7B
		mov	cx, 16h
		cmp	ebp, 544C552Eh	; .ULT
		jz	short loc_19C80
		mov	si, offset unk_1DC70
		mov	cx, 16h
		cmp	ebp, 4D544D2Eh	; .MTM
		jz	short loc_19C80
		cmp	ebp, 4D53502Eh	; .PSM
		jz	short loc_19C80
		cmp	ebp, 5241462Eh	; .FAR
		jz	short loc_19C80
		cmp	ebp, 3936362Eh	; .669
		jz	short loc_19C71
		mov	si, offset _byte_1DC7C
		mov	cx, 16h
		cmp	ebp, 5353542Eh	; .TSS
		jz	short loc_19C80
		mov	si, offset _buffer_1DC6C
		mov	cx, 16h
		mov	word ptr [si+14h], '  '
		jmp	short loc_19C80
; ---------------------------------------------------------------------------

loc_19C71::				; CODE XREF: _modules_search+200j
		mov	si, (offset _buffer_1DC6C+1)
		mov	cx, 54h	; 'T'

loc_19C77::				; CODE XREF: _modules_search+228j
		inc	si
		cmp	byte ptr [si], ' '
		loope	loc_19C77
		mov	cx, 16h

loc_19C80::				; CODE XREF: _modules_search+195j
					; _modules_search+1C7j ...
		mov	di, 1Ah
		call	_cpy_printable

loc_19C86::				; CODE XREF: _modules_search+19Ej
					; _modules_search+1B8j
		mov	ax, es
		add	ax, 3
		mov	es, ax
		assume es:nothing
		inc	_word_1DE54
		cmp	_word_1DE54, 52Bh
		jnb	short loc_19CA2

loc_19C99::				; CODE XREF: _modules_search+10Dj
					; _modules_search+11Bj ...
		push	es
		call	_dosfindnext
		pop	es
		jnb	loc_19B3C

loc_19CA2::				; CODE XREF: _modules_search+6Cj
					; _modules_search+C6j ...
		mov	ah, 19h
		int	21h		; DOS -	GET DEFAULT DISK NUMBER
		push	ax
		xor	dl, dl

loc_19CA9::				; CODE XREF: _modules_search+291j
		push	dx
		mov	ah, 0Eh
		int	21h		; DOS -	SELECT DISK
					; DL = new default drive number	(0 = A,	1 = B, etc.)
					; Return: AL = number of logical drives
		mov	ah, 19h
		int	21h		; DOS -	GET DEFAULT DISK NUMBER
		pop	dx
		cmp	al, dl
		jnz	short loc_19CDF
		mov	eax, 5D3A415Bh	; [A:]
		add	ah, dl
		mov	es:[0Ch], eax
		mov	byte ptr es:[10h], 0
		mov	byte ptr es:[2], 1
		inc	_word_1DE5A
		mov	ax, es
		add	ax, 3
		mov	es, ax
		assume es:nothing
		inc	_word_1DE54

loc_19CDF::				; CODE XREF: _modules_search+262j
		inc	dl

loc_19CE1::
		cmp	dl, 1Ah
		jb	short loc_19CA9
		pop	dx
		mov	ah, 0Eh
		int	21h		; DOS -	SELECT DISK
					; DL = new default drive number	(0 = A,	1 = B, etc.)
					; Return: AL = number of logical drives
		mov	es, _word_1DE52
		assume es:nothing
		mov	ax, _word_1DE54
		mov	bx, ax
		shl	ax, 1
		add	bx, ax
		mov	ah, 4Ah
		int	21h		; DOS -	2+ - ADJUST MEMORY BLOCK SIZE (SETBLOCK)
					; ES = segment address of block	to change
					; BX = new size	in paragraphs
		clc
		retn
_modules_search endp


; =============== S U B	R O U T	I N E =======================================


_parse_cmdline proc near	; CODE XREF: _start+44p
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		xor	ebp, ebp
		mov	ds, _esseg_atstart
		assume ds:nothing
		mov	si, 80h	; 'Ä'   ; psp:80h commandline
		mov	di, offset _buffer_1DB6C
		xor	dl, dl
		cld
		lodsb
		movzx	cx, al		; number of bytes on commandline
		stc
		jcxz	short loc_19D64

loc_19D19::				; CODE XREF: _parse_cmdline+29j
					; _parse_cmdline+47j ...
		lodsb
		cmp	al, 0Dh
		jz	short loc_19D63
		or	al, al
		jz	short loc_19D63
		cmp	al, ' '
		jnz	short loc_19D4E
		dec	cx
		jnz	short loc_19D19
		stc
		jmp	short loc_19D64
; ---------------------------------------------------------------------------

loc_19D2C::				; CODE XREF: _parse_cmdline+52j
		lodsb
		cmp	al, 0Dh
		jz	short loc_19D63
		or	al, al
		jz	short loc_19D63
		cmp	al, '?'
		jz	short loc_19D47
		and	al, 0DFh
		sub	al, 'A'
		movzx	eax, al
		bts	ebp, eax
		jmp	short loc_19D19
; ---------------------------------------------------------------------------

loc_19D47::				; CODE XREF: _parse_cmdline+39j
		bts	ebp, 1Fh
		jmp	short loc_19D19
; ---------------------------------------------------------------------------

loc_19D4E::				; CODE XREF: _parse_cmdline+26j
					; _parse_cmdline+62j
		cmp	al, '/'
		jz	short loc_19D2C
		stosb
		lodsb
		cmp	al, 0Dh
		jz	short loc_19D63
		or	al, al
		jz	short loc_19D63
		cmp	al, ' '
		jz	short loc_19D63
		loop	loc_19D4E
		stosb

loc_19D63::				; CODE XREF: _parse_cmdline+1Ej
					; _parse_cmdline+22j ...
		clc

loc_19D64::				; CODE XREF: _parse_cmdline+19j
					; _parse_cmdline+2Cj
		mov	byte ptr es:[di], 0
		mov	ax, es
		mov	ds, ax
		assume ds:dseg
		retn
_parse_cmdline endp


; =============== S U B	R O U T	I N E =======================================


_readallmoules proc near	; CODE XREF: _start+1ABp
					; _readallmoules+12j ...
		mov	dx, offset _buffer_1DB6C
		call	_read_module
		jb	short loc_19D83

loc_19D75::
		cmp	_word_1DE50, 1
		jz	short loc_19D81
		call	_dosfindnext
		jnb	short _readallmoules

loc_19D81::				; CODE XREF: _readallmoules+Dj
		clc
		retn
; ---------------------------------------------------------------------------

loc_19D83::				; CODE XREF: _readallmoules+6j
		mov	_byte_1DE7E, 3
		mov	word ptr [_messagepointer], offset _aModuleLoadErro ; "Module load error.\r\n$"
		mov	word ptr [_messagepointer+2], ds
		stc
		retn
_readallmoules endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR _start

loc_19D94::				; CODE XREF: _start+18Ej
		mov	_byte_1DE7E, 4
		mov	word ptr [_messagepointer], offset _aListFileNotFou ; "List file not found.\r\n$"
		mov	word ptr [_messagepointer+2], ds
		mov	dx, (offset _buffer_1DB6C+1)
		mov	ax, 3D00h
		int	21h		; DOS -	2+ - OPEN DISK FILE WITH HANDLE
					; DS:DX	-> ASCIZ filename
					; AL = access mode
					; 0 - read
		jb	loc_19256
		push	ax
		call	_callsubx
		pop	bx
		jb	loc_19256

loc_19DB8::				; CODE XREF: _start+D4Fj _start+D56j ...
		mov	dx, offset _buffer_1DB6C

loc_19DBB::				; CODE XREF: _start+D45j
		mov	cx, 1
		mov	ah, 3Fh	; '?'
		push	bx
		push	dx
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		pop	dx
		pop	bx
		mov	di, dx
		jb	short loc_19E03
		or	ax, ax
		jz	short loc_19E03
		inc	dx
		cmp	byte ptr [di], 1Ah
		jz	short loc_19E03
		cmp	byte ptr [di], ' '
		jnb	short loc_19DBB
		mov	byte ptr [di], 0
		cmp	_buffer_1DB6C,	0
		jz	short loc_19DB8
		cmp	_buffer_1DB6C,	';'
		jz	short loc_19DB8
		push	bx
		mov	_word_1DE4E, 2
		call	_find_mods
		jb	short loc_19DF9
		call	_readallmoules

loc_19DF9::				; CODE XREF: _start+D62j
		pop	bx
		cmp	_word_1DE50, 1
		jnz	short loc_19DB8
		jmp	short loc_19E09
; ---------------------------------------------------------------------------

loc_19E03::				; CODE XREF: _start+D36j _start+D3Aj ...
		mov	byte ptr [di], 0
		call	_readallmoules

loc_19E09::				; CODE XREF: _start+D6Fj
		mov	_byte_1DE7E, 0
		jmp	loc_19250
; END OF FUNCTION CHUNK	FOR _start

; =============== S U B	R O U T	I N E =======================================


_read_module proc near	; CODE XREF: _start+424p
					; _readallmoules+3p
		mov	_byte_1DE7E, 3
		mov	word ptr [_messagepointer], offset _aModuleLoadErro ; "Module load error.\r\n$"
		mov	word ptr [_messagepointer+2], ds
		mov	si, dx

loc_19E22::				; CODE XREF: _read_module+16j
		inc	si
		cmp	byte ptr [si-1], 0
		jnz	short loc_19E22
		mov	cx, 0Ch

loc_19E2C::				; CODE XREF: _read_module+2Dj
		dec	si
		cmp	byte ptr [si-1], ':'
		jz	short loc_19E41
		cmp	byte ptr [si-1], '\'
		jz	short loc_19E41
		cmp	si, dx
		jbe	short loc_19E41
		dec	cx
		jnz	short loc_19E2C
		dec	si

loc_19E41::				; CODE XREF: _read_module+20j
					; _read_module+26j ...
		mov	di, offset _aFilename_ext ; "FileName.Ext"
		mov	cx, 0Ch

loc_19E47::				; CODE XREF: _read_module+4Bj
		mov	al, [si]
		inc	si
		or	al, al
		jz	short loc_19E5E
		cmp	al, 'a'
		jb	short loc_19E58
		cmp	al, 'z'
		ja	short loc_19E58
		and	al, 0DFh	; upper	case

loc_19E58::				; CODE XREF: _read_module+3Fj
					; _read_module+43j
		mov	[di], al
		inc	di
		dec	cx
		jnz	short loc_19E47

loc_19E5E::				; CODE XREF: _read_module+3Bj
		mov	ax, ds
		mov	es, ax
		mov	al, ' '
		cld
		rep stosb
		call	_moduleread
		jb	loc_1A042
		mov	_current_patterns, 0
		mov	_byte_1DE84, 0
		call	sub_126A9
		mov	dword ptr [_module_type_txt], eax ; "	"
		xor	ch, ch
		mov	_amount_of_x, cx
		mov	_byte_1DE73, bl
		call	_read_sndsettings
		mov	_outp_freq, bp
		call	sub_1265D
		mov	_byte_1DE78, dl
		mov	al, dh
		and	al, 10h
		shr	al, 4
		mov	_byte_1DE7B, al
		mov	word ptr [_segfsbx_1DE28], si
		mov	word ptr [_segfsbx_1DE28+2], es
		mov	si, di
		mov	di, offset asc_1CC2D ; "			      "
		mov	cx, 30

loc_19EBA::				; CODE XREF: _read_module+B4j
		mov	al, es:[si]
		or	al, al
		jz	short loc_19EC7
		mov	[di], al
		inc	si
		inc	di
		loop	loc_19EBA

loc_19EC7::				; CODE XREF: _read_module+AEj
		mov	cx, 17
		xor	si, si

loc_19ECC::				; CODE XREF: _read_module+C3j
		mov	al, es:[si]
		mov	byte ptr [_a130295211558+si], al	; "13/02/95 21:15:58"
		inc	si
		loop	loc_19ECC
		call	_video_prp_mtr_positn
		xor	edx, edx
		mov	eax, 317
		movzx	ebx, _amount_of_x
		div	ebx
		mov	_volume_1DE34,	eax
		mov	_byte_1DE7C, 0
		call	sub_12EBA
		call	[off_1DE3C]
_read_module endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_keyb_screen_loop proc near		; CODE XREF: _start+6EBp _keyb_screen_loop+5Aj ...
		call	sub_1265D
		mov	_byte_1DE72, ah
		mov	_byte_1DE74, al
		mov	_byte_1DE75, bh
		mov	_byte_1DE76, ch
		mov	ax, -1
		call	_change_volume
		mov	_word_1DE6A, ax
		mov	ax, -1
		call	_getset_amplif
		mov	_word_1DE6C, ax
		call	_get_playsettings
		mov	_flg_play_settings, al
		call	[_offs_draw]
		cmp	_byte_1DE7C, 1
		jz	loc_1A393
		test	_byte_1DE90, 2
		jnz	loc_1A3C5
		test	_byte_1DE90, 1
		jnz	loc_1A3A7
		xor	ax, ax
		xchg	ax, cs:[_key_code]
		or	ax, ax
		jz	short _keyb_screen_loop
		mov	_word_1DE50, ax
		mov	cx, 2
		cmp	ax, 0E04Dh	; gr_right
		jz	_l_1A044
		mov	cx, 10
		cmp	ax, 0E048h	; gr_up

loc_19F6C::
		jz	_l_1A044
		mov	cx, 2
		cmp	ax, 0E04Bh	; gr_left
		jz	loc_1A070
		mov	ecx, 10
		cmp	ax, 0E050h
		jz	loc_1A070	; gr_down
		cmp	al, 4Dh	; 'M'
		jz	_l_right
		cmp	al, 48h	; 'H'
		jz	_l_up
		cmp	al, 4Bh	; 'K'
		jz	_l_left
		cmp	al, 50h	; 'P'
		jz	_l_down
		cmp	al, 4Eh	; 'N'
		jz	_l_plus
		cmp	al, 4Ah	; 'J'
		jz	_l_minus
		cmp	al, 1Ah
		jz	_l_lbracket
		cmp	al, 1Bh
		jz	_l_rbracket
		cmp	al, 3Bh	; ';'
		jz	_l_f1
		cmp	al, 3Ch	; '<'
		jz	_l_f2
		cmp	al, 3Dh	; '='
		jz	_l_f3
		cmp	al, 3Eh	; '>'
		jz	_l_f4
		cmp	al, 3Fh	; '?'
		jz	_l_f5
		cmp	al, 40h	; '@'
		jz	_l_f6
		cmp	al, 42h	; 'B'
		jz	_l_f8
		cmp	al, 43h	; 'C'
		jz	_l_f9
		cmp	al, 44h	; 'D'
		jz	_l_f10
		cmp	al, 57h	; 'W'
		jz	_l_f11
		cmp	al, 58h	; 'X'
		jz	_l_f12
		cmp	al, 26h	; '&'
		jz	_l_l
		cmp	al, 32h	; '2'
		jz	_l_m
		cmp	al, 13h
		jz	_l_r
		cmp	al, 1Fh
		jz	_l_s
		cmp	al, 0Fh
		jz	_l_tab
		cmp	al, 45h	; 'E'
		jz	_l_numlock
		cmp	al, 46h	; 'F'
		jz	_l_scrollock
		cmp	al, 4Fh	; 'O'
		jz	_l_1_end
		cmp	al, 1Ch
		jz	_l_enter
		cmp	al, 1
		jz	_l_esc
		jb	_keyb_screen_loop
		cmp	al, 0Bh
		jbe	loc_1A33E
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

loc_1A042::				; CODE XREF: _read_module+5Bj
		stc
		retn
; ---------------------------------------------------------------------------

_l_1A044::				; CODE XREF: _keyb_screen_loop+65j
					; _keyb_screen_loop:loc_19F6Cj ...
		push	cx
		call	_get_12F7C
		and	bx, 3Fh
		movzx	eax, ax
		shl	eax, 6
		or	al, bl
		inc	eax
		mov	bl, al
		and	bl, 3Fh
		shr	eax, 6
		mov	bh, 1
		call	sub_12F56
		pop	cx
		dec	cx
		jnz	short _l_1A044
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

loc_1A070::				; CODE XREF: _keyb_screen_loop+79j
					; _keyb_screen_loop+86j ...
		push	cx
		call	_get_12F7C
		and	bx, 3Fh
		movzx	eax, ax
		shl	eax, 6
		or	al, bl
		sub	eax, 1
		jb	short loc_1A0A0
		mov	bl, al
		and	bl, 3Fh
		shr	eax, 6
		mov	bh, 1
		call	sub_12F56
		pop	cx
		dec	cx
		jnz	short loc_1A070
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

loc_1A0A0::				; CODE XREF: _keyb_screen_loop+18Aj
		pop	cx
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_up::					; CODE XREF: _keyb_screen_loop+92j
		sub	_byte_1DE84, 1
		jnb	_keyb_screen_loop
		mov	_byte_1DE84, 0
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_down::					; CODE XREF: _keyb_screen_loop+9Ej
		inc	_byte_1DE84
		mov	ax, _amount_of_x
		cmp	_byte_1DE84, al
		jb	_keyb_screen_loop
		dec	al
		mov	_byte_1DE84, al
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_right::				; CODE XREF: _keyb_screen_loop+8Cj
		lfs	bx, _segfsbx_1DE28
		mov	al, 50h	; 'P'
		mul	_byte_1DE84
		add	bx, ax
		mov	cl, 8
		test	cs:[_keyb_switches], 3
		jnz	short loc_1A0E6
		mov	cl, 1

loc_1A0E6::				; CODE XREF: _keyb_screen_loop+1E5j
		mov	al, fs:[bx+3Ah]
		add	al, cl
		cmp	al, 80h	; 'Ä'
		jbe	short loc_1A0F2
		mov	al, 80h	; 'Ä'

loc_1A0F2::				; CODE XREF: _keyb_screen_loop+1F1j
					; _keyb_screen_loop+221j ...
		mov	ch, _byte_1DE84
		call	sub_12AFD
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_left::					; CODE XREF: _keyb_screen_loop+98j
		lfs	bx, _segfsbx_1DE28
		mov	al, 50h	; 'P'
		mul	_byte_1DE84
		add	bx, ax
		mov	cl, 8
		test	cs:[_keyb_switches], 3
		jnz	short loc_1A118
		mov	cl, 1

loc_1A118::				; CODE XREF: _keyb_screen_loop+217j
		mov	al, fs:[bx+3Ah]
		sub	al, cl
		jnb	short loc_1A0F2
		mov	al, 0
		jmp	short loc_1A0F2
; ---------------------------------------------------------------------------

_l_l::					; CODE XREF: _keyb_screen_loop+FEj
		mov	al, 0
		jmp	short loc_1A0F2
; ---------------------------------------------------------------------------

_l_m::					; CODE XREF: _keyb_screen_loop+104j
		mov	al, 64
		jmp	short loc_1A0F2
; ---------------------------------------------------------------------------

_l_r::					; CODE XREF: _keyb_screen_loop+10Aj
		mov	al, 128
		jmp	short loc_1A0F2
; ---------------------------------------------------------------------------

_l_s::					; CODE XREF: _keyb_screen_loop+110j
		mov	al, 166
		jmp	short loc_1A0F2
; ---------------------------------------------------------------------------

_l_plus::					; CODE XREF: _keyb_screen_loop+A4j
		mov	ax, -1
		call	_change_volume
		mov	cx, 32
		test	cs:[_keyb_switches], 3
		jnz	short loc_1A14B
		mov	cx, 2

loc_1A14B::				; CODE XREF: _keyb_screen_loop+249j
		add	ax, cx
		cmp	ax, 256
		jb	short loc_1A155
		mov	ax, 256

loc_1A155::				; CODE XREF: _keyb_screen_loop+253j
		call	_change_volume
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_minus::				; CODE XREF: _keyb_screen_loop+AAj
		mov	ax, -1
		call	_change_volume
		mov	cx, 32
		test	cs:[_keyb_switches], 3
		jnz	short loc_1A174
		mov	cx, 2

loc_1A174::				; CODE XREF: _keyb_screen_loop+272j
		sub	ax, cx
		jnb	short loc_1A17A
		xor	ax, ax

loc_1A17A::				; CODE XREF: _keyb_screen_loop+279j
		call	_change_volume
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_rbracket::				; CODE XREF: _keyb_screen_loop+B6j
		mov	ax, 0FFFFh
		call	_getset_amplif
		mov	cx, 1
		test	cs:[_keyb_switches], 3
		jnz	short loc_1A199
		mov	cx, 0Ah

loc_1A199::				; CODE XREF: _keyb_screen_loop+297j
		add	ax, cx
		cmp	ax, 2500
		jb	short loc_1A1A3
		mov	ax, 2500

loc_1A1A3::				; CODE XREF: _keyb_screen_loop+2A1j
		call	_getset_amplif
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_lbracket::				; CODE XREF: _keyb_screen_loop+B0j
		mov	ax, -1
		call	_getset_amplif
		mov	cx, 1
		test	cs:[_keyb_switches], 3
		jnz	short loc_1A1C2
		mov	cx, 10

loc_1A1C2::				; CODE XREF: _keyb_screen_loop+2C0j
		sub	ax, cx
		jnb	short loc_1A1C9
		mov	ax, 50

loc_1A1C9::				; CODE XREF: _keyb_screen_loop+2C7j
		cmp	ax, 50
		ja	short loc_1A1D1
		mov	ax, 50

loc_1A1D1::				; CODE XREF: _keyb_screen_loop+2CFj
		call	_getset_amplif
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_f1::					; CODE XREF: _keyb_screen_loop+BCj
		call	_f1_help
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_f2::					; CODE XREF: _keyb_screen_loop+C2j
		call	_f2_waves
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_f3::					; CODE XREF: _keyb_screen_loop+C8j
		call	_f3_textmetter
		mov	_byte_1DE85, 0
		test	cs:[_keyb_switches], 3
		jz	_keyb_screen_loop
		mov	_byte_1DE85, 1
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_f4::					; CODE XREF: _keyb_screen_loop+CEj
		cmp	_offs_draw, offset _f4_draw
		jnz	short loc_1A219
		mov	ax, _word_1DE6E
		dec	ax
		add	_current_patterns, ax
		mov	ax, _current_patterns
		cmp	ax, _word_1DE46
		jb	short loc_1A21F

loc_1A219::				; CODE XREF: _keyb_screen_loop+309j
		mov	_current_patterns, 0

loc_1A21F::				; CODE XREF: _keyb_screen_loop+31Aj
		call	_f4_patternnae
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_f5::					; CODE XREF: _keyb_screen_loop+D4j
		call	_f5_graphspectr
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_f6::					; CODE XREF: _keyb_screen_loop+DAj
		call	_f6_undoc
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_f8::					; CODE XREF: _keyb_screen_loop+E0j
		call	[off_1DE42]
		call	_dosexec
		mov	_byte_1DE70, 0FFh
		call	[off_1DE3C]
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_f9::					; CODE XREF: _keyb_screen_loop+E6j
		test	cs:[_keyb_switches], 100b
		jnz	short _l_f11
		call	_get_playsettings
		xor	al, 1
		call	_set_playsettings
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_f10::					; CODE XREF: _keyb_screen_loop+ECj
		test	cs:[_keyb_switches], 100b
		jnz	short _l_f12
		call	_get_playsettings
		xor	al, 2
		call	_set_playsettings
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_f11::					; CODE XREF: _keyb_screen_loop+F2j
					; _keyb_screen_loop+34Ej
		call	_get_playsettings
		xor	al, 4
		call	_set_playsettings
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_f12::					; CODE XREF: _keyb_screen_loop+F8j
					; _keyb_screen_loop+366j
		call	_get_playsettings

loc_1A288::
		xor	al, 10h
		call	_set_playsettings

loc_1A28F::
		xor	byte ptr [_configword+1], 1
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_tab::					; CODE XREF: _keyb_screen_loop+116j
		test	cs:[_keyb_switches], 100b
		jnz	short loc_1A2C1

loc_1A2A0::
		test	cs:[_keyb_switches], 1000b
		jnz	short loc_1A2D1
		test	cs:[_keyb_switches], 11b
		jnz	short loc_1A2E1
		call	_get_playsettings
		xor	al, 8
		call	_set_playsettings

loc_1A2BE::
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

loc_1A2C1::				; CODE XREF: _keyb_screen_loop+3A1j
		mov	cx, 0FFh
		xor	bx, bx
		mov	dx, 7D0Fh
		call	sub_12CAD
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

loc_1A2D1::				; CODE XREF: _keyb_screen_loop+3AAj
		mov	cx, 0FFh
		xor	bx, bx
		mov	dx, 910Fh
		call	sub_12CAD
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

loc_1A2E1::				; CODE XREF: _keyb_screen_loop+3B3j
		mov	cx, 0FFh
		xor	bx, bx
		mov	dx, 960Fh
		call	sub_12CAD
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_numlock::				; CODE XREF: _keyb_screen_loop+11Cj
		test	cs:[_keyb_switches], 100b
		jz	_keyb_screen_loop
		mov	al, 0FFh
		call	_getset_playstate
		mov	ah, al
		mov	al, 1
		cmp	ah, al
		jnz	short loc_1A30D
		mov	al, 0

loc_1A30D::				; CODE XREF: _keyb_screen_loop+40Cj
		call	_getset_playstate
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_scrollock::				; CODE XREF: _keyb_screen_loop+122j
		mov	al, 0FFh
		call	_getset_playstate
		mov	ah, al
		mov	al, 2
		cmp	ah, al
		jnz	short loc_1A326
		mov	al, 0

loc_1A326::				; CODE XREF: _keyb_screen_loop+425j
		call	_getset_playstate
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_1_end::				; CODE XREF: _keyb_screen_loop+128j
		mov	cx, 0FFh
		xor	bx, bx
		mov	dx, 0Dh
		call	sub_12CAD
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

loc_1A33E::				; CODE XREF: _keyb_screen_loop+13Ej
		sub	al, 2
		test	cs:[_keyb_switches], 11b
		jz	short loc_1A34B
		add	al, 10

loc_1A34B::				; CODE XREF: _keyb_screen_loop+44Aj
		test	cs:[_keyb_switches], 100b
		jz	short loc_1A356
		add	al, 20

loc_1A356::				; CODE XREF: _keyb_screen_loop+455j
		cmp	al, byte ptr [_amount_of_x]
		jnb	_keyb_screen_loop
		mov	ch, al
		lfs	bx, _segfsbx_1DE28
		mov	ah, 80
		mul	ah
		add	bx, ax
		xor	byte ptr fs:[bx+17h], 2
		mov	bx, 0FEh ; '˛'
		xor	cl, cl
		xor	dx, dx
		call	sub_12CAD
		jmp	_keyb_screen_loop
; ---------------------------------------------------------------------------

_l_enter::				; CODE XREF: _keyb_screen_loop+12Ej
					; DATA XREF: dseg:stru_244ABo
		call	[_offs_draw]
		call	[_offs_draw2]
		clc
		retn
; ---------------------------------------------------------------------------

_l_esc::					; CODE XREF: _keyb_screen_loop+134j
					; DATA XREF: dseg:stru_244B7o
		mov	_byte_1DE7C, 1
		and	_byte_1DE90, 0FDh

loc_1A393::				; CODE XREF: _keyb_screen_loop+3Bj
		call	[_offs_draw]
		call	[_offs_draw2]
		call	_snd_offx
		call	_memfree_125DA
		clc
		retn
; ---------------------------------------------------------------------------

loc_1A3A7::				; CODE XREF: _keyb_screen_loop+4Dj
		and	_byte_1DE90, 0FEh
;		mov	bx, offset stru_244AB ;	_mystr
		mov	ax, _mousecolumn
		mov	bp, _mouserow
		shr	ax, 3
		shr	bp, 3
		call	_mouse_1C7CF
		jb	_keyb_screen_loop
		jmp	bx
; ---------------------------------------------------------------------------

loc_1A3C5::				; CODE XREF: _keyb_screen_loop+44j
;		mov	bx, offset stru_244B7 ;	_mystr
		mov	ax, _mousecolumn
		mov	bp, _mouserow
		shr	ax, 3
		shr	bp, 3
		call	_mouse_1C7CF
		jb	_keyb_screen_loop
		push	es
		xor	dx, dx
		mov	es, dx
		assume es:nothing
		mov	edx, es:[46Ch]
		cmp	edx, _dword_1DE88
		jz	short loc_1A3F6
		mov	_dword_1DE88, edx
		pop	es
		assume es:nothing
		jmp	bx
; ---------------------------------------------------------------------------

loc_1A3F6::				; CODE XREF: _keyb_screen_loop+4EFj
		pop	es
		jmp	loc_193BC
_keyb_screen_loop endp


; =============== S U B	R O U T	I N E =======================================


_f1_help proc	near		; CODE XREF: _keyb_screen_loop:_l_f1p
					; DATA XREF: dseg:02A6o
		mov	[off_1DE3C], offset _text_init
		mov	_offs_draw, offset _f1_draw
		mov	_offs_draw2, offset _text_init2
		mov	[off_1DE42], offset loc_1A4A6
		call	_text_init
		retn
_f1_help endp


; =============== S U B	R O U T	I N E =======================================


_f2_waves proc near		; CODE XREF: _keyb_screen_loop:_l_f2p
					; DATA XREF: dseg:02A0o
		mov	[off_1DE3C], offset _init_vga_waves
		mov	_offs_draw, offset _f2_draw_waves
		mov	_offs_draw2, offset _f2_draw_waves2
		mov	[off_1DE42], offset _init_vga_waves
		call	_init_vga_waves
		retn
_f2_waves endp


; =============== S U B	R O U T	I N E =======================================


_f3_textmetter proc near	; CODE XREF: _keyb_screen_loop:_l_f3p
					; DATA XREF: dseg:off_1CA8Eo
		mov	[off_1DE3C], offset _text_init
		mov	_offs_draw, offset _f3_draw
		mov	_offs_draw2, offset _text_init2
		mov	[off_1DE42], offset loc_1A4A6
		call	_text_init
		retn
_f3_textmetter endp


; =============== S U B	R O U T	I N E =======================================


_f4_patternnae proc near	; CODE XREF: _keyb_screen_loop:loc_1A21Fp
					; DATA XREF: dseg:02A4o
		mov	[off_1DE3C], offset _text_init
		mov	_offs_draw, offset _f4_draw
		mov	_offs_draw2, offset _text_init2
		mov	[off_1DE42], offset loc_1A4A6
		call	_text_init
		retn
_f4_patternnae endp


; =============== S U B	R O U T	I N E =======================================


_f5_graphspectr proc near	; CODE XREF: _keyb_screen_loop:_l_f5p
					; DATA XREF: dseg:02A2o
		mov	[off_1DE3C], offset _init_f5_spectr
		mov	_offs_draw, offset _f5_draw_spectr
		mov	_offs_draw2, offset _f5_draw_spectr
		mov	[off_1DE42], offset _init_f5_spectr
		call	_init_f5_spectr
		retn
_f5_graphspectr endp


; =============== S U B	R O U T	I N E =======================================


_f6_undoc proc near		; CODE XREF: _keyb_screen_loop:_l_f6p
					; DATA XREF: dseg:02A8o
		mov	[off_1DE3C], offset _text_init
		mov	_offs_draw, offset _f6_draw
		mov	_offs_draw2, offset _text_init2
		mov	[off_1DE42], offset loc_1A4A6
		call	_text_init
		retn
_f6_undoc endp


; =============== S U B	R O U T	I N E =======================================


_text_init proc near		; CODE XREF: _f1_help+18p
					; _f3_textmetter+18p ...
		call	_text_init2
		retn
_text_init endp

; ---------------------------------------------------------------------------

loc_1A4A6::				; DATA XREF: _f1_help+12o
					; _f3_textmetter+12o ...
		call	_text_init2
		retn

; =============== S U B	R O U T	I N E =======================================


_text_init2 proc near		; CODE XREF: _text_initp
					; seg001:loc_1A4A6p
					; DATA XREF: ...

; FUNCTION CHUNK AT 14A2 SIZE 0000026B BYTES

		cmp	_byte_1DE86, 1
		jz	short loc_1A4F2
		cmp	_amount_of_x, 0Ah
		jbe	short loc_1A4F2
		jmp	loc_1A5AB
_text_init2 endp


; =============== S U B	R O U T	I N E =======================================


_setvideomode proc near	; CODE XREF: _start:loc_192F7p
					; _text_init2:loc_1A4F2p
		cmp	_byte_1DE70, 0
		jz	short locret_1A4F1
		cmp	_byte_1DE70, 1
		jz	short locret_1A4F1
		mov	ax, 3
		cmp	_byte_1DE70, 2
		jnz	short loc_1A4D5
		or	al, 80h

loc_1A4D5::				; CODE XREF: _setvideomode+16j
		int	10h		; - VIDEO - SET	VIDEO MODE
					; AL = mode
		call	_txt_blinkingoff
		cmp	_byte_1DE86, 1
		jz	short loc_1A4E8
		mov	ax, 1111h
		xor	bl, bl
		int	10h		; - VIDEO - TEXT-MODE CHARACTER	GENERATOR FUNCTIONS (PS, EGA, VGA)
					; AL = 00h, 10h: load user-specified patterns
					; AL = 01h, 11h: load ROM monochrome patterns (8 by 14)
					; AL = 02h, 12h: load ROM 8 by 8 double-dot patterns
					; AL = 03h: set	block specifier

loc_1A4E8::				; CODE XREF: _setvideomode+24j
		mov	dx, 1D00h
		xor	bh, bh
		mov	ah, 2
		int	10h		; - VIDEO - SET	CURSOR POSITION
					; DH,DL	= row, column (0,0 = upper left)
					; BH = page number

locret_1A4F1::				; CODE XREF: _setvideomode+5j
					; _setvideomode+Cj
		retn
_setvideomode endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR _text_init2

loc_1A4F2::				; CODE XREF: _text_init2+5j
					; _text_init2+Cj
		call	_setvideomode
		cmp	_byte_1DE86, 1
		jz	short loc_1A55B
		mov	_word_1DE6E, 0Ah
		mov	eax, _videomempointer
		add	ax, 36*80
		mov	_videopoint_shiftd, eax
		cmp	_byte_1DE70, 0
		jz	short loc_1A545
		cmp	_byte_1DE70, 1
		jz	short loc_1A529
		mov	cx, 0
		mov	dx, 1B4Fh
		mov	bl, 78h	; 'x'
		mov	ax, 7F03h
		call	_draw_frame

loc_1A529::				; CODE XREF: _text_init2+6Fj
		mov	cx, 1103h
		mov	dx, 1A25h
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame
		mov	cx, 112Ah
		mov	dx, 1A4Ch
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame

loc_1A545::				; CODE XREF: _text_init2+68j
		mov	cx, 501h
		mov	dx, 104Eh
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame
		mov	_byte_1DE70, 0
		jmp	loc_1A628
; ---------------------------------------------------------------------------

loc_1A55B::				; CODE XREF: _text_init2+50j
		mov	_word_1DE6E, 7
		mov	eax, _videomempointer
		add	ax, 30*80
		mov	_videopoint_shiftd, eax
		mov	cx, 0
		mov	dx, 184Fh
		mov	bl, 78h	; 'x'
		mov	ax, 7F03h
		call	_draw_frame
		mov	cx, 0E03h
		mov	dx, 1725h
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame
		mov	cx, 0E2Ah
		mov	dx, 174Ch
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame
		mov	cx, 501h
		mov	dx, 0D4Eh
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame
		mov	_byte_1DE70, 0
		jmp	short loc_1A628
; ---------------------------------------------------------------------------

loc_1A5AB::				; CODE XREF: _text_init2+Ej
		cmp	_byte_1DE70, 2
		jz	short loc_1A61A
		mov	ax, 3
		cmp	_byte_1DE70, 1
		jz	short loc_1A5C3
		cmp	_byte_1DE70, 0
		jnz	short loc_1A5C5

loc_1A5C3::				; CODE XREF: _text_init2+110j
		or	al, 80h

loc_1A5C5::				; CODE XREF: _text_init2+117j
		int	10h		; - VIDEO - SET	VIDEO MODE
					; AL = mode
		call	_txt_blinkingoff
		mov	ax, 1112h
		xor	bl, bl
		int	10h		; - VIDEO - TEXT-MODE CHARACTER	GENERATOR FUNCTIONS (PS, EGA, VGA)
					; AL = 00h, 10h: load user-specified patterns
					; AL = 01h, 11h: load ROM monochrome patterns (8 by 14)
					; AL = 02h, 12h: load ROM 8 by 8 double-dot patterns
					; AL = 03h: set	block specifier
		mov	dx, 3300h
		xor	bh, bh
		mov	ah, 2
		int	10h		; - VIDEO - SET	CURSOR POSITION
					; DH,DL	= row, column (0,0 = upper left)
					; BH = page number
		mov	_byte_1DE70, 2
		mov	_word_1DE6E, 20h ; ' '
		mov	eax, _videomempointer
		add	ax, 6400
		mov	_videopoint_shiftd, eax
		mov	cx, 0
		mov	dx, 314Fh
		mov	bl, 78h	; 'x'
		mov	ax, 7F03h
		call	_draw_frame
		mov	cx, 2703h
		mov	dx, 3025h
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame
		mov	cx, 272Ah
		mov	dx, 304Ch
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame

loc_1A61A::				; CODE XREF: _text_init2+106j
		mov	cx, 501h
		mov	dx, 264Eh
		mov	bl, 7Fh	; ''
		mov	ax, 7803h
		call	_draw_frame

loc_1A628::				; CODE XREF: _text_init2+AEj
					; _text_init2+FFj
		call	_txt_draw_top_title
		mov	ax, ds
		mov	bx, offset _buffer_1 ; 2800h
		shr	bx, 4
		add	ax, bx
		mov	_buffer_1seg, ax
		movzx	si, _snd_card_type
		cmp	si, 0Ah
		jb	short loc_1A645
		mov	si, 0Ah

loc_1A645::				; CODE XREF: _text_init2+196j
		shl	si, 1
		mov	si, [_table_sndcrdname+si] ; str
		les	di, _videopoint_shiftd
		add	di, 58h	; 'X'   ; videoptr
		mov	ah, 7Fh	; ''   ; color
		call	_put_message
		cmp	_snd_card_type, 0
		jnz	short loc_1A687
		push	di
		push	es
		call	sub_1265D
		pop	es
		pop	di
		mov	word ptr es:[di], 7F20h
		add	di, 2		; videoptr
		movzx	si, dh
		and	si, 1100000b
		shr	si, 3
		add	si, offset _a256	; str
		mov	ah, 7Fh	; ''   ; color
		call	_put_message
		mov	si, offset _aKb	; str
		call	_put_message

loc_1A687::				; CODE XREF: _text_init2+1B2j
		cmp	_snd_card_type, 0Ah
		jz	short loc_1A6B7
		mov	si, (offset _buffer_1DC6C+2)

loc_1A691::				; ' )'
		mov	word ptr [_buffer_1DC6C], 2820h
		xor	dx, dx

loc_1A699::
		mov	ax, _outp_freq
		mov	cx, 1000
		div	cx

loc_1A6A1::
		call	_my_u8toa10
		mov	dword ptr [si],	297A486Bh ; 'kHz('
		mov	byte ptr [si+4], 0
		mov	si, offset _buffer_1DC6C	; str
		mov	ah, 7Fh	; ''   ; color
		call	_put_message

loc_1A6B7::				; CODE XREF: _text_init2+1E2j
		mov	al, 78h	; 'x'
		cmp	_byte_1DE7B, 1
		jnz	short loc_1A6C2
		mov	al, 7Ch	; '|'

loc_1A6C2::				; CODE XREF: _text_init2+214j
		mov	_byte_1CCEB, al
		les	di, _videopoint_shiftd
		mov	si, offset _bottom_menu ; str
		call	_write_scr
		call	sub_126A9
		mov	word ptr [_dword_1DE2C],	si
		mov	word ptr [_dword_1DE2C+2], es
		push	cx
		mov	si, offset _buffer_1DC6C
		mov	al, ch
		push	bx
		call	_my_u8toa10
		pop	bx
		mov	byte ptr [si], '/'
		inc	si
		movzx	ax, bh
		mov	_word_1DE46, ax
		call	_my_u8toa10
		mov	dword ptr [si],	'   '
		mov	si, offset _buffer_1DC6C	; str
		les	di, _videopoint_shiftd
		add	di, 2AAh	; videoptr
		mov	ah, 7Fh	; ''   ; color
		call	_put_message
		pop	cx
		mov	si, offset _buffer_1DC6C
		mov	al, cl
		call	_my_u8toa10
		mov	dword ptr [si],	'   '
		sub	si, cx		; str
		les	di, _videopoint_shiftd
		add	di, 20Ah	; videoptr
		mov	ah, 7Fh	; ''   ; color
		call	_put_message
		mov	bx, 0FFFFh
		mov	ah, 48h
		int	21h		; DOS -	2+ - ALLOCATE MEMORY
					; BX = number of 16-byte paragraphs desired
		mov	ax, bx
		shr	ax, 6
		mov	si, offset _buffer_1DC6C
		call	_my_u16toa10
		mov	dword ptr [si],	20424Bh	; 'KB '
		cmp	si, (offset _buffer_1DC6C+2)
		jb	short loc_1A74D
		mov	byte ptr [si+2], 0

loc_1A74D::				; CODE XREF: _text_init2+29Dj
		sub	si, cx		; str
		les	di, _videopoint_shiftd
		add	di, 12Eh	; videoptr
		mov	ah, 7Fh	; ''   ; color
		call	_put_message
		retn
; END OF FUNCTION CHUNK	FOR _text_init2

; =============== S U B	R O U T	I N E =======================================


_txt_draw_top_title proc near	; CODE XREF: _start+201p _start+27Bp ...
		mov	cx, 102h
		mov	dx, 44Dh
		mov	bl, 78h	; 'x'
		mov	ax, 7F03h
		call	_draw_frame
		les	di, _videomempointer
		mov	si, offset _atop_title ;	str
		call	_write_scr
		retn
_txt_draw_top_title endp


; =============== S U B	R O U T	I N E =======================================


_txt_draw_bottom proc	near	; CODE XREF: seg001:_f3_drawp
					; seg001:_f4_drawp ...
		mov	si, offset _buffer_1DC6C
		mov	eax, '    '
		mov	[si], eax
		mov	[si+4],	eax
		mov	[si+8],	eax
		mov	[si+0Ch], al
		mov	byte ptr [si+0Dh], 0
		mov	al, _byte_1DE75
		call	_my_u8toa10
		mov	dword ptr [si],	20746120h ; ' at '
		add	si, 4
		mov	al, _byte_1DE76
		call	_my_u8toa10
		mov	word ptr [si], 7062h ; bp
		mov	byte ptr [si+2], 'm'
		mov	si, offset _buffer_1DC6C	; str
		les	di, _videopoint_shiftd
		add	di, 48Ah	; videoptr
		mov	ah, 7Fh	; ''   ; color
		call	_put_message
		mov	si, offset _aPal	; "(PAL) "
		test	_flg_play_settings, 8
		jnz	short loc_1A7CC
		mov	si, offset _aNtsc ; str

loc_1A7CC::				; CODE XREF: _txt_draw_bottom+51j
		les	di, _videopoint_shiftd
		add	di, 476h	; videoptr
		mov	ah, 7Eh	; '~'   ; color
		call	_put_message
		mov	si, offset _buffer_1DC6C
		mov	al, _byte_1DE72
		inc	al
		call	_my_u8toa10
		mov	byte ptr [si], '/'
		inc	si
		mov	al, _byte_1DE73
		call	_my_u8toa10
		mov	dword ptr [si],	'   '
		mov	si, offset _buffer_1DC6C	; str
		les	di, _videopoint_shiftd
		add	di, 34Ah	; videoptr
		mov	ah, 7Fh	; ''   ; color
		call	_put_message
		mov	si, offset _buffer_1DC6C
		mov	al, _byte_1DE74
		inc	al
		call	_my_u8toa10
		mov	dword ptr [si],	2034362Fh ; /64
		mov	word ptr [si+4], ' '
		sub	si, cx		; str
		les	di, _videopoint_shiftd
		add	di, 3EAh	; videoptr
		mov	ah, 7Fh	; ''   ; color
		call	_put_message
		les	di, _videopoint_shiftd
		add	di, 198h
		mov	ah, 7Ch	; '|'
		test	_flg_play_settings, 1
		jnz	short loc_1A83E
		mov	ah, 78h	; 'x'

loc_1A83E::				; CODE XREF: _txt_draw_bottom+C4j
		mov	al, 0FEh ; '˛'
		mov	es:[di], ax
		les	di, _videopoint_shiftd
		add	di, 238h
		mov	ah, 7Ch	; '|'
		test	_flg_play_settings, 2
		jnz	short loc_1A856
		mov	ah, 78h	; 'x'

loc_1A856::				; CODE XREF: _txt_draw_bottom+DCj
		mov	al, 0FEh ; '˛'
		mov	es:[di], ax
		les	di, _videopoint_shiftd
		add	di, 2D8h
		mov	ah, 7Ch	; '|'
		test	_flg_play_settings, 4
		jnz	short loc_1A86E
		mov	ah, 78h	; 'x'

loc_1A86E::				; CODE XREF: _txt_draw_bottom+F4j
		mov	al, 0FEh ; '˛'
		mov	es:[di], ax
		les	di, _videopoint_shiftd
		add	di, 378h	; interp text offset
		mov	ah, 7Ch	; '|'
		test	_flg_play_settings, 10h
		jnz	short loc_1A886
		mov	ah, 78h	; 'x'

loc_1A886::				; CODE XREF: _txt_draw_bottom+10Cj
		mov	al, 0FEh ; '˛'
		mov	es:[di], ax
		mov	si, offset _buffer_1DC6C
		imul	ax, _word_1DE6A, 100
		mov	al, ah
		call	_my_u8toa10
		mov	dword ptr [si],	202025h	; '%  '
		sub	si, cx		; str
		les	di, _videopoint_shiftd
		add	di, 43Ah	; videoptr
		mov	ah, 7Fh	; ''   ; color
		call	_put_message
		mov	si, offset _buffer_1DC6C
		mov	ax, _word_1DE6C
		call	_my_u16toa10
		mov	dword ptr [si],	202025h	; '%  '
		sub	si, cx		; str
		les	di, _videopoint_shiftd
		add	di, 4DAh	; videoptr
		mov	ah, 7Fh	; ''   ; color
		call	_put_message
		mov	al, 0FFh
		call	_getset_playstate
		movzx	si, al
		shl	si, 2
		add	si, offset _aPlaypausloop ; "PlayPausLoop"
		les	di, _videopoint_shiftd
		add	di, 0FCh ; '¸'
		mov	ah, 7Eh	; '~'
		mov	cx, 4

loc_1A8EB::				; CODE XREF: _txt_draw_bottom+17Fj
		mov	al, [si]
		mov	es:[di], ax
		inc	si
		add	di, 2
		dec	cx
		jnz	short loc_1A8EB
		retn
_txt_draw_bottom endp

; ---------------------------------------------------------------------------

_f3_draw  proc near			; DATA XREF: _f3_textmetter+6o
		call	_txt_draw_bottom
		cmp	_byte_1DE85, 1
		jz	short loc_1A913
		mov	es, _buffer_1seg
		assume es:nothing
		xor	di, di
		mov	cx, 50h	; 'P'
		mov	ax, 4001h
		call	_volume_prep

loc_1A913::				; CODE XREF: seg001:18B0j
		mov	_buffer_2seg, offset _buffer_1 ; 2800h
		lfs	bx, _segfsbx_1DE28
		les	di, _videomempointer
		assume es:nothing
		add	di, 3C4h
		mov	cx, _amount_of_x
		cmp	cx, _word_1DE6E
		jbe	short loc_1A934
		mov	cx, _word_1DE6E

loc_1A934::				; CODE XREF: seg001:18DEj
		inc	_byte_1DE71
		xor	ax, ax

loc_1A93A::				; CODE XREF: seg001:1AB8j
		push	ax
		push	cx
		push	di
		mov	dl, al
		add	al, '1'
		cmp	al, '9'
		jbe	short loc_1A947
		add	al, 7

loc_1A947::				; CODE XREF: seg001:18F3j
		mov	ah, 1Eh
		cmp	dl, _byte_1DE84
		jz	short loc_1A951
		mov	ah, 7Eh	; '~'

loc_1A951::				; CODE XREF: seg001:18FDj
		mov	es:[di+2], ax
		mov	al, ' '
		mov	es:[di], ax
		mov	es:[di+4], ax
		add	di, 6
		movzx	si, byte ptr fs:[bx+35h]
		mov	al, ' '
		test	si, 0Fh
		jz	short loc_1A975
		mov	ax, si
		shr	al, 4
		add	al, '0'

loc_1A975::				; CODE XREF: seg001:191Cj
		mov	ah, 7Fh	; ''
		mov	es:[di+4], ax
		and	si, 0Fh
		shl	si, 1
		mov	al, byte ptr [_notes+si]	; "  C-C#D-D#E-F-F#G-G#A-A#B-"
		mov	es:[di], ax
		mov	al, byte ptr [(_notes+1)+si]
		mov	es:[di+2], ax
		add	di, 8
		test	byte ptr fs:[bx+17h], 1
		jnz	short loc_1A9AD
		mov	si, offset _aMute ; "<Mute>		  "
		mov	ah, 7Fh	; ''
		test	byte ptr fs:[bx+17h], 2
		jnz	short loc_1A9A8

loc_1A9A5::				; CODE XREF: seg001:1964j
		mov	si, offset asc_1DA00 ; "		      "

loc_1A9A8::				; CODE XREF: seg001:1953j
		call	_put_message
		jmp	short loc_1A9C2
; ---------------------------------------------------------------------------

loc_1A9AD::				; CODE XREF: seg001:1947j
		movzx	eax, byte ptr fs:[bx+2]
		dec	ax
		js	short loc_1A9A5
		shl	ax, 6
		mov	si, ax
		add	si, word ptr [_dword_1DE2C]
		call	_txt_1ABAE

loc_1A9C2::				; CODE XREF: seg001:195Bj
		add	di, 2
		cmp	_byte_1DE85, 1
		jnz	short loc_1AA1A
		push	di
		push	es
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	di, offset _buffer_1 ; 2800h
		cld
		movzx	eax, byte ptr fs:[bx+2]
		mov	bp, 2
		call	_my_u32toa_fill
		mov	bp, 4
		movzx	eax, byte ptr fs:[bx+22h]
		call	_my_u32toa_fill
		mov	bp, 7
		movzx	eax, word ptr fs:[bx+1Eh]
		call	_my_u32toa_fill
		mov	ax, ds
		mov	es, ax
		mov	eax, '    '
		mov	cx, 4
		rep stosd
		mov	byte ptr [di], 0
		pop	es
		assume es:nothing
		pop	di
		mov	si, offset _buffer_1 ; 2800h
		mov	ah, 7Eh	; '~'
		call	_put_message

loc_1AA17::
		jmp	loc_1AACB
; ---------------------------------------------------------------------------

loc_1AA1A::				; CODE XREF: seg001:197Aj
		cmp	_snd_card_type, 0Ah
		jz	short loc_1AA62
		mov	si, _buffer_2seg
		mov	cx, 50h	; 'P'
		xor	eax, eax
		xor	edx, edx

loc_1AA2E::				; CODE XREF: seg001:19EBj
		mov	dl, [si]
		or	dl, dl
		jns	short loc_1AA36
		neg	dl

loc_1AA36::				; CODE XREF: seg001:19E2j
		inc	si
		add	eax, edx
		dec	cx
		jnz	short loc_1AA2E
		mov	_buffer_2seg, si
		xor	edx, edx
		div	_volume_1DE34
		cmp	al, 60
		jb	short loc_1AA4F
		mov	al, 60

loc_1AA4F::				; CODE XREF: seg001:19FBj
		cmp	_byte_1DE83, 0
		jz	short loc_1AA5C
		cmp	al, fs:[bx+1Ah]
		jb	short loc_1AA73

loc_1AA5C::				; CODE XREF: seg001:1A04j
		mov	fs:[bx+1Ah], al
		jmp	short loc_1AA88
; ---------------------------------------------------------------------------

loc_1AA62::				; CODE XREF: seg001:19CFj
		test	byte ptr fs:[bx+17h], 1
		jz	short loc_1AA73
		mov	al, fs:[bx+22h]
		mov	fs:[bx+1Ah], al
		jmp	short loc_1AA88
; ---------------------------------------------------------------------------

loc_1AA73::				; CODE XREF: seg001:1A0Aj seg001:1A17j
					; DATA XREF: ...
		and	_byte_1DE71, 1Fh
		jnz	short loc_1AA88
		mov	al, _byte_1DE83
		sub	fs:[bx+1Ah], al
		jns	short loc_1AA88
		mov	byte ptr fs:[bx+1Ah], 0

loc_1AA88::				; CODE XREF: seg001:1A10j seg001:1A21j ...
		movzx	cx, byte ptr fs:[bx+1Ah]
		shr	cx, 1
		mov	dx, 30
		sub	dx, cx
		jcxz	short _volume_endstr
		mov	si, cx
		mov	cx, 0Dh
		cmp	si, cx
		ja	short _volume_medium
		mov	cx, si

_volume_medium::				; CODE XREF: seg001:1A4Dj
		mov	ax, 7A16h
		cld
		rep stosw
		sub	si, 0Dh
		jbe	short _volume_endstr
		mov	cx, 0Ch
		cmp	si, cx
		ja	short _volume_higher
		mov	cx, si

_volume_higher::				; CODE XREF: seg001:1A61j
		mov	ah, 7Eh	; '~'
		rep stosw
		sub	si, 0Ch
		jbe	short _volume_endstr
		mov	cx, si
		mov	ah, 7Ch	; '|'
		rep stosw

_volume_endstr::				; CODE XREF: seg001:1A44j seg001:1A5Aj ...
		mov	cx, dx
		mov	ax, 7816h
		rep stosw

loc_1AACB::				; CODE XREF: seg001:loc_1AA17j
		pop	di
		push	di
		add	di, 7Ah	; 'z'
		mov	si, offset asc_1D6E0 ; "	       "
		mov	al, fs:[bx+0Ah]
		cmp	al, 1Dh
		jz	short loc_1AB0D
		cmp	al, 0Eh
		jnz	short loc_1AAF0
		mov	si, offset _aSetLoopPoint ; "Set	Loop Point "
		mov	al, fs:[bx+0Bh]
		cmp	al, 60h	; '`'
		jz	short loc_1AAF7
		mov	si, offset _aSetFilter ;	"Set Filter	"
		shr	al, 4

loc_1AAF0::				; CODE XREF: seg001:1A8Dj
		xor	ah, ah
		shl	ax, 4
		add	si, ax

loc_1AAF7::				; CODE XREF: seg001:1A98j seg001:1AF2j ...
		mov	ah, 7Eh	; '~'
		call	_put_message
		pop	di
		pop	cx
		pop	ax
		add	di, 0A0h ; '†'
		add	bx, 50h	; 'P'
		inc	ax
		dec	cx
		jnz	loc_1A93A
		retn
_f3_draw endp
; ---------------------------------------------------------------------------

loc_1AB0D proc near				; CODE XREF: seg001:1A89j
		mov	si, offset _aArpeggio ; "Arpeggio       "
		mov	al, fs:[bx+0Bh]
		cmp	al, 37h	; '7'
		jz	short loc_1AB5D	; min
		cmp	al, 47h	; 'G'
		jz	short loc_1AB67	; maj
		cmp	al, 48h	; 'H'
		jz	short loc_1AB53	; puls
		xor	cl, cl
		call	sub_1AB8C
		mov	[si+9],	ax
		mov	cl, fs:[bx+0Bh]
		shr	cl, 4
		call	sub_1AB8C
		mov	[si+0Bh], ax
		mov	cl, fs:[bx+0Bh]
		and	cl, 0Fh
		call	sub_1AB8C
		mov	[si+0Dh], ax
		jmp	short loc_1AAF7
; ---------------------------------------------------------------------------

loc_1AB44::				; CODE XREF: seg001:1B26j seg001:1B2Aj
		mov	dword ptr [si+9], '    '
		mov	word ptr [si+0Dh], '  '
		jmp	short loc_1AAF7
; ---------------------------------------------------------------------------

loc_1AB53::				; CODE XREF: seg001:1ACEj
		mov	dword ptr [si+0Bh], 73756C70h ;	puls
		jmp	short loc_1AB6F
; ---------------------------------------------------------------------------

loc_1AB5D::				; CODE XREF: seg001:1AC6j
		mov	dword ptr [si+0Bh], 206E696Dh ;	min
		jmp	short loc_1AB6F
; ---------------------------------------------------------------------------

loc_1AB67::				; CODE XREF: seg001:1ACAj
		mov	dword ptr [si+0Bh], 206A616Dh ;	maj

loc_1AB6F::				; CODE XREF: seg001:1B0Bj seg001:1B15j
		mov	al, fs:[bx+35h]
		and	ax, 0Fh
		jz	short loc_1AB44
		cmp	al, 0Ch
		ja	short loc_1AB44
		shl	ax, 1
		push	si
		mov	si, ax
		mov	ax, word ptr [_notes+si]	; "  C-C#D-D#E-F-F#G-G#A-A#B-"
		pop	si
		mov	[si+9],	ax
		jmp	loc_1AAF7

loc_1AB0D endp
; =============== S U B	R O U T	I N E =======================================


sub_1AB8C proc near		; CODE XREF: seg001:1AD2p seg001:1ADFp ...
		mov	al, fs:[bx+35h]
		and	ax, 0Fh
		add	al, cl
		cmp	al, 0Ch
		jbe	short loc_1AB9B
		sub	al, 0Ch

loc_1AB9B::				; CODE XREF: sub_1AB8C+Bj
		shl	ax, 1
		push	si
		mov	si, ax
		mov	ax, word ptr [_notes+si]	; "  C-C#D-D#E-F-F#G-G#A-A#B-"
		pop	si
		cmp	ah, 2Dh	; '-'
		jz	short loc_1ABAB
		retn
; ---------------------------------------------------------------------------

loc_1ABAB::				; CODE XREF: sub_1AB8C+1Cj
		mov	ah, 20h	; ' '
		retn
sub_1AB8C endp


; =============== S U B	R O U T	I N E =======================================


_txt_1ABAE proc near		; CODE XREF: seg001:196Fp
		mov	ah, 7Bh	; '{'
		mov	cx, 16h

loc_1ABB3::				; CODE XREF: _txt_1ABAE+10j
		mov	al, fs:[si]
		mov	es:[di], ax
		inc	si
		add	di, 2
		dec	cx
		jnz	short loc_1ABB3
		retn
_txt_1ABAE endp

; ---------------------------------------------------------------------------

_f4_draw proc near				; DATA XREF: _keyb_screen_loop:_l_f4o
					; _f4_patternnae+6o
		call	_txt_draw_bottom
		les	di, _videomempointer
		add	di, 3C6h
		mov	si, offset _aSamplename ; "# SampleName	 "
		mov	ah, 7Eh	; '~'
		call	_text_1BF69
		mov	di, word ptr [_videomempointer]
		add	di, 464h
		lfs	bx, _dword_1DE2C
		mov	bp, _current_patterns
		imul	ax, bp,	40h
		add	bx, ax
		mov	dl, byte ptr [_word_1DE6E]
		dec	dl

loc_1ABF0::				; CODE XREF: seg001:1CA1j
		cmp	bp, _word_1DE46
		jnb	locret_1ACF5
		push	bx
		push	dx
		push	bp
		push	di
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	di, offset _buffer_1 ; 2800h
		cld
		movzx	eax, bp
		inc	ax
		mov	bp, 2
		call	_my_pnt_u32toa_fill
		mov	dword ptr [di],	7B0220h	;  {
		add	di, 3
		mov	si, bx
		mov	cx, 8
		cld
		rep movs dword	ptr es:[di], dword ptr fs:[si]
		test	byte ptr fs:[bx+3Ch], 1
		jnz	short loc_1AC35
		mov	si, offset unk_1D6C3
		mov	cx, 9
		rep movsb
		jmp	loc_1ACD2
; ---------------------------------------------------------------------------

loc_1AC35::				; CODE XREF: seg001:1BD8j
		mov	eax, fs:[bx+20h]
		mov	bp, 7
		call	_my_pnt_u32toa_fill
		movzx	eax, byte ptr fs:[bx+3Dh]
		mov	bp, 3
		call	_my_pnt_u32toa_fill
		mov	eax, 363120h	; ' 16'
		test	byte ptr fs:[bx+3Ch], 4
		jnz	short loc_1AC5F
		mov	eax, 382020h	; '  8'

loc_1AC5F::				; CODE XREF: seg001:1C07j
		mov	[di], eax
		mov	al, 1Dh
		test	byte ptr fs:[bx+3Ch], 10h
		jnz	short loc_1AC6D
		mov	al, ' '

loc_1AC6D::				; CODE XREF: seg001:1C19j
		mov	[di+3],	al
		add	di, 4
		movzx	eax, word ptr fs:[bx+36h]
		mov	bp, 6
		call	_my_pnt_u32toa_fill
		mov	dword ptr [di],	7A487E02h ;  H
		add	di, 4
		mov	dword ptr [di],	7F0220h	;  square
		add	di, 3
		mov	eax, '    '
		mov	ah, fs:[bx+3Eh]
		and	ah, 0Fh
		test	ah, 8
		jz	short loc_1ACAC
		mov	al, '-'
		neg	ah
		add	ah, 10h

loc_1ACAC::				; CODE XREF: seg001:1C53j
		or	ah, '0'
		mov	[di], eax
		add	di, 4
		test	byte ptr fs:[bx+3Ch], 8
		jz	short loc_1ACD2
		mov	eax, fs:[bx+24h]
		mov	bp, 7
		call	_my_pnt_u32toa_fill
		mov	eax, fs:[bx+2Ch]
		mov	bp, 7
		call	_my_pnt_u32toa_fill

loc_1ACD2::				; CODE XREF: seg001:1BE2j seg001:1C6Aj
		mov	byte ptr [di], 0
		pop	di
		push	di
		mov	es, word ptr [_videomempointer+2]
		assume es:nothing
		mov	si, offset _buffer_1 ; 2800h
		mov	ah, 7Fh	; ''
		call	_text_1BF69
		pop	di
		pop	bp
		pop	dx
		pop	bx
		add	bx, 40h	; '@'
		inc	bp
		add	di, 0A0h ; '†'
		dec	dl
		jnz	loc_1ABF0

locret_1ACF5::				; CODE XREF: seg001:1BA4j
		retn
_f4_draw  endp
; =============== S U B	R O U T	I N E =======================================


; void __usercall _my_pnt_u32toa_fill(int count<ebp>)
_my_pnt_u32toa_fill proc near	; CODE XREF: seg001:1BBCp seg001:1BEDp ...
		mov	word ptr [di], 7F02h
		add	di, 2
_my_pnt_u32toa_fill endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


; void __usercall _my_u32toa_fill(int count<ebp>)
_my_u32toa_fill proc near	; CODE XREF: _filelist_198B8+82p
					; _filelist_198B8+92p ...
		mov	si, offset _buffer_1DC6C
		push	bx
		push	di
		push	bp
		call	_my_u32toa10
		pop	bp
		pop	di
		pop	bx
		cmp	cx, bp
		jb	short loc_1AD0F
		mov	cx, bp

loc_1AD0F::				; CODE XREF: _my_u32toa_fill+Ej
		sub	si, cx
		mov	dx, cx
		neg	cx
		add	cx, bp
		mov	al, ' '
		cld
		rep stosb
		mov	cx, dx
		rep movsb
		retn
_my_u32toa_fill endp

; ---------------------------------------------------------------------------

_f1_draw proc near				; DATA XREF: _f1_help+6o
		call	_txt_draw_bottom
		les	di, _videomempointer
		assume es:nothing
		mov	si, offset _f1_help_text
		call	_write_scr
		retn

_f1_draw endp
; =============== S U B	R O U T	I N E =======================================


_init_vga_waves proc near	; CODE XREF: _f2_waves+18p
					; DATA XREF: _f2_waveso	...

; FUNCTION CHUNK AT 1DDD SIZE 0000008D BYTES

		cmp	_byte_1DE70, 3
		jz	loc_1AEB2
		mov	_byte_1DE70, 3
		mov	ax, 12h		; VGA 640x480, 16-color; 80 bytes per line; 1 byte-8 pixels
		int	10h		; - VIDEO - SET	VIDEO MODE
					; AL = mode
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	dx, offset _palette_24404
		mov	ax, 1002h
		int	10h		; - VIDEO - SET	ALL PALETTE REGISTERS (Jr, PS, TANDY 1000, EGA,	VGA)
					; ES:DX	-> 17-byte palette register list
		mov	dx, offset _vga_palette
		mov	cx, 10h
		xor	bx, bx
		mov	ax, 1012h
		int	10h		; - VIDEO - SET	BLOCK OF DAC REGISTERS (EGA, VGA/MCGA)
					; BX = starting	color register,	CX = number of registers to set
					; ES:DX	-> table of 3*CX bytes where each 3 byte group represents one
					; byte each of red, green and blue (0-63)
		mov	si, offset _buffer_1DC6C
		call	_getexename
		jb	loc_1AE66
		mov	dx, offset _buffer_1DC6C
		mov	ax, 3D00h
		int	21h		; DOS -	2+ - OPEN DISK FILE WITH HANDLE
					; DS:DX	-> ASCIZ filename
					; AL = access mode
					; 0 - read
		jb	loc_1AE66
		mov	_fhandle_1DE68, ax
		xor	dx, dx
		xor	cx, cx
		mov	bx, _fhandle_1DE68
		mov	ax, 4202h	; get file size
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from end of file
		jb	loc_1AE5E
		mov	cx, dx
		mov	dx, ax
		sub	dx, 5AB3h	; read from the	end of file - 15AB3h = the size	of picture
		sbb	cx, 1
		mov	bx, _fhandle_1DE68
		mov	ax, 4200h
		int	21h		; DOS -	2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
					; AL = method: offset from beginning of	file
		jb	loc_1AE5E
		mov	dx, offset _buffer_1DC6C
		mov	cx, 2
		mov	bx, _fhandle_1DE68
		mov	ah, 3Fh
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		jb	loc_1AE5E
		cmp	ax, 2
		jnz	loc_1AE5E
		cmp	word ptr [_buffer_1DC6C], 4453h ; 'SD' check picture signature
		jnz	loc_1AE5E
		call	_set_egasequencer
		call	_read2buffer
		mov	dx, 3CEh
		mov	ax, 3
		out	dx, ax		; EGA: graph 1 and 2 addr reg:
					; data rotate and function select for write mode 00. Bits:
					; 0-2: set rotate count	for write mode 00
					; 3-4: fn for write modes 00 and 02
					;      00=no change; 01=AND; 10=OR; 11=XOR
		mov	ax, 0FF08h
		out	dx, ax		; EGA: graph 1 and 2 addr reg:
					; unknown register
		mov	ax, 0A000h
		mov	es, ax
		assume es:nothing
		mov	dx, 3C4h
		mov	al, 2
		out	dx, al		; EGA: sequencer address reg
					; map mask: data bits 0-3 enable writes	to bit planes 0-3
		inc	dl
		xor	bx, bx

loc_1ADE0::				; CODE XREF: _init_vga_waves+12Aj
		mov	ah, 1

loc_1ADE2::				; CODE XREF: _init_vga_waves+121j
		mov	al, ah
		out	dx, al		; EGA port: sequencer data register
		xor	di, di

loc_1ADE7::				; CODE XREF: _init_vga_waves+11Aj
		mov	cl, [si]
		inc	si
		cmp	si, offset _buffer_1seg
		jnb	short loc_1AE0C	; WARNING: push	returns	address	to stack

loc_1ADF0::				; DATA XREF: _init_vga_waves:loc_1AE0Co
		or	cl, cl
		js	short loc_1AE2D
		inc	cl

loc_1ADF6::				; CODE XREF: _init_vga_waves+D9j
		mov	al, es:[bx+di]
		mov	al, [si]
		mov	es:[bx+di], al
		inc	si
		cmp	si, offset _buffer_1seg
		jnb	short loc_1AE11

loc_1AE05::				; DATA XREF: _init_vga_waves:loc_1AE11o
		inc	di
		dec	cl
		jnz	short loc_1ADF6
		jmp	short loc_1AE46
; ---------------------------------------------------------------------------

loc_1AE0C::				; CODE XREF: _init_vga_waves+BFj
		push	offset loc_1ADF0 ; WARNING: push returns address to stack
		jmp	short _read2buffer
; ---------------------------------------------------------------------------

loc_1AE11::				; CODE XREF: _init_vga_waves+D4j
		push	offset loc_1AE05
		jmp	short _read2buffer
; ---------------------------------------------------------------------------

loc_1AE16::				; CODE XREF: _init_vga_waves+109j
		push	offset loc_1AE3A
_init_vga_waves endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_read2buffer proc near	; CODE XREF: _init_vga_waves+94p
					; _init_vga_waves+E0j ...
		pusha
		mov	dx, offset _buffer_1 ; 2800h
		mov	cx, 5000h
		mov	bx, _fhandle_1DE68
		mov	ah, 3Fh
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		popa
		mov	si, offset _buffer_1 ; 2800h
		retn
_read2buffer endp

; ---------------------------------------------------------------------------
_init_vga_waves_chunk proc near

loc_1AE2D::				; CODE XREF: _init_vga_waves+C3j
		neg	cl
		inc	cl
		mov	al, [si]
		inc	si
		cmp	si, offset _buffer_1seg
		jnb	short loc_1AE16

loc_1AE3A::				; CODE XREF: _init_vga_waves+115j
					; DATA XREF: _init_vga_waves:loc_1AE16o
		test	byte ptr es:[bx+di], 0
		mov	es:[bx+di], al
		inc	di
		dec	cl
		jnz	short loc_1AE3A

loc_1AE46::				; CODE XREF: _init_vga_waves+DBj
		cmp	di, 50h	; 'P'
		jb	short loc_1ADE7
		shl	ah, 1
		test	ah, 10h
		jz	short loc_1ADE2
		add	bx, 50h	; 'P'
		cmp	bx, 9600h
		jb	short loc_1ADE0
		call	_graph_1C070

loc_1AE5E::				; CODE XREF: _init_vga_waves+52j
					; _init_vga_waves+6Aj ...
		mov	bx, _fhandle_1DE68
		mov	ah, 3Eh
		int	21h		; DOS -	2+ - CLOSE A FILE WITH HANDLE
					; BX = file handle

loc_1AE66::				; CODE XREF: _init_vga_waves+32j
					; _init_vga_waves+3Ej
		pushf
		cli
		mov	dx, 3C4h
		mov	ax, 802h
		out	dx, ax		; EGA: sequencer address reg
					; unknown register
		mov	dx, 3CEh
		mov	ax, 205h
		out	dx, ax		; EGA: graph 1 and 2 addr reg:
					; unknown register
		mov	ax, 3
		out	dx, ax		; EGA: graph 1 and 2 addr reg:
					; data rotate and function select for write mode 00. Bits:
					; 0-2: set rotate count	for write mode 00
					; 3-4: fn for write modes 00 and 02
					;      00=no change; 01=AND; 10=OR; 11=XOR
		popf
		call	_video_prp_mtr_positn

loc_1AE7E::				; CODE XREF: _init_vga_waves+189j
					; _f2_draw_waves2+69j
		mov	ax, ds
		mov	bx, offset _buffer_1 ; 2800h
		shr	bx, 4
		add	ax, bx
		mov	_buffer_1seg, ax
		add	ax, 280h	; (offset buffer_206E0 - offset	buffer_1DEE0)/16
		mov	_buffer_2seg, ax
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	di, offset _buffer_1 ; 2800h
		mov	cx, 0A00h
		xor	eax, eax
		cld
		rep stosd
		mov	di, offset _buffer_2
		mov	cx, 0A00h
		mov	eax, 1010101h
		rep stosd
		retn
; ---------------------------------------------------------------------------

loc_1AEB2::				; CODE XREF: _init_vga_waves+5j
		call	_f2_draw_waves2
		call	_video_prp_mtr_positn
		jmp	short loc_1AE7E
_init_vga_waves_chunk endp

; =============== S U B	R O U T	I N E =======================================


_f2_draw_waves proc near	; DATA XREF: _f2_waves+6o
		mov	es, _buffer_1seg
		assume es:nothing
		xor	di, di
		mov	cx, 128h
		mov	ax, 0C001h
		call	_volume_prep
		mov	ax, 0A000h
		mov	es, ax
		assume es:nothing
		mov	fs, _buffer_1seg
		mov	gs, _buffer_2seg
		mov	di, offset _x_storage
		xor	si, si
		mov	cx, _amount_of_x

_lc_next_meter::				; CODE XREF: _f2_draw_waves+9Cj
		push	cx
		push	si
		push	di
		mov	bp, [di]	; bp = x * 8
		mov	dx, 3CEh
		mov	al, 8
		out	dx, al		; EGA: graph 1 and 2 addr reg:
					; bit mask
					; Bits 0-7 select bits to be masked in all planes
		mov	al, 10000000b	; bits of display memory which can be modified

_lc_nextvideobit::			; CODE XREF: _f2_draw_waves+8Fj
		mov	ah, 37		; 37 * 8 = 296 by x
		mov	dx, 3CFh
		out	dx, al		; EGA port: graphics controller	data register
		mov	bx, bp		; reinit (x*8)

_lc_next_x8::				; CODE XREF: _f2_draw_waves+86j
		movsx	di, byte ptr gs:[si] ; y1
		movsx	dx, byte ptr fs:[si] ; y2
		cmp	di, dx
		jz	short loc_1AF3A
		neg	di
		mov	cx, di
		shl	di, 6
		shl	cx, 4		; multiply by 80 (1 line 80 bytes)
		add	di, cx		; di = y * 80
		lea	cx, [bx+di]
		or	cx, cx		; y * 80 + x
		js	short loc_1AF1E
		cmp	cx, 280*80	; bottom y margin 280
		jnb	short loc_1AF1E
		and	byte ptr es:[bx+di], 111b ; clean previous dot

loc_1AF1E::				; CODE XREF: _f2_draw_waves+58j
					; _f2_draw_waves+5Ej
		neg	dx
		mov	di, dx
		shl	di, 6
		shl	dx, 4
		add	di, dx
		lea	cx, [bx+di]
		or	cx, cx
		js	short loc_1AF3A
		cmp	cx, 280*80
		jnb	short loc_1AF3A
		or	byte ptr es:[bx+di], 1000b ; set new dot

loc_1AF3A::				; CODE XREF: _f2_draw_waves+46j
					; _f2_draw_waves+74j ...
		add	si, 8
		inc	bx		; (x*8)++
		dec	ah
		jnz	short _lc_next_x8 ; y1
		sub	si, 128h
		inc	si
		shr	al, 1		; next video bit
		jnb	short _lc_nextvideobit ;	37 * 8 = 296 by	x
		pop	di
		pop	si
		pop	cx
		add	si, 128h
		add	di, 2		; next x
		dec	cx
		jnz	short _lc_next_meter
		mov	ax, _buffer_1seg
		xchg	ax, _buffer_2seg
		mov	_buffer_1seg, ax
		retn
_f2_draw_waves endp


; =============== S U B	R O U T	I N E =======================================


_f2_draw_waves2 proc near	; CODE XREF: _init_vga_waves:loc_1AEB2p
					; DATA XREF: _f2_waves+Co
		mov	ax, 0A000h
		mov	es, ax
		mov	fs, _buffer_1seg
		mov	gs, _buffer_2seg
		mov	di, offset _x_storage
		xor	si, si
		mov	cx, _amount_of_x

loc_1AF79::				; CODE XREF: _f2_draw_waves2+67j
		push	cx
		push	si
		push	di
		mov	bp, [di]
		mov	dx, 3CEh
		mov	al, 8
		out	dx, al		; EGA: graph 1 and 2 addr reg:
					; bit mask
					; Bits 0-7 select bits to be masked in all planes
		mov	al, 10000000b

loc_1AF86::				; CODE XREF: _f2_draw_waves2+5Aj
		mov	ah, 37
		mov	dx, 3CFh
		out	dx, al		; EGA port: graphics controller	data register
		mov	bx, bp

loc_1AF8E::				; CODE XREF: _f2_draw_waves2+51j
		movsx	di, byte ptr gs:[si]
		neg	di
		mov	cx, di
		shl	di, 6
		shl	cx, 4
		add	di, cx
		lea	cx, [bx+di]
		or	cx, cx
		js	short loc_1AFAE
		cmp	cx, 22400
		jnb	short loc_1AFAE
		and	byte ptr es:[bx+di], 111b

loc_1AFAE::				; CODE XREF: _f2_draw_waves2+3Fj
					; _f2_draw_waves2+45j
		add	si, 8
		inc	bx
		dec	ah
		jnz	short loc_1AF8E
		sub	si, 128h
		inc	si
		shr	al, 1
		jnb	short loc_1AF86
		pop	di
		pop	si
		pop	cx
		add	si, 128h
		add	di, 2
		dec	cx
		jnz	short loc_1AF79
		jmp	loc_1AE7E
_f2_draw_waves2 endp


; =============== S U B	R O U T	I N E =======================================


_init_f5_spectr proc near	; CODE XREF: _f5_graphspectr+18p
					; DATA XREF: _f5_graphspectro ...
		cmp	_byte_1DE70, 4
		jz	locret_1B083
		mov	_byte_1DE70, 4
		mov	ax, 13h
		int	10h		; - VIDEO - SET	VIDEO MODE
					; AL = mode
		call	_set_egasequencer
		mov	dx, 3C8h
		xor	al, al
		out	dx, al
		jmp	short $+2
		jmp	short $+2
		inc	dx
		out	dx, al
		jmp	short $+2
		out	dx, al
		jmp	short $+2
		out	dx, al
		jmp	short $+2
		mov	bp, 16Ch
		xor	bx, bx

loc_1AFFE::				; CODE XREF: _init_f5_spectr+43j
		mov	al, bh
		out	dx, al
		jmp	short $+2
		mov	al, 3Fh	; '?'
		out	dx, al
		jmp	short $+2
		xor	al, al
		out	dx, al
		jmp	short $+2
		add	bx, bp
		cmp	bh, 40h	; '@'
		jb	short loc_1AFFE

loc_1B014::				; CODE XREF: _init_f5_spectr+58j
		sub	bx, bp
		mov	al, 3Fh	; '?'
		out	dx, al
		jmp	short $+2
		mov	al, bh
		out	dx, al
		jmp	short $+2
		xor	al, al
		out	dx, al
		jmp	short $+2
		or	bh, bh
		jns	short loc_1B014
		mov	dx, 3C8h
		mov	al, 0FCh ; '¸'
		out	dx, al
		jmp	short $+2
		jmp	short $+2
		inc	dx
		xor	al, al
		out	dx, al
		jmp	short $+2
		out	dx, al
		jmp	short $+2
		out	dx, al
		jmp	short $+2
		mov	al, 10h
		out	dx, al
		jmp	short $+2
		out	dx, al
		jmp	short $+2
		mov	al, 30h	; '0'
		out	dx, al
		jmp	short $+2
		mov	al, 10h
		out	dx, al
		jmp	short $+2
		out	dx, al
		jmp	short $+2
		out	dx, al
		jmp	short $+2
		mov	al, 3Fh	; '?'
		out	dx, al
		jmp	short $+2
		out	dx, al
		jmp	short $+2
		out	dx, al
		call	_graph_1C070
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	di, offset unk_23EE4
		mov	cx, 0C8h ; '»'
		xor	eax, eax
		cld
		rep stosd
		mov	ax, 200h
		test	byte ptr [_configword], 8
		jnz	short loc_1B080
		shr	ax, 1

loc_1B080::				; CODE XREF: _init_f5_spectr+ADj
		mov	_word_24524, ax

locret_1B083::				; CODE XREF: _init_f5_spectr+5j
		retn
_init_f5_spectr endp


; =============== S U B	R O U T	I N E =======================================


_spectr_1B084 proc near
		mov	_word_2450E, di
		mov	al, 1
		cmp	al, 1
		jnz	loc_1B240
		call	_spectr_1B406
		mov	ax, _word_24514
		xor	si, si

loc_1B098::				; CODE XREF: _spectr_1B084+1Bj
		add	si, 4
		shr	ax, 1
		test	ax, ax
		jnz	short loc_1B098
		sub	si, 4
		mov	ebx, [_tabledword_24526+si]
		mov	_multip_244D0,	ebx
		mov	eax, [_tabledword_24562+si]
		mov	_multip_244CC,	eax
		add	eax, 10000h
		mov	_dword_244C8, eax
		mov	_dword_244D4, ebx
		mov	cx, _word_24514
		shr	cx, 1
		mov	ax, 2

loc_1B0CF::				; CODE XREF: _spectr_1B084+1A2j
		push	cx
		push	ax
		shl	ax, 1
		dec	ax
		mov	di, ax
		inc	ax
		neg	ax
		add	ax, 3
		add	ax, _word_24514
		add	ax, _word_24514
		mov	si, ax
		mov	eax, _dword_244C8
		mov	_dword_244F4, eax
		mov	eax, _dword_244D4
		mov	_dword_244F8, eax
		dec	si
		shl	si, 2
		dec	di

loc_1B0FB::
		shl	di, 2
		add	si, _word_2450E
		add	di, _word_2450E
		mov	eax, [di]
		add	eax, [si]
		sar	eax, 1
		mov	_dword_244E4, eax
		mov	eax, [di+4]
		sub	eax, [si+4]
		sar	eax, 1
		mov	_dword_244E8, eax
		mov	eax, [di+4]
		add	eax, [si+4]
		sar	eax, 1
		mov	_dword_244EC, eax
		mov	eax, [si]

loc_1B134::
		sub	eax, [di]
		sar	eax, 1
		mov	_dword_244F0, eax
		mov	ecx, _dword_244F4
		mov	eax, _dword_244EC
		imul	ecx
		shrd	eax, edx, 16
		mov	_dword_244FC, eax
		mov	eax, _dword_244F0
		imul	ecx
		shrd	eax, edx, 16
		mov	_dword_24500, eax
		mov	ecx, _dword_244F8
		mov	eax, _dword_244EC
		imul	ecx
		shrd	eax, edx, 16
		mov	_dword_24508, eax
		mov	eax, _dword_244F0
		imul	ecx
		shrd	eax, edx, 16
		mov	_dword_24504, eax
		mov	eax, _dword_244E4
		add	eax, _dword_244FC
		sub	eax, _dword_24504
		mov	[di], eax
		mov	eax, _dword_244E8
		add	eax, _dword_24500
		add	eax, _dword_24508
		mov	[di+4],	eax
		mov	eax, _dword_244E4
		sub	eax, _dword_244FC
		add	eax, _dword_24504
		mov	[si], eax
		mov	eax, _dword_24500
		sub	eax, _dword_244E8
		add	eax, _dword_24508
		mov	[si+4],	eax
		mov	eax, _dword_244C8
		mov	dword ptr [unk_244C4], eax
		mov	eax, _multip_244CC
		imul	_dword_244C8
		shrd	eax, edx, 10h
		add	_dword_244C8, eax
		mov	eax, _dword_244D4
		imul	_multip_244D0
		shrd	eax, edx, 10h
		sub	_dword_244C8, eax
		mov	eax, _dword_244D4
		imul	_multip_244CC
		shrd	eax, edx, 10h
		add	_dword_244D4, eax
		mov	eax, [dword ptr	unk_244C4]
		imul	_multip_244D0
		shrd	eax, edx, 10h
		add	_dword_244D4, eax
		pop	ax
		pop	cx
		inc	ax
		dec	cx
		jnz	loc_1B0CF
		mov	si, _word_2450E
		mov	eax, [si]
		mov	ebx, [si+4]
		add	[si], ebx
		sub	eax, ebx
		mov	[si+4],	eax
		retn
; ---------------------------------------------------------------------------

loc_1B240::				; CODE XREF: _spectr_1B084+8j
		mov	ax, _word_24514
		xor	si, si

loc_1B245::				; CODE XREF: _spectr_1B084+1C8j
		add	si, 4
		shr	ax, 1
		test	ax, ax
		jnz	short loc_1B245
		sub	si, 4
		mov	ebx, [_tabledword_24526+si]
		neg	ebx
		mov	_multip_244D0,	ebx
		mov	eax, [_tabledword_24562+si]
		neg	eax
		mov	_multip_244CC,	eax
		add	eax, 10000h
		mov	_dword_244C8, eax
		mov	_dword_244D4, ebx
		mov	cx, _word_24514
		shr	cx, 1
		mov	ax, 2

loc_1B282::				; CODE XREF: _spectr_1B084+357j
		push	cx
		push	ax
		shl	ax, 1
		dec	ax
		mov	di, ax
		neg	ax
		add	ax, 3
		add	ax, _word_24514
		add	ax, _word_24514
		mov	si, ax
		mov	eax, _dword_244C8
		mov	_dword_244F4, eax
		mov	eax, _dword_244D4
		mov	_dword_244F8, eax
		dec	si
		shl	si, 2
		dec	di
		shl	di, 2
		mov	eax, [di]
		add	eax, [si]
		sar	eax, 1
		mov	_dword_244E4, eax
		mov	eax, [di+4]
		sub	eax, [si+4]
		sar	eax, 1
		mov	_dword_244E8, eax
		mov	eax, [di+4]
		add	eax, [si+4]
		neg	eax
		sar	eax, 1
		mov	_dword_244EC, eax
		mov	eax, [di]
		sub	eax, [si]
		sar	eax, 1
		mov	_dword_244F0, eax
		mov	ecx, _dword_244F4
		mov	eax, _dword_244EC
		imul	ecx
		shrd	eax, edx, 10h
		mov	_dword_244FC, eax
		mov	eax, _dword_244F0
		imul	ecx
		shrd	eax, edx, 10h
		mov	_dword_24500, eax
		mov	ecx, _dword_244F8
		mov	eax, _dword_244EC
		imul	ecx
		shrd	eax, edx, 10h
		mov	_dword_24508, eax
		mov	eax, _dword_244F0
		imul	ecx
		shrd	eax, edx, 10h
		mov	_dword_24504, eax
		mov	eax, _dword_244E4
		add	eax, _dword_244FC
		sub	eax, _dword_24504
		mov	[di], eax
		mov	eax, _dword_244E8
		add	eax, _dword_24500
		add	eax, _dword_24508
		mov	[di+4],	eax
		mov	eax, _dword_244E4
		sub	eax, _dword_244FC
		add	eax, _dword_24504
		mov	[si], eax
		mov	eax, _dword_24500
		sub	eax, _dword_244E8
		add	eax, _dword_24508
		mov	[si+4],	eax
		mov	eax, _dword_244C8
		mov	dword ptr [unk_244C4], eax
		mov	eax, _dword_244C8
		mov	dword ptr [unk_244C4], eax
		mov	eax, _multip_244CC
		imul	_dword_244C8
		shrd	eax, edx, 10h
		add	_dword_244C8, eax
		mov	eax, _dword_244D4
		imul	_multip_244D0
		shrd	eax, edx, 10h
		sub	_dword_244C8, eax
		mov	eax, _dword_244D4
		imul	_multip_244CC
		shrd	eax, edx, 10h
		add	_dword_244D4, eax
		mov	eax, [dword ptr	unk_244C4]
		imul	_multip_244D0
		shrd	eax, edx, 10h
		add	_dword_244D4, eax
		pop	ax
		pop	cx
		inc	ax
		dec	cx
		jnz	loc_1B282
		mov	si, _word_2450E
		mov	eax, [si]
		mov	ebx, [si+4]
		mov	ecx, eax
		add	ecx, ebx
		sar	ecx, 1
		mov	[si], ecx
		mov	ecx, eax
		sub	ecx, ebx
		sar	ecx, 1
		mov	[si], ecx
		call	_spectr_1B406
		retn
_spectr_1B084 endp


; =============== S U B	R O U T	I N E =======================================


_spectr_1B406 proc near	; CODE XREF: _spectr_1B084+Cp
					; _spectr_1B084+37Ep ...
		mov	_word_2450E, di
		mov	_word_2450C, 0
		mov	cx, _word_24520
		shl	cx, 1
		mov	_word_24522, cx
		mov	si, _word_2450E
		shr	cx, 1
		mov	di, _word_2450E
		mov	bp, di

loc_1B426::				; CODE XREF: _spectr_1B406+5Fj
		push	cx
		cmp	si, di
		jle	short loc_1B440
		mov	edx, [si]
		mov	ebx, [si+4]
		xchg	edx, [di]
		xchg	ebx, [di+4]
		mov	[si], edx
		mov	[si+4],	ebx

loc_1B440::				; CODE XREF: _spectr_1B406+23j
		sub	si, bp
		shr	si, 2
		mov	ax, _word_24522
		shr	ax, 1

loc_1B44A::				; CODE XREF: _spectr_1B406+51j
		cmp	ax, 2
		jl	short loc_1B459
		cmp	si, ax
		jl	short loc_1B459
		sub	si, ax
		shr	ax, 1
		jmp	short loc_1B44A
; ---------------------------------------------------------------------------

loc_1B459::				; CODE XREF: _spectr_1B406+47j
					; _spectr_1B406+4Bj
		add	si, ax
		shl	si, 2
		add	si, bp
		pop	cx
		add	di, 8
		dec	cx
		jnz	short loc_1B426
		mov	_word_24516, 2

loc_1B46D::				; CODE XREF: _spectr_1B406+1BEj
		mov	ax, _word_24516
		cmp	_word_24522, ax
		jle	locret_1B5C7
		shl	ax, 1
		mov	_word_2451C, ax
		mov	si, _word_2450C
		mov	eax, [_tabledword_24526+si]
		mov	_multip_244D0,	eax
		mov	eax, [_tabledword_24562+si]
		mov	_multip_244CC,	eax
		add	_word_2450C, 4
		mov	_dword_244C8, 10000h
		mov	_dword_244D4, 0
		mov	cx, _word_24516
		shr	cx, 1
		mov	ax, 1

loc_1B4B3::				; CODE XREF: _spectr_1B406+1B4j
		push	cx
		push	ax
		shl	ax, 1
		dec	ax
		mov	_word_24518, ax
		mov	ax, _word_24522
		sub	ax, _word_24518
		cwd
		idiv	_word_2451C
		mov	cx, ax
		inc	cx
		mov	ax, 0

loc_1B4CD::				; CODE XREF: _spectr_1B406+156j
		push	cx
		push	ax
		imul	_word_2451C
		add	ax, _word_24518
		mov	_word_2451E, ax
		add	ax, _word_24516
		mov	_word_2451A, ax
		mov	si, _word_2451A
		dec	si
		shl	si, 2
		add	si, _word_2450E
		mov	di, _word_2451E
		dec	di
		shl	di, 2
		add	di, _word_2450E
		mov	ecx, _dword_244C8
		mov	ebp, _dword_244D4
		mov	eax, [si]
		imul	ecx
		shrd	eax, edx, 10h
		mov	ebx, eax
		mov	eax, [si+4]
		imul	ebp
		shrd	eax, edx, 10h
		sub	ebx, eax
		mov	eax, [si+4]
		imul	ecx
		shrd	eax, edx, 10h
		mov	ecx, eax
		mov	eax, [si]
		imul	ebp
		shrd	eax, edx, 10h
		add	ecx, eax
		mov	eax, [di]
		sub	eax, ebx
		mov	[si], eax
		mov	eax, [di+4]
		sub	eax, ecx
		mov	[si+4],	eax
		add	[di], ebx
		add	[di+4],	ecx
		pop	ax
		pop	cx
		inc	ax
		dec	cx
		jnz	loc_1B4CD
		mov	ecx, _multip_244CC
		mov	ebp, _multip_244D0
		mov	eax, _dword_244C8
		mov	dword ptr [unk_244C4], eax
		mov	eax, _dword_244C8
		imul	ecx
		shrd	eax, edx, 10h
		add	_dword_244C8, eax
		mov	eax, _dword_244D4
		imul	ebp
		shrd	eax, edx, 10h
		sub	_dword_244C8, eax
		mov	eax, _dword_244D4
		imul	ecx
		shrd	eax, edx, 10h
		add	_dword_244D4, eax
		mov	eax, [dword ptr	unk_244C4]
		imul	ebp
		shrd	eax, edx, 10h
		add	_dword_244D4, eax
		pop	ax
		pop	cx
		inc	ax
		dec	cx
		jnz	loc_1B4B3
		mov	ax, _word_2451C
		mov	_word_24516, ax
		jmp	loc_1B46D
; ---------------------------------------------------------------------------

locret_1B5C7::				; CODE XREF: _spectr_1B406+6Ej
		retn
_spectr_1B406 endp


; =============== S U B	R O U T	I N E =======================================


_f5_draw_spectr proc near	; DATA XREF: _f5_graphspectr+6o
					; _f5_graphspectr+Co
		mov	ax, ds
		mov	es, ax
		mov	di, offset _buffer_1 ; 2800h
		mov	cx, 200h
		mov	ax, 4001h
		call	_volume_prep
		lfs	bx, _segfsbx_1DE28
		mov	si, offset _buffer_1 ; 2800h
		mov	di, offset _byte_24204
		mov	bp, _amount_of_x

loc_1B5EC::				; CODE XREF: _f5_draw_spectr+2A1j
		mov	cx, bp
		xor	dx, dx
		cmp	byte ptr fs:[bx+3Ah], 64
		ja	short loc_1B5FC
		mov	al, [si]
		cbw
		add	dx, ax

loc_1B5FC::				; CODE XREF: _f5_draw_spectr+2Dj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+8Ah], 64
		ja	short loc_1B610
		mov	al, [si+200h]
		cbw
		add	dx, ax

loc_1B610::				; CODE XREF: _f5_draw_spectr+3Fj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+0DAh], 64
		ja	short loc_1B624
		mov	al, [si+400h]
		cbw
		add	dx, ax

loc_1B624::				; CODE XREF: _f5_draw_spectr+53j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+12Ah], 64
		ja	short loc_1B638
		mov	al, [si+600h]
		cbw
		add	dx, ax

loc_1B638::				; CODE XREF: _f5_draw_spectr+67j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+17Ah], 64
		ja	short loc_1B64C
		mov	al, [si+800h]
		cbw
		add	dx, ax

loc_1B64C::				; CODE XREF: _f5_draw_spectr+7Bj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+1CAh], 64
		ja	short loc_1B660
		mov	al, [si+0A00h]
		cbw
		add	dx, ax

loc_1B660::				; CODE XREF: _f5_draw_spectr+8Fj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+21Ah], 64
		ja	short loc_1B674
		mov	al, [si+0C00h]
		cbw
		add	dx, ax

loc_1B674::				; CODE XREF: _f5_draw_spectr+A3j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+26Ah], 64
		ja	short loc_1B688
		mov	al, [si+0E00h]
		cbw
		add	dx, ax

loc_1B688::				; CODE XREF: _f5_draw_spectr+B7j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+2BAh], 64
		ja	short loc_1B69C
		mov	al, [si+1000h]
		cbw
		add	dx, ax

loc_1B69C::				; CODE XREF: _f5_draw_spectr+CBj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+30Ah], 64
		ja	short loc_1B6B0
		mov	al, [si+1200h]
		cbw
		add	dx, ax

loc_1B6B0::				; CODE XREF: _f5_draw_spectr+DFj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+35Ah], 64
		ja	short loc_1B6C4
		mov	al, [si+1400h]
		cbw
		add	dx, ax

loc_1B6C4::				; CODE XREF: _f5_draw_spectr+F3j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+3AAh], 64
		ja	short loc_1B6D8
		mov	al, [si+1600h]
		cbw
		add	dx, ax

loc_1B6D8::				; CODE XREF: _f5_draw_spectr+107j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+3FAh], 64
		ja	short loc_1B6EC
		mov	al, [si+1800h]
		cbw
		add	dx, ax

loc_1B6EC::				; CODE XREF: _f5_draw_spectr+11Bj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+44Ah], 64
		ja	short loc_1B700
		mov	al, [si+1A00h]
		cbw
		add	dx, ax

loc_1B700::				; CODE XREF: _f5_draw_spectr+12Fj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+49Ah], 64
		ja	short loc_1B714
		mov	al, [si+1C00h]
		cbw
		add	dx, ax

loc_1B714::				; CODE XREF: _f5_draw_spectr+143j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+4EAh], 40h ; '@'
		ja	short loc_1B728
		mov	al, [si+1E00h]
		cbw
		add	dx, ax

loc_1B728::				; CODE XREF: _f5_draw_spectr+157j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+53Ah], 40h ; '@'
		ja	short loc_1B73C
		mov	al, [si+2000h]
		cbw
		add	dx, ax

loc_1B73C::				; CODE XREF: _f5_draw_spectr+16Bj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+58Ah], 40h ; '@'
		ja	short loc_1B750
		mov	al, [si+2200h]
		cbw
		add	dx, ax

loc_1B750::				; CODE XREF: _f5_draw_spectr+17Fj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+5DAh], 40h ; '@'
		ja	short loc_1B764
		mov	al, [si+2400h]
		cbw
		add	dx, ax

loc_1B764::				; CODE XREF: _f5_draw_spectr+193j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+62Ah], 40h ; '@'
		ja	short loc_1B778
		mov	al, [si+2600h]
		cbw
		add	dx, ax

loc_1B778::				; CODE XREF: _f5_draw_spectr+1A7j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+67Ah], 40h ; '@'
		ja	short loc_1B78C
		mov	al, [si+2800h]
		cbw
		add	dx, ax

loc_1B78C::				; CODE XREF: _f5_draw_spectr+1BBj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+6CAh], 40h ; '@'
		ja	short loc_1B7A0
		mov	al, [si+2A00h]
		cbw
		add	dx, ax

loc_1B7A0::				; CODE XREF: _f5_draw_spectr+1CFj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+71Ah], 40h ; '@'
		ja	short loc_1B7B4
		mov	al, [si+2C00h]
		cbw
		add	dx, ax

loc_1B7B4::				; CODE XREF: _f5_draw_spectr+1E3j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+76Ah], 40h ; '@'
		ja	short loc_1B7C8
		mov	al, [si+2E00h]
		cbw
		add	dx, ax

loc_1B7C8::				; CODE XREF: _f5_draw_spectr+1F7j
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+7BAh], 40h ; '@'
		ja	short loc_1B7DC
		mov	al, [si+3000h]
		cbw
		add	dx, ax

loc_1B7DC::				; CODE XREF: _f5_draw_spectr+20Bj
		dec	cx
		jz	loc_1B85F
		cmp	byte ptr fs:[bx+80Ah], 40h ; '@'
		ja	short loc_1B7F0
		mov	al, [si+3200h]
		cbw
		add	dx, ax

loc_1B7F0::				; CODE XREF: _f5_draw_spectr+21Fj
		dec	cx
		jz	short loc_1B85F
		cmp	byte ptr fs:[bx+85Ah], 40h ; '@'
		ja	short loc_1B802
		mov	al, [si+3400h]
		cbw
		add	dx, ax

loc_1B802::				; CODE XREF: _f5_draw_spectr+231j
		dec	cx
		jz	short loc_1B85F
		cmp	byte ptr fs:[bx+8AAh], 40h ; '@'
		ja	short loc_1B814
		mov	al, [si+3600h]
		cbw
		add	dx, ax

loc_1B814::				; CODE XREF: _f5_draw_spectr+243j
		dec	cx
		jz	short loc_1B85F
		cmp	byte ptr fs:[bx+8FAh], 40h ; '@'
		ja	short loc_1B826
		mov	al, [si+3800h]
		cbw
		add	dx, ax

loc_1B826::				; CODE XREF: _f5_draw_spectr+255j
		dec	cx
		jz	short loc_1B85F
		cmp	byte ptr fs:[bx+94Ah], 40h ; '@'
		ja	short loc_1B838
		mov	al, [si+3A00h]
		cbw
		add	dx, ax

loc_1B838::				; CODE XREF: _f5_draw_spectr+267j
		dec	cx
		jz	short loc_1B85F
		cmp	byte ptr fs:[bx+99Ah], 40h ; '@'
		ja	short loc_1B84A
		mov	al, [si+3C00h]
		cbw
		add	dx, ax

loc_1B84A::				; CODE XREF: _f5_draw_spectr+279j
		dec	cx
		jz	short loc_1B85F
		cmp	byte ptr fs:[bx+9EAh], 40h ; '@'
		ja	short loc_1B85C
		mov	al, [si+3E00h]
		cbw
		add	dx, ax

loc_1B85C::				; CODE XREF: _f5_draw_spectr+28Bj
		dec	cx
		jz	short $+2

loc_1B85F::				; CODE XREF: _f5_draw_spectr+35j
					; _f5_draw_spectr+49j ...
		sar	dx, 1
		mov	[di], dl
		inc	si
		inc	di
		cmp	si, offset _byte_1E0E0
		jb	loc_1B5EC
		mov	si, offset _byte_24204
		mov	di, offset _byte_22EE4
		mov	cx, 200h

loc_1B876::				; CODE XREF: _f5_draw_spectr+2C5j
		movsx	eax, byte ptr [si]
		shl	eax, 10h
		mov	[di], eax
		mov	dword ptr [di+4], 0
		inc	si
		add	di, 8
		loop	loc_1B876
		mov	ax, _word_24524
		mov	_word_24520, ax
		mov	_word_24514, ax
		mov	di, offset _byte_22EE4
		call	_spectr_1B406
		mov	si, offset _byte_22EE4
		mov	di, offset unk_23EE4
		mov	cx, 64h	; 'd'
		call	_spectr_1BBC1
		lfs	bx, _segfsbx_1DE28
		mov	si, offset _buffer_1 ; 2800h
		mov	di, offset _byte_24204
		mov	bp, _amount_of_x

loc_1B8BC::				; CODE XREF: _f5_draw_spectr+571j
		mov	cx, bp
		xor	dx, dx
		cmp	byte ptr fs:[bx+3Ah], 40h ; '@'
		jb	short loc_1B8CC
		mov	al, [si]
		cbw
		add	dx, ax

loc_1B8CC::				; CODE XREF: _f5_draw_spectr+2FDj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+8Ah], 40h ; '@'
		jb	short loc_1B8E0
		mov	al, [si+200h]
		cbw
		add	dx, ax

loc_1B8E0::				; CODE XREF: _f5_draw_spectr+30Fj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+0DAh], 40h ; '@'
		jb	short loc_1B8F4
		mov	al, [si+400h]
		cbw
		add	dx, ax

loc_1B8F4::				; CODE XREF: _f5_draw_spectr+323j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+12Ah], 40h ; '@'
		jb	short loc_1B908
		mov	al, [si+600h]
		cbw
		add	dx, ax

loc_1B908::				; CODE XREF: _f5_draw_spectr+337j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+17Ah], 40h ; '@'
		jb	short loc_1B91C
		mov	al, [si+800h]
		cbw
		add	dx, ax

loc_1B91C::				; CODE XREF: _f5_draw_spectr+34Bj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+1CAh], 40h ; '@'
		jb	short loc_1B930
		mov	al, [si+0A00h]
		cbw
		add	dx, ax

loc_1B930::				; CODE XREF: _f5_draw_spectr+35Fj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+21Ah], 40h ; '@'
		jb	short loc_1B944
		mov	al, [si+0C00h]
		cbw
		add	dx, ax

loc_1B944::				; CODE XREF: _f5_draw_spectr+373j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+26Ah], 40h ; '@'
		jb	short loc_1B958
		mov	al, [si+0E00h]
		cbw
		add	dx, ax

loc_1B958::				; CODE XREF: _f5_draw_spectr+387j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+2BAh], 40h ; '@'
		jb	short loc_1B96C
		mov	al, [si+1000h]
		cbw
		add	dx, ax

loc_1B96C::				; CODE XREF: _f5_draw_spectr+39Bj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+30Ah], 40h ; '@'
		jb	short loc_1B980
		mov	al, [si+1200h]
		cbw
		add	dx, ax

loc_1B980::				; CODE XREF: _f5_draw_spectr+3AFj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+35Ah], 40h ; '@'
		jb	short loc_1B994
		mov	al, [si+1400h]
		cbw
		add	dx, ax

loc_1B994::				; CODE XREF: _f5_draw_spectr+3C3j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+3AAh], 40h ; '@'
		jb	short loc_1B9A8
		mov	al, [si+1600h]
		cbw
		add	dx, ax

loc_1B9A8::				; CODE XREF: _f5_draw_spectr+3D7j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+3FAh], 40h ; '@'
		jb	short loc_1B9BC
		mov	al, [si+1800h]
		cbw
		add	dx, ax

loc_1B9BC::				; CODE XREF: _f5_draw_spectr+3EBj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+44Ah], 40h ; '@'
		jb	short loc_1B9D0
		mov	al, [si+1A00h]
		cbw
		add	dx, ax

loc_1B9D0::				; CODE XREF: _f5_draw_spectr+3FFj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+49Ah], 40h ; '@'
		jb	short loc_1B9E4
		mov	al, [si+1C00h]
		cbw
		add	dx, ax

loc_1B9E4::				; CODE XREF: _f5_draw_spectr+413j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+4EAh], 40h ; '@'
		jb	short loc_1B9F8
		mov	al, [si+1E00h]
		cbw
		add	dx, ax

loc_1B9F8::				; CODE XREF: _f5_draw_spectr+427j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+53Ah], 40h ; '@'
		jb	short loc_1BA0C
		mov	al, [si+2000h]
		cbw
		add	dx, ax

loc_1BA0C::				; CODE XREF: _f5_draw_spectr+43Bj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+58Ah], 40h ; '@'
		jb	short loc_1BA20
		mov	al, [si+2200h]
		cbw
		add	dx, ax

loc_1BA20::				; CODE XREF: _f5_draw_spectr+44Fj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+5DAh], 40h ; '@'
		jb	short loc_1BA34
		mov	al, [si+2400h]
		cbw
		add	dx, ax

loc_1BA34::				; CODE XREF: _f5_draw_spectr+463j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+62Ah], 40h ; '@'
		jb	short loc_1BA48
		mov	al, [si+2600h]
		cbw
		add	dx, ax

loc_1BA48::				; CODE XREF: _f5_draw_spectr+477j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+67Ah], 40h ; '@'
		jb	short loc_1BA5C
		mov	al, [si+2800h]
		cbw
		add	dx, ax

loc_1BA5C::				; CODE XREF: _f5_draw_spectr+48Bj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+6CAh], 40h ; '@'
		jb	short loc_1BA70
		mov	al, [si+2A00h]
		cbw
		add	dx, ax

loc_1BA70::				; CODE XREF: _f5_draw_spectr+49Fj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+71Ah], 40h ; '@'
		jb	short loc_1BA84
		mov	al, [si+2C00h]
		cbw
		add	dx, ax

loc_1BA84::				; CODE XREF: _f5_draw_spectr+4B3j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+76Ah], 40h ; '@'
		jb	short loc_1BA98
		mov	al, [si+2E00h]
		cbw
		add	dx, ax

loc_1BA98::				; CODE XREF: _f5_draw_spectr+4C7j
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+7BAh], 40h ; '@'
		jb	short loc_1BAAC
		mov	al, [si+3000h]
		cbw
		add	dx, ax

loc_1BAAC::				; CODE XREF: _f5_draw_spectr+4DBj
		dec	cx
		jz	loc_1BB2F
		cmp	byte ptr fs:[bx+80Ah], 40h ; '@'
		jb	short loc_1BAC0
		mov	al, [si+3200h]
		cbw
		add	dx, ax

loc_1BAC0::				; CODE XREF: _f5_draw_spectr+4EFj
		dec	cx
		jz	short loc_1BB2F
		cmp	byte ptr fs:[bx+85Ah], 40h ; '@'
		jb	short loc_1BAD2
		mov	al, [si+3400h]
		cbw
		add	dx, ax

loc_1BAD2::				; CODE XREF: _f5_draw_spectr+501j
		dec	cx
		jz	short loc_1BB2F
		cmp	byte ptr fs:[bx+8AAh], 40h ; '@'
		jb	short loc_1BAE4
		mov	al, [si+3600h]
		cbw
		add	dx, ax

loc_1BAE4::				; CODE XREF: _f5_draw_spectr+513j
		dec	cx
		jz	short loc_1BB2F
		cmp	byte ptr fs:[bx+8FAh], 40h ; '@'
		jb	short loc_1BAF6
		mov	al, [si+3800h]
		cbw
		add	dx, ax

loc_1BAF6::				; CODE XREF: _f5_draw_spectr+525j
		dec	cx
		jz	short loc_1BB2F
		cmp	byte ptr fs:[bx+94Ah], 40h ; '@'
		jb	short loc_1BB08
		mov	al, [si+3A00h]
		cbw
		add	dx, ax

loc_1BB08::				; CODE XREF: _f5_draw_spectr+537j
		dec	cx
		jz	short loc_1BB2F
		cmp	byte ptr fs:[bx+99Ah], 40h ; '@'
		jb	short loc_1BB1A
		mov	al, [si+3C00h]
		cbw
		add	dx, ax

loc_1BB1A::				; CODE XREF: _f5_draw_spectr+549j
		dec	cx
		jz	short loc_1BB2F
		cmp	byte ptr fs:[bx+9EAh], 40h ; '@'
		jb	short loc_1BB2C
		mov	al, [si+3E00h]
		cbw
		add	dx, ax

loc_1BB2C::				; CODE XREF: _f5_draw_spectr+55Bj
		dec	cx
		jz	short $+2

loc_1BB2F::				; CODE XREF: _f5_draw_spectr+305j
					; _f5_draw_spectr+319j ...
		sar	dx, 1
		mov	[di], dl
		inc	si
		inc	di
		cmp	si, offset _byte_1E0E0
		jb	loc_1B8BC
		mov	si, offset _byte_24204
		mov	di, offset _byte_22EE4
		mov	cx, 200h

loc_1BB46::				; CODE XREF: _f5_draw_spectr+595j
		movsx	eax, byte ptr [si]
		shl	eax, 10h
		mov	[di], eax
		mov	dword ptr [di+4], 0
		inc	si
		add	di, 8
		loop	loc_1BB46
		mov	ax, _word_24524
		mov	_word_24520, ax
		mov	_word_24514, ax
		mov	di, offset _byte_22EE4
		call	_spectr_1B406
		mov	si, offset _byte_22EE4
		mov	di, offset unk_24074
		mov	cx, 64h	; 'd'
		call	_spectr_1BBC1
		mov	ax, 0A000h
		mov	es, ax
		assume es:nothing
		mov	bx, offset unk_23EE4
		mov	bp, 7BC4h
		call	_spectr_1BCE9
		mov	bx, offset _byte_23EE5
		mov	bp, 7BD6h
		call	_spectr_1BC2D
		mov	bx, offset unk_24074
		mov	bp, 0F8C4h
		call	_spectr_1BCE9
		mov	bx, offset _byte_24075
		mov	bp, 0F8D6h
		call	_spectr_1BC2D
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	si, offset unk_23EE4
		mov	di, offset _byte_23F48
		mov	cx, 19h
		cld
		rep movsd
		mov	si, offset unk_24074
		mov	di, offset _byte_240D8
		mov	cx, 19h
		rep movsd
		retn
_f5_draw_spectr endp


; =============== S U B	R O U T	I N E =======================================


_spectr_1BBC1 proc near	; CODE XREF: _f5_draw_spectr+2DFp
					; _f5_draw_spectr+5AFp ...
		push	cx
		mov	eax, [si]
		imul	eax
		mov	ebx, eax
		mov	ecx, edx
		mov	eax, [si+4]
		imul	eax
		mov	eax, edx
		add	eax, ebx
		adc	edx, ecx
		mov	eax, edx
		mov	cl, _byte_1DE81
		sar	eax, cl
		mov	ebx, eax
		call	_spectr_1C4F8
		or	ah, ah
		jz	short loc_1BBF4
		mov	al, 0FFh

loc_1BBF4::				; CODE XREF: _spectr_1BBC1+2Fj
		cmp	_byte_1DE82, 0
		jz	short loc_1BC0C
		mov	ah, [di+64h]
		sub	ah, _byte_1DE82
		jnb	short loc_1BC06
		xor	ah, ah

loc_1BC06::				; CODE XREF: _spectr_1BBC1+41j
		cmp	ah, al
		jb	short loc_1BC0C
		mov	al, ah

loc_1BC0C::				; CODE XREF: _spectr_1BBC1+38j
					; _spectr_1BBC1+47j
		mov	[di], al
		cmp	byte ptr [di+12Ch], 0
		jz	short loc_1BC1B
		cmp	al, [di+0C8h]
		jb	short loc_1BC24

loc_1BC1B::				; CODE XREF: _spectr_1BBC1+52j
		mov	[di+0C8h], al
		mov	byte ptr [di+12Ch], 14h

loc_1BC24::				; CODE XREF: _spectr_1BBC1+58j
		inc	di
		add	si, 8
		pop	cx
		dec	cx
		jnz	short _spectr_1BBC1
		retn
_spectr_1BBC1 endp


; =============== S U B	R O U T	I N E =======================================


_spectr_1BC2D proc near	; CODE XREF: _f5_draw_spectr+5C6p
					; _f5_draw_spectr+5D8p
		mov	cx, 99

loc_1BC30::				; CODE XREF: _spectr_1BC2D+B7j
		mov	al, [bx]
		cmp	al, 90
		jb	short loc_1BC38
		mov	al, 90

loc_1BC38::				; CODE XREF: _spectr_1BC2D+7j
		mov	ah, [bx+64h]
		cmp	ah, 90
		jb	short loc_1BC42
		mov	ah, 90

loc_1BC42::				; CODE XREF: _spectr_1BC2D+11j
		cmp	al, ah
		jz	short loc_1BC92
		jb	short loc_1BC70
		movzx	dx, ah
		shl	dx, 6
		mov	di, dx
		shl	dx, 2
		add	di, dx
		neg	di
		add	di, bp
		mov	dl, al
		sub	dl, ah
		mov	al, ah

loc_1BC5F::				; CODE XREF: _spectr_1BC2D+3Fj
		mov	es:[di], ax
		inc	al
		inc	ah
		sub	di, 140h
		dec	dl
		jnz	short loc_1BC5F
		jmp	short loc_1BC92
; ---------------------------------------------------------------------------

loc_1BC70::				; CODE XREF: _spectr_1BC2D+19j
		movzx	dx, al
		shl	dx, 6
		mov	di, dx
		shl	dx, 2
		add	di, dx
		neg	di
		add	di, bp
		mov	dl, ah
		sub	dl, al
		xor	ax, ax

loc_1BC87::				; CODE XREF: _spectr_1BC2D+63j
		mov	es:[di], ax
		sub	di, 140h
		dec	dl
		jnz	short loc_1BC87

loc_1BC92::				; CODE XREF: _spectr_1BC2D+17j
					; _spectr_1BC2D+41j
		cmp	byte ptr [bx+12Ch], 0
		jz	short loc_1BCDF
		dec	byte ptr [bx+12Ch]
		jnz	short loc_1BCC0
		movzx	dx, byte ptr [bx+0C8h]
		cmp	dl, 5Ah	; 'Z'
		jb	short loc_1BCAB
		mov	dl, 5Ah	; 'Z'

loc_1BCAB::				; CODE XREF: _spectr_1BC2D+7Aj
		shl	dx, 6
		mov	di, dx
		shl	dx, 2
		add	di, dx
		neg	di

loc_1BCB7::
		add	di, bp
		mov	word ptr es:[di], 0
		jmp	short loc_1BCDF
; ---------------------------------------------------------------------------

loc_1BCC0::				; CODE XREF: _spectr_1BC2D+70j
		movzx	dx, byte ptr [bx+0C8h]
		cmp	dl, 5Ah	; 'Z'
		jb	short loc_1BCCC
		mov	dl, 5Ah	; 'Z'

loc_1BCCC::				; CODE XREF: _spectr_1BC2D+9Bj
		shl	dx, 6
		mov	di, dx
		shl	dx, 2
		add	di, dx
		neg	di
		add	di, bp
		mov	word ptr es:[di], 0FEFEh

loc_1BCDF::				; CODE XREF: _spectr_1BC2D+6Aj
					; _spectr_1BC2D+91j
		inc	bx
		add	bp, 3
		dec	cx
		jnz	loc_1BC30
		retn
_spectr_1BC2D endp


; =============== S U B	R O U T	I N E =======================================


_spectr_1BCE9 proc near	; CODE XREF: _f5_draw_spectr+5BDp
					; _f5_draw_spectr+5CFp
		mov	al, [bx]
		cmp	al, 90
		jb	short loc_1BCF1
		mov	al, 90

loc_1BCF1::				; CODE XREF: _spectr_1BCE9+4j
		mov	ah, [bx+64h]
		cmp	ah, 90
		jb	short loc_1BCFB
		mov	ah, 90

loc_1BCFB::				; CODE XREF: _spectr_1BCE9+Ej
		cmp	al, ah
		jz	short locret_1BD67
		jb	short loc_1BD3E
		movzx	dx, ah
		shl	dx, 6
		mov	di, dx
		shl	dx, 2
		add	di, dx
		neg	di
		add	di, bp
		mov	dl, al
		sub	dl, ah
		shr	ah, 1
		mov	eax, 0FCFCFCFCh
		jnb	short loc_1BD26
		or	eax, 1010101h

loc_1BD26::				; CODE XREF: _spectr_1BCE9+35j
					; _spectr_1BCE9+52j
		mov	es:[di], eax
		mov	es:[di+4], eax
		sub	di, 140h
		xor	eax, 1010101h
		dec	dl
		jnz	short loc_1BD26
		retn
; ---------------------------------------------------------------------------

loc_1BD3E::				; CODE XREF: _spectr_1BCE9+16j
		movzx	dx, al
		shl	dx, 6
		mov	di, dx
		shl	dx, 2
		add	di, dx
		neg	di
		add	di, bp
		mov	dl, ah
		sub	dl, al
		xor	eax, eax

loc_1BD56::				; CODE XREF: _spectr_1BCE9+7Cj
		mov	es:[di], eax
		mov	es:[di+4], eax
		sub	di, 140h
		dec	dl
		jnz	short loc_1BD56

locret_1BD67::				; CODE XREF: _spectr_1BCE9+14j
		retn
_spectr_1BCE9 endp

; ---------------------------------------------------------------------------

_f6_draw::				; DATA XREF: _f6_undoc+6o
		call	_txt_draw_bottom
		lfs	bx, _segfsbx_1DE28
		les	di, _videomempointer
		assume es:nothing
		add	di, 3C4h
		mov	cx, _amount_of_x
		cmp	cx, _word_1DE6E

loc_1BD80::
		jbe	short loc_1BD86
		mov	cx, _word_1DE6E

loc_1BD86::				; CODE XREF: seg001:loc_1BD80j
		xor	ax, ax

loc_1BD88::				; CODE XREF: seg001:2DDCj
		push	ax
		push	cx
		push	di
		mov	dl, al
		add	al, 31h	; '1'
		cmp	al, 39h	; '9'
		jbe	short loc_1BD95
		add	al, 7

loc_1BD95::				; CODE XREF: seg001:2D41j
		mov	ah, 1Eh
		cmp	dl, _byte_1DE84
		jz	short loc_1BD9F
		mov	ah, 7Eh	; '~'

loc_1BD9F::				; CODE XREF: seg001:2D4Bj
		mov	es:[di+2], ax
		mov	al, 20h	; ' '
		mov	es:[di], ax
		mov	es:[di+4], ax
		add	di, 6
		mov	si, offset _buffer_1 ; 2800h
		mov	eax, 0C4C4C4C4h
		mov	word ptr [si], 2020h
		mov	[si+2],	eax
		mov	[si+6],	eax
		mov	[si+0Ah], eax
		mov	[si+0Eh], eax
		mov	word ptr [si+12h], 20C4h
		lea	bp, [si+14h]
		mov	al, fs:[bx+3Ah]
		mov	dl, al
		shr	al, 3
		and	ax, 1Fh
		add	si, 2
		add	si, ax
		mov	al, 4Dh	; 'M'
		cmp	dl, 40h	; '@'
		jz	short loc_1BDF2
		mov	al, 4Ch	; 'L'
		jb	short loc_1BDF2
		mov	al, 52h	; 'R'

loc_1BDF2::				; CODE XREF: seg001:2D9Aj seg001:2D9Ej
		mov	[si], al
		mov	si, bp
		mov	dword ptr [si],	20202020h
		cld
		mov	al, fs:[bx+3Ah]
		sub	al, 40h	; '@'
		js	short loc_1BE07
		inc	si

loc_1BE07::				; CODE XREF: seg001:2DB4j
		cmp	al, 0F7h ; '˜'
		jl	short loc_1BE10
		cmp	al, 9
		jg	short loc_1BE10
		inc	si

loc_1BE10::				; CODE XREF: seg001:2DB9j seg001:2DBDj
		push	bx
		call	_my_i8toa10
		pop	bx
		mov	byte ptr [si], 0
		mov	si, offset _buffer_1 ; 2800h
		mov	ah, 7Eh	; '~'
		call	_put_message
		pop	di
		pop	cx
		pop	ax
		add	di, 0A0h ; '†'
		add	bx, 50h	; 'P'
		inc	ax
		dec	cx
		jnz	loc_1BD88
		retn
; ---------------------------------------------------------------------------
		push	ax
		shr	al, 4
		call	_hex_1BE39
		pop	ax

; =============== S U B	R O U T	I N E =======================================


_hex_1BE39 proc near		; CODE XREF: seg001:2DE5p
		and	al, 0Fh
		or	al, 30h
		cmp	al, 39h	; '9'
		jbe	short loc_1BE43
		add	al, 7

loc_1BE43::				; CODE XREF: _hex_1BE39+6j
		mov	es:[di], ax
		add	di, 2
		retn
_hex_1BE39 endp


; =============== S U B	R O U T	I N E =======================================

; Attributes: bp-based frame




; =============== S U B	R O U T	I N E =======================================


; int __usercall _message_1BE77<eax>(char *_msg<esi>)
_message_1BE77 proc near	; CODE XREF: _start+2FDp _start+420p ...
		push	ax
		push	si
		mov	ch, al
		sub	ch, 2
		mov	dh, al
		add	dh, 2
		mov	ah, 0FFh

loc_1BE85::				; CODE XREF: _message_1BE77+15j
		mov	al, [si]
		inc	ah
		inc	si
		or	al, al
		jnz	short loc_1BE85
		mov	cl, 4Eh	; 'N'
		sub	cl, ah
		shr	ah, 1
		mov	dl, 2Ah	; '*'
		add	dl, ah
		mov	al, ch
		inc	al
		mov	ah, 0A0h ; '†'
		mul	ah
		movzx	di, cl
		and	di, 0FFFEh
		add	ax, di
		add	ax, 0A4h ; '§'
		push	ax
		shr	cl, 1
		mov	bl, 78h	; 'x'
		mov	ax, 7F03h
		call	_draw_frame
		pop	ax
		pop	si		; str
		les	di, _videomempointer
		add	di, ax
		pop	ax
		call	_text_1BF69
		retn
_message_1BE77 endp


; =============== S U B	R O U T	I N E =======================================


_draw_frame proc near		; CODE XREF: _start+1FEp _start+278p ...
		push	es
		les	bp, _videomempointer
		movzx	di, cl
		movzx	si, ch
		imul	si, 80
		add	di, si
		shl	di, 1
		add	di, bp
		movzx	bp, dl
		inc	bp
		movzx	si, cl
		sub	bp, si
		jbe	short loc_1BF57
		sub	bp, 2
		jb	short loc_1BF57
		mov	dl, dh
		inc	dl
		sub	dl, ch
		jbe	short loc_1BF57
		xor	dh, dh
		sub	dx, 2
		jb	short loc_1BF57
		cmp	al, 6
		jnb	short loc_1BF57
		movzx	si, al
		imul	si, 6
		add	si, offset _frameborder ; "	€€€€€€…ª»ºÕ∫⁄ø¿Ÿƒ≥÷∑”Ωƒ∫’∏‘æÕ≥"
		mov	al, [si]
		cld
		stosw
		mov	cx, bp
		jcxz	short loc_1BF11
		mov	al, [si+4]
		rep stosw

loc_1BF11::				; CODE XREF: _draw_frame+47j
		xchg	bl, ah
		mov	al, [si+1]
		stosw
		or	dx, dx
		jz	short loc_1BF3A

loc_1BF1B::				; CODE XREF: _draw_frame+75j
		xchg	bl, ah
		add	di, 156
		sub	di, bp
		sub	di, bp
		mov	al, [si+5]
		stosw
		mov	cx, bp
		jcxz	short loc_1BF31
		mov	al, ' '
		rep stosw

loc_1BF31::				; CODE XREF: _draw_frame+68j
		xchg	bl, ah
		mov	al, [si+5]
		stosw
		dec	dx
		jnz	short loc_1BF1B

loc_1BF3A::				; CODE XREF: _draw_frame+56j
		xchg	bl, ah
		add	di, 156
		sub	di, bp
		sub	di, bp
		mov	al, [si+2]
		stosw
		xchg	bl, ah
		mov	cx, bp
		jcxz	short loc_1BF53
		mov	al, [si+4]
		rep stosw

loc_1BF53::				; CODE XREF: _draw_frame+89j
		mov	al, [si+3]
		stosw

loc_1BF57::				; CODE XREF: _draw_frame+1Dj
					; _draw_frame+22j ...
		pop	es
		retn
_draw_frame endp


; =============== S U B	R O U T	I N E =======================================


; void __usercall _write_scr(char *str<esi>)
_write_scr proc near		; CODE XREF: _start+20Bp _start+72Ap ...
		mov	bp, di
		add	di, [si]
		add	si, 2
		jmp	short _n2_setcolor
_write_scr endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR _text_1BF69

_n1_movepos::				; CODE XREF: _text_1BF69+9j
		mov	di, [si]
		add	di, bp
		add	si, 2
; END OF FUNCTION CHUNK	FOR _text_1BF69

; =============== S U B	R O U T	I N E =======================================


; void __usercall _text_1BF69(char *str<esi>)
_text_1BF69 proc near		; CODE XREF: _filelist_198B8+102p
					; _filelist_198B8+112p ...

; FUNCTION CHUNK AT 2F12 SIZE 00000007 BYTES

		mov	al, [si]
		inc	si		; str
		or	al, al
		jz	short locret_1BF85
		cmp	al, 1
		jz	short _n1_movepos
		cmp	al, 2
		jz	short _n2_setcolor
		mov	es:[di], ax
		add	di, 2
		jmp	short _text_1BF69
; ---------------------------------------------------------------------------

_n2_setcolor::				; CODE XREF: _write_scr+7j
					; _text_1BF69+Dj
		lodsb
		mov	ah, al
		jmp	short _text_1BF69
; ---------------------------------------------------------------------------

locret_1BF85::				; CODE XREF: _text_1BF69+5j
		retn
_text_1BF69 endp ; sp-analysis failed

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR _put_message

loc_1BF86::				; CODE XREF: _put_message+4j
		stosw
; END OF FUNCTION CHUNK	FOR _put_message

; =============== S U B	R O U T	I N E =======================================


; void __usercall _put_message(char color<ah>, char *str<esi>, void *videoptr<edi>)
_put_message proc near	; CODE XREF: _start+2A8p _start+2EDp ...

; FUNCTION CHUNK AT 2F36 SIZE 00000001 BYTES

		cld
		lodsb
		or	al, al
		jnz	short loc_1BF86
		retn
_put_message endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


; void __usercall _put_message2(char *str<esi>, void *buf<edi>)
_put_message2 proc near	; CODE XREF: _put_message2+6j
		stosw
		cld
		lods	byte ptr fs:[si]
		or	al, al
		jnz	short _put_message2
		retn
_put_message2 endp


; =============== S U B	R O U T	I N E =======================================


_loadcfg proc	near		; CODE XREF: _start:loc_190D3p
		mov	dx, offset _sIplay_cfg ;	"C:\\IPLAY.CFG"
		mov	ax, 3D00h
		int	21h		; DOS -	2+ - OPEN DISK FILE WITH HANDLE
					; DS:DX	-> ASCIZ filename
					; AL = access mode
					; 0 - read
		jb	short loc_1BFE3
		mov	bx, ax
		mov	dx, offset _cfg_buffer
		mov	cx, 4
		mov	ah, 3Fh	; '?'
		push	bx
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		pop	bx
		jb	short loc_1BFC9
		mov	eax, _dword_1DCEC
		cmp	eax, [dword ptr	_cfg_buffer]
		stc
		jnz	short loc_1BFC9
		mov	dx, offset _snd_card_type
		mov	cx, 0Ch
		mov	ah, 3Fh	; '?'
		push	bx
		int	21h		; DOS -	2+ - READ FROM FILE WITH HANDLE
					; BX = file handle, CX = number	of bytes to read
					; DS:DX	-> buffer
		pop	bx

loc_1BFC9::				; CODE XREF: _loadcfg+18j _loadcfg+24j
		pushf
		mov	ah, 3Eh
		int	21h		; DOS -	2+ - CLOSE A FILE WITH HANDLE
					; BX = file handle
		popf
		jb	short loc_1BFE3
		mov	si, offset _snd_card_type
		mov	cx, 0Ch
		xor	al, al

loc_1BFD9::				; CODE XREF: _loadcfg+45j
		add	al, [si]
		inc	si
		loop	loc_1BFD9
		or	al, al
		jnz	short loc_1BFE3
		retn
; ---------------------------------------------------------------------------

loc_1BFE3::				; CODE XREF: _loadcfg+8j _loadcfg+38j ...
		mov	ax, cs
		mov	ds, ax
		assume ds:seg001
		mov	dx, offset _aConfigFileNotF ; "Config file not found. Run ISETUP	first"...
		mov	ah, 9
		int	21h		; DOS -	PRINT STRING
					; DS:DX	-> string terminated by	"$"
		mov	ax, 4C01h
		int	21h		; DOS -	2+ - QUIT WITH EXIT CODE (EXIT)
_loadcfg endp			; AL = exit code

; ---------------------------------------------------------------------------
_aConfigFileNotF	db 'Config file not found. Run ISETUP first',0Dh,0Ah,'$'
					; DATA XREF: _loadcfg+50o
		db 0Dh,0Ah,'$'
		assume ds:dseg

; =============== S U B	R O U T	I N E =======================================


; char *__usercall _getexename<esi>()
_getexename proc near		; CODE XREF: _init_vga_waves+2Fp
		mov	es, _esseg_atstart
		mov	es, word ptr es:[2Ch]
		xor	di, di
		xor	al, al
		cld
		mov	cx, 8000h

loc_1C031::				; CODE XREF: _getexename+18j
		repne scasb
		jnz	short loc_1C050
		cmp	es:[di], al
		jnz	short loc_1C031
		mov	cx, es:[di+1]
		jcxz	short loc_1C050
		add	di, 3

loc_1C043::				; CODE XREF: _getexename+2Cj
		mov	al, es:[di]
		mov	[si], al
		inc	di
		inc	si
		or	al, al
		jnz	short loc_1C043
		clc
		retn
; ---------------------------------------------------------------------------

loc_1C050::				; CODE XREF: _getexename+13j
					; _getexename+1Ej
		stc
		retn
_getexename endp


; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


_set_egasequencer proc near	; CODE XREF: _init_vga_waves+91p
					; _init_f5_spectr+13p
		mov	dx, 3C4h
		mov	al, 1
		out	dx, al		; EGA: sequencer address reg
					; clocking mode. Data bits:
					; 0: 1=8 dots/char; 0=9	dots/char
					; 1: CRT bandwidth: 1=low; 0=high
					; 2: 1=shift every char; 0=every 2nd char
					; 3: dot clock:	1=halved
		inc	dl
		in	al, dx		; EGA port: sequencer data register
		or	al, 100000b	; 9 dots/char
					; high bandwidth
					; shift	every 2	char
					; dot clock
					; .
					; dont know
		out	dx, al		; EGA port: sequencer data register
		retn
_set_egasequencer endp


; =============== S U B	R O U T	I N E =======================================


_graph_1C070 proc near	; CODE XREF: _init_vga_waves+12Cp
					; _init_f5_spectr+91p
		mov	dx, 3C4h
		mov	al, 1
		out	dx, al		; EGA: sequencer address reg
					; clocking mode. Data bits:
					; 0: 1=8 dots/char; 0=9	dots/char
					; 1: CRT bandwidth: 1=low; 0=high
					; 2: 1=shift every char; 0=every 2nd char
					; 3: dot clock:	1=halved
		inc	dl
		in	al, dx		; EGA port: sequencer data register
		and	al, 0DFh
		out	dx, al		; EGA port: sequencer data register
		retn
_graph_1C070 endp

		assume ds:seg003

; =============== S U B	R O U T	I N E =======================================


_int9_keyb proc far		; DATA XREF: _start+133o
		cmp	cs:[_byte_1C1B8], 1
		jz	loc_1C11F
		push	ax
		in	al, 60h		; 8042 keyboard	controller data	register
		cmp	al, 0E0h ; '‡'
		jz	_l_escaped_scancode
		cmp	al, 0E1h ; '·'
		jz	_l_escaped_scancode
		mov	ah, cs:[_prev_scan_code]
		or	ah, ah
		jz	short loc_1C0A5
		mov	cs:[_prev_scan_code], 0

loc_1C0A5::				; CODE XREF: _int9_keyb+20j
		cmp	al, 36h	; '6'
		jz	short _l_rshift
		cmp	al, 0B6h ; '∂'
		jz	short _l_rshiftup
		cmp	al, 2Ah	; '*'
		jz	short _l_lshift
		cmp	al, 0AAh ; '™'
		jz	short _l_lshiftup
		cmp	al, 1Dh
		jz	short _l_ctrl
		cmp	al, 9Dh	; 'ù'
		jz	short _l_lctrlup
		cmp	al, 38h	; '8'
		jz	short _l_alt
		cmp	al, 0B8h ; '∏'
		jz	short _l_altup
		mov	cs:[_key_code], ax

loc_1C0C9::				; CODE XREF: _int9_keyb+62j
					; _int9_keyb+6Aj ...
		in	al, 61h		; PC/XT	PPI port B bits:
					; 0: Tmr 2 gate	ÕÀÕ OR	03H=spkr ON
					; 1: Tmr 2 data	Õº  AND	0fcH=spkr OFF
					; 3: 1=read high switches
					; 4: 0=enable RAM parity checking
					; 5: 0=enable I/O channel check
					; 6: 0=hold keyboard clock low
					; 7: 0=enable kbrd
		or	al, 80h
		out	61h, al		; PC/XT	PPI port B bits:
					; 0: Tmr 2 gate	ÕÀÕ OR	03H=spkr ON
					; 1: Tmr 2 data	Õº  AND	0fcH=spkr OFF
					; 3: 1=read high switches
					; 4: 0=enable RAM parity checking
					; 5: 0=enable I/O channel check
					; 6: 0=hold keyboard clock low
					; 7: 0=enable kbrd
		and	al, 7Fh
		out	61h, al		; PC/XT	PPI port B bits:
					; 0: Tmr 2 gate	ÕÀÕ OR	03H=spkr ON
					; 1: Tmr 2 data	Õº  AND	0fcH=spkr OFF
					; 3: 1=read high switches
					; 4: 0=enable RAM parity checking
					; 5: 0=enable I/O channel check
					; 6: 0=hold keyboard clock low
					; 7: 0=enable kbrd
		mov	al, 20h	; ' '
		out	20h, al		; Interrupt controller,	8259A.
		pop	ax
		iret
; ---------------------------------------------------------------------------

_l_rshift::				; CODE XREF: _int9_keyb+2Aj
		or	cs:[_keyb_switches], 1
		jmp	short loc_1C0C9
; ---------------------------------------------------------------------------

_l_rshiftup::				; CODE XREF: _int9_keyb+2Ej
		and	cs:[_keyb_switches], not	1
		jmp	short loc_1C0C9
; ---------------------------------------------------------------------------

_l_lshift::				; CODE XREF: _int9_keyb+32j
		or	cs:[_keyb_switches], 10b
		jmp	short loc_1C0C9
; ---------------------------------------------------------------------------

_l_lshiftup::				; CODE XREF: _int9_keyb+36j
		and	cs:[_keyb_switches], not	10b
		jmp	short loc_1C0C9
; ---------------------------------------------------------------------------

_l_ctrl::					; CODE XREF: _int9_keyb+3Aj
		or	cs:[_keyb_switches], 100b
		jmp	short loc_1C0C9
; ---------------------------------------------------------------------------

_l_lctrlup::				; CODE XREF: _int9_keyb+3Ej
		and	cs:[_keyb_switches], not	100b
		jmp	short loc_1C0C9
; ---------------------------------------------------------------------------
		assume ds:dseg

_l_alt::					; CODE XREF: _int9_keyb+42j
		or	cs:[_keyb_switches], 1000b
		jmp	short loc_1C0C9
; ---------------------------------------------------------------------------

_l_altup::				; CODE XREF: _int9_keyb+46j
		and	cs:[_keyb_switches], not	1000b
		jmp	short loc_1C0C9
; ---------------------------------------------------------------------------

_l_escaped_scancode::			; CODE XREF: _int9_keyb+Fj
					; _int9_keyb+15j
		mov	cs:[_prev_scan_code], al
		jmp	short loc_1C0C9
; ---------------------------------------------------------------------------

loc_1C11F::				; CODE XREF: _int9_keyb+6j
		jmp	cs:[_oint9_1C1A4]
_int9_keyb endp


; =============== S U B	R O U T	I N E =======================================


_get_keybsw proc near
		push	es
		xor	ax, ax
		mov	es, ax
		assume es:nothing
		mov	ax, es:[17h]
		mov	cs:[_keyb_switches], ax
		pop	es
		assume es:nothing
		retn
_get_keybsw endp


; =============== S U B	R O U T	I N E =======================================


_set_keybsw proc near
		push	es
		xor	ax, ax
		mov	es, ax
		assume es:nothing
		mov	ax, cs:[_keyb_switches]
		mov	es:[17h], ax
		pop	es
		assume es:nothing
		retn
_set_keybsw endp

; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
_key_code	dw 0			; DATA XREF: _start:loc_193FFr
					; _start+37Aw ...
_keyb_switches	dw 0			; DATA XREF: _start+5D8r
					; _keyb_screen_loop+1DEr ...
_prev_scan_code	db 0			; DATA XREF: _int9_keyb+19r
					; _int9_keyb+22w ...

; =============== S U B	R O U T	I N E =======================================


_int24 proc far		; DATA XREF: _start+13Bo
		mov	al, 3
		test	ah, 8
		jnz	short locret_1C159
		mov	al, 0
		test	ah, 20h
		jnz	short locret_1C159
		mov	al, 1

locret_1C159::				; CODE XREF: _int24+5j _int24+Cj
		iret
_int24 endp


; =============== S U B	R O U T	I N E =======================================


_int2f_checkmyself proc far	; DATA XREF: _start+143o
		pushf
		cmp	ax, 60FFh
		jz	short _lyesitsme	; DS

loc_1C160::				; CODE XREF: _int2f_checkmyself+10j
					; _int2f_checkmyself+16j
		popf
		jmp	cs:[_oint2f_1C1B4]
; ---------------------------------------------------------------------------

_lyesitsme::				; CODE XREF: _int2f_checkmyself+4j
		cmp	bx, 5344h	; DS
		jnz	short loc_1C160
		cmp	cx, 4D50h	; PM
		jnz	short loc_1C160
		popf
		cmp	dl, 1
		jz	short loc_1C17C
		mov	ax, 4F4Bh	; KO
		iret
; ---------------------------------------------------------------------------

loc_1C17C::				; CODE XREF: _int2f_checkmyself+1Cj
		push	ax
		push	ds
		mov	ax, dseg
		mov	ds, ax
		mov	_byte_1DE7C, 1
		pop	ds
		pop	ax
		iret
_int2f_checkmyself endp


; =============== S U B	R O U T	I N E =======================================


_int1a_timer proc near	; DATA XREF: _dosexec+47o
		pushf
		or	ah, ah
		jnz	short loc_1C19C
		pushad
		push	ds
		push	es
		call	_rereadrtc_settmr
		pop	es
		pop	ds
		popad

loc_1C19C::				; CODE XREF: _int1a_timer+3j
		popf
		jmp	cs:[_int1Avect]
_int1a_timer endp

; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
_oint9_1C1A4	dd 0			; DATA XREF: _start+106w _start+1E0r ...
_int1Avect	dd 0			; DATA XREF: _int1a_timer+12r
					; _dosexec+38w ...
_oint24_1C1AC	dd 0			; DATA XREF: _start+115w _start+1D4r ...
		db 4 dup(0)
_oint2f_1C1B4	dd 0			; DATA XREF: _start+124w _start+1C8r ...
_byte_1C1B8	db 0			; DATA XREF: _int9_keybr _dosexec+58w ...

; =============== S U B	R O U T	I N E =======================================


_dosexec proc	near		; CODE XREF: _start+747p
					; _keyb_screen_loop+338p
		mov	ax, 3
		int	10h		; - VIDEO - SET	VIDEO MODE
					; AL = mode
		call	_txt_enableblink
		mov	cx, 0
		mov	dx, 94Fh
		mov	bl, 78h	; 'x'
		mov	ax, 7F03h
		call	_draw_frame
		call	_txt_draw_top_title
		mov	si, offset _word_1D26D ;	str
		les	di, _videomempointer
		call	_write_scr
		mov	dx, 0A00h
		xor	bh, bh
		mov	ah, 2
		int	10h		; - VIDEO - SET	CURSOR POSITION
					; DH,DL	= row, column (0,0 = upper left)
					; BH = page number
		test	_byte_1DE78, 2
		jz	short loc_1C209
		mov	ax, 351Ah
		int	21h		; DOS -	2+ - GET INTERRUPT VECTOR
					; AL = interrupt number
					; Return: ES:BX	= value	of interrupt vector
		mov	word ptr cs:[_int1Avect], bx
		mov	word ptr cs:[_int1Avect+2], es
		push	ds
		mov	ax, cs
		mov	ds, ax
		assume ds:seg001
		mov	dx, offset _int1a_timer
		mov	ax, 251Ah
		int	21h		; DOS -	SET INTERRUPT VECTOR
					; AL = interrupt number
					; DS:DX	= new vector to	be used	for specified interrupt
		pop	ds
		assume ds:dseg

loc_1C209::				; CODE XREF: _dosexec+31j
		mov	si, offset _byte_1DD3F ;	str
		call	_dosgetcurdir
		mov	al, 1
		mov	cs:[_byte_1C1B8], al
		call	sub_12D35
		mov	es, _esseg_atstart
		mov	ax, es:[2Ch]
		mov	_word_24445, ax
		call	_get_comspec
		jb	short loc_1C23E
		mov	dx, di
		push	ds
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	bx, offset _word_24445
		mov	ds, _word_24445
		mov	ax, 4B00h
		int	21h		; DOS -	2+ - LOAD OR EXECUTE (EXEC)
					; DS:DX	-> ASCIZ filename
					; ES:BX	-> parameter block
					; AL = subfunc:	load & execute program
		pop	ds

loc_1C23E::				; CODE XREF: _dosexec+6Fj
		mov	al, 0
		mov	cs:[_byte_1C1B8], al
		call	sub_12D35
		test	_byte_1DE78, 2
		jz	short loc_1C25C
		push	ds
		lds	dx, cs:[_int1Avect]
		mov	ax, 251Ah
		int	21h		; DOS -	SET INTERRUPT VECTOR
					; AL = interrupt number
					; DS:DX	= new vector to	be used	for specified interrupt
		pop	ds

loc_1C25C::				; CODE XREF: _dosexec+95j
		mov	si, offset _byte_1DD3F ;	str
		call	_doschdir
		mov	_byte_1DE70, 0FFh
		retn
_dosexec endp


; =============== S U B	R O U T	I N E =======================================


_get_comspec proc near	; CODE XREF: _dosexec+6Cp
		mov	es, _esseg_atstart
		assume es:nothing
		mov	es, word ptr es:[2Ch]
		xor	di, di

loc_1C273::				; CODE XREF: _get_comspec+2Fj
		cmp	byte ptr es:[di], 0
		stc
		jz	short locret_1C29D
		cmp	dword ptr es:[di], 534D4F43h ; COMSPEC=
		jnz	short loc_1C28F
		cmp	dword ptr es:[di+4], 3D434550h
		jz	short loc_1C299

loc_1C28F::				; CODE XREF: _get_comspec+1Aj
					; _get_comspec+2Cj
		inc	di
		cmp	byte ptr es:[di], 0
		jnz	short loc_1C28F
		inc	di
		jmp	short loc_1C273
; ---------------------------------------------------------------------------

loc_1C299::				; CODE XREF: _get_comspec+25j
		add	di, 8
		clc

locret_1C29D::				; CODE XREF: _get_comspec+10j
		retn
_get_comspec endp


; =============== S U B	R O U T	I N E =======================================


_find_mods proc near		; CODE XREF: _start+1A1p
					; _modules_search+65p ...
		mov	ax, ds
		mov	es, ax
		assume es:dseg
		mov	di, offset _buffer_1DB6C
		mov	si, di
		mov	cx, 120
		xor	al, al
		cld
		repne scasb
		jnz	short loc_1C321
		dec	di
		mov	_word_1DE4A, di

loc_1C2B6::				; CODE XREF: _find_mods+2Aj
		mov	al, [di-1]
		or	al, al
		jz	short loc_1C2CA
		cmp	al, '\'
		jz	short loc_1C2CA
		cmp	al, ':'
		jz	short loc_1C2CA
		dec	di
		cmp	si, di
		jb	short loc_1C2B6

loc_1C2CA::				; CODE XREF: _find_mods+1Dj
					; _find_mods+21j ...
		sub	di, si
		mov	_word_1DE4C, di
		mov	dx, offset _buffer_1DBEC
		mov	ah, 1Ah
		int	21h		; DOS -	SET DISK TRANSFER AREA ADDRESS
					; DS:DX	-> disk	transfer buffer
		mov	dx, offset _buffer_1DB6C
		mov	cx, _word_1DE4E
		mov	ah, 4Eh
		int	21h		; DOS -	2+ - FIND FIRST	ASCIZ (FINDFIRST)
					; CX = search attributes
					; DS:DX	-> ASCIZ filespec
					; (drive, path,	and wildcards allowed)
		jnb	short loc_1C309
		mov	si, offset _a_mod_nst_669_s ; ".MOD.NST.669.STM.S3M.MTM.PSM.WOW.INR.FA"...

loc_1C2E7::				; CODE XREF: _find_mods+69j
		cmp	byte ptr [si], 0
		jz	short loc_1C321
		mov	di, _word_1DE4A
		mov	eax, [si]
		mov	[di], eax
		mov	byte ptr [di+4], 0
		add	si, 4
		mov	dx, offset _buffer_1DB6C
		mov	cx, 2
		mov	ah, 4Eh
		int	21h		; DOS -	2+ - FIND FIRST	ASCIZ (FINDFIRST)
					; CX = search attributes
					; DS:DX	-> ASCIZ filespec
					; (drive, path,	and wildcards allowed)
		jb	short loc_1C2E7

loc_1C309::				; CODE XREF: _find_mods+44j
					; _dosfindnext+Bj
		mov	ax, ds
		mov	es, ax
		mov	si, offset _byte_1DC0A
		mov	di, offset _buffer_1DB6C
		add	di, _word_1DE4C
		cld
		mov	cx, 3
		rep movsd
		movsb
		clc
		retn
; ---------------------------------------------------------------------------

loc_1C321::				; CODE XREF: _find_mods+11j
					; _find_mods+4Cj
		mov	_byte_1DE7E, 2
		mov	word ptr [_messagepointer], offset _aModuleNotFound ; "Module not	found.\r\n$"
		mov	word ptr [_messagepointer+2], ds
		stc
		retn
_find_mods endp


; =============== S U B	R O U T	I N E =======================================


_dosfindnext proc near	; CODE XREF: _modules_search+CBp
					; _modules_search+247p ...
		mov	dx, offset _buffer_1DBEC
		mov	ah, 1Ah
		int	21h		; DOS -	SET DISK TRANSFER AREA ADDRESS
					; DS:DX	-> disk	transfer buffer
		mov	ah, 4Fh
		int	21h		; DOS -	2+ - FIND NEXT ASCIZ (FINDNEXT)
					; [DTA]	= data block from
					; last AH = 4Eh/4Fh call
		jnb	short loc_1C309
		retn
_dosfindnext endp


; =============== S U B	R O U T	I N E =======================================


_video_prp_mtr_positn proc near ; CODE XREF: _read_module+C5p
					; _init_vga_waves+14Cp ...
		pushf
		cli
		mov	_byte_1DE79, 0
		mov	_byte_1DE7A, 0
		lfs	bx, _segfsbx_1DE28
		mov	cx, _amount_of_x

loc_1C355::				; CODE XREF: _video_prp_mtr_positn+2Dj
		mov	al, fs:[bx+3Ah]
		cmp	al, 40h	; '@'
		jb	short loc_1C365
		inc	_byte_1DE7A
		cmp	al, 40h	; '@'
		ja	short loc_1C369

loc_1C365::				; CODE XREF: _video_prp_mtr_positn+1Bj
		inc	_byte_1DE79

loc_1C369::				; CODE XREF: _video_prp_mtr_positn+23j
		add	bx, 50h	; 'P'
		dec	cx
		jnz	short loc_1C355
		movzx	ecx, _byte_1DE79
		cmp	cl, _byte_1DE7A
		ja	short loc_1C37F
		mov	cl, _byte_1DE7A

loc_1C37F::				; CODE XREF: _video_prp_mtr_positn+39j
		mov	al, 3
		cmp	cl, 2
		jbe	short loc_1C396
		mov	al, 2
		cmp	cl, 4
		jbe	short loc_1C396
		mov	al, 1
		cmp	cl, 8
		jbe	short loc_1C396
		mov	al, 0

loc_1C396::				; CODE XREF: _video_prp_mtr_positn+44j
					; _video_prp_mtr_positn+4Bj ...
		add	al, 8
		mov	_byte_1DE81, al
		xor	edx, edx
		mov	eax, 18350080
		jcxz	short loc_1C3A9
		div	ecx

loc_1C3A9::				; CODE XREF: _video_prp_mtr_positn+64j
		mov	ebp, eax
		mov	si, offset _x_storage
		mov	cx, _amount_of_x
		lfs	bx, _segfsbx_1DE28
		mov	edi, ebp
		shr	edi, 1
		mov	edx, edi

loc_1C3C1::				; CODE XREF: _video_prp_mtr_positn+B5j
		cmp	byte ptr fs:[bx+3Ah], 40h ; '@'
		jz	short loc_1C3EE
		ja	short loc_1C3DC
		mov	eax, edi
		shr	eax, 16
		imul	ax, 80
		add	ax, 1
		add	edi, ebp
		jmp	short loc_1C3EC
; ---------------------------------------------------------------------------

loc_1C3DC::				; CODE XREF: _video_prp_mtr_positn+88j
		mov	eax, edx
		shr	eax, 16
		imul	ax, 80
		add	ax, 42
		add	edx, ebp

loc_1C3EC::				; CODE XREF: _video_prp_mtr_positn+9Aj
		mov	[si], ax

loc_1C3EE::				; CODE XREF: _video_prp_mtr_positn+86j
		add	si, 2
		add	bx, 50h	; 'P'
		dec	cx
		jnz	short loc_1C3C1
		mov	si, offset _x_storage
		mov	cx, _amount_of_x
		lfs	bx, _segfsbx_1DE28
		cmp	edi, edx
		ja	short loc_1C40B
		mov	edi, edx

loc_1C40B::				; CODE XREF: _video_prp_mtr_positn+C6j
					; _video_prp_mtr_positn+EBj
		cmp	byte ptr fs:[bx+3Ah], 40h ; '@'
		jnz	short loc_1C424
		mov	eax, edi
		shr	eax, 16
		imul	ax, 80
		add	ax, 21
		add	edi, ebp
		mov	[si], ax

loc_1C424::				; CODE XREF: _video_prp_mtr_positn+D0j
		add	si, 2
		add	bx, 50h	; 'P'
		dec	cx
		jnz	short loc_1C40B
		popf
		retn
_video_prp_mtr_positn endp


; =============== S U B	R O U T	I N E =======================================


_callsubx proc near		; CODE XREF: _start:loc_19050p
					; _start+1A6p ...
		mov	al, _snd_card_type
		mov	dx, _snd_base_port_0
		mov	cl, _irq_number_1
		mov	ch, _dma_channel_1
		mov	ah, _freq_1DCF6
		movzx	di, _byte_1DCFB
		mov	si, _configword
		mov	bl, _byte_1DCF7
		mov	bh, _byte_1DCF8
		call	sub_12DA8
		mov	_byte_1DE7E, 1
		mov	word ptr [_messagepointer], dx
		mov	word ptr [_messagepointer+2], fs
		jb	short locret_1C4A7
		mov	_byte_1DE7E, 0
		call	_read_sndsettings
		mov	_snd_card_type, al
		mov	_snd_base_port_0, dx
		mov	_irq_number_1,	cl
		mov	_dma_channel_1, ch
		mov	_freq_1DCF6, ah
		mov	_byte_1DCF7, bl
		mov	_byte_1DCF8, bh
		mov	_configword, si
		mov	_outp_freq, bp
		mov	_byte_1DE7C, 1
		cmp	_snd_card_type, 0
		jnz	short loc_1C4A6
		mov	byte ptr cs:[loc_1AA73+4], 0Fh

loc_1C4A6::				; CODE XREF: _callsubx+6Fj
		clc

locret_1C4A7::				; CODE XREF: _callsubx+36j
		retn
_callsubx endp


; =============== S U B	R O U T	I N E =======================================


_rereadrtc_settmr proc near	; CODE XREF: _int1a_timer+9p
		mov	ah, 2
		int	1Ah		; CLOCK	- READ REAL TIME CLOCK (AT,XT286,CONV,PS)
					; Return: CH = hours in	BCD
					; CL = minutes in BCD
					; DH = seconds in BCD
		xor	eax, eax
		mov	al, dh
		mov	ah, al
		shr	ah, 4
		and	al, 0Fh
		aad
		mov	ebx, eax
		mov	al, ch
		mov	ah, al
		shr	ah, 4
		and	al, 0Fh
		aad
		imul	edx, eax, 60
		mov	al, cl
		mov	ah, al
		shr	ah, 4
		and	al, 0Fh
		aad
		add	dx, ax
		imul	eax, edx, 60
		add	eax, ebx
		mov	edx, 1193180
		mul	edx
		shrd	eax, edx, 10h
		xor	dx, dx
		mov	es, dx
		assume es:nothing
		mov	es:[46Ch], eax
		retn
_rereadrtc_settmr endp


; =============== S U B	R O U T	I N E =======================================


_spectr_1C4F8 proc near	; CODE XREF: _spectr_1BBC1+2Ap
		xor	eax, eax
		mov	edx, 40000000h

loc_1C501::				; CODE XREF: _spectr_1C4F8+21j
		mov	ecx, eax
		add	ecx, edx
		shr	eax, 1
		cmp	ecx, ebx
		jg	short loc_1C515
		sub	ebx, ecx
		add	eax, edx

loc_1C515::				; CODE XREF: _spectr_1C4F8+15j
		shr	edx, 2
		jnz	short loc_1C501
		cmp	eax, ebx
		jge	short locret_1C521
		inc	ax

locret_1C521::				; CODE XREF: _spectr_1C4F8+26j
		retn
_spectr_1C4F8 endp


; =============== S U B	R O U T	I N E =======================================


_txt_blinkingoff proc	near	; CODE XREF: _setvideomode+1Cp
					; _text_init2+11Dp
		xor	bl, bl
		mov	ax, 1003h
		int	10h		; - VIDEO - TOGGLE INTENSITY/BLINKING BIT (Jr, PS, TANDY 1000, EGA, VGA)
					; BL = 00h enable background intensity
					; = 01h	enable blink
		retn
_txt_blinkingoff endp


; =============== S U B	R O U T	I N E =======================================


_txt_enableblink proc	near	; CODE XREF: _start+1F0p _dosexec+5p
		mov	bl, 1
		mov	ax, 1003h
		int	10h		; - VIDEO - TOGGLE INTENSITY/BLINKING BIT (Jr, PS, TANDY 1000, EGA, VGA)
					; BL = 00h enable background intensity
					; = 01h	enable blink
		retn
_txt_enableblink endp


; =============== S U B	R O U T	I N E =======================================


_my_u32tox proc near		; CODE XREF: _useless_sprint_12+8p
		ror	eax, 10h
		call	_my_u16tox
		ror	eax, 10h
_my_u32tox endp ; sp-analysis	failed


; =============== S U B	R O U T	I N E =======================================


_my_u16tox proc near		; CODE XREF: _my_u32tox+4p
					; _useless_sprint_11+7p
		xchg	al, ah
		call	_my_u8tox
		mov	al, ah
_my_u16tox endp ; sp-analysis	failed


; =============== S U B	R O U T	I N E =======================================


_my_u8tox proc near		; CODE XREF: _my_u16tox+2p
					; _useless_sprint_10+7p
		push	ax
		shr	al, 4
		call	_my_u4tox
		pop	ax
_my_u8tox endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_my_u4tox proc near		; CODE XREF: _my_u8tox+4p
		and	al, 0Fh
		or	al, '0'
		cmp	al, '9'
		jbe	short loc_1C556
		add	al, 7

loc_1C556::				; CODE XREF: _my_u4tox+6j
		mov	[si], al
		inc	si
		retn
_my_u4tox endp


; =============== S U B	R O U T	I N E =======================================


_my_i8toa10 proc near		; CODE XREF: seg001:2DC1p
					; _useless_sprint_7+7p
		cbw

_my_i16toa10::				; CODE XREF: _useless_sprint_8+7p
		cwde

_my_i32toa10::				; CODE XREF: _useless_sprint_9+8p
		xor	cx, cx
		or	eax, eax
		jns	short _my_i32toa10_
		mov	dl, '-'
		call	_myputdigit
		neg	eax
		jmp	short _my_i32toa10_
_my_i8toa10 endp


; =============== S U B	R O U T	I N E =======================================


_my_u8toa10 proc near		; CODE XREF: _text_init2:loc_1A6A1p
					; _text_init2+239p ...
		xor	ah, ah
_my_u8toa10 endp


; =============== S U B	R O U T	I N E =======================================


_my_u16toa10 proc near	; CODE XREF: _text_init2+28Fp
					; _txt_draw_bottom+13Ep	...
		movzx	eax, ax
_my_u16toa10 endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_my_u32toa10 proc near	; CODE XREF: _my_u32toa_fill+6p
					; _useless_sprint_6+8p
		xor	cx, cx

_my_i32toa10_::				; CODE XREF: _my_i8toa10+8j
					; _my_i8toa10+12j
		mov	ebx, 10
_my_u32toa10 endp ; sp-analysis failed


; =============== S U B	R O U T	I N E =======================================


_my_u32toa proc near		; CODE XREF: _my_u32toa+Dp
		xor	edx, edx
		div	ebx
		or	eax, eax
		jz	short loc_1C58E
		push	edx
		call	_my_u32toa
		pop	edx

loc_1C58E::				; CODE XREF: _my_u32toa+9j
		or	dl, '0'
_my_u32toa endp ; sp-analysis	failed


; =============== S U B	R O U T	I N E =======================================


_myputdigit proc near		; CODE XREF: _my_i8toa10+Cp
		mov	[si], dl
		inc	si
		inc	cx
		retn
_myputdigit endp

; ---------------------------------------------------------------------------

; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================





; =============== S U B	R O U T	I N E =======================================


_mystrlen proc near		; CODE XREF: _start+2D6p
		mov	ax, -1
		dec	si

loc_1C6AB::				; CODE XREF: _mystrlen+9j
		inc	ax
		inc	si
		cmp	byte ptr [si], 0
		jnz	short loc_1C6AB
		sub	si, ax
		retn
_mystrlen endp


; =============== S U B	R O U T	I N E =======================================


_strcpy_count proc near	; CODE XREF: _useless_mysprintf+26p
					; _useless_mysprintf+43p
		xor	cx, cx
		jmp	short loc_1C6BE
; ---------------------------------------------------------------------------

loc_1C6B9::				; CODE XREF: _strcpy_count+Ej
		mov	es:[di], al
		inc	si
		inc	di

loc_1C6BE::				; CODE XREF: _strcpy_count+2j
		mov	al, [si]
		inc	cx
		or	al, al
		jnz	short loc_1C6B9
		retn
_strcpy_count endp


; =============== S U B	R O U T	I N E =======================================


_mouse_init proc near		; CODE XREF: _start+16Dp _start+74Ap
		mov	_mouse_visible, 0
		xor	ax, ax
		mov	es, ax
		assume es:nothing
		cmp	dword ptr es:[0CCh], 0
		jz	short loc_1C708
		mov	ax, 21h	; '!'
		int	33h		; - MS MOUSE - SOFTWARE	RESET
					; Return: AX = FFFFh if	mouse driver installed
					; AX = 0021h if	mouse driver not installed
					; BX = 2 if mouse driver is installed
		cmp	ax, 0FFFFh
		jz	short loc_1C6EF
		xor	ax, ax
		int	33h		; - MS MOUSE - RESET DRIVER AND	READ STATUS
					; Return: AX = status
					; BX = number of buttons
		test	ax, ax
		jz	short loc_1C708
		cmp	ax, 0FFFFh
		jnz	short loc_1C708

loc_1C6EF::				; CODE XREF: _mouse_init+1Aj
		mov	_mouse_exist_flag, 1
		push	es
		mov	ax, seg001
		mov	es, ax
		assume es:seg001
		mov	dx, offset loc_1C72C
		mov	cx, 1Fh
		mov	ax, 0Ch
		int	33h		; - MS MOUSE - DEFINE INTERRUPT	SUBROUTINE PARAMETERS
					; CX = call mask, ES:DX	-> FAR routine
		pop	es
		assume es:nothing
		clc
		retn
; ---------------------------------------------------------------------------

loc_1C708::				; CODE XREF: _mouse_init+10j
					; _mouse_init+22j ...
		mov	_mouse_exist_flag, 0
		stc
		retn
_mouse_init endp


; =============== S U B	R O U T	I N E =======================================


_mouse_deinit proc near	; CODE XREF: _start:loc_19256p
					; _start:loc_197D6p
		cmp	_mouse_exist_flag, 1
		jnz	short locret_1C72B
		mov	_mouse_exist_flag, 0
		mov	_mouse_visible, 0
		xor	dx, dx
		mov	es, dx
		assume es:nothing
		mov	cx, dx
		mov	ax, 0Ch
		int	33h		; - MS MOUSE - DEFINE INTERRUPT	SUBROUTINE PARAMETERS
					; CX = call mask, ES:DX	-> FAR routine

locret_1C72B::				; CODE XREF: _mouse_deinit+5j
		retn
_mouse_deinit endp

; ---------------------------------------------------------------------------

loc_1C72C::				; DATA XREF: _mouse_init+34o
		push	ds
		push	ax
		mov	ax, dseg
		mov	ds, ax
		pop	ax
		mov	_mousecolumn, cx
		mov	_mouserow, dx
		mov	_byte_1DE90, bl
		pop	ds
		retf

; =============== S U B	R O U T	I N E =======================================


_mouse_show proc near		; CODE XREF: _start+332p
		cmp	_mouse_exist_flag, 1
		jnz	short locret_1C755
		cmp	_mouse_visible, 1
		jz	short locret_1C755
		mov	_mouse_visible, 1
		call	_mouse_showcur

locret_1C755::				; CODE XREF: _mouse_show+5j
					; _mouse_show+Cj
		retn
_mouse_show endp


; =============== S U B	R O U T	I N E =======================================


_mouse_hide proc near		; CODE XREF: _start+376p
					; _start:loc_19827p ...
		cmp	_mouse_exist_flag, 1
		jnz	short locret_1C76C
		cmp	_mouse_visible, 0
		jz	short locret_1C76C
		mov	_mouse_visible, 0
		call	_mouse_hide2

locret_1C76C::				; CODE XREF: _mouse_hide+5j
					; _mouse_hide+Cj
		retn
_mouse_hide endp


; =============== S U B	R O U T	I N E =======================================


_mouse_getpos proc near
		cmp	_mouse_exist_flag, 1
		jnz	short loc_1C783
		mov	ax, 3
		int	33h		; - MS MOUSE - RETURN POSITION AND BUTTON STATUS
					; Return: BX = button status, CX = column, DX =	row
		mov	_mousecolumn, cx
		mov	_mouserow, dx
		clc
		retn
; ---------------------------------------------------------------------------

loc_1C783::				; CODE XREF: _mouse_getpos+5j
		xor	bx, bx
		xor	cx, cx
		xor	dx, dx
		stc
		retn
_mouse_getpos endp


; =============== S U B	R O U T	I N E =======================================


_mouse_showcur proc near	; CODE XREF: _mouse_show+13p
		cmp	_mouse_exist_flag, 1
		jnz	short loc_1C7A7
		mov	ax, 1
		int	33h		; - MS MOUSE - SHOW MOUSE CURSOR
					; SeeAlso: AX=0002h, INT 16/AX=FFFEh
		clc
		retn
_mouse_showcur endp


; =============== S U B	R O U T	I N E =======================================


_mouse_hide2 proc near	; CODE XREF: _mouse_hide+13p
		cmp	_mouse_exist_flag, 1
		jnz	short loc_1C7A7
		mov	ax, 2
		int	33h		; - MS MOUSE - HIDE MOUSE CURSOR
					; SeeAlso: AX=0001h, INT 16/AX=FFFFh
		clc
		retn
; ---------------------------------------------------------------------------

loc_1C7A7::				; CODE XREF: _mouse_showcur+5j
					; _mouse_hide2+5j
		stc
		retn
_mouse_hide2 endp


; =============== S U B	R O U T	I N E =======================================


_mouse_1C7A9 proc near	; CODE XREF: _mouse_1C7CF+10p
		cmp	cx, si
		jbe	short loc_1C7AF
		xchg	cx, si

loc_1C7AF::				; CODE XREF: _mouse_1C7A9+2j
		cmp	dx, di
		jbe	short loc_1C7B5
		xchg	dx, di

loc_1C7B5::				; CODE XREF: _mouse_1C7A9+8j
		cmp	ax, cx
		jb	short loc_1C7CA
		cmp	ax, si
		ja	short loc_1C7CA
		cmp	bp, dx
		jb	short loc_1C7CA
		cmp	bp, di
		ja	short loc_1C7CA
		sub	ax, cx
		sub	bp, dx
		retn
; ---------------------------------------------------------------------------

loc_1C7CA::				; CODE XREF: _mouse_1C7A9+Ej
					; _mouse_1C7A9+12j ...
		stc
		retn
_mouse_1C7A9 endp

; ---------------------------------------------------------------------------
; START	OF FUNCTION CHUNK FOR _mouse_1C7CF

loc_1C7CC::				; CODE XREF: _mouse_1C7CF+13j
		add	bx, 0Ah
; END OF FUNCTION CHUNK	FOR _mouse_1C7CF

; =============== S U B	R O U T	I N E =======================================


; void *__usercall _mouse_1C7CF<ebx>(struct struct_0 *_mystr<ebx>)
_mouse_1C7CF proc near	; CODE XREF: _start+7ADp _start+7C9p ...

; FUNCTION CHUNK AT 377C SIZE 00000003 BYTES

		mov	cx, [bx]
		cmp	cx, -1
		jz	short loc_1C7E9
		mov	dx, [bx+2]
		mov	si, [bx+4]
		mov	di, [bx+6]
		call	_mouse_1C7A9
		jb	short loc_1C7CC
		mov	bx, [bx+8]
		clc
		retn
; ---------------------------------------------------------------------------

loc_1C7E9::				; CODE XREF: _mouse_1C7CF+5j
		stc
		retn
_mouse_1C7CF endp ; sp-analysis failed

; ---------------------------------------------------------------------------
		db 5 dup(0)
seg001 ends

; ===========================================================================

; Segment type:	Pure data
dseg segment para public 'DATA' use16
		assume cs:dseg
_aInertiaPlayerV1_ db 'Inertia Player V1.22 written by Stefan Danes and Ramon van Gorkom'
		db 0Dh,0Ah
		db 0Ah
		db 'Usage: IPLAY [Switches] [FileName.Ext|@FileList.Ext]',0Dh,0Ah
		db 0Ah
		db 'Switches:',0Dh,0Ah
		db ' /?  This help screen',0Dh,0Ah
		db ' /h  This help screen (again???)',0Dh,0Ah
		db ' /i  Display current soundcard settings',0Dh,0Ah
		db ' /l  Loop module when it',27h,'s done',0Dh,0Ah
		db ' /d  Disable module looping',0Dh,0Ah
		db ' /g  Start with graphical scopes [F-2]',0Dh,0Ah
		db ' /t  Start with textmode VU meters [F-3]',0Dh,0Ah
		db ' /f  Start with FastFourier screen [F-5]',0Dh,0Ah
		db ' /e  Disable EMS usage',0Dh,0Ah
		db ' /u  Disable UMB usage',0Dh,0Ah
		db ' /o  Enable GUS Channel Optimize [Use minimum number of voices]',0Dh
		db 0Ah
		db ' /c  Disable GUS Channel Optimize [Always use 32 voices]',0Dh,0Ah
		db ' /v  Desqview mode: Only use 80x25 textmode',0Dh,0Ah,'$'
_aCurrentSoundcard db 0Dh,'Current Soundcard settings:',0Dh,0Ah ; DATA XREF: _start:loc_19057o
		db 0Ah,'$'
_myendl		db 0Dh,0Ah,'$'          ; DATA XREF: _start-1Do
off_1CA8E	dw offset _f3_textmetter	; DATA XREF: _start+182r
		dw offset _f2_waves
		dw offset _f5_graphspectr
		dw offset _f4_patternnae
		dw offset _f1_help
		dw offset _f6_undoc
_table_sndcrdname dw offset _aGravisUltrasou ; DATA XREF:	_text_init2+19Dr
					; "Gravis UltraSound"
		dw offset _aProAudioSpectr ; "Pro Audio Spectrum	16"
		dw offset _aWindowsSoundSy ; "Windows Sound System"
		dw offset _aSoundBlaster16 ; "Sound Blaster 16/16ASP"
		dw offset _aSoundBlasterPr ; "Sound Blaster Pro"
		dw offset _aSoundBlaster	; "Sound Blaster"
		dw offset _aCovox	; "Covox"
		dw offset _aStereoOn1	; "Stereo-On-1"
		dw offset _aAdlibSoundcard ; "Adlib SoundCard"
		dw offset _aPcHonker	; "PC Honker"
		dw offset _aGeneralMidi	; "General MIDI"
_aGravisUltrasou	db 'Gravis UltraSound',0 ; DATA XREF: dseg:_table_sndcrdnameo
_aGravisMaxCodec	db 'Gravis MAX Codec',0
_aProAudioSpectr	db 'Pro Audio Spectrum 16',0 ; DATA XREF: dseg:02ACo
_aWindowsSoundSy	db 'Windows Sound System',0 ; DATA XREF: dseg:02AEo
_aSoundBlaster16	db 'Sound Blaster 16/16ASP',0 ; DATA XREF: dseg:02B0o
_aSoundBlasterPr	db 'Sound Blaster Pro',0 ; DATA XREF: dseg:02B2o
_aSoundBlaster	db 'Sound Blaster',0    ; DATA XREF: dseg:02B4o
_aCovox		db 'Covox',0            ; DATA XREF: dseg:02B6o
_aStereoOn1	db 'Stereo-On-1',0      ; DATA XREF: dseg:02B8o
_aAdlibSoundcard	db 'Adlib SoundCard',0  ; DATA XREF: dseg:02BAo
_aPcHonker	db 'PC Honker',0        ; DATA XREF: dseg:02BCo
_aGeneralMidi	db 'General MIDI',0     ; DATA XREF: dseg:02BEo
_atop_title	dw 152h			; DATA XREF: _txt_draw_top_title+12o
		db 7Fh
_aInertiaPlayerV1_22A db	'Inertia Player V1.22 Assembly ',27h,'94 CD Edition by Sound Solution'
		db 's'
		db    1
		db 0F4h	; Ù
		db    1
_aCopyrightC1994	db 'Copyright (c) 1994,1995 by Stefan Danes and Ramon van Gorkom',0
		db 2
		db  78h	; x
		db    1
		db 0AAh	; ™
		db    1
_aShell130295211	db 'Shell: 13/02/95 21:15:58'
		db    1
		db  46h	; F
		db    1
_aPlayer13029521	db 'Player: '
_a130295211558	db '13/02/95 21:15:58',0 ; DATA XREF: _read_module+BEw
_bottom_menu	dw 0Ah			; DATA XREF: _text_init2+21Fo
		db 7Fh
asc_1CC2D	db '                              ' ; DATA XREF: _read_module+A3o
		db    1
		dw 0AAh
		db    2
		db 7Eh
_aFilename_0	db 'Filename      : '
		db    2
		db 7Fh
_aFilename_ext	db 'FileName.Ext'       ; DATA XREF: _read_module:loc_19E41o
		db    1
		dw 14Ah
		db    2
		db 7Eh
_aModuleType_0	db 'Module Type   : '
		db    2
		db  7Fh	; 
_module_type_txt	db '    '               ; DATA XREF: _read_module+6Fw
		db    2
		db  7Eh	; ~
		db    1
		dw 1EAh
_aChannels	db 'Channels      :'
		db    1
		dw 28Ah
_aSamplesUsed	db 'Samples Used  :'
		db    1
		dw 32Ah
_aCurrentTrack	db 'Current Track :'
		db    1
		dw 3CAh
_aTrackPosition	db 'Track Position:'
		db    1
		dw 46Ah
_aSpeed		db 'Speed'
		db    1
		dw 486h
		db ':'
		db    2
		db  78h	; x
		db    1
		dw 4A4h
_aTab		db 'Tab'
		db    1
		dw 0F8h
		db    2
_byte_1CCEB	db 78h			; DATA XREF: _text_init2:loc_1A6C2w
		db 0FEh	; ˛
		db    2
		db 7Eh
_aPlayingInStereoFree db	' Playing in Stereo, Free:'
		db    1
		dw 198h
		db    2
		db  78h	; x
		db 0FEh	; ˛
		db    2
		db 7Eh
_aProtracker1_0_0 db ' ProTracker 1.0'
		db    1
		dw 1CEh
		db    2
		db 78h
_aF9_4		db 'F-9'
		db    1
		dw 238h
		db    2
		db  78h	; x
		db 0FEh	; ˛
		db    2
		db 7Eh
_aIgnoreBpmChanges db ' Ignore BPM changes'
		db    1
		dw 26Eh
		db    2
		db 78h
_aF10_1		db 'F-10'
		db    2
		db  7Eh	; ~
		db    1
		dw 2D8h
		db    2
		db  78h	; x
		db 0FEh	; ˛
		db    2
		db 7Eh
_aLoopModuleWhenDone db ' Loop Module when done'
		db    1
		dw 30Eh
		db    2
		db 78h
_aF11_1		db 'F-11'
		db    2
		db 7Eh
		db    1
		dw 378h
		db    2
		db 78h
		db 0FEh	; ˛
		db    2
		db 7Eh
_a24bitInterpolation db ' 24bit Interpolation'
		db    1
		dw 3AEh
		db    2
		db 78h
_aF12_1		db 'F-12'
		db    2
		db  7Eh	; ~
		db    1
		dw 418h
_aMainVolume	db 'Main Volume   :'
		db 1
		dw 44Eh
		db    2
		db 78h
		db '- +'
		db    2
		db  7Eh	; ~
		db    1
		dw 4B8h
_aVolumeAmplify	db 'Volume Amplify:'
		db    1
		dw 4EEh
		db    2
		db  78h	; x
		db '[ ]',0
_f1_help_text	dw 3F8h			; DATA XREF: seg001:1CD8o
		db 7Fh
_aSoYouWantedSomeHelp db	'So you wanted some help?'
		db    1
		dw 468h
		db    2
		db 7Fh
_aF2_0		db 'F-2'
		db    2
		db 7Eh
_aGraphicalScopesOneF db	'  Graphical scopes, one for each channel'
		db    1
		dw 508h
		db    2
		db 7Fh
_aF3_0		db 'F-3'
		db    2
		db 7Eh
_aRealtimeVuMeters db '  Realtime VU meters'
		db    1
		dw 5A8h
		db    2
		db 7Fh
_aF4_0		db 'F-4'
		db    2
		db 7Eh
_aViewSampleNamesTwic db	'  View sample names (twice for more)'
		db    1
		dw 648h
		db    2
		db 7Fh
_aF5_0		db 'F-5'
		db    2
		db 7Eh
_aFastfourierFrequenc db	'  FastFourier Frequency Analysis'
		db    1
		dw 6E8h
		db    2
		db 7Fh
_aF8_0		db 'F-8'
		db    2
_aDosShellTypeEx	db 7Eh
_aDosShellTypeExitToR db	'  DOS Shell (Type EXIT to return)'
		db    1
		dw 788h
		db    2
_aF9_1		db 7Fh
_aF9_2		db 'F-9'
		db    2
_aProtracker1_0C	db 7Eh
_aProtracker1_0Compat db	'  ProTracker 1.0 compatibility on/off'
		db    1
		dw 828h
		db    2
_aF10		db 7Fh
_aF10_0		db 'F-10'
		db    2
_aDisableBpmOnOf	db 7Eh
_aDisableBpmOnOff db ' Disable BPM on/off'
		db    1
		dw 8C8h
		db    2
_aF11		db 7Fh
_aF11_0		db 'F-11'
		db    2
_aLoopModule	db 7Eh
_aLoopModule_0	db ' Loop module'
		db    1
		dw 968h
		db    2
_aF12		db 7Fh
_aF12_0		db 'F-12'
		db    2
_aToggle24bitInt	db 7Eh
_aToggle24bitInterpol db	' Toggle 24bit Interpolation'
		db    1
		dw 4C4h
		db    2
_aGray		db 7Fh
_aGray_0		db 'Gray - +'
		db    2
_aDecIncVolume	db 7Eh
_aDecIncVolume_0	db '  Dec/Inc volume'
		db    1
		dw 56Eh
		db    2
		db 7Fh
		db '[ ]'
		db    2
_aDecIncAmplify	db 7Eh
_aDecIncAmplify_0 db '  Dec/Inc amplify'
		db    1
		dw 600h
		db    2
_aCursor		db 7Fh
_aCursor_1	db 'Cursor '
		db  1Ah
		db ' '
		db  18h
		db    2
		db  7Eh	; ~
_aFastErForward	db '  Fast(er) forward'
		db    1
		dw 6A0h
		db    2
		db 7Fh
_aCursor_0	db 'Cursor ',1Bh,' '
		db  19h
		db    2
		db  7Eh	; ~
_aFastErRewind	db '  Fast(er) rewind'
		db    1
		dw 744h
		db    2
		db 7Fh
_a1Thru0		db '1 Thru 0'
		db    2
		db  7Eh	; ~
_aMuteChannel	db '  Mute channel'
		db    1
		dw 7E0h
		db    2
		db 7Fh
_aScrolllock	db 'ScrollLock'
		db    2
		db  7Eh	; ~
_aLoopPattern	db '  Loop pattern'
		db    1
		dw 88Ah
		db    2
		db  7Fh	; 
_aPause		db 'Pause'
		db    2
		db  7Eh	; ~
_aGuess___	db '  Guess...'
		db    1
		dw 92Eh
		db    2
		db 7Fh
_aEnd		db 'End'
		db    2
		db  7Eh	; ~
_aEndPattern	db '  End pattern'
		db    1
		dw 9CEh
		db    2
		db 7Fh
_aTab_0		db 'Tab'
		db    2
		db  7Eh	; ~
_aTogglePalNtsc	db '  Toggle PAL/NTSC',0
; char _hopeyoulike[1]
_hopeyoulike	dw 3C6h			; DATA XREF: _start+204o
		db 7Eh
_aHopeYouLikedUsingTh db	'Hope you liked using the '
		db    2
		db  7Fh	; 
_aInertiaPlayer	db 'Inertia Player'
		db    2
		db  7Eh	; ~
_aWhichIsWrittenIn db ' which is written in '
		db    2
		db  7Fh	; 
_a100Assembler	db '100% assembler!'
		db    2
		db  7Eh	; ~
		db    1
		dw 50Ch
_aIfYouHaveBugReports db	'If you have bug-reports, suggestions or comments send a message t'
		db 'o:'
		db    1
		dw 5ACh
_aInternet	db 'Internet : '
		db    2
		db  7Fh	; 
_aSdanes@marvels_hack db	'sdanes@marvels.hacktic.nl'
		db    2
		db  7Eh	; ~
		db    1
		dw 64Ch
_aFidonet	db 'FidoNet  : '
		db    2
		db  7Fh	; 
_a2284116_8	db '2:284/116.8'
		db    2
		db  7Eh	; ~
		db    1
		dw 826h
_aSendEmailTo	db 'Send email to '
		db    2
		db  7Fh	; 
_aListserver@oliver_s db	'listserver@oliver.sun.ac.za'
		db    2
		db  7Eh	; ~
_aToSubscribeToOneOrB db	' to subscribe to one or both of'
		db    1
		dw 8C6h
_aThe		db 'the '
		db    2
		db  7Fh	; 
_aInertiaMailinglists db	'Inertia Mailinglists'
		db    2
		db  7Eh	; ~
_aAndWriteFollowingTe db	' and write following text in your message:'
		db    1
		dw 966h
_aToConnectToBinaryIn db	'To connect to Binary Inertia releases: '
		db    2
		db  7Fh	; 
_aSubscribeInertiaLis db	'subscribe inertia-list YourRealName'
		db    2
		db  7Eh	; ~
		db    1
		dw 0A06h
_aToConnectToDiscussi db	'To connect to Discussion Mailing list: '
		db    2
		db  7Fh	; 
_aSubscribeInertiaTal db	'subscribe inertia-talk YourRealName',0
; char _word_1D26D
_word_1D26D	dw 3F2h			; DATA XREF: _dosexec+19o
		db  7Eh	; ~
_aShellingToOperating db	'Shelling to Operating System...'
		db    1
		dw 52Ah
_aType		db 'Type '
		db    2
		db  7Fh	; 
_aExit		db 'EXIT'
		db    2
		db  7Eh	; ~
_aToReturnTo	db ' to return to '
		db    2
		db  7Fh	; 
_aInertiaPlayer_0 db 'Inertia Player',0
; char _msg[]
_msg		db 'Searching directory for modules  ',0 ; DATA XREF: _start+2F7o
; char _aLoadingModule[]
_aLoadingModule	db 'Loading module',0   ; DATA XREF: _start+41Ao
; char _aModuleIsCorrupt[]
_aModuleIsCorrupt db 'Module is corrupt!',0 ; DATA XREF: _start+439o
_aNotEnoughDram_0 db 'Not enough DRAM on your UltraSound to load all samples!',0
					; DATA XREF: _start+431o
_aNotEnoughMemo_0 db 'Not enough memory available to load all samples!',0
					; DATA XREF: _start+429o
; char _aDeleteMarkedFil[]
_aDeleteMarkedFil db 'Delete marked files? [Y/N]',0 ; DATA XREF: _start+635o
; char _aDeletingFile[15]
_aDeletingFile	db 'Deleting file: '    ; DATA XREF: _start+69Fo
_aFile		db 'File'               ; DATA XREF: _start+689w _start+6A8o
_aName		db 'name'               ; DATA XREF: _start+692w
_a_ext		db '.Ext'               ; DATA XREF: _start+69Bw
		db 0
_aPal		db '(PAL) ',0           ; DATA XREF: _txt_draw_bottom+49o
; char _aNtsc[]
_aNtsc		db '(NTSC)',0           ; DATA XREF: _txt_draw_bottom+53o
; char _word_1D3B0[1]
_word_1D3B0	dw 49Eh			; DATA XREF: _start+723o
		db  7Bh	; {
_aFileSelectorHelp db 'File Selector Help'
		db    1
		dw 6F2h
		db    2
		db  7Eh	; ~
_aUse		db 'Use '
		db    2
		db  7Fh	; 
_aHome		db 'Home'
		db    2
		db  7Eh	; ~
		db ','
		db    2
		db  7Fh	; 
_aEnd_0		db 'End'
		db    2
		db  7Eh	; ~
		db ','
		db    2
		db  7Fh	; 
_aPgup		db 'PgUp'
		db    2
		db  7Eh	; ~
		db ','
		db    2
		db  7Fh	; 
_aPgdn		db 'PgDn'
		db    2
		db  7Eh	; ~
		db ','
		db    2
		db  7Fh	; 
		db  18h
		db    2
		db  7Eh	; ~
_aAnd		db ' and '
		db    2
		db  7Fh	; 
		db  19h
		db    2
		db  7Eh	; ~
_aToMoveTheHighlighte db	' to move the highlighted bar'
		db    1
		dw 792h
_aPress		db 'Press '
		db    2
		db  7Fh	; 
_aEnter		db 'Enter'
		db    2
		db  7Eh	; ~
_aToPlayTheModuleOrSe db	' to play the module or select the drive/directory'
		db    1
		dw 8D2h
		db    2
		db  7Fh	; 
_aEsc		db 'ESC'
		db    2
		db  7Eh	; ~
		db    1
		dw 8E6h
_aQuitIplay	db 'Quit IPLAY'
		db    1
		dw 972h
		db    2
		db  7Fh	; 
_aF1		db 'F-1'
		db    2
		db  7Eh	; ~
		db    1
		dw 986h
_aThisHelpScreenButIG db	'This help screen, but I guess you already found it...'
		db    1
		dw 0A12h
		db    2
		db  7Fh	; 
_aF8_1		db 'F-8'
		db    2
		db  7Eh	; ~
		db    1
		dw 0A26h
_aDosShellTypeExitT_0 db	'DOS Shell (Type EXIT to return)'
		db    1
		dw 0AB2h
		db    2
		db  7Fh	; 
_aF9_3		db 'F-9'
		db    2
		db  7Eh	; ~
		db    1
		dw 0AC6h
_aToggleQuickreadingO db	'Toggle QuickReading of module name'
		db    1
		dw 0B52h
unk_1D516	db    2
		db  7Fh	; 
_aDel		db 'Del'
		db    2
		db  7Eh	; ~
		db    1
		dw 0B66h
_aMarkFileToDelete db 'Mark file to delete'
		db    1
		dw 0BF2h
		db    2
		db  7Fh	; 
_aCtrlDel	db 'Ctrl Del'
		db    2
		db  7Eh	; ~
		db    1
		dw 0C06h
_aDeleteAllFilesWhich db	'Delete all files which are marked to delete'
		db    1
		dw 0C92h
		db    2
		db  7Fh	; 
_aBackspace	db 'BackSpace'
		db    2
		db  7Eh	; ~
		db    1
		dw 0CA6h
_aReturnToPlaymodeOnl db	'Return to playmode [Only if the music is playing]'
		db    1
		dw 0E86h
		db    2
		db  7Eh	; ~
_aPressAnyKeyToReturn db	'Press any key to return to the fileselector',0
; char _aPressF1ForHelpQu[11]
_aPressF1ForHelpQu db '                 Press F-1 for help, QuickRead='
					; DATA XREF: _start+367o
_word_1D614	dw 2020h		; DATA XREF: _useless_197F2+7w
					; _useless_197F2:loc_19810w
_byte_1D616	db 20h			; DATA XREF: _useless_197F2+Dw
					; _useless_197F2+24w
_aF9		db ' [F-9]              ',0
_aHitBackspaceToRe db 'Hit backspace to return to playmode, F-1 for help, QuickRead='
					; DATA XREF: _start+35Do
_word_1D669	dw 2020h		; DATA XREF: _useless_197F2+12w
					; _useless_197F2+29w
_byte_1D66B	db 20h			; DATA XREF: _useless_197F2+18w
					; _useless_197F2+2Fw
_aF9_0		db ' [F-9]',0
_aSamplename	db '# SampleName   '    ; DATA XREF: seg001:1B7Co
		db    2
_aXpressF4ForMor	db 'xPress F-4 for more'
		db    2
_aSizeVolModeC2T	db '~   Size Vol Mode  C-2 Tune LoopPos LoopEnd',0
unk_1D6C3	db    2			; DATA XREF: seg001:1BDAo
_aUnused256	db ' Unused'
_a256		db '256',0              ; DATA XREF: _text_init2+1CEo
_a512		db '512',0
_a768		db '768',0
_a1024		db '1024',0
; char _aKb[]
_aKb		db 'KB',0               ; DATA XREF: _text_init2+1D7o
asc_1D6E0	db '               ',0  ; DATA XREF: seg001:1A80o
_aPortamentoUp	db 'Portamento Up  ',0
_aPortamentoDown	db 'Portamento Down',0
_aTonePortamento	db 'Tone Portamento',0
_aVibrato	db 'Vibrato        ',0
_aPortVolslide	db 'Port + VolSlide',0
_aVibrVolslide	db 'Vibr + VolSlide',0
_aTremolo	db 'Tremolo        ',0
_aFinePanning	db 'Fine Panning   ',0
_aSetSampleOfs	db 'Set Sample Ofs ',0
_aVolumeSliding	db 'Volume Sliding ',0
_aPositionJump	db 'Position Jump  ',0
_aVolumeChange	db 'Volume Change  ',0
_aPatternBreak	db 'Pattern Break  ',0
_aE_command	db 'E_Command      ',0
_aSetSpeedBpm	db 'Set Speed/BPM  ',0
_aSetSpeed	db 'Set Speed      ',0
_aFinePortaUp	db 'Fine Porta Up  ',0
_aFinePortaDown	db 'Fine Porta Down',0
_aFineTonePorta	db 'Fine Tone Porta',0
_aFineVibrato	db 'Fine Vibrato   ',0
_aFineVolSlide	db 'Fine Vol Slide ',0
_aFinePortVolsl	db 'Fine Port+VolSl',0
_aFineVibrVolsl	db 'Fine Vibr+VolSl',0
_aSetStmSpeed	db 'Set STM Speed  ',0
_aAutoToneporta	db 'Auto TonePorta ',0
_aTriller	db 'Triller        ',0
_aTremor		db 'Tremor         ',0
_aRetrigVolume	db 'Retrig+Volume  ',0
_aArpeggio	db 'Arpeggio       ',0  ; DATA XREF: seg001:loc_1AB0Do
_aSetAmplify	db 'Set Amplify    ',0
_aFarTempo	db 'FAR Tempo      ',0
_aFarFineTempo	db 'FAR Fine Tempo ',0
_aSetFilter	db 'Set Filter     ',0  ; DATA XREF: seg001:1A9Ao
_aFineslideUp	db 'FineSlide Up   ',0
_aFineslideDown	db 'FineSlide Down ',0
_aGlissandoCtrl	db 'Glissando Ctrl ',0
_aVibratoControl	db 'Vibrato Control',0
_aSetFinetune	db 'Set FineTune   ',0
_aJumpToLoop	db 'Jump To Loop   ',0
_aTremoloControl	db 'Tremolo Control',0
_aSetPanning	db 'Set Panning    ',0
_aRetriggerNote	db 'Retrigger Note ',0
_aFinevolumeUp	db 'FineVolume Up  ',0
_aFinevolumeDown	db 'FineVolume Down',0
_aNoteCut	db 'Note Cut       ',0
_aNoteDelay	db 'Note Delay     ',0
_aPatternDelay	db 'Pattern Delay  ',0
_aInvertLoop	db 'Invert Loop    ',0
_aSetLoopPoint	db 'Set Loop Point ',0  ; DATA XREF: seg001:1A8Fo
asc_1DA00	db '                      ',0 ; DATA XREF: _modules_search:loc_19BDDo
					; seg001:loc_1A9A5o
_aMute		db '<Mute>                ',0 ; DATA XREF: seg001:1949o
; char _aMarkedToDelete[]
_aMarkedToDelete	db '<Marked to Delete>    ',0 ; DATA XREF: _filelist_198B8+10Do
_notes		db '  C-C#D-D#E-F-F#G-G#A-A#B-' ; DATA XREF: seg001:1930r
					; seg001:1B31r	...
_slider		db 'ƒ\|/ƒ\|/'           ; DATA XREF: _modules_search+7Fr
					; _modules_search+F8r
_aModuleNotFound	db 'Module not found.',0Dh,0Ah,'$' ; DATA XREF: _find_mods+88o
_aModuleLoadErro	db 'Module load error.',0Dh,0Ah,'$' ; DATA XREF: _readallmoules+1Bo
					; _read_module+5o
_aNotEnoughMemor	db 'Not enough memory.',0Dh,0Ah,'$' ; DATA XREF: _start+23Do
_aListFileNotFou	db 'List file not found.',0Dh,0Ah,'$' ; DATA XREF: _start+D07o
_aCriticalErrorT	db 0Dh,0Ah		; DATA XREF: _start+31o
		db 0Ah
		db 'Critical error: The player jumped to DOS, and should not be invok'
		db 'ed again.',0Dh,0Ah
		db 'Close this DOS session first with the "EXIT" command.',0Dh,0Ah
		db 0Ah
		db '(Press any key to continue)$'
_sIplay_cfg	db 'IPLAY.CFG',0     ; DATA XREF: _loadcfgo
_buffer_1DB6C	db 128 dup(0)		; DATA XREF: _start+189r _start+192r ...
_buffer_1DBEC	db 0			; DATA XREF: _find_mods+32o
					; _dosfindnexto
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
unk_1DC01	db    0			; DATA XREF: _modules_search+8Fr
					; _modules_search+108r ...
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
_byte_1DC0A	db 62h dup(0)		; DATA XREF: _find_mods+6Fo
; char _buffer_1DC6C[]
_buffer_1DC6C	dd 0			; DATA XREF: _start+2C5w _start+2D3o ...
unk_1DC70	db    0			; DATA XREF: _modules_search+1D8o
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db 0
unk_1DC7B	db    0			; DATA XREF: _modules_search+1C9o
_byte_1DC7C	db 70h dup(0)		; DATA XREF: _modules_search+1BAo
					; _modules_search+202o
_dword_1DCEC	dd 10524E49h		; DATA XREF: _loadcfg+1Ar
_cfg_buffer	db    4			; DATA XREF: _loadcfg+Co _loadcfg+1Er
_snd_card_type	db 3			; DATA XREF: _text_init2+18Er
					; _text_init2+1ADr ...
_snd_base_port_0	dw 0FFFFh		; DATA XREF: _callsubx+3r _callsubx+45w
_irq_number_1	db 0FFh			; DATA XREF: _callsubx+7r _callsubx+49w
_dma_channel_1	db 0FFh			; DATA XREF: _callsubx+Br _callsubx+4Dw
_freq_1DCF6	db 2Ch			; DATA XREF: _callsubx+Fr _callsubx+51w
_byte_1DCF7	db 0FFh			; DATA XREF: _callsubx+1Cr _callsubx+55w
_byte_1DCF8	db 14h			; DATA XREF: _start+DAr	_callsubx+20r ...
_configword	dw 218Bh		; DATA XREF: _start+60w	_start+6Cw ...
_byte_1DCFB	db 4Bh			; DATA XREF: _callsubx+13r
		db    0
; char _mystr[66]
_mystr		db 42h dup(0)		; DATA XREF: _start:loc_192E0o
					; _start:loc_1964Eo
; char _byte_1DD3F[69]
_byte_1DD3F	db 45h dup(0)		; DATA XREF: _dosexec:loc_1C209o
					; _dosexec:loc_1C25Co
_a_mod_nst_669_s	db '.MOD.NST.669.STM.S3M.MTM.PSM.WOW.INR.FAR.ULT.OKT.OCT',0,0,0,0
					; DATA XREF: _modules_search+12Do
					; _find_mods+46o
_aPlaypausloop	db 'PlayPausLoop'       ; DATA XREF: _txt_draw_bottom+164o
_aJanfebmaraprmayj db '   JanFebMarAprMayJunJulAugSepOctNovDec'
					; DATA XREF: _filelist_198B8+A4o
_frameborder	db '      €€€€€€…ª»ºÕ∫⁄ø¿Ÿƒ≥÷∑”Ωƒ∫’∏‘æÕ≥',0 ; DATA XREF: _draw_frame+3Do
_oint8off_1DE14	dw 0			; DATA XREF: _start+F9w
_oint8seg_1DE16	dw 0			; DATA XREF: _start+FDw
_critsectpoint_off dw 0			; DATA XREF: _start+150w
_critsectpoint_seg dw 0			; DATA XREF: _start+154w
_swapdata_off	dw 0			; DATA XREF: _start+161w
_swapdata_seg	dw 0			; DATA XREF: _start+165w
_videomempointer	dd 0			; DATA XREF: _start:loc_1917Dw
					; _start+207r ...
_videopoint_shiftd dd 0			; DATA XREF: _text_init2+5Fw
					; _text_init2+BEw ...
_segfsbx_1DE28	dd 0			; DATA XREF: _read_module+99w
					; _keyb_screen_loop:_l_rightr ...
_dword_1DE2C	dd 0			; DATA XREF: _text_init2+22Aw
					; seg001:196Br	...
_messagepointer	dd 0			; DATA XREF: _start+228r _start+23Dw ...
_volume_1DE34	dd 0			; DATA XREF: _read_module+DAw
					; seg001:19F4r
_outp_freq	dw 0			; DATA XREF: _read_module+82w
					; _text_init2:loc_1A699r ...
_esseg_atstart	dw 0			; DATA XREF: _start+5w _parse_cmdline+7r ...
off_1DE3C	dw offset loc_19050	; DATA XREF: _start+186w _start+6E7r ...
_offs_draw	dw offset loc_19050	; DATA XREF: _keyb_screen_loop+32r
					; _keyb_screen_loop:_l_f4r ...
_offs_draw2	dw offset loc_19050	; DATA XREF: _keyb_screen_loop+486r
					; _keyb_screen_loop+49Ar ...
off_1DE42	dw offset loc_19050	; DATA XREF: _keyb_screen_loop:_l_f8r
					; _f1_help+12w ...
_amount_of_x	dw 0			; DATA XREF: _read_module+75w
					; _read_module+D1r ...
_word_1DE46	dw 0			; DATA XREF: _keyb_screen_loop+316r
					; _text_init2+244w ...
_current_patterns dw 0			; DATA XREF: _read_module+5Fw
					; _keyb_screen_loop+30Fw ...
_word_1DE4A	dw 0			; DATA XREF: _find_mods+14w
					; _find_mods+4Er
_word_1DE4C	dw 0			; DATA XREF: _find_mods+2Ew
					; _find_mods+75r
_word_1DE4E	dw 0			; DATA XREF: _start+19Bw
					; _modules_search+5Fw ...
_word_1DE50	dw 0			; DATA XREF: _start:loc_19242r
					; _readallmoules:loc_19D75r ...
_word_1DE52	dw 0			; DATA XREF: _start+3DCr _start+5D2r ...
_word_1DE54	dw 0			; DATA XREF: _start+4B1r
					; _start:loc_1955Dr ...
_word_1DE56	dw 0			; DATA XREF: _modules_search+4Cw
					; _modules_search+B1w
_word_1DE58	dw 0			; DATA XREF: _modules_search+52w
					; _modules_search+172w
_word_1DE5A	dw 0			; DATA XREF: _modules_search+58w
					; _modules_search+27Dw
_word_1DE5C	dw 0			; DATA XREF: _start+5FFw _start+60Cw ...
_word_1DE5E	dw 0			; DATA XREF: _start+25Aw
					; _start:loc_19464r ...
_word_1DE60	dw 0			; DATA XREF: _start+308w _start+5EBw ...
_word_1DE62	dw 0			; DATA XREF: _start+254w _start+31Fr ...
_word_1DE64	dw 0			; DATA XREF: _modules_searchw
					; _modules_search+75r ...
_word_1DE66	dw 0			; DATA XREF: _modules_search+6w
					; _modules_search+79r ...
_fhandle_1DE68	dw 0			; DATA XREF: _init_vga_waves+42w
					; _init_vga_waves+49r ...
_word_1DE6A	dw 0			; DATA XREF: _keyb_screen_loop+1Cw
					; _txt_draw_bottom+118r
_word_1DE6C	dw 0			; DATA XREF: _keyb_screen_loop+27w
					; _txt_draw_bottom+13Br
_word_1DE6E	dw 0			; DATA XREF: _keyb_screen_loop+30Br
					; _text_init2+52w ...
_byte_1DE70	db 0			; DATA XREF: _start+168w _start+268w ...
_byte_1DE71	db 0			; DATA XREF: seg001:loc_1A934w
					; seg001:loc_1AA73w
_byte_1DE72	db 0			; DATA XREF: _keyb_screen_loop+5w
					; _txt_draw_bottom+66r
_byte_1DE73	db 0			; DATA XREF: _read_module+79w
					; _txt_draw_bottom+72r
_byte_1DE74	db 0			; DATA XREF: _keyb_screen_loop+9w
					; _txt_draw_bottom+92r
_byte_1DE75	db 0			; DATA XREF: _keyb_screen_loop+Cw
					; _txt_draw_bottom+1Br
_byte_1DE76	db 0			; DATA XREF: _keyb_screen_loop+10w
					; _txt_draw_bottom+2Br
_flg_play_settings db 0			; DATA XREF: _keyb_screen_loop+2Fw
					; _txt_draw_bottom+4Cr ...
_byte_1DE78	db 0			; DATA XREF: _read_module+8Bw
					; _dosexec+2Cr ...
_byte_1DE79	db 0			; DATA XREF: _video_prp_mtr_positn+2w
					; _video_prp_mtr_positn:loc_1C365w ...
_byte_1DE7A	db 0			; DATA XREF: _video_prp_mtr_positn+7w
					; _video_prp_mtr_positn+1Dw ...
_byte_1DE7B	db 0			; DATA XREF: _read_module+96w
					; _text_init2+20Fr
_byte_1DE7C	db 0			; DATA XREF: _start:loc_193BCr
					; _start+347r ...
_byte_1DE7D	db 0			; DATA XREF: _start+32Fw _start+34Ar ...
_byte_1DE7E	db 0			; DATA XREF: _start+1B9w _start+217r ...
_byte_1DE7F	db 0			; DATA XREF: _start+260w _start+2F0r ...
		db    1
_byte_1DE81	db 0			; DATA XREF: _spectr_1BBC1+20r
					; _video_prp_mtr_positn+58w
_byte_1DE82	db 0			; DATA XREF: _start+E1w
					; _spectr_1BBC1:loc_1BBF4r ...
_byte_1DE83	db 3			; DATA XREF: _start+E7w
					; seg001:loc_1AA4Fr ...
_byte_1DE84	db 0			; DATA XREF: _read_module+65w
					; _keyb_screen_loop:_l_upw ...
_byte_1DE85	db 0			; DATA XREF: _keyb_screen_loop+2EBw
					; _keyb_screen_loop+2FBw ...
_byte_1DE86	db 0			; DATA XREF: _start+D7w	_text_init2r ...
		db 0
_dword_1DE88	dd 0			; DATA XREF: _start+7DBr _start+7E2w ...
_mousecolumn	dw 0			; DATA XREF: _start+7A0r _start+7BCr ...
_mouserow	dw 0			; DATA XREF: _start+7A3r _start+7BFr ...
_byte_1DE90	db 0			; DATA XREF: _start:loc_193C7r
					; _start+33Er ...
_mouse_exist_flag db 0			; DATA XREF: _mouse_init:loc_1C6EFw
					; _mouse_init:loc_1C708w ...
_mouse_visible	db 0Ah dup(0)		; DATA XREF: _mouse_initw
					; _mouse_deinit+Cw ...
_x_storage	dw  0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0
					; DATA XREF: _f2_draw_waves+1Eo
					; _f2_draw_waves2+Do ...
		dw  0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0
		db    0
		align 10h
; char _buffer_1[272]
_buffer_1	db 200h	dup(0)		; DATA XREF: _start-30o
					; _start:loc_1906Eo ...
					; 2800h
_byte_1E0E0	db 7BBh	dup(0)		; DATA XREF: _f5_draw_spectr+29Do
					; _f5_draw_spectr+56Do
_byte_1E89B	db 1E44h dup(0)
		align 10h
_buffer_2	db 2800h dup(0)		; DATA XREF: _init_vga_waves+173o
_buffer_1seg	dw 0			; DATA XREF: _text_init2+18Bw
					; seg001:18B2r	...
_buffer_2seg	dw 0			; DATA XREF: seg001:loc_1A913w
					; seg001:19D1r	...
_byte_22EE4	db 1000h dup(0)		; DATA XREF: _f5_draw_spectr+2A8o
					; _f5_draw_spectr+2D0o ...
unk_23EE4	db    0			; DATA XREF: _init_f5_spectr+98o
					; _f5_draw_spectr+2D9o ...
_byte_23EE5	db 63h dup(0)		; DATA XREF: _f5_draw_spectr+5C0o
_byte_23F48	db 12Ch	dup(0)		; DATA XREF: _f5_draw_spectr+5E2o
unk_24074	db    0			; DATA XREF: _f5_draw_spectr+5A9o
					; _f5_draw_spectr+5C9o ...
_byte_24075	db 63h dup(0)		; DATA XREF: _f5_draw_spectr+5D2o
_byte_240D8	db 12Ch	dup(0)		; DATA XREF: _f5_draw_spectr+5EFo
_byte_24204	db 200h	dup(0)		; DATA XREF: _f5_draw_spectr+1Ao
					; _f5_draw_spectr+2A5o ...
_palette_24404	db    0			; DATA XREF: _init_vga_waves+17o
		db    1
		db    2
		db    3
		db    4
		db    5
		db    6
		db    7
		db    8
		db    9
		db  0Ah
		db  0Bh
		db  0Ch
		db  0Dh
		db  0Eh
		db  0Fh
		db    0
_vga_palette	db 0,0,0		; DATA XREF: _init_vga_waves+1Fo
		db 2,2,6
		db 4,5,0Dh
		db 7,8,15h
		db 0Ah,0Bh,1Bh
		db 0Dh,0Eh,21h
		db 10h,11h,2Ah
		db 13h,14h,31h
		db 0,2Ah,2Ah
		db 0,2Dh,2Dh
		db 0,30h,30h
		db 0,33h,33h
		db 0,36h,36h
		db 0,39h,39h
		db 0,3Ch,3Ch
		db 0,3Fh,3Fh
_word_24445	dw 0			; DATA XREF: _dosexec+69w _dosexec+78o ...
		dd unk_24453
		dd unk_24456
		dd unk_24456
unk_24453	db    0			; DATA XREF: dseg:7C57o
		db  20h
		db  0Dh
unk_24456	db  20h			; DATA XREF: dseg:7C5Bo dseg:7C5Fo
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
; struct struct_0 _str_24461
;_str_24461	struct_0 <6, 9,	49h, 9,	offset loc_1957F> ; DATA XREF: _start+79Do
;		struct_0 <6, 19h, 49h, 19h, offset loc_1953C>
;		struct_0 <8, 0Ah, 47h, 18h, offset loc_19880>
;		struct_0 <2, 1,	4Dh, 4,	offset loc_19762>
;		dw 0FFFFh
; struct struct_0 stru_2448B
;stru_2448B	struct_0 <6, 9,	49h, 9,	offset loc_1957F> ; DATA XREF: _start+7B9o
;		struct_0 <6, 19h, 49h, 19h, offset loc_1953C>
;		struct_0 <2, 1,	4Dh, 4,	offset loc_1964E>
;		dw 0FFFFh
; struct struct_0 stru_244AB
;stru_244AB	struct_0 <2, 1,	4Dh, 4,	offset _l_enter>	; DATA XREF: _keyb_screen_loop+4AFo
;		dw 0FFFFh
; struct struct_0 stru_244B7
;stru_244B7	struct_0 <0, 0,	4Fh, 31h, offset _l_esc>	; DATA XREF: _keyb_screen_loop:loc_1A3C5o
		dw 0FFFFh
		db    0
unk_244C4	db    0			; DATA XREF: _spectr_1B084+14Ew
					; _spectr_1B084+18Br ...
		db    0
		db    0
		db    0
_dword_244C8	dd 0			; DATA XREF: _spectr_1B084+39w
					; _spectr_1B084+62r ...
_multip_244CC	dd 0			; DATA XREF: _spectr_1B084+2Fw
					; _spectr_1B084+152r ...
_multip_244D0	dd 0			; DATA XREF: _spectr_1B084+25w
					; _spectr_1B084+169r ...
_dword_244D4	dd 0			; DATA XREF: _spectr_1B084+3Dw
					; _spectr_1B084+6Ar ...
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
_dword_244E4	dd 0			; DATA XREF: _spectr_1B084+8Bw
					; _spectr_1B084+104r ...
_dword_244E8	dd 0			; DATA XREF: _spectr_1B084+9Aw
					; _spectr_1B084+115r ...
_dword_244EC	dd 0			; DATA XREF: _spectr_1B084+A9w
					; _spectr_1B084+BFr ...
_dword_244F0	dd 0			; DATA XREF: _spectr_1B084+B6w
					; _spectr_1B084+CFr ...
_dword_244F4	dd 0			; DATA XREF: _spectr_1B084+66w
					; _spectr_1B084+BAr ...
_dword_244F8	dd 0			; DATA XREF: _spectr_1B084+6Ew
					; _spectr_1B084+DFr ...
_dword_244FC	dd 0			; DATA XREF: _spectr_1B084+CBw
					; _spectr_1B084+108r ...
_dword_24500	dd 0			; DATA XREF: _spectr_1B084+DBw
					; _spectr_1B084+119r ...
_dword_24504	dd 0			; DATA XREF: _spectr_1B084+100w
					; _spectr_1B084+10Dr ...
_dword_24508	dd 0			; DATA XREF: _spectr_1B084+F0w
					; _spectr_1B084+11Er ...
_word_2450C	dw 0			; DATA XREF: _spectr_1B406+4w
					; _spectr_1B406+77r ...
_word_2450E	dw 0			; DATA XREF: _spectr_1B084w
					; _spectr_1B084+7Ar ...
		db    0
		db    0
		db    0
		db    0
_word_24514	dw 0			; DATA XREF: _spectr_1B084+Fr
					; _spectr_1B084+42r ...
_word_24516	dw 0			; DATA XREF: _spectr_1B406+61w
					; _spectr_1B406:loc_1B46Dr ...
_word_24518	dw 0			; DATA XREF: _spectr_1B406+B2w
					; _spectr_1B406+B8r ...
_word_2451A	dw 0			; DATA XREF: _spectr_1B406+D8w
					; _spectr_1B406+DBr
_word_2451C	dw 0			; DATA XREF: _spectr_1B406+74w
					; _spectr_1B406+BDr ...
_word_2451E	dw 0			; DATA XREF: _spectr_1B406+D1w
					; _spectr_1B406+E7r
_word_24520	dw 0			; DATA XREF: _spectr_1B406+Ar
					; _f5_draw_spectr+2CAw ...
_word_24522	dw 0			; DATA XREF: _spectr_1B406+10w
					; _spectr_1B406+3Fr ...
_word_24524	dw 0			; DATA XREF: _init_f5_spectr:loc_1B080w
					; _f5_draw_spectr+2C7r ...
_tabledword_24526 dd    0,65536,46340,25079,12785,6423,3215,1608, 804, 402
					; DATA XREF: _spectr_1B084+20r
					; _spectr_1B084+1CDr ...
		dd  201, 100,  50,  25,	 12
_tabledword_24562 dd -131072,-65536,-19196,-4989,-1260,-316, -79, -20,  -5
					; DATA XREF: _spectr_1B084+2Ar
					; _spectr_1B084+1DAr ...
		dd   -2,  -1,  -1,  -1,	 -1,   0
		db    0
		db    0
dseg ends

; ===========================================================================

; Segment type:	Regular
seg003 segment byte public 'UNK' use16
		assume cs:seg003
		assume es:nothing, ss:nothing, ds:dseg,	fs:nothing, gs:nothing
_a070295122642	db '07/02/95 12:26:42',0 ; DATA XREF: seg003:off_2462Eo
					; seg003:off_24656o
_pointer_245B4	dd 0			; DATA XREF: sub_135CA+1Cr
					; sub_135CA:loc_135FDw	...
_dma_buf_pointer	dd 0			; DATA XREF: _mod_readfile_11F4E+9Cw
					; _mod_readfile_11F4E+144r ...
_dword_245BC	dd 0			; DATA XREF: _someplaymode+59w
					; sub_13177+1Dr ...
_dword_245C0	dd 0			; DATA XREF: _someplaymode:loc_12C3Cw
					; sub_13177+31r ...
_dword_245C4	dd 0			; DATA XREF: _mod_n_t_module:loc_101F4r
					; _mod_1024A+33w ...
off_245C8	dw offset _moduleread	; DATA XREF: sub_12DA8+3Ew
					; sub_13623:loc_13791r	...
off_245CA	dw offset _moduleread	; DATA XREF: sub_12DA8+38w
					; sub_135CA+4Dr ...
off_245CC	dw offset _moduleread	; DATA XREF: _change_volume+1Ar
					; sub_12DA8+44w ...
off_245CE	dw offset _moduleread	; DATA XREF: _volume_12A66+Fr
					; sub_12DA8+4Aw ...
_savesp_245D0	dw 0			; DATA XREF: _moduleread+15w
					; _moduleread+B6r ...
_word_245D2	dw 0			; DATA XREF: _mod_n_t_module+9w
					; _mod_n_t_module+43w ...
_mod_channels_number	dw 0			; DATA XREF: _moduleread+81r
					; _moduleread+87r ...
_word_245D6	dw 0			; DATA XREF: _clean_11C43+4Aw
					; sub_12B83+52w ...
_word_245D8	dw 0			; DATA XREF: _clean_11C43+50w
					; sub_12B83+57w
_word_245DA	dw 0			; DATA XREF: _clean_11C43+56w
					; sub_12B83+5Cw
_word_245DC	dw 0			; DATA XREF: sub_13623+12Dw
					; sub_13623+29Cr ...
_freq_245DE	dw 0			; DATA XREF: _mod_1024A+40r
					; __2stm_module+26w ...
off_245E0	dw offset _chrin		; DATA XREF: sub_12DA8+E9w
					; prepare_samples+57Fr
off_245E2	dw offset _myin		; DATA XREF: sub_12DA8+EFw
					; prepare_samples:loc_17202r
_word_245E4	dw 0			; DATA XREF: _configure_timer+18r
					; sub_13CF6+4Dw ...
_word_245E8	dw 0			; DATA XREF: sub_12DA8+5Aw
					; sub_12DA8+F9w ...
_word_245EA	dw 0			; DATA XREF: sub_13CF6:loc_13D36w
					; prepare_samples+13r
_word_245EC	dw 0			; DATA XREF: sub_13CF6+44w
					; prepare_samples+19r
_word_245EE	dw 0			; DATA XREF: sub_13CF6+47w
					; prepare_samples+Aw	...
_word_245F0	dw 0			; DATA XREF: _clean_11C43+21w
					; sub_1265D+46r ...
_word_245F2	dw 0			; DATA XREF: _mod_n_t_module+12Dr
					; _mod_102F5+17w ...
_my_seg_index	dw 0			; DATA XREF: _psm_module+136r
					; _mem_reallocxr ...
_word_245F6	dw 0			; DATA XREF: _clean_11C43+27w
					; sub_1265D+42r ...
_word_245F8	dw 0			; DATA XREF: _mod_1021E:loc_10230w
					; _far_module+9Aw ...
_word_245FA	dw 0			; DATA XREF: _mod_1021E+8w
					; __2stm_module:loc_104F2w ...
_volume_245FC	dw 100h			; DATA XREF: sub_1265D+5r
					; _change_volume+Bw ...
_amplification	dw 100			; DATA XREF: _clean_11C43+83w
					; _volume_prepare_waves+72r ...
_samples_outoffs_24600	dw 0			; DATA XREF: sub_12EBA+2Cw
					; sub_13017+1Bw ...
_word_24602	dw 0			; DATA XREF: sub_12EBA+32w
					; _proaud_14700:loc_14E10w ...
_interrupt_mask	dw 0			; DATA XREF: _setsnd_handler+Cw
					; _restore_intvector+3r
_old_intprocoffset dw 0			; DATA XREF: _setsnd_handler+3Aw
					; _restore_intvector+14r
_old_intprocseg	dw 0			; DATA XREF: _setsnd_handler+3Ew
					; _restore_intvector+1Ar
_intvectoffset	dw 0			; DATA XREF: _setsnd_handler+2Dw
					; _restore_intvector+Cr
_word_2460C	dw 0			; DATA XREF: _deinit_125B9+12r
					; sub_12D35+1Fr ...
_word_2460E	dw 0			; DATA XREF: _gravis_int+D5w
					; _proaud_set+8w ...
_word_24610	dw 0			; DATA XREF: _volume_prep+6w
					; sub_1279A+4r	...
_my_size		dw 0			; DATA XREF: _volume_prep+9w
					; sub_1279A+Dr	...
_word_24614	dw 0			; DATA XREF: sub_154F4+3Ew
					; sub_1609F-909r ...
_byte_24616	db 0			; DATA XREF: sub_154F4+41w
					; sub_154F4+52w ...
_byte_24617	db 0			; DATA XREF: _ems_realloc2w
					; _mod_readfile_11F4Ew ...
_byte_24618	db 0			; DATA XREF: sub_13D95-6w sub_13D95w ...
_byte_24619	db 0			; DATA XREF: sub_13D95+16w
					; _gravis_int+7Cw ...
_byte_2461A	db 0			; DATA XREF: _s3m_module+1Fw
					; _useless_writeinr+44r	...
_byte_2461B	db 0			; DATA XREF: _moduleread:loc_10028w
					; _moduleread:loc_1007Br ...
_byte_2461C	db 0			; DATA XREF: sub_12B18+14w
					; sub_12B18:loc_12B71w
_byte_2461D	db 0			; DATA XREF: sub_12B18+19w
					; sub_12B18+51w
_byte_2461E	db 0			; DATA XREF: _s3m_module:loc_10652r
					; _e669_module+3Cr ...
_byte_2461F	db 0			; DATA XREF: _s3m_module+C5r
					; _e669_module+38r ...
_byte_24620	db 0			; DATA XREF: sub_12EBA+38w
					; _proaud_14700+71Cw ...
_byte_24621	db 0			; DATA XREF: sub_12EBA+3Dw
					; _proaud_14700+73Bw ...
_sndflags_24622	db 0			; DATA XREF: _useless_11787+9r
					; _inr_read_118B0+AEr ...
_is_stereo	db 0			; DATA XREF: sub_1265D+33r
					; sub_12DA8+50w ...
_bit_mode	db 8			; DATA XREF: sub_12DA8+55w
					; sub_12DA8+E2r ...
_byte_24625	db 0			; DATA XREF: _clean_11C43+89w
					; _getset_amplif+Ew ...
_gravis_port	dw 0			; DATA XREF: _volume_prep+61r
					; _gravis_13215+49r ...
_byte_24628	db 0			; DATA XREF: _mod_readfile_11F4E+1BFr
					; sub_1265D+27r ...
_byte_24629	db 20h			; DATA XREF: _someplaymode+64r
					; _gravis_18079:loc_18088w ...
_irq_number_0	db 0			; DATA XREF: _gravis_init+35w
					; _gravis_init+59w ...
_byte_2462B	db 0			; DATA XREF: _gravis_init+38w
					; _gravis_init+61w ...
_dma_channel_0	db 0			; DATA XREF: _mod_readfile_11F4E+8Er
					; _volume_prep+43r ...
_byte_2462D	db 0			; DATA XREF: _gravis_init+41w
					; _gravis_init+6Dw ...
off_2462E	dw offset _a070295122642	; DATA XREF: sub_13044+1Bw
					; sub_13044+2Ew ...
					; "07/02/95 12:26:42"
_word_24630	dw 0			; DATA XREF: _clean_11C43+32w
					; _mod_readfile_11F4E+EDr ...
_word_24632	dw 0			; DATA XREF: _gravis_int+DEr
					; _nongravis_182E7+4Dw
_word_24634	dw 0			; DATA XREF: _gravis_int+D8r
					; _nongravis_182E7+45w
_word_24636	dw 0			; DATA XREF: _gravis_int+B5r
					; _gravis_int+D1w ...
_freq2		dw 0			; DATA XREF: _read_sndsettings+2Cr
					; _gravis_18079+1Dw
		db    0
_byte_2463B	db 0			; DATA XREF: _gravis_init+Fw
					; _gravis_init+1Bw ...
_dword_2463C	dd 0			; DATA XREF: _someplaymode+8Aw
					; _gravis_13215+2Ar
_dword_24640	dd 0			; DATA XREF: _memfree_125DA+13w
					; _volume_prep+4Dw ...
		db    0
_byte_24645	db 0			; DATA XREF: sub_182DB+5w
					; _nongravis_182E7+Cw ...
_word_24646	dw 0			; DATA XREF: _proaud_init:loc_145A6w
_sound_port	dw 0			; DATA XREF: _proaud_init+42w
					; _proaud_init+90r ...
_byte_2464A	db 0			; DATA XREF: _proaud_init+6Aw
					; _proaud_init+9Br ...
_byte_2464B	db 0			; DATA XREF: _proaud_init+64w
					; _proaud_set+11r ...
_base_port2	dw 0			; DATA XREF: _wss_init:loc_147C3w
					; _wss_init+75r	...
_dma_channel2	db 0			; DATA XREF: _wss_init:loc_147DCw
					; _wss_init:loc_14801r ...
_irq_number2	db 0			; DATA XREF: _wss_init:loc_147D0w
					; _wss_init+41r	...
_byte_24650	db 0			; DATA XREF: _wss_set+54w _wss_sndoff+4r
_byte_24651	db 0			; DATA XREF: _wss_set+61w _wss_sndoff+Cr
_sb_base_port	dw 0			; DATA XREF: _sb16_on+17r _sb16_on+44r ...
_word_24654	dw 0			; DATA XREF: _sb16_detect_port+78w
off_24656	dw offset _a070295122642	; DATA XREF: sub_13044+21w
					; sub_13044+34w ...
					; "07/02/95 12:26:42"
_dma_chn_mask	db 0			; DATA XREF: _sb16_init+4Bw
					; _sb16_init:loc_14AF5w	...
_sb_irq_number	db 0			; DATA XREF: _sb16_init+1Cw
					; _sb16_init:loc_14AB3w	...
_sb_timeconst	db 0			; DATA XREF: _sbpro_init+51w _sb_set-D1r ...
_word_2465C	dw 0			; DATA XREF: _snd_initialze:loc_15302w
					; _midi_153F1r ...
_freq1		dw 22050		; DATA XREF: _volume_prepare_waves+48r
					; _someplaymode+49r ...
_fhandle_module	dw 0			; DATA XREF: _moduleread+19w
					; _moduleread:loc_1006Br ...
_word_24662	dw 0			; DATA XREF: _moduleread:loc_1002Dw
					; _moduleread+73w ...
_byte_24664	db 0			; DATA XREF: _sb_set-FFw
					; _sbpro_sndoff+1Dr
_byte_24665	db 0			; DATA XREF: _moduleread:loc_10064w
					; _memfree_125DA:loc_125F6r
_byte_24666	db 0			; DATA XREF: sub_1265D+18r
					; sub_13CF6+2w
_byte_24667	db 0			; DATA XREF: sub_1265D+1Dr
					; _eff_13CE8+4w	...
_byte_24668	db 0			; DATA XREF: sub_12EBA+71w
					; sub_12F56+1Aw ...
_byte_24669	db 0			; DATA XREF: sub_12EBA+9w sub_12F56+Cw ...
_byte_2466A	db 0			; DATA XREF: sub_12EBA+Ew
					; _eff_13B06+5Aw ...
_byte_2466B	db 0			; DATA XREF: sub_12EBA+13w
					; _eff_13C02+25w ...
_byte_2466C	db 0			; DATA XREF: sub_12EBA+18w
					; _eff_13CC9+10w ...
_byte_2466D	db 0			; DATA XREF: sub_12EBA+1Dw
					; _eff_13CC9+7r	...
_byte_2466E	db 0			; DATA XREF: _mod_readfile_11F4E:loc_1212Br
					; _volume_prep:loc_1276Cr ...
_dma_mode	db 0			; DATA XREF: _proaud_set+3w _wss_set+3w	...
_sb_int_counter	db 0			; DATA XREF: _sb_test_interruptw
					; _sb_test_interrupt:loc_184B0r	...
_byte_24671	db 0			; DATA XREF: sub_1265D+3Ar
					; sub_12EBA+22w ...
_flag_playsetttings db 0			; DATA XREF: _clean_11C43+68r
					; _get_playsettings+6r ...
_byte_24673	db 0			; DATA XREF: _s3m_module+14w
					; _s3m_module+2Bw ...
_byte_24674	db 0			; DATA XREF: _mod_readfile_11F4E+32w
					; _mod_readfile_11F4E+BFw ...
_byte_24675	db 0			; DATA XREF: _mod_readfile_11F4E+2Aw
					; _mod_readfile_11F4E+B7w ...
_byte_24676	db 0			; DATA XREF: _mod_sub_12220:loc_12228r
					; _mod_sub_12220+23w
_byte_24677	db 0			; DATA XREF: _midi_15413+4r
					; _midi_15413+Aw
_byte_24678	db 0			; DATA XREF: _midi_15413+2Aw
_byte_24679	db 0			; DATA XREF: __2stm_module+31w
					; _s3m_module+EFw ...
_byte_2467A	db 0			; DATA XREF: __2stm_module+35w
					; _s3m_module+F5w ...
_byte_2467B	db 0			; DATA XREF: _far_module+2Cw
					; sub_12EBA+7Fw ...
_byte_2467C	db 0			; DATA XREF: _far_module+2Fw
					; sub_12EBA+82w ...
_byte_2467D	db 0			; DATA XREF: sub_13044:loc_1305Aw
					; sub_13044:loc_1306Dw	...
_byte_2467E	db 0			; DATA XREF: _s3m_module+Fw
					; _e669_module+1Fw ...
_play_state	db 0			; DATA XREF: _getset_playstate+Bw
					; _getset_playstate:loc_12CA7r ...
_snd_init	db 0			; DATA XREF: sub_12D05+Br
					; _snd_initialzer ...
_snd_set_flag	db 0			; DATA XREF: sub_12DA8+60w _snd_on+7r ...
_byte_24682	db 0			; DATA XREF: prepare_samples:loc_16C88w
					; prepare_samples+3Fr ...
_byte_24683	db 0			; DATA XREF: sub_154F4+6w
					; sub_1609F:loc_15698r	...
_dword_24684	dd 0			; DATA XREF: _alloc_dma_bufw
					; _alloc_dma_buf+22r ...
		db    0
_word_2468C	dw 0			; DATA XREF: _alloc_dma_buf:loc_18A0Aw
					; _alloc_dma_buf+B5r
;		align 10h
		db    0
;algn_24691::
_dword_24694	dd 0			; DATA XREF: _dma_186E3+5Dr
					; _dma_186E3+A2r ...
_myseg_24698	dw 0			; DATA XREF: _alloc_dma_buf+31w
					; _alloc_dma_buf+A6r ...
_memflg_2469A	db 0			; DATA XREF: _alloc_dma_buf+8w
					; _alloc_dma_buf+B0w ...
_byte_2469B	db 0			; DATA XREF: _alloc_dma_buf+Dw
_byte_2469C	db 0			; DATA XREF: _alloc_dma_buf+4w
					; _alloc_dma_buf+3Br
		db 0
_ems_pageframe	dw 0			; DATA XREF: _useless_11787+3Er
					; _ems_init+61w	...
_ems_handle	dw 0			; DATA XREF: _ems_init+74w
					; _ems_release+Dr ...
_ems_log_pagenum	dw 0			; DATA XREF: _ems_init+7Dw
					; _ems_release+15w ...
_ems_enabled	db 0			; DATA XREF: _ems_initw	_ems_init+78w ...
_byte_246A5	db 0			; DATA XREF: _ems_save_mapctx+1Bw
					; _ems_restore_mapctx+7r
_word_246A6	dw 0			; DATA XREF: sub_12CAD+9o
					; sub_12CAD+18w
_byte_246A8	db 0			; DATA XREF: sub_12CAD+14w
_word_246A9	dw 0			; DATA XREF: sub_12CAD+10w
_module_type_text dd 20202020h		; DATA XREF: _mod_n_t_modulew
					; _mod_n_t_module:_mod_flt8_modulew ...
; char asc_246B0[32]
asc_246B0	db '                                ' ; DATA XREF: _mod_1021E+22o
					; __2stm_module+47o ...
_moduleflag_246D0 dw 0			; DATA XREF: _mod_n_t_module+3Dw
					; _mod_read_10311+12r ...
_sndcard_type	db 0			; DATA XREF: _mtm_module+2Er
					; _far_module+3Fr ...
_snd_base_port	dw 0			; DATA XREF: _read_sndsettings+9r
					; _useless_12D61+9w ...
_irq_number	db 0			; DATA XREF: _read_sndsettings+Dr
					; _useless_12D61+Cw ...
_dma_channel	db 0			; DATA XREF: _read_sndsettings+11r
					; _useless_12D61+Fw ...
_freq_246D7	db 0			; DATA XREF: _read_sndsettings+15r
					; sub_12DA8+17w ...
_byte_246D8	db 0			; DATA XREF: _read_sndsettings+19r
					; _useless_12D61+12w ...
_byte_246D9	db 0			; DATA XREF: _read_sndsettings+1Dr
					; _useless_12D61+15w ...
_config_word	dw 0			; DATA XREF: _ems_init+8r
					; _read_sndsettings:loc_12CFFr ...
_byte_246DC	db 0			; DATA XREF: sub_12DA8+33w
					; sub_12DA8+88r
_word_246DE	dw 6B00h,6500h,5F40h,5A00h,54C0h,5000h,4B80h,4740h,4340h
					; DATA XREF: sub_13826+42o
					; sub_13826:loc_1386Cr
		dw 3F80h,3C00h,38A0h
_table_246F6	dw 8363,8422,8482,8543,8604,8667,8730,8794,7901,7954,8007
					; DATA XREF: _eff_13BC8+21r
		dw 8062,8116,8191,8231,8305
_table_24716	dw 8000h,9000h,0A000h,0A952h,0B000h,0B521h,0B952h,0BCDEh
					; DATA XREF: sub_13044+41o
		dw 0C000h,0C2B5h,0C521h,0C752h,0C952h,0CB29h,0CCDEh,0CE74h
		dw 0D000h,0D164h,0D2B5h,0D3F3h,0D521h,0D640h,0D752h,0D858h
		dw 0D952h,0DA42h,0DB29h,0DC07h,0DCDEh,0DDACh,0DE74h,0DF35h
		dw 0E000h,0E0B5h,0E164h,0E20Fh,0E2B5h,0E356h,0E3F3h,0E48Ch
		dw 0E521h,0E5B2h,0E640h,0E6CBh,0E752h,0E7D6h,0E858h,0E8D6h
		dw 0E952h,0E9CCh,0EA42h,0EAB7h,0EB29h,0EB99h,0EC07h,0EC73h
		dw 0ECDEh,0ED46h,0EDACh,0EE11h,0EE74h,0EED5h,0EF35h,0EF93h
		dw 0EFF0h
_table_24798	dw 8000h,9800h,0A000h,0A800h,0B000h,0B400h,0B800h,0BC00h
					; DATA XREF: sub_13044+1Bo
		dw 0C000h,0C200h,0C400h,0C600h,0C800h,0CA00h,0CC00h,0CE00h
		dw 0D000h,0D100h,0D200h,0D300h,0D400h,0D500h,0D600h,0D700h
		dw 0D800h,0D900h,0DA00h,0DB00h,0DC00h,0DD00h,0DE00h,0DF00h
		dw 0E080h,0E100h,0E180h,0E200h,0E280h,0E300h,0E380h,0E400h
		dw 0E480h,0E500h,0E580h,0E600h,0E680h,0E700h,0E780h,0E800h
		dw 0E880h,0E900h,0E980h,0EA00h,0EA80h,0EB00h,0EB80h,0EC00h
		dw 0EC80h,0ED00h,0ED80h,0EE00h,0EE80h,0EF00h,0EF80h,0EFF0h
_table_24818	dw 8000h,9800h,0A000h,0A800h,0B000h,0B400h,0B800h,0BC00h
					; DATA XREF: sub_13044+2Eo
		dw 0C000h,0C200h,0C400h,0C600h,0C800h,0CA00h,0CC00h,0CE00h
		dw 0D000h,0D100h,0D200h,0D300h,0D400h,0D500h,0D600h,0D700h
		dw 0D800h,0D900h,0DA00h,0DB00h,0DC00h,0DD00h,0DE00h,0DF00h
		dw 0E080h,0E100h,0E180h,0E200h,0E280h,0E300h,0E380h,0E400h
		dw 0E480h,0E500h,0E580h,0E600h,0E680h,0E700h,0E780h,0E800h
		dw 0E880h,0E900h,0E980h,0EA00h,0EA80h,0EB00h,0EB80h,0EC00h
		dw 0EC80h,0ED00h,0ED80h,0EE00h,0EE80h,0EF00h,0EF80h,0EFF0h
_table_24898	db 1Eh,1Eh,1Eh,1Eh,1Eh,1Eh,1Eh,1Eh,1Eh,1Eh,1Eh,1Eh,1Eh,1Eh,1Eh,1Eh
					; DATA XREF: _text:512Do
		db 1Eh,1Eh,1Eh,1Eh,1Eh,1Eh,1Dh,1Dh,1Dh,1Dh,1Dh,1Dh,1Dh,1Dh,1Dh,1Dh
		db 1Dh,1Dh,1Dh,1Dh,1Dh,1Dh,1Dh,1Dh,1Dh,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch
		db 1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch
		db 1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Ch,1Bh,1Bh,1Bh,1Bh,1Bh,1Bh,1Bh,1Bh,1Bh
		db 1Bh,1Bh,1Bh,1Bh,1Bh,1Bh,1Bh,1Bh,1Bh,1Ah,1Ah,1Ah,1Ah,1Ah,1Ah,1Ah
		db 1Ah,1Ah,19h,19h,19h,19h,19h,19h,19h,18h,18h,18h,18h,17h,17h,17h
		db 17h,16h,16h,15h,15h,14h,14h,13h,13h,13h,12h,12h,11h,11h,10h,10h
		db 0Fh,0Fh,0Eh,0Eh,0Dh,0Dh,0Ch,0Ch,0Bh,0Bh,0Ah,0Ah,0Ah,9,9,8
		db 8,8,8,7,7,7,7,6,6,6,6,6,6,5,5,5
		db 5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4
		db 4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3
		db 3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2
		db 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
		db 2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
		db 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
_word_24998	dw 6B00h,6500h,5F40h,5A00h,54C0h,5000h,4B80h,4740h,4340h
					; DATA XREF: sub_13623+D2o
					; _eff_13BC8+18o
		dw 3F80h,3C00h,38A0h,3580h,3280h,2FA0h,2D00h,2A60h,2800h
		dw 25C0h,23A0h,21A0h,1FC0h,1E00h,1C50h,1AC0h,1940h,17D0h
		dw 1680h,1530h,1400h,12E0h,11D0h,10D0h,0FE0h,0F00h,0E20h
		dw 0D60h,0CA0h,0BE0h,0B40h,0AA0h,0A00h,970h,8F0h,870h
		dw 7F0h,780h,710h,6B0h,650h,5F0h,5A0h,550h,500h,4B0h,470h
		dw 430h,3F0h,3C0h,380h,6A40h,6440h,5EA0h,5960h,5440h,4FA0h
		dw 4B20h,46E0h,42E0h,3F20h,3BA0h,3840h,3520h,3220h,2F50h
		dw 2CB0h,2A20h,27D0h,2590h,2370h,2170h,1F90h,1DD0h,1C20h
		dw 1A90h,1910h,17B0h,1650h,1510h,13E0h,12C0h,11C0h,10C0h
		dw 0FD0h,0EF0h,0E10h,0D50h,0C90h,0BD0h,0B30h,0A90h,9F0h
		dw 960h,8E0h,860h,7E0h,770h,710h,6A0h,640h,5E0h,590h,540h
		dw 4F0h,4B0h,470h,430h,3F0h,3B0h,380h,6980h,6380h,5E00h
		dw 58A0h,53C0h,4F00h,4AA0h,4660h,4280h,3EC0h,3B40h,37E0h
		dw 34C0h,31C0h,2F00h,2C50h,29E0h,2780h,2550h,2330h,2140h
		dw 1F60h,1DA0h,1BF0h,1A60h,18E0h,1780h,1630h,14F0h,13C0h
		dw 12A0h,11A0h,10A0h,0FB0h,0ED0h,0E00h,0D30h,0C70h,0BC0h
		dw 0B10h,0A70h,9E0h,950h,8D0h,850h,7D0h,760h,700h,690h
		dw 630h,5E0h,580h,530h,4F0h,4A0h,460h,420h,3E0h,3B0h,380h
		dw 68C0h,62E0h,5D40h,5800h,5320h,4E80h,4A00h,45E0h,4200h
		dw 3E40h,3AC0h,3780h,3460h,3170h,2EA0h,2C00h,2990h,2740h
		dw 2500h,22F0h,2100h,1F20h,1D60h,1BC0h,1A30h,18B0h,1750h
		dw 1600h,14C0h,13A0h,1280h,1180h,1080h,0F90h,0EB0h,0DE0h
		dw 0D10h,0C60h,0BB0h,0B00h,0A60h,9D0h,940h,8C0h,840h,7D0h
		dw 760h,6F0h,680h,630h,5D0h,580h,530h,4E0h,4A0h,460h,420h
		dw 3E0h,3B0h,370h,6800h,6220h,5CA0h,5760h,5280h,4DE0h
		dw 4980h,4560h,4180h,3DE0h,3A60h,3720h,3400h,3110h,2E50h
		dw 2BB0h,2940h,26F0h,24C0h,22B0h,20C0h,1EF0h,1D30h,1B90h
		dw 1A00h,1880h,1720h,15E0h,14A0h,1380h,1260h,1160h,1060h
		dw 0F70h,0E90h,0DC0h,0D00h,0C40h,0B90h,0AF0h,0A50h,9C0h
		dw 930h,8B0h,830h,7C0h,750h,6E0h,680h,620h,5C0h,570h,520h
		dw 4E0h,490h,450h,410h,3E0h,3A0h,370h,6740h,6160h,5C00h
		dw 56C0h,51E0h,4D60h,4900h,44E0h,4100h,3D60h,39E0h,36A0h
		dw 33A0h,30B0h,2E00h,2B60h,28F0h,26B0h,2480h,2270h,2080h
		dw 1EB0h,1CF0h,1B50h,19D0h,1860h,1700h,15B0h,1480h,1350h
		dw 1240h,1140h,1040h,0F50h,0E80h,0DB0h,0CE0h,0C30h,0B80h
		dw 0AE0h,0A40h,9B0h,920h,8A0h,820h,7B0h,740h,6D0h,670h
		dw 610h,5C0h,570h,520h,4D0h,490h,450h,410h,3D0h,3A0h,360h
		dw 6680h,60C0h,5B40h,5620h,5160h,4CC0h,4880h,4460h,4080h
		dw 3CE0h,3980h,3640h,3340h,3060h,2DA0h,2B10h,28B0h,2660h
		dw 2440h,2230h,2040h,1E70h,1CC0h,1B20h,19A0h,1830h,16D0h
		dw 1590h,1450h,1330h,1220h,1120h,1020h,0F40h,0E60h,0D90h
		dw 0CD0h,0C10h,0B70h,0AC0h,0A30h,9A0h,910h,890h,810h,7A0h
		dw 730h,6D0h,660h,600h,5B0h,560h,510h,4D0h,480h,440h,400h
		dw 3D0h,390h,360h,65C0h,6000h,5AA0h,5580h,50C0h,4C40h
		dw 47E0h,43E0h,4020h,3C80h,3920h,35E0h,32E0h,3000h,2D50h
		dw 2AC0h,2860h,2620h,23F0h,21F0h,2010h,1E40h,1C90h,1AF0h
		dw 1970h,1800h,16B0h,1560h,1430h,1310h,1200h,1100h,1000h
		dw 0F20h,0E40h,0D80h,0CC0h,0C00h,0B50h,0AB0h,0A10h,980h
		dw 900h,880h,800h,790h,720h,6C0h,660h,600h,5A0h,550h,500h
		dw 4C0h,480h,440h,400h,3C0h,390h,360h,7160h,6B00h,6500h
		dw 5F40h,5A00h,54C0h,5000h,4B80h,4740h,4340h,3F80h,3C00h
		dw 38B0h,3580h,3280h,2FA0h,2D00h,2A60h,2800h,25C0h,23A0h
		dw 21A0h,1FC0h,1E00h,1C50h,1AC0h,1940h,17D0h,1680h,1530h
		dw 1400h,12E0h,11D0h,10D0h,0FE0h,0F00h,0E20h,0D60h,0CA0h
		dw 0BE0h,0B40h,0AA0h,0A00h,970h,8F0h,870h,7F0h,780h,710h
		dw 6B0h,650h,5F0h,5A0h,550h,500h,4B0h,470h,430h,3F0h,3C0h
		dw 7080h,6A40h,6440h,5EA0h,5960h,5460h,4F80h,4B20h,46E0h
		dw 42E0h,3F20h,3BA0h,3840h,3520h,3220h,2F50h,2CB0h,2A30h
		dw 27C0h,2590h,2370h,2170h,1F90h,1DD0h,1C20h,1A90h,1910h
		dw 17B0h,1650h,1510h,13E0h,12C0h,11C0h,10C0h,0FD0h,0EE0h
		dw 0E10h,0D40h,0C80h,0BD0h,0B30h,0A90h,9F0h,960h,8E0h
		dw 860h,7E0h,770h,700h,6A0h,640h,5E0h,590h,540h,4F0h,4B0h
		dw 470h,430h,3F0h,3B0h,6FC0h,6980h,6380h,5E00h,58A0h,53C0h
		dw 4F00h,4AA0h,4660h,4280h,3EC0h,3B40h,37E0h,34C0h,31C0h
		dw 2F00h,2C50h,29E0h,2780h,2550h,2330h,2140h,1F60h,1DA0h
		dw 1BF0h,1A60h,18E0h,1780h,1630h,14F0h,13C0h,12A0h,11A0h
		dw 10A0h,0FB0h,0ED0h,0DF0h,0D30h,0C70h,0BC0h,0B10h,0A70h
		dw 9E0h,950h,8D0h,850h,7D0h,760h,6F0h,690h,630h,5E0h,580h
		dw 530h,4F0h,4A0h,460h,420h,3E0h,3B0h,6EE0h,68C0h,62E0h
		dw 5D40h,5800h,5320h,4E80h,4A00h,45E0h,4200h,3E40h,3AC0h
		dw 3770h,3460h,3170h,2EA0h,2C00h,2990h,2740h,2500h,22F0h
		dw 2100h,1F20h,1D60h,1BC0h,1A30h,18B0h,1750h,1600h,14C0h
		dw 13A0h,1280h,1180h,1080h,0F90h,0EB0h,0DE0h,0D10h,0C60h
		dw 0BB0h,0B00h,0A60h,9D0h,940h,8C0h,840h,7D0h,760h,6F0h
		dw 680h,630h,5D0h,580h,530h,4E0h,4A0h,460h,420h,3E0h,3B0h
		dw 6E20h,6800h,6220h,5CA0h,5760h,5280h,4DE0h,4980h,4560h
		dw 4180h,3DC0h,3A60h,3710h,3400h,3110h,2E50h,2BB0h,2940h
		dw 26F0h,24C0h,22B0h,20C0h,1EE0h,1D30h,1B90h,1A00h,1880h
		dw 1720h,15E0h,14A0h,1380h,1260h,1160h,1060h,0F70h,0E90h
		dw 0DC0h,0D00h,0C40h,0B90h,0AF0h,0A50h,9C0h,930h,8B0h
		dw 830h,7B0h,750h,6E0h,680h,620h,5C0h,570h,520h,4E0h,490h
		dw 450h,410h,3D0h,3A0h,6D60h,6740h,6160h,5C00h,56C0h,51E0h
		dw 4D60h,4900h,44E0h,4100h,3D60h,39E0h,36B0h,33A0h,30B0h
		dw 2E00h,2B60h,28F0h,26B0h,2480h,2270h,2080h,1EB0h,1CF0h
		dw 1B50h,19D0h,1860h,1700h,15B0h,1480h,1350h,1240h,1140h
		dw 1040h,0F50h,0E80h,0DB0h,0CE0h,0C30h,0B80h,0AE0h,0A40h
		dw 9B0h,920h,8A0h,820h,7B0h,740h,6D0h,670h,610h,5C0h,570h
		dw 520h,4D0h,490h,450h,410h,3D0h,3A0h,6C80h,6680h,60C0h
		dw 5B40h,5620h,5160h,4CC0h,4880h,4460h,4080h,3CE0h,3980h
		dw 3640h,3340h,3060h,2DA0h,2B10h,28B0h,2660h,2440h,2230h
		dw 2040h,1E70h,1CC0h,1B20h,19A0h,1830h,16D0h,1590h,1450h
		dw 1330h,1220h,1120h,1020h,0F40h,0E60h,0D90h,0CD0h,0C10h
		dw 0B70h,0AC0h,0A30h,9A0h,910h,890h,810h,7A0h,730h,6C0h
		dw 660h,600h,5B0h,560h,510h,4D0h,480h,440h,400h,3D0h,390h
		dw 6BC0h,65C0h,6000h,5AA0h,5580h,50C0h,4C40h,47E0h,43E0h
		dw 4020h,3C80h,3920h,35E0h,32E0h,3000h,2D50h,2AC0h,2860h
		dw 2620h,23F0h,21F0h,2010h,1E40h,1C90h,1AF0h,1970h,1800h
		dw 16B0h,1560h,1430h,1310h,1200h,1100h,1000h,0F20h,0E40h
		dw 0D80h,0CB0h,0C00h,0B50h,0AB0h,0A10h,980h,900h,880h
		dw 800h,790h,720h,6C0h,650h,600h,5A0h,550h,500h,4C0h,480h
		dw 440h,400h,3C0h,390h
_table_25118	dw 1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,906,856,808,762,720,678,640,604,570,538,508,480,453
					; DATA XREF: _mod_read_10311:loc_10399r
		dw 428,404,381,360,339,320,302,285,269,254,240,226,214,202,190,180,170,160,151,143,135,127,120,113
		dw 107,101,95,90,85,80,75,71,67,63,60,56,53,50,47,45,42,40,37,35,33,31,30,28
		dw 26,25,23,22,21,20,18,17,16,15,15,14
_table_251C0	db  0,18h,31h,4Ah,61h,78h,8Dh,0A1h,0B4h,0C5h,0D4h,0E0h
					; DATA XREF: _eff_1392F+4Er
					; _eff_139B9+4Fr
		db 0EBh,0F4h,0FAh,0FDh,0FFh,0FDh,0FAh,0F4h,0EBh,0E0h,0D4h
		db 0C5h,0B4h,0A1h,8Dh,78h,61h,4Ah,31h,18h
_table_251E0	db  0,15h,20h,29h,30h,37h,3Dh,44h,49h,4Fh,54h,59h,5Eh
					; DATA XREF: sub_13044+47o
		db 62h,67h,6Bh,6Fh,73h,77h,7Ch,7Fh,83h,86h,8Ah,8Eh,91h
		db 95h,98h,9Bh,9Fh,0A2h,0A5h,0A9h,0ABh,0AFh,0B3h,0B5h
		db 0B8h,0BBh,0BEh,0C1h,0C3h,0C6h,0C9h,0CCh,0CFh,0D1h,0D4h
		db 0D7h,0DAh,0DDh,0DFh,0E2h,0E5h,0E7h,0EAh,0ECh,0EFh,0F1h
		db 0F4h,0F6h,0F9h,0FBh,0FEh,0FFh
_table_25221	db  0, 4, 8,0Ch,10h,14h,18h,1Ch,20h,24h,28h,2Ch,30h,34h
					; DATA XREF: sub_13044+21o
		db 38h,3Ch,40h,44h,48h,4Ch,50h,55h,59h,5Dh,61h,65h,69h
		db 6Dh,71h,75h,79h,7Dh,81h,85h,89h,8Dh,91h,95h,99h,9Dh
		db 0A1h,0A5h,0AAh,0AEh,0B2h,0B6h,0BAh,0BEh,0C2h,0C6h,0CAh
		db 0CEh,0D2h,0D6h,0DAh,0DEh,0E2h,0E6h,0EAh,0EEh,0F2h,0F6h
		db 0FAh,0FFh
_table_25261	db  0, 4, 8,0Ch,10h,14h,18h,1Ch,20h,24h,28h,2Ch,30h,34h
					; DATA XREF: sub_13044+34o
		db 38h,3Ch,40h,44h,48h,4Ch,50h,54h,58h,5Ch,60h,64h,68h
		db 6Ch,70h,74h,78h,7Ch,80h,84h,88h,8Ch,90h,94h,98h,9Ch
		db 0A0h,0A4h,0A8h,0ACh,0B0h,0B4h,0B8h,0BCh,0C0h,0C4h,0C8h
		db 0CCh,0D0h,0D4h,0D8h,0DCh,0E0h,0E4h,0E8h,0ECh,0F0h,0F4h
		db 0F8h,0FFh, 0
		dw offset _sb16_init
		dw offset _covox_init
		dw offset _stereo_init
		dw offset _adlib_init
		dw offset _pcspeaker_init
		dw offset _midi_init
		dw offset _sb16_on
		dw offset _covox_on
		dw offset _stereo_set
		dw offset _adlib_set
		dw offset _pcspeaker_on
		dw offset _midi_set
		dw offset _sb16_off
		dw offset _covox_off
		dw offset _stereo_sndoff
		dw offset _adlib_sndoff
		dw offset _pcspeaker_off
		dw offset _midi_sndoff
		dw offset _sb16_deinit
		dw offset _covox_deinit
		dw offset _stereo_clean
		dw offset _adlib_clean
		dw offset _pcspeaker_deinit
		dw offset _midi_clean
_snd_cards_offs	dw offset _aGravisUltrasoun ; DATA XREF:	seg003:114Eo
					; seg003:1194o	...
					; "Gravis UltraSound"
		dw offset _aProAudioSpectrum ; "Pro Audio Spectrum 16"
		dw offset _aWindowsSoundSyst ; "Windows Sound System"
		dw offset _aSoundBlaster1616 ; "Sound Blaster 16/16ASP"
		dw offset _aSoundBlasterPro ; "Sound Blaster Pro"
		dw offset _aSoundBlaster_0 ; "Sound Blaster"
		dw offset _aCovox_0	; "Covox"
		dw offset _aStereoOn1_0	; "Stereo-On-1"
		dw offset _aAdlibSoundcard_0 ; "Adlib SoundCard"
		dw offset _aPcHonker_0	; "PC Honker"
		dw offset _aGeneralMidi_0 ; "General MIDI"
		dw offset _sb16_txt
		dw offset _sb16_txt
		dw offset _sb16_txt
		dw offset _sb16_txt
		dw offset _sb16_txt
		dw offset _covox_txt
		dw offset _covox_txt
		dw offset _pcspeaker_txt
		dw offset _pcspeaker_txt
		dw offset _midi_txt
off_25326	dw offset _inr_module	; DATA XREF: _moduleread:loc_10040o
					; INR
		db    0
		db    0
		db 16
_aInertiaModule_1 db 'Inertia Module: '
		dw offset _mod_mk_module
		db  38h	; 8
		db    4
		db    4
_aM_k_		db 'M.K.'
		dw offset _mod_mk_module
		db  38h	; 8
		db    4
		db    4
_a_m_k		db '.M.K'
		dw offset _mod_mk_module
		db  38h	; 8
		db    4
		db    4
_aMK		db 'M&K!'
		dw offset _mod_mk_module
		db  38h	; 8
		db    4
		db    4
_aMK_0		db 'M!K!'
		dw offset _mod_mk_module
		db  38h	; 8
		db    4
		db    4
_aGsft		db 'GSFT'
		dw offset _mod_mk_module
		db  38h	; 8
		db    4
		db    4
_aE_g_		db 'E.G.'
		dw offset _mod_mk_module
		db  38h	; 8
		db    4
		db    4
_aFlt4		db 'FLT4'
		dw offset _mod_flt8_module ; FLT8
		db  38h	; 8
		db    4
		db    4
_aFlt8		db 'FLT8'
		dw offset _mod_cd81_module
		db  38h	; 8
		db    4
		db    4
_aCd81		db 'CD81'
		dw offset _mod_cd81_module
		db  38h	; 8
		db    4
		db    4
_aOcta		db 'OCTA'
		dw offset _mod_chn_module
		db  39h	; 9
		db    4
		db    3
_aChn		db 'CHN'
		dw offset _mod_ch_module
		db  3Ah	; :
		db 4
		db    2
_aCh		db 'CH'
		dw offset _mod_tdz_module
		db  38h	; 8
		db    4
		db    3
_aTdz		db 'TDZ'
		dw offset _stm_module	; STM
		db  14h
		db    0
		db    8
_aScream		db '!Scream!'
		dw offset __2stm_module	; 2STM
		db  14h
		db    0
		db    8
_aBmod2stm	db 'BMOD2STM'
		dw offset _s3m_module	; S3M
		db  2Ch	; ,
		db    0
		db    4
_aScrm		db 'SCRM'
		dw offset _mtm_module	; MTM
		db    0
		db    0
		db    3
_aMtm		db 'MTM'
		dw offset _psm_module	; PSM
		db    0
		db    0
		db    4
_aPsm		db 'PSM˛'
		dw offset _far_module	; FAR
		db    0
		db    0
		db    4
_aFar		db 'FAR˛'
		dw offset _ult_module	; ULT
		db    0
		db    0
		db  0Ch
_aMas_utrack_v	db 'MAS_UTrack_V'
		dw offset __669_module	; 669
		db    0
		db    0
		db    2
_aIf		db 'if'
		dw offset _e669_module	; E669
		db    0
		db    0
		db    2
_aJn		db 'JN'
_eModuleNotFound	db 'Module not found',0Dh,0Ah,0 ; DATA XREF: _moduleread+1Co
_aNotEnoughMemory db 'Not enough Memory available',0Dh,0Ah,0
					; DATA XREF: _moduleread:loc_10099o
_aNotEnoughDramOn db 'Not enough DRAM on UltraSound',0Dh,0Ah,0
					; DATA XREF: _mod_readfile_11F4E+1CCo
_aSomeFunctionsOf db 'Some functions of the UltraSound do not work!',0Dh,0Ah
		db 0Ah
		db 'Probably the AT-BUS Clock Speed is too high.',0Dh,0Ah
		db 'Try changing the AT-BUS Clock in the CMOS Setup.',0Dh,0Ah,0
_aCouldNotFindThe db 'Could not find the ULTRASND environment string',0Dh,0Ah,0
					; DATA XREF: _gravis_init:loc_1432Fo
_aCouldNotFindT_0 db 'Could not find the Gravis UltraSound at the specified port addres'
		db 's',0Dh,0Ah,0
_aThisProgramRequ db 'This program requires the soundcards device driver.',0Dh,0Ah,0
_aErrorSoundcardN db 'Error: Soundcard not found!',0Dh,0Ah,'$',0
					; DATA XREF: _proaud_init:loc_1464Fo
					; _wss_test:loc_149ACo ...
_aErrorCouldNotFi db 'Error: Could not find IRQ/DMA!',0Dh,0Ah,0
_aErrorCouldNot_0 db 'Error: Could not find IRQ!',0Dh,0Ah,0 ; DATA XREF: _sb_detect_irq+4Co
_aErrorCouldNot_1 db 'Error: Could not find DMA!',0Dh,0Ah,0 ; DATA XREF: _sb_detect_irq+D6o
_aDeviceNotIniti	db 'Device not initialised!',0 ; DATA XREF: sub_12D05+8o
_aAt		db ' at',0              ; DATA XREF: seg003:10BFo seg003:1152o ...
_aBasePort	db ' base port ',0      ; DATA XREF: seg003:10C3o seg003:1156o ...
_aMixedAt	db ', mixed at ',0      ; DATA XREF: seg003:1173o seg003:11A5o ...
_aKhz		db 'kHz',0              ; DATA XREF: seg003:117Bo seg003:11ADo ...
_aGravisUltrasoun db 'Gravis UltraSound',0 ; DATA XREF: seg003:_snd_cards_offso
					; seg003:10BBo
_gravis_txt	db    1			; DATA XREF: seg003:_sndcards_text_tblo
		db    0
		dw offset _aGravisUltrasoun ; "Gravis UltraSound"
		db    1
		db    0
		dw offset _aAt		; " at"
		db    1
		db    0
		dw offset _aBasePort	; " base port "
		db  0Bh
		db    0
		dw offset _snd_base_port
_aHGf1Irq	db 'h, GF1-IRQ '
		db    4
		db    0
		dw offset _irq_number
_aDramDma	db ', DRAM-DMA '
		db    4
		db    0
		dw offset _dma_channel
		db    0
_aProAudioSpectrum db 'Pro Audio Spectrum 16',0 ; DATA XREF: seg003:0D5Co
_aWindowsSoundSyst db 'Windows Sound System',0 ; DATA XREF: seg003:0D5Eo
_aSoundBlaster1616 db 'Sound Blaster 16/16ASP',0 ; DATA XREF: seg003:0D60o
_aSoundBlasterPro db 'Sound Blaster Pro',0 ; DATA XREF: seg003:0D62o
_aSoundBlaster_0	db 'Sound Blaster',0    ; DATA XREF: seg003:0D64o
_sb16_txt	db    2			; DATA XREF: seg003:0D72o seg003:0D74o ...
		db    0
		dw offset _sndcard_type
		dw offset _snd_cards_offs
		db    1
		db    0
		dw offset _aAt		; " at"
		db    1
		db    0
		dw offset _aBasePort	; " base port "
		db 0Bh
		db    0
		dw offset _snd_base_port
_aHIrq		db 'h, IRQ '
		db    4
		db    0
		dw offset _irq_number
_aDma		db ', DMA '
		db    4
		db    0
		dw offset _dma_channel
		db    1
		db    0
		dw offset _aMixedAt	; ", mixed at "
		db    4
		db    0
		dw offset _freq_246D7
		db    1
		db    0
		dw offset _aKhz		; "kHz"
		db    0
_aCovox_0	db 'Covox',0            ; DATA XREF: seg003:0D66o
_aStereoOn1_0	db 'Stereo-On-1',0      ; DATA XREF: seg003:0D68o
_covox_txt	db    2			; DATA XREF: seg003:0D7Co seg003:0D7Eo
		db    0
		dw offset _sndcard_type
		dw offset _snd_cards_offs
		db    1
		db    0
		dw offset _aAt		; " at"
		db    1
		db    0
		dw offset _aBasePort	; " base port "
		db  0Bh
		db    0
		dw offset _snd_base_port
		db 'h'
		db    1
		db    0
		dw offset _aMixedAt	; ", mixed at "
		db    4
		db    0
		dw offset _freq_246D7
		db    1
		db    0
		dw offset _aKhz		; "kHz"
		db    0
_aAdlibSoundcard_0 db 'Adlib SoundCard',0 ; DATA XREF: seg003:0D6Ao
_aPcHonker_0	db 'PC Honker',0        ; DATA XREF: seg003:0D6Co
_pcspeaker_txt	db    2			; DATA XREF: seg003:0D80o seg003:0D82o
		db    0
		dw offset _sndcard_type
		dw offset _snd_cards_offs
		db    1
		db    0
		dw offset _aMixedAt	; ", mixed at "
		db    4
		db    0
		dw offset _freq_246D7
		db    1
		db    0
		dw offset _aKhz		; "kHz"
		db    0
_aGeneralMidi_0	db 'General MIDI',0     ; DATA XREF: seg003:0D6Eo
_midi_txt	db    2			; DATA XREF: seg003:0D84o
		db    0
		dw offset _sndcard_type
		dw offset _snd_cards_offs
		db    1
		db    0
		dw offset _aAt		; " at"
		db    1
		db    0
		dw offset _aBasePort	; " base port "
		db  0Bh
		db    0
		dw offset _snd_base_port
		db 'h'
		db    0
		db    0
		db    0
_dword_257A0	dd 0			; DATA XREF: _useless_writeinr+170w
					; _inr_read_118B0+82w ...
_word_257A4	dw 0			; DATA XREF: _useless_writeinr+106w
					; _useless_writeinr+111r ...
_aInertiaModule	db 'Inertia Module: ',0 ; DATA XREF: _useless_writeinr+29o
					; _useless_writeinr+80o	...
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db  0Dh
		db  0Ah
		db  1Ah
unk_257D9	db    0
_byte_257DA	db 10h			; DATA XREF: _useless_writeinr+3Fw
_byte_257DB	db 0			; DATA XREF: _useless_writeinr+47w
					; _inr_module+3Er
_byte_257DC	db 0			; DATA XREF: _useless_writeinr+4Dw
					; _inr_module+44r
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
_word_257E6	dw 4			; DATA XREF: _useless_writeinr+53w
					; _inr_module+4Ar
_word_257E8	dw 0			; DATA XREF: _useless_writeinr+59w
_word_257EA	dw 0			; DATA XREF: _useless_writeinr+5Fw
_word_257EC	dw 0			; DATA XREF: _useless_writeinr+65w
					; _inr_module+54r
_word_257EE	dw 0			; DATA XREF: _useless_writeinr+6Bw
					; _inr_module+5Ar
_word_257F0	dw 0			; DATA XREF: _useless_writeinr+71w
					; _inr_module+60r
_byte_257F2	db 0			; DATA XREF: _useless_writeinr+77w
					; _inr_module+66r
_byte_257F3	db 0			; DATA XREF: _useless_writeinr+7Dw
					; _inr_module+6Cr
		db    0
		db    0
_aInertiaModule_0 db 'Inertia Module: ',0 ; DATA XREF: _useless_writeinr+23o
		db 1Fh dup(0)
		db  0Dh
		db  0Ah
		db  1Ah
		db    0
		db  10h
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    4
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
_aInertiaSample	db 'Inertia Sample: '   ; DATA XREF: _useless_writeinr_118+11o
					; _useless_writeinr_118+5Fo ...
asc_25856	db '                                ',0Dh,0Ah,1Ah
					; DATA XREF: _useless_writeinr_118+21o
					; _inr_read_118B0+26o
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
_dword_25886	dd 0			; DATA XREF: _useless_writeinr_118+59w
					; _inr_read_118B0+64r
		db  10h
_byte_2588B	db 0			; DATA XREF: _useless_writeinr_118+4Cw
					; _inr_read_118B0+58r
_byte_2588C	db 0			; DATA XREF: _useless_writeinr_118+46w
					; _inr_read_118B0+52r
_byte_2588D	db 0			; DATA XREF: _useless_writeinr_118+52w
					; _inr_read_118B0+5Er
_word_2588E	dw 0			; DATA XREF: _useless_writeinr_118+40w
					; _inr_read_118B0+4Cr
		db    0
		db    0
_dword_25892	dd 0			; DATA XREF: _useless_writeinr_118+31w
					; _inr_read_118B0+32r
_dword_25896	dd 0			; DATA XREF: _useless_writeinr_118+39w
					; _inr_read_118B0+3Br
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
unk_258A6	db  49h	; I		; DATA XREF: _useless_writeinr_118+Eo
		db  6Eh	; n
		db  65h	; e
		db  72h	; r
		db  74h	; t
		db  69h	; i
		db  61h	; a
		db  20h
		db  53h	; S
		db  61h	; a
		db  6Dh	; m
		db  70h	; p
		db  6Ch	; l
		db  65h	; e
		db  3Ah	; :
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  20h
		db  0Dh
		db  0Ah
		db  1Ah
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db  10h
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
_channels_25908	db 0A00h dup(0)		; DATA XREF: _s3m_module+89o
					; _clean_11C43+A2o ...
; char _myout[152]
_myout		db 18C0h dup(?)		; DATA XREF: _mod_1024A+3o
					; __2stm_module+53o ...
_dword_27BC8	dd ?			; DATA XREF: _moduleread+8Eo
					; _s3m_module:loc_1065Fw ...
_dword_27BCC	dd ?			; DATA XREF: _e669_module+4Ew
		db 18h dup(?)
_segs_table	dw 100h	dup( ?)		; DATA XREF: _useless_writeinr+13Cr
					; _inr_module+10Dw ...
_myseg_size	dw 100h	dup( ?)		; DATA XREF: _useless_writeinr+117r
					; _inr_module+116w ...
_byte_27FE8	db 0FFh	dup( ?)		; DATA XREF: _mod_n_t_module+55o
					; _mod_1021E+15o ...
_byte_280E7	db ?			; DATA XREF: _s3m_module+1F3w
_byte_280E8	db 100h	dup( ?)		; DATA XREF: _e669_module+80w
					; _e669_module+97r ...
_byte_281E8	db 100h	dup( ?)		; DATA XREF: _e669_module+88w
					; _psm_module+148w ...
_byte_282E8	db 20h dup( ?)		; DATA XREF: _clean_11C43+AEo
					; _clean_11C43+11Co ...
_vlm_byte_table	db 8200h dup( ?)	; DATA XREF: _volume_prepare_waves+8Ao
					; sub_13044:loc_13091o	...
; char _chrin[]
_chrin		dd ?			; DATA XREF: _moduleread:loc_10033o
					; _moduleread:loc_10049o ...
; char _myin[]
_myin		dd ?			; DATA XREF: _mtm_module+22o
					; _psm_module+3Fo ...
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
_word_30515	dw ?			; DATA XREF: _ult_module+1Ar
					; _ult_module+1Fw ...
; char _myin_0
_myin_0		db ?			; DATA XREF: _ult_module+3Ao
_dword_30518	dd ?			; DATA XREF: _ult_module:loc_113F8o
					; _ult_module+200o
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
_word_30520	dw ?			; DATA XREF: _snd_off-3644r
					; _snd_off-3543r
_byte_30522	db ?			; DATA XREF: _mtm_module+58r
_byte_30523	db ?			; DATA XREF: _mtm_module+60r
_word_30524	dw ?			; DATA XREF: _snd_off-3534r
_byte_30526	db ?			; DATA XREF: _mtm_module:loc_10B25r
		db    ?	;
unk_30528	db    ?	;		; DATA XREF: _s3m_module+102r
					; _s3m_module+1D5r ...
_byte_30529	db ?			; DATA XREF: __2stm_module+38r
_word_3052A	dw ?			; DATA XREF: _s3m_module+D0r
					; _s3m_module+225r ...
_word_3052C	dw ?			; DATA XREF: _s3m_module+DEr
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
_word_30532	dw ?			; DATA XREF: _s3m_module+24r
		db    ?	;
		db    ?	;
		db    ?	;
_byte_30537	db ?			; DATA XREF: _ult_module+4Cr
; char _my_in
_my_in		db ?			; DATA XREF: __2stm_module+50o
					; _ult_module+46o ...
; char _byte_30539
_byte_30539	db ?			; DATA XREF: _s3m_module+ECr
					; _ult_module+6Bo ...
_byte_3053A	db ?			; DATA XREF: _s3m_module+F2r
_byte_3053B	db ?			; DATA XREF: _s3m_module+4Ar
					; _s3m_module+53r
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
_byte_30548	db ?			; DATA XREF: _s3m_module:loc_10628r
					; _s3m_module+BEr
		db    ?	;
unk_3054A	db    ?	;		; DATA XREF: _mtm_module+7Bo
_byte_3054B	db ?			; DATA XREF: _psm_module+21r
_byte_3054C	db ?			; DATA XREF: _psm_module+27r
		db    ?	;
		db    ?	;
		db    ?	;
_byte_30550	db ?			; DATA XREF: _psm_module+2Dr
		db    ?	;
_word_30552	dw ?			; DATA XREF: _psm_module+35r
					; _far_module+1Fr
_word_30554	dw ?			; DATA XREF: _psm_module+15r
					; _far_module:loc_10F6Ar
_word_30556	dw ?			; DATA XREF: _psm_module+Fr
		db    ?	;
		db    ?	;
_dword_3055A	dd ?			; DATA XREF: _psm_module+105r
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
_word_30562	dw ?			; DATA XREF: _psm_module+10Cr
_word_30564	dw ?			; DATA XREF: _psm_module+110r
_dword_30566	dd ?			; DATA XREF: _psm_module+55r
					; _s3m_module+FFo ...
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
		db    ?	;
_byte_30576	db ?			; DATA XREF: _e669_module+2Ar
_byte_30577	db ?			; DATA XREF: _e669_module+32r
		db    ?	;
_byte_30579	db 21h dup( ?)		; DATA XREF: _e669_module:loc_1096Fr
_byte_3059A	db 5Fh dup( ?)		; DATA XREF: _psm_module+4Bo
					; _psm_module+5Co
_byte_305F9	db 40h dup( ?)		; DATA XREF: _e669_module+7Cr
_byte_30639	db ?			; DATA XREF: _ult_module+169r
_byte_3063A	db ?			; DATA XREF: _ult_module+172r
_word_3063B	dw ?			; DATA XREF: _ult_module+192o
					; _ult_module:loc_113E2r ...
_dword_3063D	dd ?			; DATA XREF: _ult_module+225r
					; _ult_read+19w	...
_byte_30641	db 28h dup( ?)		; DATA XREF: _ult_module+22Cr
_byte_30669	db ?			; DATA XREF: _far_module+85r
_byte_3066A	db ?			; DATA XREF: _far_module+95r
_byte_3066B	db 0Eh dup( ?)		; DATA XREF: _far_module+AAo
					; _far_module+D9o
_byte_30679	db 65h dup( ?)		; DATA XREF: _e669_module+84r
_byte_306DE	db 1E0h	dup( ?)		; DATA XREF: _mod_n_t_module+15o
_byte_308BE	db 4Ah dup( ?)		; DATA XREF: _mod_n_t_module+4Fo
					; _mod_n_t_module+F1o
_byte_30908	db 38h dup( ?)		; DATA XREF: _ult_module+203o
					; _ult_module+255o
_byte_30940	db ?			; DATA XREF: _mod_n_t_module:_mod_chn_moduler
					; _mod_n_t_module+9Cr ...
unk_30941	db    ?	;		; DATA XREF: _mod_n_t_module+ACr
		db    ?	;
_byte_30943	db  ?			; DATA XREF: _mod_n_t_module:_mod_tdz_moduler
		db 0BC4h dup(?)
; char _word_31508[]
_word_31508	dw ?			; DATA XREF: _mod_read_10311+5o
					; _mod_read_10311+1Eo ...
_byte_3150A	db ?			; DATA XREF: _psm_module+139r
					; _far_module+130o
		db    ?	;
_byte_3150C	db 7FCh	dup( ?)		; DATA XREF: _psm_module+150o
					; _psm_module+160o
_byte_31D08	db 1800h dup( ?)	; DATA XREF: _mod_read_10311+21o
					; _mod_read_10311+2Bo
_byte_33508	db 1008h dup( ?)	; DATA XREF: _snd_off-3632o
					; _snd_off-3625o ...
seg003 ends

; ===========================================================================

; Segment type:	Uninitialized
seg004 segment byte stack 'STACK' use16
		assume cs:seg004
		assume es:nothing, ss:nothing, ds:dseg,	fs:nothing, gs:nothing
_byte_34510	db 1000h dup(?)
seg004 ends


		end _start
 