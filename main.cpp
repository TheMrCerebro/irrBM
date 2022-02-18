/*
 *
 * Copyright (c) 2022 TheMrCerebro
 *
 * irrBM - Zlib license.
 *
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held
 * liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented;
 * you must not claim that you wrote the original software.
 * If you use this software in a product, an acknowledgment
 * in the product documentation would be appreciated but
 * is not required.
 *
 * 2. Altered source versions must be plainly marked as such,
 * and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any
 * source distribution.
 *
*/

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace video;

#include "FakeGLSL.h"

// Variables globales
vec2 iResolution;
f32 iTime = 0.0f;

#include "sample0.h"
#include "sample1.h"
#include "sample2.h"
#include "sample3.h"
#include "sample4.h"
#include "sample5.h"
#include "sample6.h"
#include "sample7.h"
#include "sample8.h"
#include "sample9.h"

bool info=true;
class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
        {
            if(event.KeyInput.Key == KEY_F1 && event.KeyInput.PressedDown == false)
                info = info ? false : true;
        }
        return false;
	}
};

class ShaderCallBack : public video::IShaderConstantSetCallBack
{
public:

    virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
	{
		services->setPixelShaderConstant("iTime", &iTime, 1);
		services->setPixelShaderConstant("iResolution", &iResolution.v[0], 2);
	}
};

