/*
 Borrowing from examples:
 Reload Plugins - https://developer.x-plane.com/code-sample/reloadplugins/
 Reading DataRefs - https://developer.x-plane.com/code-sample/simdata/
 Timed Processing - https://developer.x-plane.com/code-sample/timedprocessing/
 
 API links:
 Data Refs - https://developer.x-plane.com/datarefs/
     sim/flightmodel/position/indicated_airspeed
     sim/flightmodel/position/elevation
 
 Information about plugin design, and resource consumption
 https://developer.x-plane.com/article/developing-plugins/#Guidelines_for_Plugin_Design
 
 */
#if IBM
#include <windows.h>
#endif
#include <string.h>
#include <stdio.h>

#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMMenus.h"
#include "XPLMProcessing.h"

static const float gDataRefreshRate = 0.5;

static void ReloadPluginsMenuHandler(void * mRef, void * iRef);
static float MyFlightLoopCallback(float  inElapsedSinceLastCall,
                                  float  inElapsedTimeSinceLastFlightLoop,
                                  int    inCounter,
                                  void * inRefcon);

static XPLMDataRef gElevation = NULL;
static XPLMDataRef gAirspeed = NULL;

static XPLMMenuID gPluginMenuId = NULL;
static int gDebugMenuItemIndex = -1;

PLUGIN_API int XPluginStart(
							char *		outName,
							char *		outSig,
							char *		outDesc)
{
	strcpy(outName, "IanDemo");
	strcpy(outSig, "ianleeder.examples.iandemo");
	strcpy(outDesc, "A demonstration of passing data between the game plugin and an Arduino compatible microcontroller.");
	
    int myPluginMenuIndex = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "IanDemo", NULL, 1);

    // Create the top level "MyPlugin" Menu
    gPluginMenuId = XPLMCreateMenu("IanDemo", XPLMFindPluginsMenu(), myPluginMenuIndex, ReloadPluginsMenuHandler, NULL);
    
    // Append sub-menus to my top level menu
    XPLMAppendMenuItem(gPluginMenuId, "Reload", (void *)"Reload plugins",1);
    gDebugMenuItemIndex = XPLMAppendMenuItem(gPluginMenuId, "Test2", (void *)"Reload plugins",1);
    
    // Disable my debug menu item
    XPLMEnableMenuItem(gPluginMenuId, gDebugMenuItemIndex, 0);
    
    gElevation = XPLMFindDataRef("sim/flightmodel/position/elevation");
    gAirspeed = XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed");
    
    // https://developer.x-plane.com/sdk/XPLMRegisterFlightLoopCallback/
    XPLMRegisterFlightLoopCallback(
            MyFlightLoopCallback,    /* Callback */
            gDataRefreshRate,        /* Interval */
            NULL);                   /* refcon not used. */
    
    /* We must return 1 to indicate successful initialization, otherwise we
     * will not be called back again. */
    return 1;
}

PLUGIN_API void	XPluginStop(void)
{
    gElevation = NULL;
    gAirspeed = NULL;
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

/*
 API on flight loop callback
 https://developer.x-plane.com/sdk/XPLMFlightLoop_f/
 */
float MyFlightLoopCallback(float  inElapsedSinceLastCall,
                           float  inElapsedTimeSinceLastFlightLoop,
                           int    inCounter,
                           void * inRefcon)
{
    float elapsed = XPLMGetElapsedTime();
    float speed = XPLMGetDataf(gAirspeed);
    float el = XPLMGetDataf(gElevation);
    
    /* Write the data to a file. */
    //fprintf(gOutputFile, "Time=%f, lat=%f,lon=%f,el=%f.\n",elapsed, lat, lon, el);
    
    /* Return value indicates when we want to be called again (in seconds). */
    return gDataRefreshRate;
}
