#include "asm.h"
//#include "snow.h"
//#include "iplay_masm_.h"

#include <SDL2/SDL.h>
#include <assert.h>

#include <time.h>
#include <thread>

struct /*__attribute__((__packed__))*/ Memory;
extern struct Memory m;

/* https://commons.wikimedia.org/wiki/File:Table_of_x86_Registers_svg.svg */


//struct uc_x86_state {
//    REGDEF_nol(flags);
//};



/*
#undef REGDEF_hl
#undef REGDEF_l
#undef REGDEF_bwd
#undef REGDEF_nol

struct __fl
{
	bool _CF:1;
	bool res1:1;
	bool _PF:1;
	bool res2:1;
	bool _AF:1;
	bool res3:1;
	bool _ZF:1;
	bool _SF:1;
	bool _TF:1;
	bool _IF:1;
	bool _DF:1;
	bool _OF:1;
	int _IOPL:2;
	bool _NT:1;
	bool res4:1;
	bool res5:16;
};
static struct __fl __eflags;

#define CF __eflags._CF
#define ZF __eflags._ZF
#define DF __eflags._DF
#define SF __eflags._SF
*/
// SDL2 VGA
    SDL_Renderer *renderer;
    SDL_Window *window;

db vgaPalette[256*3];
dd selectorsPointer;
dd selectors[NB_SELECTORS];

dd heapPointer;
db vgaRamPaddingBefore[VGARAM_SIZE];
db vgaRam[VGARAM_SIZE];
db vgaRamPaddingAfter[VGARAM_SIZE];
db* diskTransferAddr = 0;
//#include "memmgr.c"


bool isLittle;
bool jumpToBackGround;
char *path;
bool executionFinished;
db exitCode;

FILE * logDebug=NULL;

#define MAX_FMT_SIZE 1024
void log_error(const char *fmt, ...) {
	char formatted_string[MAX_FMT_SIZE];
	va_list argptr;
	va_start(argptr,fmt);
	vsprintf (formatted_string,fmt, argptr);
	va_end(argptr);
#ifdef __LIBRETRO__
	log_cb(RETRO_LOG_ERROR,"%s",formatted_string);
#else
	if (logDebug!=NULL) { fprintf(logDebug,"%s",formatted_string); fflush(logDebug);} else { printf("%s",formatted_string); }
#endif
}
void log_debug(const char *fmt, ...) {
#ifdef DEBUG
	char formatted_string[MAX_FMT_SIZE];
	va_list argptr;
	va_start(argptr,fmt);
	vsprintf (formatted_string,fmt, argptr);
	va_end(argptr);
#ifdef __LIBRETRO__
	log_cb(RETRO_LOG_DEBUG,"%s",formatted_string);
#else
	if (logDebug!=NULL) { fprintf(logDebug,"%s",formatted_string); fflush(logDebug); } else { printf("%s",formatted_string); }
#endif
#endif
}

void log_info(const char *fmt, ...) {
	char formatted_string[MAX_FMT_SIZE];
	va_list argptr;
	va_start(argptr,fmt);
	vsprintf (formatted_string,fmt, argptr);
	va_end(argptr);
#ifdef __LIBRETRO__
	log_cb(RETRO_LOG_INFO,"%s",formatted_string);
#else
	if (logDebug!=NULL) { fprintf(logDebug,"%s",formatted_string); fflush(logDebug); } else { printf("%s",formatted_string); }
#endif
}

void log_debug2(const char *fmt, ...) {
#if DEBUG>=2
	char formatted_string[MAX_FMT_SIZE];
	va_list argptr;
	va_start(argptr,fmt);
	vsprintf (formatted_string,fmt, argptr);
	va_end(argptr);
	log_debug(formatted_string);
#endif
}

void checkIfVgaRamEmpty() {
	int i;
	int vgaram_empty = 1;
	for(i = 0; i < VGARAM_SIZE; i++)
		if(vgaRam[i])
			vgaram_empty = 0;
	log_debug("vgaram_empty : %s\n", vgaram_empty ? "true" : "false");
	(void) vgaram_empty;
}