int main(int argc, char** argv)
{
    u32 frames = 0;
    u32 curFPS = 0, minFPS = 0, maxFPS = 0;
    u32 curRAM = 0, minRAM = 0, maxRAM = 0;
    //
    const c8 *sample[] = { "Twist", "Tunnel", "Sun", "Mobius", "Spheres", "Synthwave", "Crater", "Psychedelic", "Stars", "Fork" };

    u32 rWidth[] = { 0, 640, 800, 1024, 1280, 1920, 3840 };
    u32 rHeight[] = { 0, 480, 600, 768, 720, 1080, 2160 };

    printf( "============================\n"
            "irrBM v1.0 - by TheMrCerebro\n"
            "============================\n\n" );

    printf( "This tool has been created for the purpose of comparing how the same code\n"
            "used for GLSL can be executed in C++. There are some examples that are very\n"
            "complex and consume a lot of resources, use with caution!\n\n" );

	printf( "Driver:\n"
    "   (1) - CPU\n"
    "   (2) - GPU\n" );
    u32 drv;
    scanf("%u",&drv);

	printf( "Resolution:\n"
    " (1) - 640x480\n"
    " (2) - 800x600\n"
    " (3) - 1024x768\n"
    " (4) - 1280x720 HD\n"
    " (5) - 1920x1080 Full HD\n"
    " (6) - 3840x2160 Ultra HD\n" );
    u32 resolution;
    scanf("%u",&resolution);

	printf( "Fullscreen:\n"
    " (y) - Yes\n"
    " (n) - No\n" );
    c8 fullscreen;
    scanf("%s",&fullscreen);

	printf( "VSync:\n"
    " (y) - Yes\n"
    " (n) - No\n" );
    c8 vsync;
    scanf("%s",&vsync);

	printf( "Sample:\n"
	" (0) - Twist\n"
    " (1) - Tunnel\n"
    " (2) - Sun\n"
    " (3) - Mobius\n"
    " (4) - Spheres\n"
    " (5) - Synthwave\n"
    " (6) - Crater\n"
    " (7) - Psychedelic\n"
	" (8) - Stars\n"
	" (9) - Fork\n" );
    u32 opt;
    scanf("%u",&opt);

//==================================
// Irrlicht config

    system("cls");
    MyEventReceiver receiver;

    IrrlichtDevice *device = createDevice(drv==1 ? video::EDT_SOFTWARE : video::EDT_OPENGL,
                                          core::dimension2du(rWidth[resolution],rHeight[resolution]),
                                          32,
                                          fullscreen == 'y' ? true : false,
                                          false,
                                          vsync == 'y' ? true : false,
                                          &receiver);

    device->setWindowCaption(L"Rendering...");

//==================================
// OS section

    irr::IOSOperator* opr = device->getOSOperator();

//==================================
// Video section

    video::IVideoDriver* driver = device->getVideoDriver();

    u32 sWidth = driver->getScreenSize().Width;
    u32 sHeight = driver->getScreenSize().Height;

    iResolution = vec2(sWidth,sHeight);

//==================================
// GPU section

    video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();

    core::stringc fname = "shaders/sample"; fname += opt; fname += ".frag";
    s32 shader = gpu->addHighLevelShaderMaterialFromFiles("shaders/basic.vert", "main", EVST_VS_2_0, fname, "main", EPST_PS_2_0, new ShaderCallBack());

//==================================
// GUI

    gui::IGUIFont* font = device->getGUIEnvironment()->getBuiltInFont();

//==================================
// Scene section

    irr::ITimer* tmr = device->getTimer();

    while(device->run())
    {
        driver->beginScene();

        if(drv==1)
        {
            for(u32 i=0; i<(sWidth*sHeight); i++)
            {
                u32 x = i % sWidth;
                u32 y = i / sWidth;

                vec3 out;

                switch(opt)
                {
                case 0: out = sample0::test(x,sHeight-y); break;
                case 1: out = sample1::test(x,sHeight-y); break;
                case 2: out = sample2::test(x,sHeight-y); break;
                case 3: out = sample3::test(x,sHeight-y); break;
                case 4: out = sample4::test(x,sHeight-y); break;
                case 5: out = sample5::test(x,sHeight-y); break;
                case 6: out = sample6::test(x,sHeight-y); break;
                case 7: out = sample7::test(x,sHeight-y); break;
                case 8: out = sample8::test(x,sHeight-y); break;
                case 9: out = sample9::test(x,sHeight-y); break;
                }

                out = clamp(out, 0.0f, 1.0f);

                driver->drawPixel(x, y, video::SColorf(out.v[0], out.v[1], out.v[2], 1.0f).toSColor());
            }
        }
        else
        {
            video::SMaterial Material;
            Material.MaterialType=(video::E_MATERIAL_TYPE)shader;
            driver->setMaterial(Material);

            video::S3DVertex Vertices[6];
            Vertices[0] = video::S3DVertex(-1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, video::SColor(0x0), 0.0f, 1.0f);
            Vertices[1] = video::S3DVertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, video::SColor(0x0), 0.0f, 0.0f);
            Vertices[2] = video::S3DVertex( 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, video::SColor(0x0), 1.0f, 0.0f);
            Vertices[3] = video::S3DVertex( 1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, video::SColor(0x0), 1.0f, 1.0f);
            Vertices[4] = video::S3DVertex(-1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, video::SColor(0x0), 0.0f, 1.0f);
            Vertices[5] = video::S3DVertex( 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, video::SColor(0x0), 1.0f, 0.0f);

            irr::u16 indices[6] = {0, 1, 2, 3, 4, 5};
            driver->drawIndexedTriangleList(&Vertices[0], 6, &indices[0], 2);
        }

        ++frames;

        //====================================
        // Draw GUI

        u32 progress = tmr->getTime()*(((sWidth-8.f)/(60.f*1000.f))*100.f);//1.32f;//((792.f/(60.f*1000.f))*100.f);//( (res. width / millisecs) * 100)

        if(info)
        {
            font->draw(L"F1 - Toggle Help", core::recti(sWidth-120, 2, sWidth-120, 2), video::SColor(255, 255, 255, 255));

        //====================================
        // UI

            driver->draw2DRectangle(video::SColor(128,0,0,0), core::recti(2,2,384,50+16));

            core::stringw sInfo = "irrBM v1.0 - Burn-in test, "; sInfo += sWidth; sInfo += "x"; sInfo += sHeight;
            font->draw(sInfo, core::recti(4,2,4,2), video::SColor(255,255,255,0));

        //====================================
        // FPS

            core::stringw sFPS = "Frames: ";
            sFPS += frames;
            sFPS += " - FPS: ";
            sFPS += curFPS;
            sFPS += "  (min: ";
            sFPS += minFPS;
            sFPS += "  max: ";
            sFPS += maxFPS;
            sFPS += ")";
            font->draw(sFPS,core::recti(4,16,4,16),video::SColor(255,255,255,255));

        //====================================
        // RAM

            u32 totalM;
            opr->getSystemMemory(&totalM, &curRAM);
            core::stringw sRAM = "RAM: "; sRAM += "(total: "; sRAM += totalM; sRAM += "kB aval: "; sRAM += curRAM; sRAM += "kB)";
            font->draw(sRAM,core::recti(4,30,4,30),video::SColor(255,255,255,255));

        //====================================
        // Driver

            core::stringw sDriver = driver->getName();
            font->draw(sDriver,core::recti(4,30+16,4,30+16),video::SColor(255,0,255,0));

        //====================================
        // Progress bar

            driver->draw2DRectangle(video::SColor(128,0,0,0), core::recti(2,sHeight-28,sWidth-2,sHeight-2));

            driver->draw2DRectangle(video::SColor(128,0,255,0), core::recti(4, sHeight-26, (progress/100), sHeight-4));

            core::stringw sProgress = "Progress "; sProgress += (progress/(sWidth-8)); sProgress += " %";
            font->draw(sProgress, core::recti(sWidth/2,sHeight-16,sWidth/2,sHeight-16), video::SColor(255,255,255,255),true,true);
        }

        if((progress/(sWidth-8)) >= 100)//792)==100)//1 minuto termina
            device->closeDevice();

        // End scene
        driver->endScene();

        curFPS = driver->getFPS();

        if(tmr->getTime()<3000)
        {
            minFPS = maxFPS = curFPS;
            minRAM = maxRAM = curRAM;
        }

        if(curFPS < minFPS) minFPS = curFPS;
        if(curFPS > maxFPS) maxFPS = curFPS;

        if(curRAM < minRAM) minRAM = curRAM;
        if(curRAM > maxRAM) maxRAM = curRAM;

        iTime += 0.01f;
    }

    device->drop();

    printf("\n");
    printf("DRIVER: %s\n", (drv==1 ? "Software" : "OpenGL"));
    printf("SCENE: %s\n", sample[opt]);
    printf("RESOLUTION: %ix%i\n", sWidth, sHeight);
    printf("FULLSCREEN: %s\n", (fullscreen=='y' ? "Yes" : "No"));
    printf("VSYNC: %s\n", (vsync=='y' ? "Yes" : "No"));
    printf("FPS: Min:%i, Max:%i, Avr:%i\n", minFPS, maxFPS, curFPS);
    printf("RAM: Min:%ikB, Max:%ikB, Avr:%ikB\n", minRAM, maxRAM, curRAM);
    printf("\n");

    system("pause");
    return 0;
}
