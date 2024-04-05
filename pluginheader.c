/* Objectheader

	Name:		extheader.c
	Main:		plugin demo
	Version:	$VER: extheader.c 1.1 (30.11.2003)
	Description:	C header for demo plugins
	Author:		SDI
	Distribution:	PD

 1.0   10.06.03 : first version
 1.1   30.11.03 : fixed for GCC
*/

#define __NOLIBBASE__
#include <proto/exec.h>
#include <proto/locale.h>

#include <tools/textedit/plugin.h>
#include <tools/textedit/filetypes.h>

/* To make this a extern Object module it is necessary to force this
structure to be the really first stuff in the file. */

extern UBYTE version[];
extern struct Plugin filetypePlugin;

#ifdef __VBCC__
static
#endif
const struct PluginHead Head =
{
  PLUGINHEAD_SECURITY,		/* ULONG 	       ph_Security */
  FILETYPEPLUGINHEAD_ID,	/* ULONG 	       ph_ID */
  0,						/* BPTR                ph_SegList */
  0,                    /* struct PluginHead * ph_Next */
  PLUGINHEAD_VERSION,   /* UWORD 	       ph_Version */
  0,                    /* UWORD 	       ph_Reserved */
  version,              /* STRPTR	       ph_VersString */
  &filetypePlugin          /* struct Plugin *     ph_FirstPlugin */
};

UBYTE version[] = "Version";

struct Library *SysBase;
struct Library *UtilityBase;
