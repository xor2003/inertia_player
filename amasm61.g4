grammar amasm61;
start: module;
module: directivelist?  enddir;
linereminder: comment | ENDOFLINE;
assdir: id '=' immexpr linereminder;
addop: '+' | '-';
aexpr: term '&&' aexpr | term;
altid: id;
arbitrarytext: charlist;
asminstruction: mnemonic  exprlist? ;
assumedir: ASSUME assumelist linereminder| ASSUMENOTHING linereminder;
assumelist : assumeregister| assumeregister ',' assumelist;
assumereg: register ':' assumeval;
assumeregister: assumesegreg| assumereg;
assumesegreg: segmentregister ':' assumesegval;
assumesegval: NOTHING | ERROR | frameexpr;
assumeval: NOTHING | ERROR | qualifiedtype;
bcdconst: sign?  decnumber;
binaryop: '=='| '!='| '>='| '<='| '>'| '<'| '&' ;
eqconstexpr: '=' constexpr;
bitdef: bitfieldid ':' bitfieldsize eqconstexpr?;
bitdeflist: bitdef| bitdef ',' linereminder? bitdeflist;
bitfieldid : id ;
bitfieldsize: constexpr;
ifcexpr: U_IF cexpr;
blockstatements: U_CONTINUE ifcexpr? | U_BREAK ifcexpr? | directivelist;
bool: TRUE | FALSE ;
byteregister: AL | AH | BL | BH | CL | CH | DL | DH ;
cexpr: aexpr ':' cexpr | aexpr;
charlist: CHARACTER+;
classname: string;
commdecl: nearfar?   langtype?  id ':' commtype eqconstexpr?;
commdir: COMM commlist linereminder;
commentdir: COMMENT DELIMITER text text DELIMITER text linereminder;
commlist: commdecl| commdecl ',' commlist;
commtype: type | constexpr;
constant: digits  radixoverride? ;
constexpr: expr;
contextdir: PUSHCONTEXT contextitemlist linereminder| POPCONTEXT contextitemlist linereminder;
contextitem: ASSUMES | RADIX | LISTING | CPU | ALL ;
contextitemlist: contextitem| contextitem ',' contextitemlist;
controlblock: whileblock| repeatblock;
controldir: controlif| controlblock;
controlelseif: U_ELSEIF cexpr linereminder directivelist controlelseif? ;
elsedirectivelist: U_ELSE linereminder directivelist;
controlif: U_IF cexpr linereminder directivelist controlelseif? elsedirectivelist? U_ENDIF linereminder;
coprocessor: U_8087 | U_287 | U_387 | U_NO87 ;
crefdir: crefoption linereminder;
crefoption: U_CREF | U_XCREF idlist? | U_NOCREF idlist? ;
cxzexpr: '!' expr| expr  '==' expr| expr  '!=' expr | expr;
datadecl: DB | DW | DD | DF | DQ | DT | datatype | typeid;
datadir: id?  dataitem linereminder;
dataitem: datadecl scalarinstlist| structtag structinstlist| typeid structinstlist| uniontag structinstlist| recordtag recordinstlist;
datatype: BYTE | SBYTE | WORD | SWORD | DWORD | SDWORD | FWORD | QWORD | TBYTE | REAL4 | REAL8 | REAL10;
decnumber: decdigit+;
digits: decdigit+| digits hexdigit;
directive: generaldir| segmentdef;
directivelist: directive+;
distance: nearfar| NEAR16 | NEAR32 | FAR16 | FAR32;
e01: e02 e01a;
e01a: (orop e02 e01a)? ;
e02: e03 e02a;
e02a: (AND e03 e02a)? ;
e03: NOT e04| e04;
e04: e05 e04a;
e04a: (relop e05 e04a)? ;
e05: e06 e05a;
e05a: (addop e06 e05a)? ;

e06: e08 e06a;
e06a: (mulop e08 e06a)? | (shiftop e08 e06a);

e08: HIGH e09| LOW e09| HIGHWORD e09| LOWWORD e09| e09;

e09: OFFSET e10| SEG e10| LROFFSET e10| TYPE e10| THIS e10| e10 e09a;
e09a: (PTR e10 e09a)? | (':' e10 e09a);

e10: e11 e10a;
e10a: (U_DOT e11 e10a)?;

