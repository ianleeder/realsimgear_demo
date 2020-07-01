/*
 Borrowing from examples:
 Reload Plugins - https://developer.x-plane.com/code-sample/reloadplugins/
 Reading DataRefs - https://developer.x-plane.com/code-sample/simdata/
 
 API links:
 Data Refs - https://developer.x-plane.com/datarefs/
     sim/flightmodel/position/indicated_airspeed
     sim/flightmodel/position/elevation
 */
#if IBM
#include <windows.h>
#endif
#include <string.h>
#include <stdio.h>

#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMMenus.h"


static void ReloadPluginsMenuHandler(void * mRef, void * iRef);

static XPLMDataRef  gDataRefElevation = NULL;
static XPLMDataRef  gDataRefAirspeed = NULL;

PLUGIN_API int XPluginStart(
							char *		outName,
							char *		outSig,
							char *		outDesc)
{
    XPLMMenuID id;
    int item;
    
	strcpy(outName, "IanDemo");
	strcpy(outSig, "ianleeder.examples.iandemo");
	strcpy(outDesc, "A demonstration of passing data between the game plugin and an Arduino compatible microcontroller.");
	
    item = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "IanDemo", NULL, 1);

    id = XPLMCreateMenu("IanDemo", XPLMFindPluginsMenu(), item, ReloadPluginsMenuHandler, NULL);
    XPLMAppendMenuItem(id, "Reload", (void *)"Reload plugins",1);

    /* We must return 1 to indicate successful initialization, otherwise we
     * will not be called back again. */
     
    return 1;
}

PLUGIN_API void	XPluginStop(void)
{
    gDataRefElevation = NULL;
    gDataRefAirspeed = NULL;
}

PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) { }

void ReloadPluginsMenuHandler(void * mRef, void * iRef)
{
    if (!strcmp((char *) iRef, "Reload plugins"))
    {
        XPLMReloadPlugins();
    }
}