void stackDump(_STATE* _state) {
X86_REGREF

	log_debug("is_little_endian()=%d\n",isLittle);
	log_debug("sizeof(dd)=%zu\n",sizeof(dd));
	log_debug("sizeof(dd *)=%zu\n",sizeof(dd *));
	log_debug("sizeof(dw)=%zu\n",sizeof(dw));
	log_debug("sizeof(db)=%zu\n",sizeof(db));
	log_debug("sizeof(jmp_buf)=%zu\n",sizeof(jmp_buf));
//	log_debug("sizeof(mem)=%zu\n",sizeof(m));
	log_debug("eax: %x\n",READDD(eax));
//	hexDump(&eax,sizeof(dd));
	log_debug("ebx: %x\n",READDD(ebx));
	log_debug("ecx: %x\n",READDD(ecx));
	log_debug("edx: %x\n",READDD(edx));
	log_debug("ebp: %x\n",READDD(ebp));
	log_debug("cs: %d -> %p\n",cs,(void *) realAddress(0,cs));
	log_debug("ds: %d -> %p\n",ds,(void *) realAddress(0,ds));
	log_debug("esi: %x\n",READDD(esi));
	log_debug("ds:esi %p\n",(void *) realAddress(esi,ds));
	log_debug("es: %d -> %p\n",es,(void *) realAddress(0,es));
	hexDump(&es,sizeof(dd));
	log_debug("edi: %x\n",READDD(edi));
	log_debug("es:edi %p\n",(void *) realAddress(edi,es));
	hexDump((void *) realAddress(edi,es),50);
	log_debug("fs: %d -> %p\n",fs,(void *) realAddress(0,fs));
	log_debug("gs: %d -> %p\n",gs,(void *) realAddress(0,gs));
//	log_debug("adress heap: %p\n",(void *) &m.heap);
	log_debug("adress vgaRam: %p\n",(void *) &vgaRam);
	log_debug("first pixels vgaRam: %x\n",*vgaRam);
	log_debug("flags: ZF = %d\n",ZF);
	log_debug("top stack=%d\n",stackPointer);
	checkIfVgaRamEmpty();
}

// thanks to paxdiablo http://stackoverflow.com/users/14860/paxdiablo for the hexDump function
void hexDump (void *addr, int len) {
	int i;
	unsigned char buff[17];
	unsigned char *pc = (unsigned char*)addr;
	(void) buff;
	log_debug ("hexDump %p:\n", addr);

	if (len == 0) {
		log_debug("  ZERO LENGTH\n");
		return;
	}
	if (len < 0) {
		log_debug("  NEGATIVE LENGTH: %i\n",len);
		return;
	}

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0)
				log_debug ("  %s\n", buff);

			// Output the offset.
			log_debug ("  %04x ", i);
		}

		// Now the hex code for the specific character.
		log_debug (" %02x", pc[i]);

		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e))
			buff[i % 16] = '.';
		else
			buff[i % 16] = pc[i];
		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		log_debug ("   ");
		i++;
	}

	// And print the final ASCII bit.
	log_debug ("  %s\n", buff);
}

void asm2C_OUT(int16_t address, int data) {
	static int indexPalette = 0;
	switch(address) {
	case 0x3c8:
		indexPalette=data;
		break;
	case 0x3c9:
		if (indexPalette<768) {
			vgaPalette[indexPalette]=data;
			indexPalette++;
		} else {
			log_error("error: indexPalette>767 %d\n",indexPalette);
		}
		break;
	default:
		log_debug("unknown OUT %d,%d\n",address, data);
		break;
	}
}

int8_t asm2C_IN(int16_t address) {
	static bool vblTick = 1;
	switch(address) {
	case 0x3DA:
		if (vblTick) {
			vblTick = 0;
			return 0;
		} else {
			vblTick = 1;
			jumpToBackGround = 1;
			return 8;
		}
		break;
	default:
		log_error("Unknown IN %d\n",address);
		return 0;
	}
}

uint16_t asm2C_INW(uint16_t address) {
	switch(address) {
	case 0x3DA:
		break;
	default:
		log_error("Unknown INW %d\n",address);
		return 0;
	}
}

bool is_little_endian_real_check() {
	union
	{
		uint16_t x;
		uint8_t y[2];
	} u;

	u.x = 1;
	return u.y[0];
}

/**
 * is_little_endian:
 *
 * Checks if the system is little endian or big-endian.
 *
 * Returns: greater than 0 if little-endian,
 * otherwise big-endian.
 **/
bool is_little_endian()
{
#if defined(__x86_64) || defined(__i386) || defined(_M_IX86) || defined(_M_X64)
	return 1;
#elif defined(MSB_FIRST)
	return 0;
#else
	return is_little_endian_real_check();
#endif
}


//#if !CYGWIN
  #include <sys/time.h>
double realElapsedTime(void) {              // returns 0 first time called
//    static struct timeval t0;
    struct timeval tv;
    gettimeofday(&tv, 0);
 //   if (!t0.tv_sec)                         // one time initialization
   //     t0 = tv;
    return ((tv.tv_sec /*- t0.tv_sec*/ + (tv.tv_usec /* - t0.tv_usec*/)) / 1000000.) * 18.;
}
/*
#else
#include <windows.h>
double realElapsedTime(void) {              // granularity about 50 microsecs
    static LARGE_INTEGER freq, start;
    LARGE_INTEGER count;
    if (!QueryPerformanceCounter(&count))
        assert(0 && "QueryPerformanceCounter");
    if (!freq.QuadPart) {                   // one time initialization
        if (!QueryPerformanceFrequency(&freq))
            assert(0 && "QueryPerformanceFrequency");
        start = count;
    }
    return (double)(count.QuadPart) / freq.QuadPart;
}
#endif
*/
void asm2C_init() {
	isLittle=is_little_endian();
#ifdef MSB_FIRST
	if (isLittle) {
		log_error("Inconsistency: is_little_endian=true and MSB_FIRST defined.\n");
		exit(1);
	}
#endif
	if (isLittle!=is_little_endian_real_check()) {
		log_error("Inconsistency in little/big endianess detection. Please check if the Makefile sets MSB_FIRST properly for this architecture.\n");
		exit(1);
	}
	log_debug2("asm2C_init is_little_endian:%d\n",isLittle);
}


