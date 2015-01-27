/*
 * Copyright (c) 2015
 *	Side Effects Software Inc.  All rights reserved.
 *
 * Redistribution and use of Houdini Development Kit samples in source and
 * binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. The name of Side Effects Software may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *----------------------------------------------------------------------------
 * This is a sample pixel filter to do edge detection
 */

#include <UT/UT_DSOVersion.h>

#include "VRAY_CvexFilter.h"
#include <VRAY/VRAY_SpecialChannel.h>
#include <UT/UT_Args.h>
#include <UT/UT_StackBuffer.h>
#include <SYS/SYS_Floor.h>
#include <SYS/SYS_Math.h>

#include <iostream>

using namespace SKK_CvexFilter;

VRAY_PixelFilter *
allocPixelFilter(const char *name)
{
    // NOTE: We could use name to distinguish between multiple pixel filters,
    //       in the same library, but we only have one.
    return new VRAY_CvexFilter();
}

VRAY_CvexFilter::VRAY_CvexFilter()
    : mySamplesPerPixelX(1) 
    , mySamplesPerPixelY(1)
{
}

VRAY_CvexFilter::~VRAY_CvexFilter()
{
}

VRAY_PixelFilter *
VRAY_CvexFilter::clone() const
{
    // In this case, all of our members can be default-copy-constructed,
    // so we don't need to write a copy constructor implementation.
    VRAY_CvexFilter *pf = new VRAY_CvexFilter(*this);
    return pf;
}

void
VRAY_CvexFilter::setArgs(int argc, const char *const argv[])
{
    UT_Args args;
    args.initialize(argc, argv);
    args.stripOptions("c:o:s:w:z:");
}

void
VRAY_CvexFilter::getFilterWidth(float &x, float &y) const
{
    // NOTE: You could add support for different x and y filter widths,
    //       which might be useful for non-square pixels.
    float filterwidth = 2.0;
    x = filterwidth;
    y = filterwidth;
}

void
VRAY_CvexFilter::addNeededSpecialChannels(VRAY_Imager &imager)
{
    // if (myUseOpID)
    //     addSpecialChannel(imager, VRAY_SPECIAL_OPID);
    // if (myUseZGradient)
    //     addSpecialChannel(imager, VRAY_SPECIAL_PZ);
}


void
VRAY_CvexFilter::prepFilter(int samplesperpixelx, int samplesperpixely)
{
    mySamplesPerPixelX = samplesperpixelx;
    mySamplesPerPixelY = samplesperpixely;
    // std::cout << "samples per pixel x,y: " << mySamplesPerPixelX << ", " << mySamplesPerPixelY << std::endl;

}
void
VRAY_CvexFilter::filter(
    float *destination,
    int vectorsize,
    const VRAY_SampleBuffer &source,
    int channel,
    int sourcewidth,
    int sourceheight,
    int destwidth,
    int destheight,
    int destxoffsetinsource,
    int destyoffsetinsource,
    const VRAY_Imager &imager) const
{
    const float *const cd = getSampleData(source, channel);

    // static int bucket = 0;
    // std::cout << "This is bucket: " << bucket++ << std::endl;
    // std::cout << "Source width: " << sourcewidth << std::endl;
    // std::cout << "Source height: " << sourceheight << std::endl;
    // std::cout << "Dest width: " << destwidth << std::endl;
    // std::cout << "Dest height: " << destheight << std::endl;
    // std::cout << "Samples: " << mySamplesPerPixelX << ", " << mySamplesPerPixelY << std::endl;
    // std::cout << "Vector size: " << vectorsize << std::endl;
    // std::cout << "channel: " << channel << std::endl;
    // std::cout << "destyoffsetinsource: " << destyoffsetinsource << std::endl;
    // std::cout << "destxoffsetinsource: " << destxoffsetinsource << std::endl;


    int lwidth  = mySamplesPerPixelY * sourcewidth * vectorsize;
    int swidth  = mySamplesPerPixelX * vectorsize;
    int reoffx  = destxoffsetinsource / mySamplesPerPixelX;
    int reoffy  = destyoffsetinsource / mySamplesPerPixelY;
    const float s = 4*0.3333333333333333 + 4*0.2612038749637414 + 1.0;
    const float a = 0.3333333333333333 / s;
    const float b = 0.2612038749637414 / s;
    const float c = 1.0 / s;

    for (int desty = 0; desty < destheight; ++desty)
    {
        for (int destx = 0; destx < destwidth; ++destx)
        {       
                int dxf = destx + reoffx;
                int dyf = desty + reoffy;

                int s1 = (dyf-1) * lwidth + (dxf-1) * swidth;
                int s2 = (dyf-1) * lwidth + dxf     * swidth;
                int s3 = (dyf-1) * lwidth + (dxf+1) * swidth;
                int s4 = dyf     * lwidth + (dxf-1) * swidth;
                int sx = dyf     * lwidth + dxf     * swidth;
                int s5 = dyf     * lwidth + (dxf+1) * swidth;
                int s6 = (dyf+1) * lwidth + (dxf-1) * swidth;
                int s7 = (dyf+1) * lwidth + dxf     * swidth;
                int s8 = (dyf+1) * lwidth + (dxf+1) * swidth;

            for (int i = 0; i < vectorsize; ++i, ++destination)
            {
                // float f = cd[s1+i] + cd[s2+i] + cd[s3+i] + cd[s4+i] + cd[sx+i] + \
                //           cd[s5+i] + cd[s6+i] + cd[s7+i] + cd[s8+i];
                //       f /= 9.0;
                float f = cd[s1+i]*b + cd[s2+i]*a + cd[s3+i]*b + cd[s4+i]*a  + cd[sx+i]*c +\
                          cd[s5+i]*a + cd[s6+i]*b + cd[s7+i]*a + cd[s8+i]*b;
                 *destination  =  f;  
            }    
             
        }
    }
}
