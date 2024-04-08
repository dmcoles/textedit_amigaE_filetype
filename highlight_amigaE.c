#define __NOLIBBASE__
#include <proto/exec.h>

#include <proto/alib.h>
#include <proto/locale.h>

#include <tools/textedit/filetypes.h>

#include <stdlib.h>

extern struct Library *SysBase;
extern struct Locale *Locale;
extern struct Catalog       *Catalog;

void ensureOpenLibs(void);

static void __ASM__ init(__REG__(a0, APTR userData));

void __ASM__ terminate(__REG__ (a0, struct Filetype *fType));
Object * __ASM__ settingsGadget(__REG__(a0, struct Filetype *fType), __REG__(d0, ULONG i));
STRPTR __ASM__ settingsTitle(__REG__(a0, struct Filetype *fType), __REG__(d0, ULONG i));
void __ASM__ applySettings(__REG__(a0, struct Filetype *fType));
void __ASM__ setSettingsFromGUI(__REG__(a0, struct Filetype *fType));
void __ASM__ setGUIFromSettings(__REG__(a0, struct Filetype *fType), __REG__(a1, struct Window *window));
void __ASM__ disposeGadgets(__REG__(a0, struct Filetype *fType));
void __ASM__ processGadgetUp(__REG__(a0, struct Filetype *fType), __REG__(d0, UWORD gadgetId), __REG__(a1, struct Window *window));
BOOL __ASM__ saveHighlightSettings(__REG__(a0, struct IFFHandle *iff), __REG__(a1, struct ContextNode *cn), __REG__(a2, APTR data));
BOOL __ASM__ loadHighlightSettings(__REG__(a0, struct IFFHandle *iff), __REG__(a1, struct ContextNode *cn), __REG__(a2, APTR data));

#define INT_VERSION    1
#define INT_REVISION   0

struct Plugin filetypePlugin =
{
	NULL,
	FILETYPEPLUGIN_VERSION,
	MASTER_VERSION,
	INT_VERSION,
	INT_REVISION,
	0,
	0,
	&init
};

#define HL_PLAIN 0
#define HL_COMMENT 1
#define HL_KEYWORD 2
#define HL_STRING 3
#define HL_TYPE 4
#define HL_CONSTANT 5
#define HL_SYSTEMVAR 6
#define HL_FUNCTION 7
#define HL_PREPROC 8

static UWORD defaultStyleCodes[9] =
{
		(0<<8)|0, // plain
		(3<<8)|TBSTYLE_SETCOLOR, // comment
		(1<<8)|TBSTYLE_SETCOLOR|TBSTYLE_BOLD, // keyword
		(2<<8)|TBSTYLE_SETCOLOR, // string
		(1<<8)|TBSTYLE_SETCOLOR|TBSTYLE_BOLD, // type
		(3<<8)|TBSTYLE_SETCOLOR|TBSTYLE_ITALIC, // constant
		(3<<8)|TBSTYLE_SETCOLOR|TBSTYLE_ITALIC, // system var
		(1<<8)|TBSTYLE_SETCOLOR|TBSTYLE_ITALIC, // function
		(1<<8)|TBSTYLE_SETCOLOR|TBSTYLE_BOLD // preprocessor
};

static CONST_STRPTR stylename[9];