void asm2C_INT(_STATE* _state, int a) {
X86_REGREF
	static FILE * file;
	int i;
/*
	db ah=READDBh(eax);
	db al=READDBl(eax);
	dw ax=READDW(eax);
	dw bx=READDW(ebx);
	dw cx=READDW(ecx);
	dw dx=READDW(edx);
*/
	CF = 0;
	int rc;
#define SUCCESS         0       /* Function was successful      */
	log_debug2("asm2C_INT ah=%x al=%x ax=%x bx=%x cx=%x dx=%x\n",ah,al,ax,bx,cx,dx);

	switch(a) {
	case 0x10:
	{
		switch(ah)
		{
		case 0: { // set mode
			switch(al)
			{
			case 0x03: {
				resize_term(25, 80);
				clear();
				refresh();
				log_debug2("Switch to text mode\n");
				return;
			}
			case 0x83: {
				resize_term(25, 80);
				refresh();
				log_debug2("Switch to text mode\n");
				return;
			}
			case 0x13: {
				log_debug2("Switch to VGA\n");
// SDL2 VGA
				SDL_Init(SDL_INIT_VIDEO);
				SDL_CreateWindowAndRenderer(320, 200, 0, &window, &renderer);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderClear(renderer);
			        SDL_RenderPresent(renderer); 

				//stackDump(_state);
				return;
			}
			}
			break;
		}
		case 0x02: { // set cursor
				int y,x;
				getmaxyx(stdscr,y,x);
				if (dh >= y || dl >= x)
				{
					curs_set(0);
				}
				else
				{
//					curs_set(1);
					move(dh, dl);
					refresh();
				}
				return;
		}
		case 0x11: {        //set charset size
			switch(al)
			{
			case 0x11: {
				resize_term(30, 80);
				refresh();
				return;
			}
			case 0x12: {
				resize_term(50, 80);
				refresh();
				return;
			}
			}
			break;
		}
		}
		break;
	}
	case 0x1A:
	{
		switch(ah) {
		    case 0x02:  /* GET REAL-TIME CLOCK TIME (AT,XT286,PS) */
		         {
        	    struct tm* loctime;

		        
		    struct timeval curtime;
	            gettimeofday(&curtime, NULL);
    
 //           curtime.tv_sec += cmos.time_diff;
        	    loctime = localtime((time_t*)&curtime.tv_sec);
	        	
        	    dh = (loctime->tm_sec);
	            cl = (loctime->tm_min);

	            ch = (loctime->tm_hour);
		    dl = 0;
			break;
			}

			        break;
        		   }
		break;
	}
	case 0x21:
		switch(ah)
		{
		case 0x9:
		{
			char * s=(char *) realAddress(dx,ds);
			for (i=0; s[i]!='$'; i++) {
				printf("%c", s[i]);
			}
			return;
		}
		case 0xe: // select disk
		{
			al=1;
			return;
		}
		case 0x19: // Get default disk
		{
			// def disk is C:
			al=0x2;
			return;
		}
		case 0x1A: // Set disk transfer addr
		{
			diskTransferAddr=(db *)realAddress(dx,ds);
			return;
		}
		case 0x25: // Set disk transfer addr
		{
			*(dw *)realAddress(al*4,0)=dx;
			*(dw *)realAddress(al*4+2,0)=ds;
			return;
		}
		case 0x35: // Set disk transfer addr
		{
			bx=*(dw *)realAddress(al*4,0);
			es=*(dw *)realAddress(al*4+2,0);
			return;
		}
		case 0x2c:
		{
			//MOV(8,8,READDBh(edx),(db)2);
			// TOFIX
			dh=0x2;
			return;
		}
		case 0x3d:
		{
			char fileName[1000];
			if (path!=NULL) {
				sprintf(fileName,"%s/%s",path,(const char *) realAddress(dx, ds));
			} else {
				sprintf(fileName,"%s",(const char *) realAddress(dx, ds));
			}
			file=fopen(fileName, "rb"); //TOFIX, multiple files support
			log_debug2("Opening file %s -> %p\n",fileName,(void *) file);
			if (file!=NULL) {
				eax=1; //TOFIX
			} else {
				CF = 1;
				log_error("Error opening file %s\n",fileName);
			}
			/*
			   // [Index]AH = 3Dh - "OPEN" - OPEN EXISTING FILE
			   Entry:

			   AL = access and sharing modes
			   DS:DX -> ASCIZ filename
			   Return:

			   CF clear if successful, AX = file handle
			    CF set on error AX = error code (01h,02h,03h,04h,05h,0Ch,56h)
			 */
			// TODO
			return;
		}
		case 0x3e:
		{
			// bx: file handle to close
			//TOFIX
			log_debug2("Closing file. bx:%d\n",bx);
			if (fclose(file))  {
				CF = 1;
				perror("Error");
				log_error("Error closing file ? bx:%d %p\n",bx,(void *) file);
			}

			file=NULL;
			return;
		}
		case 0x3f:
		{
			/*
			   [Index]AH = 3Fh - "READ" - READ FROM FILE OR DEVICE

			   Entry:

			   BX = file handle
			   CX = number of bytes to read
			   DS:DX -> buffer for data
			   Return:

			   CF clear if successful - AX = number of bytes actually read (0 if at EOF before call)
			    CF set on error AX = error code (05h,06h)
			 */
			//char grosbuff[100000];
			void * buffer=(db *) realAddress(dx, ds);
			// log_debug2("Reading ecx=%d cx=%d eds=%x edx=%x -> %p file: %p\n",m.ecx,cx,m.ds,m.edx,buffer,(void *)  file);

			if (feof(file)) {
				log_debug2("feof(file)\n");
				eax=0;
			} else {
				size_t r=fread (buffer,1,cx,file);
				if (r!=cx) {
					perror("Error");
					log_error("r!=cx cx:%d R:%zu \n",cx,r);
					if(!feof(file)) {
						log_error("Error reading ? %d %zu %p\n",cx,r,(void *) file);
						CF = 1;
					}
				} else {
					log_debug2("Reading OK %p\n",(void *) file);
				}
				eax=r;
			}
			/*
			   if (ax!=cx) {
			    log_debug("Error reading ? %d %d\n",ax,cx);
			    m.CF = 1;

			   }
			 */
			return;
		}
		// [Index]AH=42h - "LSEEK" - SET CURRENT FILE POSITION
		case 0x42:
		{
			/*

			   AH=42h - "LSEEK" - SET CURRENT FILE POSITION

			   Entry:

			   AL = origin of move 00h start of file 01h current file position 02h end of file
			   BX = file handle
			   CX:DX = offset from origin of new file position

			 */
			int seek = 0;
			switch(ah) {
			case 0x0:
				seek = SEEK_SET;
				break;
			case 0x1:
				seek = SEEK_CUR;
				break;
			case 0x2:
				seek = SEEK_END;
				break;
			}
			long int offset=(((long int )cx)<<16)+dx;
			log_debug2("Seeking to offset %ld %d\n",offset,seek);
			if (fseek(file,offset,seek)!=0) {
				log_error("Error seeking\n");
				CF=1;
			}
			return;
		}
		case 0x47: // Get cur dir
		{
			// cur dir is root
			*(char *) realAddress(dx, ds)='\0';
			ax = 0x0100;
			return;
		}
		case 0x48:
		{
			
			//   ;2.29 - Function 048h - Allocate Memory Block:
			//   ;In:  AH     = 48h
			//   ;  BX  = size of block in 16xbytes (must be non-zero)
			//   ;Out: if successful:
			//   ;    carry flag clear
			//   ;    AX  =  address of allocated memory block
			if (bx==0xffff)
				{ CF=1;
				  return;
				}
/*
			int32_t nbBlocks=(bx<<4);
			log_debug2("Function 0501h - Allocate Memory Block: %d para\n",bx);

			if (heapPointer+nbBlocks>=HEAP_SIZE) {
				CF = 1;
				log_error("Not enough memory (increase HEAP_SIZE)\n");
				exit(1);
				return;
			} else {
				dd a=offsetof(struct Memory,heap)+heapPointer;
				heapPointer+=nbBlocks;
				{
					log_debug2("New top of heap: %x\n",(dd) offsetof(struct Memory,heap)+heapPointer);
				}
				ax=a >> 4;
				log_debug2("Return pointer %x, seg ax =%x\n",a,ax);
				return;
			}
*/
      /* Allocate memory */
      if ((rc = DosMemAlloc(bx, mem_access_mode, &ax, &bx)) < 0)
      {
        DosMemLargest(&bx);
        if (DosMemCheck() != SUCCESS)
           {log_error("MCB chain corrupted\n");exit(1);}
           CF=1;
      }
	CF=(rc!=SUCCESS);
      ax++;   /* DosMemAlloc() returns seg of MCB rather than data */
	return;
			break;
		}
		case 0x4E: // find first matching file
		{
			// cur dir is root
			log_debug2("Find first file %s\n",(void *) (db *) realAddress(dx, ds));
			strcpy((char*)diskTransferAddr+0x1e,"HACKER4.S3M");
			return;
		}
      /* Free memory */
	    case 0x49:
      if ((rc = DosMemFree(es - 1)) < SUCCESS)
      {
        if (DosMemCheck() != SUCCESS)
           {log_error("MCB chain corrupted\n");exit(1);}
           CF=1;
      }
	CF=(rc!=SUCCESS);
	return;
      break;

      /* Set memory block size */
		    case 0x4a:
        if (DosMemCheck() != SUCCESS)
           {log_error("before 4a: MCB chain corrupted\n");exit(1);}

      if ((rc = DosMemChange(es, bx, &bx)) < 0)
      {
        if (DosMemCheck() != SUCCESS)
           {log_error("after 4a: MCB chain corrupted\n");exit(1);}
           CF=1;
      }
      ax = es; /* Undocumented MS-DOS behaviour expected by BRUN45! */
	CF=(rc!=SUCCESS);
	return;
      break;
		case 0x4F: // find next matching file
		{
			CF=1;
			return;
		}
		case 0x4c:
		{
			stackDump(_state);
			jumpToBackGround = 1;
			executionFinished = 1;
			exitCode = al;
			log_error("Graceful exit al:0x%x\n",al);
			exit(al);
			return;
		}
		case 0x58: // mem allocation policy
		{
			return;
		}
		default:
			break;
		}
/*  protected mode temporrary disabled
	case 0x31:
		switch(ax)
		{
		case 0x0:
		{
			
			//   ;2.0 - Function 0000h - Allocate Descriptors:
			//   ;--------------------------------------------
			//   ;  Allocates one or more descriptors in the client's descriptor table. The
			//   ;descriptor(s) allocated must be initialized by the application with other
			//   ;function calls.
			//   ;In:
			//   ;  AX     = 0000h
			//   ;  CX     = number of descriptors to allocate
			//   ;Out:
			//   ;  if successful:
			//   ;    carry flag clear
			//   ;    AX     = base selector
			 
			log_debug2("Function 0000h - Allocate %d Descriptors\n",cx);
			if (selectorsPointer+cx>=NB_SELECTORS) {
				CF = 1;
				log_error("Not enough free selectors (increase NB_SELECTORS)\n");
				return;
			} else {
				eax = selectorsPointer;
				selectorsPointer+=cx;
				log_debug2("Return %x\n",eax);
			}
			return;
		}
		case 0x02:
		{
			
			//   This function Converts a real mode segment into a protected mode descriptor.
			//   BX =    real mode segment
			//   Out:
			//   if successful:
			//   carry flag clear
			//   AX =  selector
			//  if failed:
			//   carry flag set
			 
			log_debug2("Function 0002h - Converts a real mode segment into a protected mode descriptor real mode segment: %d\n",ebx);
			if (selectorsPointer+1>=NB_SELECTORS) {
				CF = 1;
				log_error("Not enough free selectors (increase NB_SELECTORS)\n");
				return;
			}
			// TOFIX ?
			// always return vga adress.
			selectors[selectorsPointer]=offsetof(struct Mem,vgaRam); // bx;
			eax=selectorsPointer;
			log_debug2("Returns new selector: eax: %d\n",eax);
			selectorsPointer++;

			// Multiple calls for the same real mode segment return the same selector. The returned descriptor should never be modified or freed. <- TOFIX
			return;
		}
		
		//   ;2.5 - Function 0007h - Set Segment Base Address:
		//   ; Sets the 32bit linear base address field in the descriptor for the specified
		//   ; segment.
		//   ; In:   AX     = 0007h
		//   ; BX     = selector
		//   ;  CX:DX  = 32bit linear base address of segment
		 
		case 0x07:
		{
			log_debug2("Function 0007h - Set Segment Base Address: ebx: %x, edx:%x ecx:%x\n",READDD(ebx),READDD(edx),READDD(ecx));
			if (bx>selectorsPointer) {
				CF = 1;
				log_error("Error: selector number doesnt exist\n");
				return;
			}
			selectors[bx]=(dx&0xffff)+(cx<<16);
			log_debug2("Address for selector %d: %x\n",bx,selectors[bx]);
			return;
		}
		case 0x08:
		{
			
			//   ;2.6 - Function 0008h - Set Segment Limit:
			//   ;-----------------------------------------
			//   ;  Sets the limit field in the descriptor for the specified segment.
			//   ;  In:
			//   ;  AX     = 0008h
			//   ;  BX     = selector
			//   ;  CX:DX  = 32bit segment limit
			//   ;  Out:
			//   ;  if successful:
			//   ;    carry flag clear
			//   ;  if failed:
			//   ;    carry flag set
			 

			// To implement...
			log_debug2("Function 0008h - Set Segment Limit for selector %d (Ignored)\n",bx);
			return;
		}
		case 0x501:
		{
			
			//   ;2.29 - Function 0501h - Allocate Memory Block:
			//   ;In:  AX     = 0501h
			//   ;  BX:CX  = size of block in bytes (must be non-zero)
			//   ;Out: if successful:
			//   ;    carry flag clear
			//   ;    BX:CX  = linear address of allocated memory block
			//   ;    SI:DI  = memory block handle (used to resize and free block)
			 
			int32_t nbBlocks=(bx<<16)+cx;
			log_debug2("Function 0501h - Allocate Memory Block: %d bytes\n",nbBlocks);

			if (heapPointer+nbBlocks>=HEAP_SIZE) {
				CF = 1;
				log_error("Not enough memory (increase HEAP_SIZE)\n");
				exit(1);
				return;
			} else {
				dd a=offsetof(struct Mem,heap)+heapPointer;
				heapPointer+=nbBlocks;
				{
					log_debug2("New top of heap: %x\n",(dd) offsetof(struct Mem,heap)+heapPointer);
				}
				ecx=a & 0xFFFF;
				ebx=a >> 16;
				edi=0; // TOFIX
				esi=0; // TOFIX
				log_debug2("Return %x ebx:ecx %x:%x\n",a,ebx,ecx);
				return;
			}
			break;
		}
		case 0x205: {
			
			//   fo implement
			//   ;2.18 - Function 0204h - Get Protected Mode Interrupt Vector:
			//   ;------------------------------------------------------------
			//   ;
			//   ;  Returns the address of the current protected mode interrupt handler for the
			//   ;specified interrupt.
			//   ;
			//   ;In:
			//   ;  AX     = 0204h
			//   ;  BL     = interrupt number
			//   ;
			//   ;Out:
			//   ;  always successful:
			//   ;    carry flag clear
			//   ;    CX:EDX = selector:offset of protected mode interrupt handler
			
			//   ;  AX     = 0204h
			//   ;  BL     = interrupt number
			//   ;
			//   ;Out:
			//   ;  always successful:
			//   ;    carry flag clear
			//   ;    CX:EDX = selector:offset of protected mode interrupt handler
			 

			return;
		}
		default:
			break;
		}
		break;
*/
	case 0x33: // mouse not implemented yet
	{
		break;
	}
	default:
		break;
	}
	CF = 1;
	log_debug("Error DOSInt 0x%x ah:0x%x al:0x%x: bx:0x%x not supported.\n",a,ah,al,bx);
}

