/*
 *  This is based on sample pixel filter from HDK docs
 */

#include <UT/UT_DSOVersion.h>

#include "VRAY_CvexFilter.h"
#include <VRAY/VRAY_SpecialChannel.h>
#include <UT/UT_Args.h>
#include <UT/UT_StackBuffer.h>
#include <SYS/SYS_Floor.h>
#include <SYS/SYS_Math.h>

#include <CVEX/CVEX_Context.h>
#include <UT/UT_Vector3.h>

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
    , myFilterWidth(2)
{
}

VRAY_CvexFilter::~VRAY_CvexFilter()
{
    delete cvex;
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
    args.stripOptions("s:w:");

    // // Filter width specified by the user:
    // if (args.found('w'))
    //     myFilterWidth = args.fargp('w');

    // if (args.found('s'))
    // {
    //     myVexCode = args.argp2('s');
    //     std::cout << args.argp2('s') << std::endl;
    // }
    // else
    //     myVexCode = "/home/symek/work/vexpixfilter/test.vex";

    // myArgv = args.argv();
    // myArgc = args.argc();
    // myArgv = argv;
    // myArgc = argc;

    myArgv = argv;
    myArgc = argc;
    // std::cout << myArgv[1] << ", ";
    // std::cout << myArgc -1 << std::endl;


}

void
VRAY_CvexFilter::getFilterWidth(float &x, float &y) const
{
    // NOTE: You could add support for different x and y filter widths,
    //       which might be useful for non-square pixels.
    
    x = myFilterWidth;
    y = myFilterWidth;
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
    // myVexCode = "/home/symek/work/vexpixfilter/test.vex";
    cvex = new CVEX_Context();
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

    // Pointer to source data:
    const float *const source_data = getSampleData(source, channel);
    const int source_size = sourcewidth*sourceheight*vectorsize*mySamplesPerPixelX*mySamplesPerPixelY;
    UT_Array<fpreal32> source_vector(source_size, source_size);
    for (int i = 0; i < source_size; ++i)
        source_vector[i] = source_data[i];
    //source_vector.aliasArray((float*)source_data);
    UT_Array<UT_Array<fpreal32> > source_array(1, 1);
    source_array[0] = source_vector;

    // Values to be send and recevied:
    CVEX_Value  *source_val, *destination_val;
 
    cvex->addInput("source",      CVEX_TYPE_FLOAT_ARRAY, true);        // Varying value
    cvex->addInput("destination", CVEX_TYPE_FLOAT_ARRAY, true);

    // Uniforms:
    cvex->addInput("vectorsize",  CVEX_TYPE_INTEGER, &vectorsize, 1); // Uniform value
    cvex->addInput("channel",     CVEX_TYPE_INTEGER, &channel, 1);
    cvex->addInput("sourcewidth", CVEX_TYPE_INTEGER, &sourcewidth, 1);
    cvex->addInput("sourceheight",CVEX_TYPE_INTEGER, &sourceheight, 1);
    cvex->addInput("destwidth",   CVEX_TYPE_INTEGER, &destwidth, 1);
    cvex->addInput("destheight",  CVEX_TYPE_INTEGER, &destheight, 1);
    cvex->addInput("destxoffsetinsource",  CVEX_TYPE_INTEGER, &destxoffsetinsource, 1);
    cvex->addInput("destyoffsetinsource",  CVEX_TYPE_INTEGER, &destyoffsetinsource, 1);

    //TODO: Can't parse argument atm:
    const char *myVexCode = "/home/symek/work/vexpixfilter/test.vex";

    if (!cvex->load(1, &myVexCode))
    {
        std::cout << "Can't load vex code " << myVexCode << std::endl;
        cvex->clear();
        return;
    }

    source_val = cvex->findInput("source", CVEX_TYPE_FLOAT_ARRAY);
    if (source_val)
        source_val->setTypedData(source_array);

    // destination_val = cvex->findOutput("destination", CVEX_TYPE_FLOAT);
    // if (destination_val)
    //     destination_val->setTypedData(destination, destwidth*destheight);


    cvex->run(1, false);
    cvex->clear();

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


    // int lwidth  = mySamplesPerPixelY * sourcewidth * vectorsize;
    // int swidth  = mySamplesPerPixelX * vectorsize;
    // int reoffx  = destxoffsetinsource / mySamplesPerPixelX;
    // int reoffy  = destyoffsetinsource / mySamplesPerPixelY;
    // const float s = 4*0.3333333333333333 + 4*0.2612038749637414 + 1.0;
    // const float a = 0.3333333333333333 / s;
    // const float b = 0.2612038749637414 / s;
    // const float c = 1.0 / s;


    // for (int desty = 0; desty < destheight; ++desty)
    // {
    //     for (int destx = 0; destx < destwidth; ++destx)
    //     {       
    //             int dxf = destx + reoffx;
    //             int dyf = desty + reoffy;

    //             int s1 = (dyf-1) * lwidth + (dxf-1) * swidth;
    //             int s2 = (dyf-1) * lwidth + dxf     * swidth;
    //             int s3 = (dyf-1) * lwidth + (dxf+1) * swidth;
    //             int s4 = dyf     * lwidth + (dxf-1) * swidth;
    //             int sx = dyf     * lwidth + dxf     * swidth;
    //             int s5 = dyf     * lwidth + (dxf+1) * swidth;
    //             int s6 = (dyf+1) * lwidth + (dxf-1) * swidth;
    //             int s7 = (dyf+1) * lwidth + dxf     * swidth;
    //             int s8 = (dyf+1) * lwidth + (dxf+1) * swidth;

    //         for (int i = 0; i < vectorsize; ++i, ++destination)
    //         {
    //             float f = cd[s1+i]*b + cd[s2+i]*a + cd[s3+i]*b + cd[s4+i]*a  + cd[sx+i]*c +\
    //                       cd[s5+i]*a + cd[s6+i]*b + cd[s7+i]*a + cd[s8+i]*b;
    //              *destination  =  f;  
    //         }    
             
    //     }
    // }
}