static char *globalcommands[] = {
	"ALWAYS",
	"AND",
	"ANDALSO",
	"ARRAYSIZE",
	"ASM",
	"BUT",
	"CASE",
	"CATCH",
	"CHIP",
	"CODE",
	"CONST",
	"CONT",
	"CONTN",
	"DATA",
	"DEC",
	"DEF",
	"DEFAULT",
	"DIR",
	"DO",
	"ELSE",
	"ELSEIF",
	"ELSEIFN",
	"ELSEWHILE",
	"ELSEWHILEN",
	"END",
	"ENDFOR",
	"ENDIF",
	"ENDLOOP",
	"ENDOBJECT",
	"ENDPROC",
	"ENDSELECT",
	"ENDTRY",
	"ENDUNION",
	"ENDWHILE",
	"ENUM",
	"EXCEPT",
	"EXIT",
	"EXITN",
	"EXPORT",
	"EXTRA",
	"FAST",
	"FATAL",
	"FOR",
	"FPEXP",
	"HANDLE",
	"IF",
	"IFN",
	"INC",
	"INCBIN",
	"INLINE",
	"IS",
	"JUMP",
	"LARGE",
	"LEGACYINT",
	"LIBRARY",
	"LOOP",
	"MODULE",
	"NEW",
	"NOALIGN",
	"NOREGS",
	"NOSTARTUP",
	"NOT",
	"NOWARN",
	"OBJECT",
	"OF",
	"OFFSETOF",
	"OLD",
	"OPT",
	"OR",
	"ORELSE",
	"OSVERSION",
	"POOL",
	"PREPROCESS",
	"PRIVATE",
	"PROC",
	"PSIZEOF",
	"PUBLIC",
	"PURE",
	"RAISE",
	"REG",
	"REPEAT",
	"RETURN",
	"RUNBG",
	"SAFE",
	"SECTION",
	"SELECT",
	"SET",
	"SIZEOF",
	"STACK",
	"STEP",
	"STRMERGE",
	"SUPER",
	"THEN",
	"TO",
	"TRY",
	"UNION",
	"UNTIL",
	"UNTILN",
	"UTILLIB",
	"VOID",
	"WARN",
	"WHILE",
	"WHILEN",
	"WITH",
	NULL
};

static char *efunctions[] = {
	"_Cons",
	"Abs",
	"Alloc",
	"And",
	"AstpCopy",
	"AstrClone",
	"AstrCopy",
	"AstringF",
	"Bounds",
	"Box",
	"Byte",
	"Car",
	"Cdr",
	"Cell",
	"Char",
	"Chk",
	"CleanUp",
	"CloseS",
	"CloseW",
	"CoerceMethod",
	"CoerceMethodA",
	"Colour",
	"Compare",
	"CtrlC",
	"CtrlD",
	"CtrlE",
	"CtrlF",
	"Dispose",
	"DisposeLink",
	"Div",
	"DoMethod",
	"DoMethodA",
	"DoSuperMethod",
	"DoSuperMethodA",
	"EndsWith",
	"Eof",
	"Eor",
	"EstrLen",
	"Eval",
	"Even",
	"Exists",
	"Fabs",
	"Facos",
	"Fasin",
	"FastDispose",
	"FastDisposeList",
	"FastNew",
	"Fatan",
	"Fceil",
	"Fclose",
	"Fcompare",
	"Fcos",
	"Fcosh",
	"Fexp",
	"Ffieee",
	"Ffloor",
	"FileLength",
	"Flog",
	"Flog10",
	"Fopen",
	"ForAll",
	"Forward",
	"Fpow",
	"Free",
	"FreeCells",
	"FreeStack",
	"Fsin",
	"Fsincos",
	"Fsinh",
	"Fsqrt",
	"Ftan",
	"Ftanh",
	"Ftieee",
	"Gadget",
	"Get",
	"GetA4",
	"Gets",
	"Inp",
	"InStr",
	"InStri",
	"Int",
	"KickVersion",
	"LeftMouse",
	"Line",
	"Link",
	"List",
	"ListAdd",
	"ListAddItem",
	"ListClone",
	"ListCmp",
	"ListCopy",
	"ListInsItem",
	"ListItem",
	"ListLen",
	"ListMax",
	"ListRemItem",
	"ListSwapItem",
	"Long",
	"LowerChar",
	"LowerStr",
	"Lsl",
	"Lsr",
	"MapList",
	"Max",
	"MemFill",
	"MidStr",
	"Min",
	"Mod",
	"Mouse",
	"MouseX",
	"MouseY",
	"MsgCode",
	"MsgIaddr",
	"MsgQualifier",
	"Mul",
	"New",
	"NewM",
	"NewR",
	"Next",
	"Not",
	"Odd",
	"OpenS",
	"OpenW",
	"Or",
	"OstrCmp",
	"Out",
	"Plot",
	"PrintF",
	"PutByte",
	"PutChar",
	"PutF",
	"PutInt",
	"PutLong",
	"PutWord",
	"Raise",
	"ReadB",
	"ReadStr",
	"RealF",
	"RealVal",
	"ReThrow",
	"RightStr",
	"Rnd",
	"RndQ",
	"Rol",
	"Ror",
	"SelectFirst",
	"SelectList",
	"Set",
	"SetChunkSize",
	"SetColour",
	"SetList",
	"Sets",
	"SetStdIn",
	"SetStdOut",
	"SetStdRast",
	"SetStr",
	"SetTopaz",
	"Shl",
	"Shr",
	"Sign",
	"Size",
	"StrAdd",
	"StrAddChar",
	"StrClone",
	"StrCmp",
	"StrCompare",
	"StrCopy",
	"StriCmp",
	"String",
	"StringF",
	"StrIns",
	"StrLen",
	"StrMax",
	"StrRem",
	"TextF",
	"Throw",
	"TotalStack",
	"TrimStr",
	"Ucompare",
	"UpperChar",
	"UpperStr",
	"UsedStack",
	"Val",
	"WaitIMessage",
	"WaitLeftMouse",
	"Word",
	"WriteB",
	"WriteF",
	NULL
};
	