//jmp_buf jmpbuffer;

/*
void program() {
int i;
#ifdef INCLUDEMAIN
dest=NULL;src=NULL;i=0; //to avoid a warning.
#endif
if (executionFinished) goto moveToBackGround;
if (jumpToBackGround) {
jumpToBackGround = 0;
#ifdef MRBOOM
if (nosetjmp) stackPointer=0; // this an an hack to avoid setJmp in saved state.
if (nosetjmp==2) goto directjeu;
if (nosetjmp==1) goto directmenu;
#endif
RET;
			}
//R(JMP(_main));
//...
executionFinished = 1;
moveToBackGround:
return ;//(executionFinished == 0);
}
*/

const char* log_spaces(int n) 
{ 
 static const char s[]="                                                                                          ";
//	memset(s, ' ', n); 
//	*(s+n) = 0; 
  return s+(88-n);
}

dw getscan()
{
 dw o=0;
 int ch = getch();
 o = ch;
//if (ch==ERR) return(0);

//log_debug(">> %x\n",ch);

 switch (ch)
{
case ERR: {o=0;break;}

case 0x31: {o=0x2;break;}
case 0x32: {o=0x3;break;}
case 0x33: {o=0x4;break;}
case 0x34: {o=0x5;break;}
case 0x35: {o=0x6;break;}
case 0x36: {o=0x7;break;}
case 0x37: {o=0x8;break;}
case 0x38: {o=0x9;break;}
case 0x39: {o=0xa;break;}
case 0x30: {o=0xb;break;}

case KEY_F(1): {o=0x3B;break;}
case KEY_F(2): {o=0x3C;break;}
case KEY_F(3): {o=0x3D;break;}
case KEY_F(4): {o=0x3E;break;}
case KEY_F(5): {o=0x3F;break;}
case KEY_F(6): {o=0x40;break;}
case KEY_F(7): {o=0x41;break;}
case KEY_F(8): {o=0x42;break;}
case KEY_F(9): {o=0x43;break;}
case KEY_F(10): {o=0x44;break;}
case KEY_LEFT: {o=0xe04B;break;}
case KEY_B2: {o=0x4C;break;}
case KEY_RIGHT: {o=0xe04D;break;}
case KEY_END: {o=0x4F;break;}
case KEY_DOWN: {o=0xe050;break;}
case KEY_NPAGE: {o=0xe051;break;}
case KEY_IC: {o=0xe052;break;}
case KEY_DC: {o=0xe053;break;}
case KEY_F(13): {o=0x54;break;}
case KEY_F(14): {o=0x55;break;}
case KEY_F(15): {o=0x56;break;}
case KEY_F(11): {o=0x57;break;}
case KEY_F(12): {o=0x58;break;}
case KEY_F(18): {o=0x59;break;}
case KEY_F(19): {o=0x5A;break;}
case KEY_F(20): {o=0x5B;break;}
case KEY_F(21): {o=0x5C;break;}
case KEY_F(22): {o=0x5D;break;}
case KEY_F(25): {o=0x5E;break;}
case KEY_F(26): {o=0x5F;break;}
case KEY_F(27): {o=0x60;break;}
case KEY_F(28): {o=0x61;break;}
case KEY_F(29): {o=0x62;break;}
case KEY_F(30): {o=0x63;break;}
case KEY_F(31): {o=0x64;break;}
case KEY_F(32): {o=0x65;break;}
case KEY_F(33): {o=0x66;break;}
case KEY_F(34): {o=0x67;break;}
case KEY_F(37): {o=0x68;break;}
case KEY_F(38): {o=0x69;break;}
case KEY_F(39): {o=0x6A;break;}
case KEY_F(40): {o=0x6B;break;}
case KEY_F(41): {o=0x6C;break;}
case KEY_F(42): {o=0x6D;break;}
case KEY_F(43): {o=0x6E;break;}
case KEY_F(44): {o=0x6F;break;}
case KEY_F(45): {o=0x70;break;}
case KEY_F(46): {o=0x71;break;}
case KEY_BTAB: {o=0xF;break;}
case KEY_HOME: {o=0xe047;break;}
case KEY_UP: {o=0xe048;break;}
case KEY_PPAGE: {o=0xe049;break;}
case KEY_F(23): {o=0x87;break;}
case KEY_F(24): {o=0x88;break;}
case KEY_F(35): {o=0x89;break;}
case KEY_F(36): {o=0x8A;break;}
case KEY_F(47): {o=0x8B;break;}
case KEY_F(48): {o=0x8C;break;}
#ifdef __PDCURSES__
case ALT_ESC: {o=0x1;break;}
case ALT_BKSP: {o=0xE;break;}
case ALT_Q: {o=0x10;break;}
case ALT_W: {o=0x11;break;}
case ALT_E: {o=0x12;break;}
case ALT_R: {o=0x13;break;}
case ALT_T: {o=0x14;break;}
case ALT_Y: {o=0x15;break;}
case ALT_U: {o=0x16;break;}
case ALT_I: {o=0x17;break;}
case ALT_O: {o=0x18;break;}
case ALT_P: {o=0x19;break;}
case ALT_LBRACKET: {o=0x1A;break;}
case ALT_RBRACKET: {o=0x1B;break;}
case ALT_ENTER: {o=0x1C;break;}
case ALT_A: {o=0x1E;break;}
case ALT_S: {o=0x1F;break;}
case ALT_D: {o=0x20;break;}
case ALT_F: {o=0x21;break;}
case ALT_G: {o=0x22;break;}
case ALT_H: {o=0x23;break;}
case ALT_J: {o=0x24;break;}
case ALT_K: {o=0x25;break;}
case ALT_L: {o=0x26;break;}
case ALT_SEMICOLON: {o=0x27;break;}
case ALT_FQUOTE: {o=0x28;break;}
case ALT_BQUOTE: {o=0x29;break;}
case ALT_BSLASH: {o=0x2B;break;}
case ALT_Z: {o=0x2C;break;}
case ALT_X: {o=0x2D;break;}
case ALT_C: {o=0x2E;break;}
case ALT_V: {o=0x2F;break;}
case ALT_B: {o=0x30;break;}
case ALT_N: {o=0x31;break;}
case ALT_M: {o=0x32;break;}
case ALT_COMMA: {o=0x33;break;}
case ALT_STOP: {o=0x34;break;}
case ALT_FSLASH: {o=0x35;break;}
case ALT_PADSTAR: {o=0x37;break;}
case ALT_PADMINUS: {o=0x4A;break;}
case ALT_PADPLUS: {o=0x4E;break;}
case CTL_LEFT: {o=0x73;break;}
case CTL_RIGHT: {o=0x74;break;}
case CTL_END: {o=0x75;break;}
case CTL_PGDN: {o=0x76;break;}
case CTL_HOME: {o=0x77;break;}
case ALT_1: {o=0x78;break;}
case ALT_2: {o=0x79;break;}
case ALT_3: {o=0x7A;break;}
case ALT_4: {o=0x7B;break;}
case ALT_5: {o=0x7C;break;}
case ALT_6: {o=0x7D;break;}
case ALT_7: {o=0x7E;break;}
case ALT_8: {o=0x7F;break;}
case ALT_9: {o=0x80;break;}
case ALT_0: {o=0x81;break;}
case ALT_MINUS: {o=0x82;break;}
case ALT_EQUAL: {o=0x83;break;}
case CTL_PGUP: {o=0x84;break;}
//case KEY_F(11): {o=0x85;break;}
//case KEY_F(12): {o=0x86;break;}
case CTL_UP: {o=0x8D;break;}
case CTL_PADMINUS: {o=0x8E;break;}
case CTL_PADCENTER: {o=0x8F;break;}
case CTL_PADPLUS: {o=0x90;break;}
case CTL_DOWN: {o=0x91;break;}
case CTL_INS: {o=0x92;break;}
case CTL_DEL: {o=0x93;break;}
case CTL_TAB: {o=0x94;break;}
case CTL_PADSLASH: {o=0x95;break;}
case CTL_PADSTAR: {o=0x96;break;}
case ALT_HOME: {o=0x97;break;}
case ALT_UP: {o=0x98;break;}
case ALT_PGUP: {o=0x99;break;}
case ALT_LEFT: {o=0x9B;break;}
case ALT_RIGHT: {o=0x9D;break;}
case ALT_END: {o=0x9F;break;}
case ALT_DOWN: {o=0xA0;break;}
case ALT_PGDN: {o=0xA1;break;}
case ALT_INS: {o=0xA2;break;}
case ALT_DEL: {o=0xA3;break;}
case ALT_PADSLASH: {o=0xA4;break;}
case ALT_TAB: {o=0xA5;break;}
case ALT_PADENTER: {o=0xA6;break;}
#endif
 }
 return o;
}

