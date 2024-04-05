# TextEditFiletypePlugins

Amiga project that aims to gather many filetype plugins (highlighters) for AmigaOS 3.2 TextEdit.

Anyone can contribute a new filetype like: E, Installer, AmigaGuide, LaTeX --
just ask. :-)


**How do the plugins work?**
----------------------------

When TextEdit starts up it searches the TextEditFiletypes drawer. Any file in here is loaded
as a plugin. A plugin file contains a pluginheader and any number of plugins which in turn
hold a pointer to an init() function

So a plugin file contains:
  Plugin header -> first plugin -> second plugin -> NULL
                        |                |
                      init()        init()


TextEdit traverses the plugins and calls the init() with a pointer to an empty Filetype
structure.

It is your responsiblity as filetype developer to implement the init() function:

- You should open libraries (the texteditor.gadget at the very least) and you should also add function pointers to do various things (see below).

- You should also fill in the names.

- And most importantly you should allocate a struct Hook, fill it in, and link to it in the 
`struct Filetype->highlighterHook`

- There are a lot of other function pointers you can fill in, but they are only relevant if you want to have settings gadgets in the settings dialog. Otherwise You can leave them NULL (remember TextEdit cleared the structure before handing it overto the plugin).

- Last important function to fill in is the terminate() function pointer, because this is where you will get the chance to free memory and close your libraries again.

**How does the highlighter hook work?**
--------------------------------------
Basically it will be called for each line, and if a user edits a line it will be called for that line over and over again while the user types.

The hook returns a status code that is given as input when the hook is called for the following line.

The hook will be called for the next line until the return value is not the same as last time the line was processed.

The incoming state value when the hook is called for the first line is alway zero. So you can use this state value to signal that a comment or string extends over more than one line.


**Build options**
---------------
It is important to build with data=far
not data=near and not data=faronly

"near" will make the compiler use a4 as base, which will mess up things
"faronly" will free a4 for other use by the compiler which will also mess up things

so make sure you build data=far. Also make sure that SAS/C points to the NDK 3.2 include_h and lib
directories, besites the standard assigns. 

IMPORTANT: There will be a bit of setup to do before you can use the NDK 3.2. with SAS/C. 


A smakefile is supplied in the example directory for use with sas/c

USAGE:

smake -f smakefile.debug (compiles and links everything)

smake -f smakefile.debug realclean (cleans everything in order to start from zero)


**How to test**
---------------
After you have built the exampleplugin simply copy it to sys:Tools/TextEditFiletypes and start TextEdit
