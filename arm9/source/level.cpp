#include <nds.h>
#include "gl2d.h"
#include <maxmod9.h>

#include "graphics/fontHandler.h"
#include "graphics/graphics.h"

#include "date.h"

#include "sonicSprite.h"
#include "chaosZ1Floor.h"

static int runAnimFrame = 0;
static bool runAnimFrameDelay = true;
static int jumpAnimFrame = 0;
static bool jumpAnimFrameDelay = true;

static bool sonicJump = false;
static bool sonicJumpUp = true;
static int sonicJumpSpeed = 6;
static int sonicJumpDelay = 0;

static int sonicYpos = 128;
static int floorXpos = 0;

static int sonicTexID, floorTexID;
static glImage sonicImage[(512 / 32) * (64 / 32)];
static glImage floorImage[(32 / 32) * (32 / 32)];

//extern mm_sound_effect snd_jump;

void levelGraphicLoad(void) {
	sonicTexID = glLoadTileSet(sonicImage, // pointer to glImage array
							32, // sprite width
							32, // sprite height
							512, // bitmap width
							64, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_512, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeY for glTexImage2D() in videoGL.h
							TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							16, // Length of the palette to use (16 colors)
							(u16*) sonicSpritePal, // Load our 16 color tiles palette
							(u8*) sonicSpriteBitmap // image data generated by GRIT
							);

	floorTexID = glLoadTileSet(floorImage, // pointer to glImage array
							32, // sprite width
							32, // sprite height
							32, // bitmap width
							32, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_32, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_32, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF, // param for glTexImage2D() in videoGL.h
							16, // Length of the palette to use (16 colors)
							(u16*) chaosZ1FloorPal, // Load our 16 color tiles palette
							(u8*) chaosZ1FloorBitmap // image data generated by GRIT
							);
}

void levelMode(void) {
	if (!isBothScreens() || isRenderingTop()) {
		printSmall(true, 192, 64, RetTime().c_str());
	} else {
		printLarge(false, 0, 0, "SonicClock");
		printLarge(false, 0, 8, "Nothing on this screen yet.");
	}

	scanKeys();
	int pressed = keysDownRepeat();
	swiWaitForVBlank();
	
	if ((pressed & KEY_A) && !sonicJump) {
		//mmEffectEx(&snd_jump);
		sonicJump = true;
		sonicJumpUp = true;
	}

	if (!isBothScreens() || isRenderingTop()) {
		if (sonicJump) {
			if (!jumpAnimFrameDelay) {
				jumpAnimFrame++;
				if (jumpAnimFrame > 4) jumpAnimFrame = 0;
			}
			jumpAnimFrameDelay = !jumpAnimFrameDelay;

			if (sonicJumpUp) {
				sonicYpos -= sonicJumpSpeed;
				if (sonicJumpDelay == 0) {
					sonicJumpSpeed--;
					if (sonicJumpSpeed < 0) {
						sonicJumpUp = false;
						sonicJumpSpeed = 0;
					}
				}
			} else {
				sonicYpos += sonicJumpSpeed;
				if (sonicJumpDelay == 0) {
					sonicJumpSpeed++;
					if (sonicJumpSpeed > 6) {
						sonicJump = false;
						jumpAnimFrame = 0;
						sonicYpos = 128;
						sonicJumpSpeed = 6;
					}
				}
			}
			sonicJumpDelay++;
			if (sonicJumpDelay > 3) sonicJumpDelay = 0;
		} else {
			if (!runAnimFrameDelay) {
				runAnimFrame++;
				if (runAnimFrame > 3) runAnimFrame = 0;
			}
			runAnimFrameDelay = !runAnimFrameDelay;
		}

		floorXpos -= 4;
		if (floorXpos <= -31) floorXpos = 0;
	}
}

static int floorRenderCount = 0;

void levelGraphicDisplay(void) {
	glBoxFilled(0, 0, 256, 192, RGB15(0/8, 128/8, 224/8));
	if (sonicJump) {
		glSprite(34, sonicYpos, GL_FLIP_NONE, &sonicImage[24+jumpAnimFrame]);
	} else {
		glSprite(32, sonicYpos, GL_FLIP_NONE, &sonicImage[16+runAnimFrame]);
	}
	for (floorRenderCount = 0; floorRenderCount <= 256; floorRenderCount += 32) {
		glSprite(floorRenderCount+floorXpos, 160, GL_FLIP_NONE, floorImage);
	}
}