void realtocurs()
{

//    if(can_change_color())
    for(int colorNumber=0;colorNumber<16; colorNumber++)
	{
	short red   =  (510*((colorNumber & 4)>>2) + 255*((colorNumber & 8)>>3))/3;
	short green =  (510*((colorNumber & 2)>>1)    + 255*((colorNumber & 8)>>3))/3;
	short blue  =  (510*((colorNumber & 1)) + 255*((colorNumber & 8)>>3))/3;
	if (colorNumber == 6) green >>= 1;
//	init_color(colorNumber, red,green,blue);
//	printw("color %d, r %x, g %x, b %x\n",colorNumber, red,green,blue);
	}
//getch();

    for( int b=0;b<16; b++)
{
       for( int f=0;f<16; f++)
        {
//           if(b !=0 && f !=0)
	                init_pair((b<<4)+f, f, b);
        }
}


/*
static short realtocurs[16] =
{
    COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, COLOR_RED,
    COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE, 
//    COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, COLOR_RED,
//    COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE

    COLOR_BLACK + 8, COLOR_BLUE + 8, COLOR_GREEN + 8, COLOR_CYAN + 8, COLOR_RED + 8,
    COLOR_MAGENTA + 8, COLOR_YELLOW + 8, COLOR_WHITE + 8

};
    for( int b=0;b<16; b++)
{
       for( int f=0;f<16; f++)
        {
           if(b !=0 && f !=0)
                init_pair((b<<4)+f, realtocurs[f], realtocurs[b]);
        }
}
*/

}

