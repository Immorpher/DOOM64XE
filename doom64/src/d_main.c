/* D_main.c  */

#include "i_main.h"
#include "doomdef.h"
#include "p_spec.h"
#include "r_local.h"

int gamevbls;		            // 80063130 /* may not really be vbls in multiplayer */
int gametic;		            // 80063134
int ticsinframe;                // 80063138 /* how many tics since last drawer */
int ticon;			            // 8006313C
int lastticon;                  // 80063140
int vblsinframe[MAXPLAYERS];	// 80063144 /* range from 4 to 8 */
int ticbuttons[MAXPLAYERS];		// 80063148
int oldticbuttons[MAXPLAYERS];	// 8006314C

buttons_t   *BT_DATA[MAXPLAYERS];// 800A559C

extern boolean run_hectic_demo;

extern void ST_Init(void);

void D_DoomMain(void) // 800027C0
{
    int exit;

    I_Init();
    Z_Init();
    W_Init();
    R_Init();
    ST_Init();

    gamevbls = 0;
    gametic = 0;
    ticsinframe = 0;
    ticon = 0;
    ticbuttons[0] = 0;
    oldticbuttons[0] = 0;

    D_SplashScreen();

    while(true)
    {
        exit = D_TitleMap();

        if(exit != ga_exit)
        {
            exit = D_RunDemo("DEMO1LMP", sk_medium, 3);
            if(exit != ga_exit)
            {
                exit = D_RunDemo("DEMO2LMP", sk_medium, 9);
                if(exit != ga_exit)
                {
                    exit = D_RunDemo("DEMO3LMP", sk_medium, 17);
                    if(exit != ga_exit)
                    {
                        if(run_hectic_demo)
                        {
                            run_hectic_demo = false;
                            exit = D_RunDemo("DEMO4LMP", sk_medium, 32);
                        }

                        if(exit != ga_exit)
                        {
							continue;
                        }
                    }
                }
            }
        }

        do {
            exit = M_RunTitle();
        } while(exit != ga_timeout);
    }
}

/*
===============
=
= M_Random
=
= Returns a 0-255 number
=
===============
*/

unsigned const char rndtable[256] = { // 8005A190 - Random numbers used as a char to save memory
	0,   8, 109, 220, 222, 241, 149, 107,  75, 248, 254, 140,  16,  66 ,
	74,  21, 211,  47,  80, 242, 154,  27, 205, 128, 161,  89,  77,  36 ,
	95, 110,  85,  48, 212, 140, 211, 249,  22,  79, 200,  50,  28, 188 ,
	52, 140, 202, 120,  68, 145,  62,  70, 184, 190,  91, 197, 152, 224 ,
	149, 104,  25, 178, 252, 182, 202, 182, 141, 197,   4,  81, 181, 242 ,
	145,  42,  39, 227, 156, 198, 225, 193, 219,  93, 122, 175, 249,   0 ,
	175, 143,  70, 239,  46, 246, 163,  53, 163, 109, 168, 135,   2, 235 ,
	25,  92,  20, 145, 138,  77,  69, 166,  78, 176, 173, 212, 166, 113 ,
	94, 161,  41,  50, 239,  49, 111, 164,  70,  60,   2,  37, 171,  75 ,
	136, 156,  11,  56,  42, 146, 138, 229,  73, 146,  77,  61,  98, 196 ,
	135, 106,  63, 197, 195,  86,  96, 203, 113, 101, 170, 247, 181, 113 ,
	80, 250, 108,   7, 255, 237, 129, 226,  79, 107, 112, 166, 103, 241 ,
	24, 223, 239, 120, 198,  58,  60,  82, 128,   3, 184,  66, 143, 224 ,
	145, 224,  81, 206, 163,  45,  63,  90, 168, 114,  59,  33, 159,  95 ,
	28, 139, 123,  98, 125, 196,  15,  70, 194, 253,  54,  14, 109, 226 ,
	71,  17, 161,  93, 186,  87, 244, 138,  20,  52, 123, 251,  26,  36 ,
	17,  46,  52, 231, 232,  76,  31, 221,  84,  37, 216, 165, 212, 106 ,
	197, 242,  98,  43,  39, 175, 254, 145, 190,  84, 118, 222, 187, 136 ,
	120, 163, 236, 249
};