expr: SHORT e05| U_TYPE e01| OPATTR e01| e01;
e11: '( expr )' | WIDTH id| MASK id| SIZE sizearg| SIZEOF sizearg| LENGTH id| LENGTHOF id | recordconst| string| constant| type| id| '$'| segmentregister| register| ST | ST '( expr )';
echodir: ECHO arbitrarytext linereminder | U_OUT arbitrarytext linereminder;
elseifblock: elseifstatement linereminder directivelist elseifblock? ;
elseifstatement: ELSEIF constexpr| ELSEIFE constexpr| ELSEIFB textitem| ELSEIFNB textitem | ELSEIFDEF id| ELSEIFNDEF id| ELSEIFDIF textitem ',' textitem| ELSEIFDIFI textitem ',' textitem| ELSEIFIDN textitem ',' textitem| ELSEIFIDNI textitem ',' textitem| 'ELSEIF1' | 'ELSEIF2' ;
enddir: END immexpr?  linereminder;
endpdir: procid ENDP linereminder;
endsdir: id ENDS linereminder;
equdir: textmacroid EQU equtype linereminder;
equtype: immexpr| textliteral;
errordir: erroropt linereminder;
erroropt: U_ERR textitem? | U_ERRE constexpr opttext? | U_ERRNZ constexpr opttext? | U_ERRB textitem opttext? | U_ERRNB textitem opttext? | U_ERRDEF id  opttext? | U_ERRNDEF id opttext? | U_ERRDIF textitem ',' textitem  opttext? | U_ERRDIFI textitem ',' textitem  opttext? | U_ERRIDN textitem ',' textitem  opttext? | U_ERRIDNI textitem ',' textitem  opttext? | U_ERR1 textitem? | U_ERR2 textitem?  ;
exitdir: U_EXIT expr?  linereminder;
exitmdir: EXITM | EXITM textitem;
exponent: 'E'  sign?  decnumber;
exprlist: expr | expr  ','  exprlist;
altidq: '( altid )';
externdef: langtype?  id altidq? ':' externtype;
externdir: externkey externlist linereminder;
externkey: EXTRN | EXTERN | EXTERNDEF ;
externlist: externdef| externdef  ',' linereminder?  externlist;
externtype: ABS | qualifiedtype;
fieldalign: constexpr;
fieldinit: initvalue | structinstance;
fieldinitlist: fieldinit ','  linereminder?  fieldinitlist | fieldinit;
filechar: DELIMITER;
filecharlist: filechar+;
filespec: filecharlist| textliteral;
flagname: ZERO_ | CARRY_ | OVERFLOW_ | SIGN_ | PARITY_;
floatnumber : sign?  decnumber U_DOT decnumber?   exponent? | digits 'R'| digits 'r';
forcdir: FORC | IRPC ;
fordir: FOR | IRP ;
dotdotforparmtype: ':' forparmtype;
forparm: id dotdotforparmtype?;
forparmtype: REQ | '=' textliteral;
frameexpr: SEG id | DGROUP ':' id| segmentregister ':' id| id;
generaldir: modeldir | segorderdir | namedir| includelibdir | commentdir| groupdir | assumedir| structdir | recorddir | typedefdir| externdir | publicdir | commdir | prototypedir| equdir | assdir | textdir| contextdir | optiondir | processordir| radixdir | titledir | pagedir | listdir | crefdir | echodir| ifdir | errordir | includedir | macrodir | macrocall | macrorepeat | purgedir| macrowhile | macrofor | macroforc| aliasdir;
aliasdir: ALIAS;
gpregister: AX | EAX | BX | EBX | CX | ECX | DX | EDX| BP | EBP | SP | ESP | DI | EDI | SI | ESI;
groupdir : groupid GROUP segidlist;
groupid: id;
idlist: id ',' idlist | id;
ifdir: ifstatement linereminder directivelist elseifblock? elsedirectivelist? ENDIF linereminder;
ifstatement: IF constexpr| IFE constexpr| IFB textitem| IFNB textitem| IFDEF id| IFNDEF id| IFDIF textitem ',' textitem| IFDIFI textitem ',' textitem| IFIDN textitem ',' textitem| IFIDNI textitem ',' textitem| 'IF1' | 'IF2' ;
immexpr: expr;
includedir: INCLUDE filespec linereminder;
includelibdir: INCLUDELIB filespec linereminder;
initvalue: immexpr| string| '?'| constexpr  DUP '( scalarinstlist )'| floatnumber| bcdconst;
insegdir: labeldef?  insegmentdir;
insegdirlist: insegdir+;
insegmentdir: instruction| datadir| controldir| startupdir| exitdir| offsetdir| labeldir| procdir  localdirlist?   insegdirlist?  endpdir| invokedir| generaldir ;
instrprefix: REP | REPE | REPZ | REPNE | REPNZ | LOCK ;
instruction: instrprefix?  asminstruction;
invokearg: register '::' register| ADDR expr | expr;
commainvokelist: ',' linereminder? invokelist;
invokedir: INVOKE expr commainvokelist? linereminder;
invokelist: invokearg  ',' linereminder?  invokelist | invokearg;
labeldef: id ':' | id '::' | MONKEY;
labeldir: id LABEL qualifiedtype linereminder;
langtype: C | PASCAL | FORTRAN | BASIC | SYSCALL | STDCALL ;
listdir: listoption linereminder;
listoption: U_LIST | U_NOLIST | U_XLIST | U_LISTALL | U_LISTIF | U_LFCOND | U_NOLISTIF | U_SFCOND | U_TFCOND | U_LISTMACROALL | U_LALL | U_NOLISTMACRO | U_SALL | U_LISTMACRO | U_XALL ;
localdef: LOCAL idlist linereminder;
localdir: LOCAL parmlist linereminder;
localdirlist: localdir+;
locallist: localdef+;
macroarg: '%' constexpr| '%' textmacroid| '%' macrofuncid '(' macroarglist ')'| string| '<' arbitrarytext '>' | arbitrarytext;
macroarglist: macroarg  ',' macroarglist | macroarg;
macrobody: locallist? macrostmtlist;
macrocall: id  '(' macroarglist ')' | id macroarglist linereminder;
macrodir: id MACRO macroparmlist?  linereminder macrobody ENDM linereminder;
macrofor: fordir forparm  ',' '<' macroarglist '>' linereminder macrobody ENDM linereminder;
macroforc: forcdir id  ',' textliteral linereminder macrobody ENDM linereminder;
macrofuncid: id;
macroid: macroprocid| macrofuncid;
macroidlist: macroid  ','  macroidlist | macroid;
macrolabel: id;
dotdotparmtype: ':' parmtype;
macroparm: id dotdotparmtype?;
macroparmlist: macroparm  ',' linereminder?  macroparmlist | macroparm;
macroprocid: id;
macrorepeat: repeatdir constexpr linereminder macrobody ENDM linereminder;
macrostmt: directive| exitmdir| ':' macrolabel| GOTO macrolabel;
macrostmtlist: macrostmt macrostmtlist linereminder | macrostmt linereminder;
macrowhile: WHILE constexpr linereminder macrobody ENDM linereminder;
maptype: ALL | NONE | NOTPUBLIC ;
memoption: TINY | SMALL | MEDIUM | COMPACT | LARGE | HUGE | FLAT ;
mnemonic: AAA| AAD| AAM| AAS| ADC| ADD| AND| CALL| CBW| CLC| CLD| CLI| CMC| CMP| CMPS| CMPSB| CMPSW| CWD| DAA| DAS| DEC| DIV| ESC| HLT| IDIV| IMUL| IN| INC| INT| INTO| IRET| JA| JAE| JB| JBE| JC| JCXZ| JE| JG| JGE| JL| JLE| JMP| JNA| JNAE| JNB| JNBE| JNC| JNE| JNG| JNGE| JNL| JNLE| JNO| JNP| JNS| JNZ| JO| JP| JPE| JPO| JS| JZ| LAHF| LDS| LEA| LES| LODS| LODSB| LODSW| LOOP| LOOPE| LOOPEW| LOOPNE| LOOPNEW| LOOPNZ| LOOPNZW| LOOPW| LOOPZ| LOOPZW| MOV| MOVS| MOVSB| MOVSW| MUL| NEG| NOP| NOT| OR| OUT| POP| POPF| PUSH| PUSHF| RCL| RCR| RET| RETF| RETN| ROL| ROR| SAHF| SAL| SAR| SBB| SCAS| SCASB| SCASW| SHL| SHR| STC| STD| STI| STOS| STOSB| STOSW| SUB| TEST| WAIT| XCHG| XLAT| XLATB| XOR| BOUND| ENTER| INS| INSB| INSW| LEAVE| OUTS| OUTSB| OUTSW| POPA| PUSHA| PUSHW| ARPL| LAR| LSL| SGDT| SIDT| SLDT| SMSW| STR| VERR| VERW| CLTS| LGDT| LIDT| LLDT| LMSW| LTR| BSF| BSR| BT| BTC| BTR| BTS| CDQ| CMPSD| CWDE| INSD| IRETD| IRETDF| IRETF| JECXZ| LFS| LGS| LODSD| LOOPD| LOOPED| LOOPNED| LOOPNZD| LOOPZD| LSS| MOVSD| MOVSX| MOVZX| OUTSD| POPAD| POPFD| PUSHAD| PUSHD| PUSHFD| SCASD| SETA| SETAE| SETB| SETBE| SETC| SETE| SETG| SETGE| SETL| SETLE| SETNA| SETNAE| SETNB| SETNBE| SETNC| SETNE| SETNG| SETNGE| SETNL| SETNLE| SETNO| SETNP| SETNS| SETNZ| SETO| SETP| SETPE| SETPO| SETS| SETZ| SHLD| SHRD| STOSD| BSWAP| CMPXCHG| INVD| INVLPG| WBINVD| XADD| F2XM1| FABS| FADD| FADDP| FBLD| FBSTP| FCHS| FCLEX| FCOM| FCOMP| FCOMPP| FDECSTP| FDISI| FDIV| FDIVP| FDIVR| FDIVRP| FENI| FFREE| FIADD| FICOM| FICOMP| FIDIV| FIDIVR| FILD| FIMUL| FINCSTP| FINIT| FIST| FISTP| FISUB| FISUBR| FLD| FLD1| FLDCW| FLDENV| FLDENVW| FLDL2E| FLDL2T| FLDLG2| FLDLN2| FLDPI| FLDZ| FMUL| FMULP| FNCLEX| FNDISI| FNENI| FNINIT| FNOP| FNSAVE| FNSAVEW| FNSTCW| FNSTENV| FNSTENVW| FNSTSW| FPATAN| FPREM| FPTAN| FRNDINT| FRSTOR| FRSTORW| FSAVE| FSAVEW| FSCALE| FSQRT| FST| FSTCW| FSTENV| FSTENVW| FSTP| FSTSW| FSUB| FSUBP| FSUBR| FSUBRP| FTST| FWAIT| FXAM| FXCH| FXTRACT| FYL2X| FYL2XP1| FSETPM| FCOS| FLDENVD| FNSAVED| FNSTENVD| FPREM1| FRSTORD| FSAVED| FSIN| FSINCOS| FSTENVD| FUCOM| FUCOMP| FUCOMPP;
commamodeloptlist: ',' modeloptlist;
modeldir: U_MODEL memoption commamodeloptlist? linereminder;
modelopt: langtype| stackoption ;
modeloptlist: modelopt| modelopt  ',' modeloptlist;
mulop: '*'| '/' | MOD ;
namedir: NAME id linereminder;
nearfar: NEAR | FAR ;
nestedstruct: structhdr  id?  linereminder structbody ENDS linereminder;
offsetdir: offsetdirtype linereminder;
offsetdirtype: EVEN | ORG immexpr| ALIGN constexpr? ;
offsettype: GROUP | SEGMENT | FLAT ;
oldrecordfieldlist: constexpr ',' oldrecordfieldlist | constexpr;
optiondir: OPTION optionlist linereminder;
readonly: READONLY;
optionitem: CASEMAP ':' maptype| DOTNAME | NODOTNAME | EMULATOR | NOEMULATOR | EPILOGUE ':' macroid| 'EXPR16' | 'EXPR32' | LANGUAGE ':' langtype| LJMP | NOLJMP | 'M510' | 'NOM510' | NOSIGNEXTEND | OFFSET ':' offsettype| OLDMACROS | NOOLDMACROS | OLDSTRUCTS | NOOLDSTRUCTS | PROC ':' ovisibility| PROLOGUE ':' macroid| readonly | NOREADONLY | SCOPED | NOSCOPED | SEGMENT ':' segsize| 'SETIF2 :' bool;
optionlist: optionitem| optionitem  ',' linereminder?  optionlist;
opttext: ',' textitem;
orop: OR | XOR ;
ovisibility: PUBLIC | PRIVATE | EXPORT ;
pagedir: PAGE pageexpr? linereminder;
commapagewidth: ',' pagewidth;
pageexpr: '+'|  pagelength commapagewidth? | commapagewidth;
pagelength: constexpr;
pagewidth: constexpr;
dotdotqualifiedtype: ':' qualifiedtype;
parm: parmid  constexpr? dotdotqualifiedtype?;
parmid: id;
parmlist: parm  ',' linereminder? parmlist | parm;
parmtype: REQ | '=' textliteral| VARARG ;
poptions: distance?  langtype?  ovisibility? ;
primary: expr binaryop expr| flagname| expr;
brmacroarglist: '<' macroarglist '>';
procdir: procid PROC poptions  brmacroarglist?  usesregs?   procparmlist? ;
processor: U_8086 | U_186 | U_286 | U_286C | U_286P | U_386 | U_386C | U_386P | U_486 | U_486P | U_586 | U_586P | U_686 | U_686P | U_K3D | U_MMX | U_XMM;
processordir: processor linereminder | coprocessor linereminder;
procid: id;
commaparmlist: ','  linereminder?  parmlist;
commaparmidvararg: ','  linereminder?  parmid U_VARARG;
procparmlist: ',' commaparmlist? ',' commaparmidvararg?;
protoarg : id? ':' qualifiedtype ;
commaprotolist: ','  linereminder?  protolist;
commaidvararg: ','  linereminder?   id? U_VARARG;
protoarglist: ',' commaprotolist? ',' commaidvararg?;
protolist: protoarg| protoarg  ',' linereminder?  protolist;
protospec: typeid | distance?   langtype?   protoarglist?;
prototypedir: id PROTO protospec;
pubdef: langtype?  id;
publicdir: PUBLIC publist linereminder;
publist: pubdef  ',' linereminder?  publist | pubdef;
purgedir: PURGE macroidlist;
qualifiedtype: distance? PTR qualifiedtype? | type;
qualifier: PROTO protospec | qualifiedtype;
quote: '\'' | '"';
radixdir: U_RADIX constexpr linereminder;
recordconst: recordtag '{' oldrecordfieldlist '}'| recordtag '<' oldrecordfieldlist '>';
recorddir: recordtag RECORD bitdeflist linereminder ;
recordfieldlist: constexpr  ','  linereminder? recordfieldlist | constexpr;
recordinstance: '{'  linereminder?  recordfieldlist  linereminder?  '}'| '<' oldrecordfieldlist '>'| constexpr DUP '(' recordinstance ')';
recordinstlist: recordinstance| recordinstance ','  linereminder?  recordinstlist;
recordtag: id;
register: specialregister| gpregister| byteregister;
reglist: register+;
relop: EQ | NE | LT | LE | GT | GE ;
repeatblock: U_REPEAT linereminder blockstatements linereminder untildir linereminder;
repeatdir: REPEAT | REPT ;
scalarinstlist: initvalue ','  linereminder? scalarinstlist | initvalue;
segalign: BYTE | WORD | DWORD | PARA | PAGE ;
segattrib: PUBLIC | STACK | COMMON | MEMORY | AT constexpr| PRIVATE ;
segdir: U_CODE segid? | U_DATA | U_DATA_| U_CONST | U_FARDATA segid? | U_FARDATA_ segid? | U_STACK constexpr?;
segid: id;
segidlist : segid  ',' segidlist | segid;
segmentdef: segmentdir  insegdirlist?  endsdir| simplesegdir  insegdirlist?   endsdir? ;
segmentdir: segid SEGMENT segoptionlist?  linereminder;
segmentregister: CS | DS | ES | FS | GS | SS;
segoption: segalign| segro| segattrib| segsize| classname;
segoptionlist: segoption+;
segorderdir: U_ALPHA | U_SEQ | U_DOSSEG | DOSSEG ;
segro: readonly;
segsize : USE16 | USE32 | FLAT;
shiftop: SHR | SHL ;
sign: '-' | '+';
simpleexpr: '(' cexpr ')'| primary;
simplesegdir: segdir linereminder;
sizearg: id| type| e10;
specialregister: 'CR0' | 'CR2' | 'CR3' | 'DR0' | 'DR1' | 'DR2' | 'DR3' | 'DR6' | 'DR7'| 'TR3' | 'TR4' | 'TR5' | 'TR6' | 'TR7';
stackoption: NEARSTACK | FARSTACK ;
startupdir: U_STARTUP linereminder;
stext: STRINGCHAR+;
string: quote  stext?  quote;
structbody: structitem structbody | structitem linereminder;
commanonuniq: ',' NONUNIQUE;
structdir: structtag structhdr  fieldalign?  commanonuniq? linereminder structbody structtag ENDS linereminder;
structhdr: STRUC | STRUCT | UNION ;
structinstance: '<'  fieldinitlist?  '>'| '{' linereminder?   fieldinitlist?   linereminder?  '}'| constexpr DUP '( structinstlist )';
structinstlist: structinstance| structinstance  ','  linereminder?  structinstlist;
structitem : datadir| generaldir| offsetdir| nestedstruct;
structtag: id;
term: '!' simpleexpr | simpleexpr;
text: '!' CHARACTER text| '!' CHARACTER| textliteral | CHARACTER+;
textdir: id textmacrodir linereminder;
textitem: textliteral| '%' constexpr| textmacroid;
textlen: constexpr;
textlist: textitem  ',' linereminder?  textlist | textitem;
textliteral: '<' text '>' linereminder;
commatextlen: ',' textlen;
textstartcomma: textstart  ',';
textmacrodir: CATSTR textlist? | TEXTEQU textlist? | SIZESTR textitem| SUBSTR textitem  ',' textstart commatextlen| INSTR textstartcomma? textitem  ',' textitem;
textmacroid: id;
textstart: constexpr;
titledir: titletype arbitrarytext linereminder;
titletype: TITLE | SUBTITLE | SUBTTL ;
type: structtag| uniontag| recordtag| distance| datatype| typeid;
typedefdir: typeid TYPEDEF qualifier;
typeid: id;
uniontag: id;
untildir: U_UNTIL cexpr linereminder U_UNTILCXZ cxzexpr?  linereminder;
usesregs: USES reglist ;
whileblock: U_WHILE cexpr linereminder blockstatements linereminder U_ENDW ;
AAA: 'AAA';
AAD: 'AAD';
AAM: 'AAM';
AAS: 'AAS';
ABS: 'ABS';
ADC: 'ADC';
ADD: 'ADD';
ADDR: 'ADDR';
AH: 'AH';
AL: 'AL';
ALIAS: 'ALIAS';
ALIGN: 'ALIGN';
ALL: 'ALL';
AND: 'AND';
ARPL: 'ARPL';
ASSUMENOTHING: 'ASSUME NOTHING';
ASSUME: 'ASSUME';
ASSUMES: 'ASSUMES';
AT: 'AT';
AX: 'AX';
BASIC: 'BASIC';
BH: 'BH';
BL: 'BL';
BOUND: 'BOUND';
BP: 'BP';
BSF: 'BSF';
BSR: 'BSR';
BSWAP: 'BSWAP';
BT: 'BT';
BTC: 'BTC';
BTR: 'BTR';
BTS: 'BTS';
BX: 'BX';
BYTE: 'BYTE';
CALL: 'CALL';
CARRY_: 'CARRY?';
CASEMAP: 'CASEMAP';
CATSTR: 'CATSTR';
CBW: 'CBW';
CDQ: 'CDQ';
CH: 'CH';
CL: 'CL';
CLC: 'CLC';
CLD: 'CLD';
CLI: 'CLI';
CLTS: 'CLTS';
CMC: 'CMC';
CMP: 'CMP';
CMPS: 'CMPS';
CMPSB: 'CMPSB';
CMPSD: 'CMPSD';
CMPSW: 'CMPSW';
CMPXCHG: 'CMPXCHG';
COMM: 'COMM';
COMMENT: 'COMMENT';
COMMON: 'COMMON';
COMPACT: 'COMPACT';
CPU: 'CPU';
CR0: 'CR0';
CR2: 'CR2';
CR3: 'CR3';
CS: 'CS';
CWD: 'CWD';
CWDE: 'CWDE';
CX: 'CX';
DAA: 'DAA';
DAS: 'DAS';
DB: 'DB';
DD: 'DD';
DEC: 'DEC';
DF: 'DF';
DGROUP: 'DGROUP';
DH: 'DH';
DI: 'DI';
DIV: 'DIV';
DL: 'DL';
DOSSEG: 'DOSSEG';
DOTNAME: 'DOTNAME';
DQ: 'DQ';
DR0: 'DR0';
DR1: 'DR1';
DR2: 'DR2';
DR3: 'DR3';
DR6: 'DR6';
DR7: 'DR7';
DS: 'DS';
DT: 'DT';
DUP: 'DUP';
DW: 'DW';
DWORD: 'DWORD';
DX: 'DX';
EAX: 'EAX';
EBP: 'EBP';
EBX: 'EBX';
ECHO: 'ECHO';
ECX: 'ECX';
EDI: 'EDI';
EDX: 'EDX';
ELSEIF1: 'ELSEIF1';
ELSEIF2: 'ELSEIF2';
ELSEIF: 'ELSEIF';
ELSEIFB: 'ELSEIFB';
ELSEIFDEF: 'ELSEIFDEF';
ELSEIFDIF: 'ELSEIFDIF';
ELSEIFDIFI: 'ELSEIFDIFI';
ELSEIFE: 'ELSEIFE';
ELSEIFIDN: 'ELSEIFIDN';
ELSEIFIDNI: 'ELSEIFIDNI';
ELSEIFNB: 'ELSEIFNB';
ELSEIFNDEF: 'ELSEIFNDEF';
EMULATOR: 'EMULATOR';
END: 'END';
ENDIF: 'ENDIF';
ENDM: 'ENDM';
ENDP: 'ENDP';
ENDS: 'ENDS';
ENTER: 'ENTER';
EPILOGUE: 'EPILOGUE';
EQ: 'EQ';
EQU: 'EQU';
ERROR: 'ERROR';
ES: 'ES';
ESC: 'ESC';
ESI: 'ESI';
ESP: 'ESP';
EVEN: 'EVEN';
EXITM: 'EXITM';
EXPORT: 'EXPORT';
EXPR16: 'EXPR16';
EXPR32: 'EXPR32';
EXTERN: 'EXTERN';
EXTERNDEF: 'EXTERNDEF';
EXTRN: 'EXTRN';
F2XM1: 'F2XM1';
FABS: 'FABS';
FADD: 'FADD';
FADDP: 'FADDP';
FALSE: 'FALSE';
FAR16: 'FAR16';
FAR32: 'FAR32';
FAR: 'FAR';
FARSTACK: 'FARSTACK';
FBLD: 'FBLD';
FBSTP: 'FBSTP';
FCHS: 'FCHS';
FCLEX: 'FCLEX';
FCOM: 'FCOM';
FCOMP: 'FCOMP';
FCOMPP: 'FCOMPP';
FCOS: 'FCOS';
FDECSTP: 'FDECSTP';
FDISI: 'FDISI';
FDIV: 'FDIV';
FDIVP: 'FDIVP';
FDIVR: 'FDIVR';
FDIVRP: 'FDIVRP';
FENI: 'FENI';
FFREE: 'FFREE';
FIADD: 'FIADD';
FICOM: 'FICOM';
FICOMP: 'FICOMP';
FIDIV: 'FIDIV';
FIDIVR: 'FIDIVR';
FILD: 'FILD';
FIMUL: 'FIMUL';
FINCSTP: 'FINCSTP';
FINIT: 'FINIT';
FIST: 'FIST';
FISTP: 'FISTP';
FISUB: 'FISUB';
FISUBR: 'FISUBR';
FLAT: 'FLAT';
FLD1: 'FLD1';
FLD: 'FLD';
FLDCW: 'FLDCW';
FLDENV: 'FLDENV';
FLDENVD: 'FLDENVD';
FLDENVW: 'FLDENVW';
FLDL2E: 'FLDL2E';
FLDL2T: 'FLDL2T';
FLDLG2: 'FLDLG2';
FLDLN2: 'FLDLN2';
FLDPI: 'FLDPI';
FLDZ: 'FLDZ';
FMUL: 'FMUL';
FMULP: 'FMULP';
FNCLEX: 'FNCLEX';
FNDISI: 'FNDISI';
FNENI: 'FNENI';
FNINIT: 'FNINIT';
FNOP: 'FNOP';
FNSAVE: 'FNSAVE';
FNSAVED: 'FNSAVED';
FNSAVEW: 'FNSAVEW';
FNSTCW: 'FNSTCW';
FNSTENV: 'FNSTENV';
FNSTENVD: 'FNSTENVD';
FNSTENVW: 'FNSTENVW';
FNSTSW: 'FNSTSW';
FOR: 'FOR';
FORC: 'FORC';
FORTRAN: 'FORTRAN';
FPATAN: 'FPATAN';
FPREM1: 'FPREM1';
FPREM: 'FPREM';
FPTAN: 'FPTAN';
FRNDINT: 'FRNDINT';
FRSTOR: 'FRSTOR';
FRSTORD: 'FRSTORD';
FRSTORW: 'FRSTORW';
FS: 'FS';
FSAVE: 'FSAVE';
FSAVED: 'FSAVED';
FSAVEW: 'FSAVEW';
FSCALE: 'FSCALE';
FSETPM: 'FSETPM';
FSIN: 'FSIN';
FSINCOS: 'FSINCOS';
FSQRT: 'FSQRT';
FST: 'FST';
FSTCW: 'FSTCW';
FSTENV: 'FSTENV';
FSTENVD: 'FSTENVD';
FSTENVW: 'FSTENVW';
FSTP: 'FSTP';
FSTSW: 'FSTSW';
FSUB: 'FSUB';
FSUBP: 'FSUBP';
FSUBR: 'FSUBR';
FSUBRP: 'FSUBRP';
FTST: 'FTST';
FUCOM: 'FUCOM';
FUCOMP: 'FUCOMP';
FUCOMPP: 'FUCOMPP';
FWAIT: 'FWAIT';
FWORD: 'FWORD';
FXAM: 'FXAM';
FXCH: 'FXCH';
FXTRACT: 'FXTRACT';
FYL2X: 'FYL2X';
FYL2XP1: 'FYL2XP1';
GE: 'GE';
GOTO: 'GOTO';
GROUP: 'GROUP';
GS: 'GS';
GT: 'GT';
HIGH: 'HIGH';
HIGHWORD: 'HIGHWORD';
HLT: 'HLT';
HUGE: 'HUGE';
IDIV: 'IDIV';
IF1: 'IF1';
IF2: 'IF2';
IF: 'IF';
IFB: 'IFB';
IFDEF: 'IFDEF';
IFDIF: 'IFDIF';
IFDIFI: 'IFDIFI';
IFE: 'IFE';
IFIDN: 'IFIDN';
IFIDNI: 'IFIDNI';
IFNB: 'IFNB';
IFNDEF: 'IFNDEF';
IMUL: 'IMUL';
IN: 'IN';
INC: 'INC';
INCLUDE: 'INCLUDE';
INCLUDELIB: 'INCLUDELIB';
INS: 'INS';
INSB: 'INSB';
INSD: 'INSD';
INSTR: 'INSTR';
INSW: 'INSW';
INT: 'INT';
INTO: 'INTO';
INVD: 'INVD';
INVLPG: 'INVLPG';
INVOKE: 'INVOKE';
IRET: 'IRET';
IRETD: 'IRETD';
IRETDF: 'IRETDF';
IRETF: 'IRETF';
IRP: 'IRP';
IRPC: 'IRPC';
JA: 'JA';
JAE: 'JAE';
JB: 'JB';
JBE: 'JBE';
JC: 'JC';
JCXZ: 'JCXZ';
JE: 'JE';
JECXZ: 'JECXZ';
JG: 'JG';
JGE: 'JGE';
JL: 'JL';
JLE: 'JLE';
JMP: 'JMP';
JNA: 'JNA';
JNAE: 'JNAE';
JNB: 'JNB';
JNBE: 'JNBE';
JNC: 'JNC';
JNE: 'JNE';
JNG: 'JNG';
JNGE: 'JNGE';
JNL: 'JNL';
JNLE: 'JNLE';
JNO: 'JNO';
JNP: 'JNP';
JNS: 'JNS';
JNZ: 'JNZ';
JO: 'JO';
JP: 'JP';
JPE: 'JPE';
JPO: 'JPO';
JS: 'JS';
JZ: 'JZ';
LABEL: 'LABEL';
LAHF: 'LAHF';
LANGUAGE: 'LANGUAGE';
LAR: 'LAR';
LARGE: 'LARGE';
LDS: 'LDS';
LE: 'LE';
LEA: 'LEA';
LEAVE: 'LEAVE';
LENGTH: 'LENGTH';
LENGTHOF: 'LENGTHOF';
LES: 'LES';
LFS: 'LFS';
LGDT: 'LGDT';
LGS: 'LGS';
LIDT: 'LIDT';
LISTING: 'LISTING';
LJMP: 'LJMP';
LLDT: 'LLDT';
LMSW: 'LMSW';
LOCAL: 'LOCAL';
LOCK: 'LOCK';
LODS: 'LODS';
LODSB: 'LODSB';
LODSD: 'LODSD';
LODSW: 'LODSW';
LOOP: 'LOOP';
LOOPD: 'LOOPD';
LOOPE: 'LOOPE';
LOOPED: 'LOOPED';
LOOPEW: 'LOOPEW';
LOOPNE: 'LOOPNE';
LOOPNED: 'LOOPNED';
LOOPNEW: 'LOOPNEW';
LOOPNZ: 'LOOPNZ';
LOOPNZD: 'LOOPNZD';
LOOPNZW: 'LOOPNZW';
LOOPW: 'LOOPW';
LOOPZ: 'LOOPZ';
LOOPZD: 'LOOPZD';
LOOPZW: 'LOOPZW';
LOW: 'LOW';
LOWWORD: 'LOWWORD';
LROFFSET: 'LROFFSET';
LSL: 'LSL';
LSS: 'LSS';
LT: 'LT';
LTR: 'LTR';
M510: 'M510';
MACRO: 'MACRO';
MASK: 'MASK';
MEDIUM: 'MEDIUM';
MEMORY: 'MEMORY';
MOD: 'MOD';
MONKEY: '@@:';
MOV: 'MOV';
MOVS: 'MOVS';
MOVSB: 'MOVSB';
MOVSD: 'MOVSD';
MOVSW: 'MOVSW';
MOVSX: 'MOVSX';
MOVZX: 'MOVZX';
MUL: 'MUL';
NAME: 'NAME';
NE: 'NE';
NEAR16: 'NEAR16';
NEAR32: 'NEAR32';
NEAR: 'NEAR';
NEARSTACK: 'NEARSTACK';
NEG: 'NEG';
NODOTNAME: 'NODOTNAME';
NOEMULATOR: 'NOEMULATOR';
NOLJMP: 'NOLJMP';
NOM510: 'NOM510';
NONE: 'NONE';
NONUNIQUE: 'NONUNIQUE';
NOOLDMACROS: 'NOOLDMACROS';
NOOLDSTRUCTS: 'NOOLDSTRUCTS';
NOP: 'NOP';
NOREADONLY: 'NOREADONLY';
NOSCOPED: 'NOSCOPED';
NOSIGNEXTEND: 'NOSIGNEXTEND';
NOT: 'NOT';
NOTHING: 'NOTHING';
NOTPUBLIC: 'NOTPUBLIC';
OFFSET: 'OFFSET';
OLDMACROS: 'OLDMACROS';
OLDSTRUCTS: 'OLDSTRUCTS';
OPATTR: 'OPATTR';
OPTION: 'OPTION';
OR: 'OR';
ORG: 'ORG';
OUT: 'OUT';
OUTS: 'OUTS';
OUTSB: 'OUTSB';
OUTSD: 'OUTSD';
OUTSW: 'OUTSW';
OVERFLOW_: 'OVERFLOW?';
PAGE: 'PAGE';
PARA: 'PARA';
PARITY_: 'PARITY?';
PASCAL: 'PASCAL';
POP: 'POP';
POPA: 'POPA';
POPAD: 'POPAD';
POPCONTEXT: 'POPCONTEXT';
POPF: 'POPF';
POPFD: 'POPFD';
PRIVATE: 'PRIVATE';
PROC: 'PROC';
PROLOGUE: 'PROLOGUE';
PROTO: 'PROTO';
PTR: 'PTR';
PUBLIC: 'PUBLIC';
PURGE: 'PURGE';
PUSH: 'PUSH';
PUSHA: 'PUSHA';
PUSHAD: 'PUSHAD';
PUSHCONTEXT: 'PUSHCONTEXT';
PUSHD: 'PUSHD';
PUSHF: 'PUSHF';
PUSHFD: 'PUSHFD';
PUSHW: 'PUSHW';
QWORD: 'QWORD';
RADIX: 'RADIX';
RCL: 'RCL';
RCR: 'RCR';
READONLY: 'READONLY';
REAL10: 'REAL10';
REAL4: 'REAL4';
REAL8: 'REAL8';
RECORD: 'RECORD';
REP: 'REP';
REPE: 'REPE';
REPEAT: 'REPEAT';
REPNE: 'REPNE';
REPNZ: 'REPNZ';
REPT: 'REPT';
REPZ: 'REPZ';
REQ: 'REQ';
RET: 'RET';
RETF: 'RETF';
RETN: 'RETN';
ROL: 'ROL';
ROR: 'ROR';
SAHF: 'SAHF';
SAL: 'SAL';
SAR: 'SAR';
SBB: 'SBB';
SBYTE: 'SBYTE';
SCAS: 'SCAS';
SCASB: 'SCASB';
SCASD: 'SCASD';
SCASW: 'SCASW';
SCOPED: 'SCOPED';
SDWORD: 'SDWORD';
SEG: 'SEG';
SEGMENT: 'SEGMENT';
SETA: 'SETA';
SETAE: 'SETAE';
SETB: 'SETB';
SETBE: 'SETBE';
SETC: 'SETC';
SETE: 'SETE';
SETG: 'SETG';
SETGE: 'SETGE';
SETIF2: 'SETIF2';
SETL: 'SETL';
SETLE: 'SETLE';
SETNA: 'SETNA';
SETNAE: 'SETNAE';
SETNB: 'SETNB';
SETNBE: 'SETNBE';
SETNC: 'SETNC';
SETNE: 'SETNE';
SETNG: 'SETNG';
SETNGE: 'SETNGE';
SETNL: 'SETNL';
SETNLE: 'SETNLE';
SETNO: 'SETNO';
SETNP: 'SETNP';
SETNS: 'SETNS';
SETNZ: 'SETNZ';
SETO: 'SETO';
SETP: 'SETP';
SETPE: 'SETPE';
SETPO: 'SETPO';
SETS: 'SETS';
SETZ: 'SETZ';
SGDT: 'SGDT';
SHL: 'SHL';
SHLD: 'SHLD';
SHORT: 'SHORT';
SHR: 'SHR';
SHRD: 'SHRD';
SI: 'SI';
SIDT: 'SIDT';
SIGN_: 'SIGN?';
SIZE: 'SIZE';
SIZEOF: 'SIZEOF';
SIZESTR: 'SIZESTR';
SLDT: 'SLDT';
SMALL: 'SMALL';
SMSW: 'SMSW';
SP: 'SP';
SS: 'SS';
ST: 'ST';
STACK: 'STACK';
STC: 'STC';
STD: 'STD';
STDCALL: 'STDCALL';
STI: 'STI';
STOS: 'STOS';
STOSB: 'STOSB';
STOSD: 'STOSD';
STOSW: 'STOSW';
STR: 'STR';
STRUC: 'STRUC';
STRUCT: 'STRUCT';
SUB: 'SUB';
SUBSTR: 'SUBSTR';
SUBTITLE: 'SUBTITLE';
SUBTTL: 'SUBTTL';
SWORD: 'SWORD';
SYSCALL: 'SYSCALL';
TBYTE: 'TBYTE';
TEST: 'TEST';
TEXTEQU: 'TEXTEQU';
THIS: 'THIS';
TINY: 'TINY';
TITLE: 'TITLE';
TR3: 'TR3';
TR4: 'TR4';
TR5: 'TR5';
TR6: 'TR6';
TR7: 'TR7';
TRUE: 'TRUE';
TYPE: 'TYPE';
TYPEDEF: 'TYPEDEF';
UNION: 'UNION';
USE16: 'USE16';
USE32: 'USE32';
USES: 'USES';
VARARG: 'VARARG';
VERR: 'VERR';
VERW: 'VERW';
WAIT: 'WAIT';
WBINVD: 'WBINVD';
WHILE: 'WHILE';
WIDTH: 'WIDTH';
WORD: 'WORD';
XADD: 'XADD';
XCHG: 'XCHG';
XLAT: 'XLAT';
XLATB: 'XLATB';
XOR: 'XOR';
ZERO_: 'ZERO?';
U_186: '.186';
U_286: '.286';
U_286C: '.286C';
U_286P: '.286P';
U_287: '.287';
U_386: '.386';
U_386C: '.386C';
U_386P: '.386P';
U_387: '.387';
U_486: '.486';
U_486P: '.486P';
U_586: '.586';
U_586P: '.586P';
U_686: '.686';
U_686P: '.686P';
U_8086: '.8086';
U_8087: '.8087';
U_ALPHA: '.ALPHA';
U_BREAK: '.BREAK';
U_CODE: '.CODE';
U_CONST: '.CONST';
U_CONTINUE: '.CONTINUE';
U_CREF: '.CREF';
U_DATA: '.DATA';
U_DATA_: '.DATA?';
U_DOSSEG: '.DOSSEG';
U_ELSE: '.ELSE';
U_ELSEIF: '.ELSEIF';
U_ENDIF: '.ENDIF';
U_ENDW: '.ENDW';
U_ERR1: '.ERR1';
U_ERR2: '.ERR2';
U_ERR: '.ERR';
U_ERRB: '.ERRB';
U_ERRDEF: '.ERRDEF';
U_ERRDIF: '.ERRDIF';
U_ERRDIFI: '.ERRDIFI';
U_ERRE: '.ERRE';
U_ERRIDN: '.ERRIDN';
U_ERRIDNI: '.ERRIDNI';
U_ERRNB: '.ERRNB';
U_ERRNDEF: '.ERRNDEF';
U_ERRNZ: '.ERRNZ';
U_EXIT: '.EXIT';
U_FARDATA: '.FARDATA';
U_FARDATA_: '.FARDATA?';
U_IF: '.IF';
U_K3D: '.K3D';
U_LALL: '.LALL';
U_LFCOND: '.LFCOND';
U_LIST: '.LIST';
U_LISTALL: '.LISTALL';
U_LISTIF: '.LISTIF';
U_LISTMACRO: '.LISTMACRO';
U_LISTMACROALL: '.LISTMACROALL';
U_MMX: '.MMX';
U_MODEL: '.MODEL';
U_NO87: '.NO87';
U_NOCREF: '.NOCREF';
U_NOLIST: '.NOLIST';
U_NOLISTIF: '.NOLISTIF';
U_NOLISTMACRO: '.NOLISTMACRO';
U_OUT: '%OUT';
U_RADIX: '.RADIX';
U_REPEAT: '.REPEAT';
U_SALL: '.SALL';
U_SEQ: '.SEQ';
U_SFCOND: '.SFCOND';
U_STACK: '.STACK';
U_STARTUP: '.STARTUP';
U_TFCOND: '.TFCOND';
U_TYPE: '.TYPE';
U_UNTIL: '.UNTIL';
U_UNTILCXZ: '.UNTILCXZ';
U_VARARG: ':VARARG';
U_WHILE: '.WHILE';
U_XALL: '.XALL';
U_XCREF: '.XCREF';
U_XLIST: '.XLIST';
U_XMM: '.XMM';
U_DOT: '.';
C: 'C';
comment: ';' text linereminder;
WS: [\b\t\f ]+ -> skip;
radixoverride: 'H' | 'O' | 'Q' | 'T' | 'Y';
decdigit: '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';
hexdigit: 'A' | 'B' | 'C' | 'D' | 'E' | 'F';
letter : 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' | 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | '_';
id: letter+ | id decdigit;
WHITESPACECHARACTER: WS;
ENDOFLINE: [\r\n]+;
CHARACTER: [^\r\n];
STRINGCHAR: [']['] | '""' | [^'"];
DELIMITER: [^\t\n\rA-Za-z0-9 ];