/*
"Programming for MS-DOS" (Ray Duncan)

                  COM                            EXE
                  ===                            ===
CS:IP           PSP:0100H             Defined by program's END statement
AL              00 if default FCB#1 has valid drive, FF if invalid drive
AH              Ditto, FCB#2
 Bill comment : FCB1 is filled from the first command argument, FCB2 from the
    second. From DOS5 (or maybe DOS6 - it's a long time ago...) FCB1 was
    left empty and FCB2 was filled from argument 1. AFAIAA, this was never
    fixed. What effect it has on this claim (re AX contents) I'm not sure.

DS              PSP                              PSP
ES              PSP                              PSP
SS              PSP                              Seg with STACK attribute
SP              0FFFEH or top word in avail.     Size of STACK segment
                 memory, whichever is least.

From: "The MS-DOS Encyclopaedia" (also Duncan) - talking about .EXE files. There is no comment on this point when discussing .COM files.

"The other processor registers (BX,CX,DX,BP,SI and DI) contain unknown values when the program receives control from MS-DOS."
*/

int init(_STATE* state);
void mainproc(_offsets _i, _STATE* state);

int main(int argc, char *argv[])
{
  _STATE state;
  _STATE* _state=&state;
  X86_REGREF
  
	_state->_indent=0;

	eax = 0x0ffff;
	ebx=ecx=edx=ebp=esi=edi=DF=0;

	init(_state);

  if (argc >= 2)
  {
	db s = strlen(argv[1]);
	*(((char*)&m)+0x80)=s+1;
	  strcpy( ((char*)&m)+0x81, argv[1]);
	*(((dw*)&m)+0x81+s)=0xD;

  }
	mainproc((_offsets) 0x1001, _state);
	return(0);
}