unsigned char lightcurve[256] = { // [Immorpher] - table to optionally boost brightness
	0, 1, 3, 4, 6, 7, 9, 11, 13, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
	34, 36, 38, 40, 42, 45, 47, 49, 51, 53, 55, 58, 60, 62, 64, 66, 69,
	71, 73, 75, 77, 80, 82, 84, 86, 89, 91, 93, 95, 97, 100, 102, 104,
	106, 109, 111, 113, 115, 117, 120, 122, 124, 126, 128, 130, 133, 135,
	137, 139, 141, 143, 145, 148, 150, 152, 154, 156, 158, 160, 162, 164,
	166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 188, 190, 192,
	194, 195, 197, 199, 201, 202, 202, 203, 204, 205, 205, 206, 207, 208,
	208, 209, 210, 210, 211, 212, 212, 213, 214, 214, 215, 216, 216, 217,
	218, 218, 219, 219, 220, 221, 221, 222, 222, 223, 223, 224, 224, 225,
	226, 226, 227, 227, 228, 228, 229, 229, 230, 230, 231, 231, 231, 232,
	232, 233, 233, 234, 234, 235, 235, 235, 236, 236, 237, 237, 237, 238,
	238, 239, 239, 239, 240, 240, 240, 241, 241, 241, 242, 242, 242, 243,
	243, 243, 244, 244, 244, 245, 245, 245, 245, 246, 246, 246, 247, 247,
	247, 247, 248, 248, 248, 248, 249, 249, 249, 249, 249, 250, 250, 250,
	250, 250, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252, 252,
	253, 253, 253, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254,
	254, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255
};

const unsigned char lightmax[256] = { // [Immorpher] - table for maximum light curve that is a quarter of a circle
	0, 23, 32, 39, 45, 50, 55, 59, 63, 67, 71, 74, 77, 80, 83, 86, 89, 92,
	94, 97, 99, 101, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124,
	125, 127, 129, 131, 132, 134, 136, 137, 139, 140, 142, 143, 145, 146,
	148, 149, 150, 152, 153, 154, 156, 157, 158, 159, 161, 162, 163, 164,
	165, 167, 168, 169, 170, 171, 172, 173, 174, 175, 177, 178, 179, 180,
	181, 182, 183, 184, 185, 185, 186, 187, 188, 189, 190, 191, 192, 193,
	194, 194, 195, 196, 197, 198, 199, 199, 200, 201, 202, 202, 203, 204,
	205, 205, 206, 207, 208, 208, 209, 210, 210, 211, 212, 212, 213, 214,
	214, 215, 216, 216, 217, 218, 218, 219, 219, 220, 221, 221, 222, 222,
	223, 223, 224, 224, 225, 226, 226, 227, 227, 228, 228, 229, 229, 230,
	230, 231, 231, 231, 232, 232, 233, 233, 234, 234, 235, 235, 235, 236,
	236, 237, 237, 237, 238, 238, 239, 239, 239, 240, 240, 240, 241, 241,
	241, 242, 242, 242, 243, 243, 243, 244, 244, 244, 245, 245, 245, 245,
	246, 246, 246, 247, 247, 247, 247, 248, 248, 248, 248, 249, 249, 249,
	249, 249, 250, 250, 250, 250, 250, 251, 251, 251, 251, 251, 252, 252,
	252, 252, 252, 252, 252, 253, 253, 253, 253, 253, 253, 253, 253, 254,
	254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

int	rndindex = 0;   // 8005A18C
int prndindex = 0;  // 8005A188

int P_Random(void) // 80002928
{
	prndindex = (prndindex + 1) & 0xff;
	return rndtable[prndindex];
}

int M_Random(void) // 80002954
{
	rndindex = (rndindex + 1) & 0xff;
	return rndtable[rndindex];
}

void M_ClearRandom(void) // 80002980
{
	rndindex = prndindex = 0;
}

/*
===============
=
= MiniLoop
=
===============
*/

int MiniLoop(void(*start)(void), void(*stop)(int), int(*ticker)(void), void(*drawer)(void)) // 80002998
{
	int		exit;
	int		buttons;

	gameaction = ga_nothing;
	gamevbls = 0;
	gametic = 0;
	ticon = 0;
	ticsinframe = 0;

	/* */
	/* setup (cache graphics, etc) */
	/* */
	if(start != NULL)
        start();

	drawsync1 = 0;
	drawsync2 = vsync;

	while (true)
	{
		vblsinframe[0] = drawsync1;

		// get buttons for next tic
		oldticbuttons[0] = ticbuttons[0];

		buttons = I_GetControllerData();
		ticbuttons[0] = buttons;

		//Read demos
		if (demoplayback)
        {
            if (demoplayback)
            {
                if (buttons & (ALL_JPAD|ALL_BUTTONS))
                {
                    exit = ga_exit;
                    break;
                }

                buttons = *demobuffer++;
                ticbuttons[0] = buttons;
            }

            if ((buttons & PAD_START) || ((((int)demobuffer - (int)demo_p) >> 2) >= 4000))
            {
                exit = ga_exitdemo;
                break;
            }
        }

		ticon += vblsinframe[0];
		if (ticsinframe < (ticon >> 1))
		{
			gametic += 1;
			ticsinframe = (ticon >> 1);
		}

        if (disabledrawing == false)
        {
            exit = ticker();
            if (exit != ga_nothing)
                break;

            drawer();
        }

		gamevbls = gametic;
	}

	I_GetScreenGrab();

	if(stop != NULL)
        stop(exit);

	oldticbuttons[0] = ticbuttons[0];

	return exit;
}
