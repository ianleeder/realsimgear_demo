/*
 Borrowing from examples:
 Reload Plugins - https://developer.x-plane.com/code-sample/reloadplugins/
 Reading DataRefs - https://developer.x-plane.com/code-sample/simdata/
 Timed Processing - https://developer.x-plane.com/code-sample/timedprocessing/
 
 API links:
 Data Refs - https://developer.x-plane.com/datarefs/
     sim/flightmodel/position/theta (pitch)
     sim/flightmodel/position/phi (roll)
 
 Information about plugin design, and resource consumption
 https://developer.x-plane.com/article/developing-plugins/#Guidelines_for_Plugin_Design
 
 */
#if IBM
#include <windows.h>
#endif
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "XPLMDataAccess.h"
#include "XPLMPlugin.h"
#include "XPLMMenus.h"
#include "XPLMProcessing.h"

static const float gDataRefreshRate = 0.5;
static const char* gSerialDevice = "/dev/tty.usbserial-143110";

static void ReloadPluginsMenuHandler(void * mRef, void * iRef);
static float MyFlightLoopCallback(float  inElapsedSinceLastCall,
                                  float  inElapsedTimeSinceLastFlightLoop,
                                  int    inCounter,
                                  void * inRefcon);
void OpenSerialPort();
void CloseSerialPort();

static XPLMDataRef gRoll = NULL;
static XPLMDataRef gPitch = NULL;

static XPLMMenuID gPluginMenuId = NULL;
static int gDebugMenuItemIndex = -1;
static int gSerialMenuItemIndex = -1;
static int gSerial = -1;

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
    gDebugMenuItemIndex = XPLMAppendMenuItem(gPluginMenuId, "Debug", (void *)"Debug",1);
    gSerialMenuItemIndex = XPLMAppendMenuItem(gPluginMenuId, "Close Serial", (void *)"Control Serial",1);
    
    // Disable my debug menu item
    XPLMEnableMenuItem(gPluginMenuId, gDebugMenuItemIndex, 0);
    
    gPitch = XPLMFindDataRef("sim/flightmodel/position/theta");
    gRoll = XPLMFindDataRef("sim/flightmodel/position/phi");
    
    // https://developer.x-plane.com/sdk/XPLMRegisterFlightLoopCallback/
    XPLMRegisterFlightLoopCallback(
            MyFlightLoopCallback,    /* Callback */
            gDataRefreshRate,        /* Interval */
            NULL);                   /* refcon not used. */
    
    OpenSerialPort();
    
    /* We must return 1 to indicate successful initialization, otherwise we
     * will not be called back again. */
    return 1;
}

void OpenSerialPort() {
    // Example opening serial port
    // https://stackoverflow.com/a/31508827/5329728
    // Documentation for OPEN command
    // https://pubs.opengroup.org/onlinepubs/009695399/functions/open.html
    
    //14330
    gSerial = open(gSerialDevice, O_WRONLY|O_NOCTTY|O_NDELAY);
        
    // https://man7.org/linux/man-pages/man3/termios.3.html
    struct termios options;
    
    tcgetattr(gSerial, &options);                   // Get the current options of the port
    bzero(&options, sizeof(options));               // Clear all the options
    cfsetispeed(&options, B9600);                 // Set the baud rate at 115200 bauds
    cfsetospeed(&options, B9600);
    
    // https://github.com/todbot/arduino-serial/blob/master/arduino-serial-lib.c
    // 8N1
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    // no flow control
    options.c_cflag &= ~CRTSCTS;
    
    options.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    options.c_oflag &= ~OPOST; // make raw
    
    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 0;
    
    tcsetattr(gSerial, TCSANOW, &options);          // Activate the settings
}

void CloseSerialPort() {
    close(gSerial);
    gSerial = -1;
}

PLUGIN_API void	XPluginStop(void)
{
    CloseSerialPort();
    gPitch = NULL;
    gRoll = NULL;
    gSerial = NULL;
}

PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) { }

void ReloadPluginsMenuHandler(void * mRef, void * iRef)
{
    if (!strcmp((char *) iRef, "Reload plugins"))
    {
        XPLMReloadPlugins();
    } else if (!strcmp((char *) iRef, "Control Serial")) {
        if(gSerial == -1) {
            OpenSerialPort();
            XPLMSetMenuItemName(gPluginMenuId, gSerialMenuItemIndex, "Close Serial", 0);
        } else {
            CloseSerialPort();
            XPLMSetMenuItemName(gPluginMenuId, gSerialMenuItemIndex, "Open Serial", 0);
        }
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
    float p = XPLMGetDataf(gPitch);
    float r = XPLMGetDataf(gRoll);
    
    char str[35];
    
    // Approx max 29 chars
    // Pitch = +179.9, Roll = -192.9
    sprintf(str, "Pitch = %+.1f, Roll = %+.1f", p, r);
    XPLMSetMenuItemName(gPluginMenuId, gDebugMenuItemIndex, str, 0);
    
    // Approx max 32 chars
    // {"pitch":+179.9, "roll":-192.9}\n
    sprintf(str, "{\"pitch\":%.1f, \"roll\":%.1f}\n", p, r);
    XPLMSetMenuItemName(gPluginMenuId, gDebugMenuItemIndex, str, 0);
    
    // Write to the serial port
    write(gSerial, str, strlen(str));
    tcflush(gSerial,TCIOFLUSH);
    
    /* Return value indicates when we want to be called again (in seconds). */
    return gDataRefreshRate;
}