static char *econsts[] = {
	"ALL",
	"EMPTY",
	"FALSE",
	"GADGETSIZE",
	"NEWFILE",
	"NIL",
	"OFFSET_BEGINING",
	"OFFSET_BEGINNING",
	"OFFSET_CURRENT",
	"OFFSET_END",
	"OLDFILE",
	"READWRITE",
	"STRLEN",
	"TAG_DONE",
	"TAG_END",
	"TAG_IGNORE",
	"TAG_MORE",
	"TAG_SKIP",
	"TAG_USER",
	"TRUE",
	NULL
};

static char *esystemvars[] = {
	"__pool",
	"arg",
	"conout",
	"dosbase",
	"exception",
	"exceptioninfo",
	"execbase",
	"gfxbase",
	"intuitionbase",
	"stdin",
	"stdout",
	"stdrast",
	"thistask",
	"utilitybase",
	"wbmessage",
	NULL
};

static char *etypes[] = {
	"ARRAY OF",
	"BYTE",
	"CHAR",
	"INT",
	"LIST",
	"LONG",
	"PTR TO",
	"STRING",
	"WORD",
	NULL
};

static char *epreproc[] = {
	"#date",
	"#define",
	"#elifdef",
	"#elifndef",
	"#else",
	"#endif",
	"#ifdef",
	"#ifndef",
	"#undef",
	NULL
};