chtype vga_to_curses[256];
void prepare_cp437_to_curses()
{
for(size_t i=0; i<256; i++)
	{ vga_to_curses[i] = i; }
    vga_to_curses['\0'] = ' ';
    vga_to_curses[0x04] = ACS_DIAMOND;
    vga_to_curses[0x18] = ACS_UARROW;
    vga_to_curses[0x19] = ACS_DARROW;
    vga_to_curses[0x1a] = ACS_RARROW;
    vga_to_curses[0x1b] = ACS_LARROW;
    vga_to_curses[0x9c] = ACS_STERLING;
    vga_to_curses[0xb0] = ACS_BOARD;
    vga_to_curses[0xb1] = ACS_CKBOARD;
    vga_to_curses[0xb3] = ACS_VLINE;
    vga_to_curses[0xb4] = ACS_RTEE;
    vga_to_curses[0xbf] = ACS_URCORNER;
    vga_to_curses[0xc0] = ACS_LLCORNER;
    vga_to_curses[0xc1] = ACS_BTEE;
    vga_to_curses[0xc2] = ACS_TTEE;
    vga_to_curses[0xc3] = ACS_LTEE;
    vga_to_curses[0xc4] = ACS_HLINE;
    vga_to_curses[0xc5] = ACS_PLUS;
    vga_to_curses[0xce] = ACS_LANTERN;
    vga_to_curses[0xd8] = ACS_NEQUAL;
    vga_to_curses[0xd9] = ACS_LRCORNER;
    vga_to_curses[0xda] = ACS_ULCORNER;
    vga_to_curses[0xdb] = ACS_BLOCK;
    vga_to_curses[0xe3] = ACS_PI;
    vga_to_curses[0xf1] = ACS_PLMINUS;
    vga_to_curses[0xf2] = ACS_GEQUAL;
    vga_to_curses[0xf3] = ACS_LEQUAL;
    vga_to_curses[0xf8] = ACS_DEGREE;
    vga_to_curses[0xfe] = ACS_BULLET;
}