static void __ASM__ init(__REG__(a0, APTR userData))
{
	struct Hook *hook;
	struct Filetype *fType = (struct Filetype *)userData;
	int i;
	ULONG startstate,state1, state2,state3;

	struct HighlighterGUI *hlGUI;
	SysBase  = (*((struct Library **) 4));

	ensureOpenLibs();

	fType->typeName = "AmigaE";
	fType->autoFileTypes = "e";
	fType->terminate = terminate;
	fType->settingsTitle = settingsTitle;
	fType->disposeGadgets = disposeGadgets;
	fType->settingsGadget = settingsGadget;
	fType->processGadgetUp = processGadgetUp;
	fType->setSettingsFromGUI = setSettingsFromGUI;
	fType->setGUIFromSettings = setGUIFromSettings;
	fType->applySettings = applySettings;
	fType->saveSettings = saveHighlightSettings;
	fType->loadSettings = loadHighlightSettings;

	fType->name = "AmigaE";

	stylename[0] = "Comment";
	stylename[1] = "Keyword";
	stylename[2] = "String";
	stylename[3] = "Type";
	stylename[4] = "Constant";
	stylename[5] = "System Variable";
	stylename[6] = "Function";
	stylename[7] = "Preprocessor";
	stylename[8] = NULL;

	hlGUI = fType->createHighlightStylesGUI(stylename, defaultStyleCodes);
	fType->pluginData = (ULONG)hlGUI;
	fType->highlighterHook = hook = fType->createHighlighter(1750, TEHL_CASESENSITIVE/*|TEHL_SERIALDEBUG*/, hlGUI->inuseStylecodes);

	if (!hook)
		return;

	fType->createHighlightState(hook, 0, CHAR_NONWORD, HL_PLAIN);
	fType->createHighlightState(hook, TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_PLAIN);
	
	//multi-line comment
	startstate = fType->createHighlightKeyword(hook, "/*", HL_COMMENT, INITIALSTATE, FALSE);
	state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SURVIVESNEWLINE|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_COMMENT);
	state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SURVIVESNEWLINE|TEHL_STATE_SELFREPEATING, '*', HL_COMMENT);
	state3 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, '/', HL_COMMENT);
	fType->linkHighlightState(hook, startstate, state1); //link start to anychar
	fType->linkHighlightState(hook, startstate, state2); //link start to *
	fType->linkHighlightState(hook, state1, state2); //link anychar to *
	fType->linkHighlightState(hook, state2, state3); //link * to /
	fType->linkHighlightState(hook, state2, state1); //link * to anychar

	//e string
	startstate = fType->createHighlightKeyword(hook, "\'", HL_STRING, INITIALSTATE, FALSE);
	state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_STRING);
	state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 39, HL_STRING);
	fType->linkHighlightState(hook, startstate, state1);
	fType->linkHighlightState(hook, startstate, state2);
	fType->linkHighlightState(hook, state1, state2);

	//e char
	startstate = fType->createHighlightKeyword(hook, "\"", HL_STRING, INITIALSTATE, FALSE);
	state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_STRING);
	state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 34, HL_STRING);
	fType->linkHighlightState(hook, startstate, state1);
	fType->linkHighlightState(hook, state1, state2);

	// -> style comment
	startstate =fType->createHighlightKeyword(hook, "->", HL_COMMENT, INITIALSTATE, FALSE);
	state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_COMMENT);
	state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 10, HL_COMMENT);
	fType->linkHighlightState(hook, startstate, state1);
	fType->linkHighlightState(hook, startstate, state2);
	fType->linkHighlightState(hook, state1, state2);

	// slash slash style comment
	startstate = fType->createHighlightKeyword(hook, "//", HL_COMMENT, INITIALSTATE, FALSE);
	state1 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT|TEHL_STATE_SELFREPEATING, CHAR_ALWAYS, HL_COMMENT);
	state2 = fType->createHighlightState(hook, TEHL_STATE_APPLYFORMAT, 10, HL_COMMENT);
	fType->linkHighlightState(hook, startstate, state1);
	fType->linkHighlightState(hook, startstate, state2);
	fType->linkHighlightState(hook, state1, state2);

	/* Set up the global commands */
	i=0;
	while (globalcommands[i])
	{
		fType->createHighlightKeyword(hook, globalcommands[i++], HL_KEYWORD, INITIALSTATE,TRUE);
	}

	//Set up the e types
	i=0;
	while (etypes[i])
	{
		fType->createHighlightKeyword(hook, etypes[i++], HL_TYPE, INITIALSTATE, TRUE);
	}	

	//Set up the e functions
	i=0;
	while (efunctions[i])
	{
		fType->createHighlightKeyword(hook, efunctions[i++], HL_FUNCTION, INITIALSTATE, TRUE);
	}

	//Set up the e consts
	i=0;
	while (econsts[i])
	{	
		fType->createHighlightKeyword(hook, econsts[i++], HL_CONSTANT, INITIALSTATE, TRUE);
	}
	
	//Set up the e systemvars
	i=0;
	while (esystemvars[i])
	{
		fType->createHighlightKeyword(hook, esystemvars[i++], HL_SYSTEMVAR, INITIALSTATE, TRUE);
	}

	//Set up the preprocessors
	i=0;
	while (epreproc[i])
	{
		fType->createHighlightKeyword(hook, epreproc[i++], HL_PREPROC, INITIALSTATE, TRUE);
	}
	
	fType->linkHighlightState(hook, DEFAULTSTATE, INITIALSTATE);
	
}
