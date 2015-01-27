/*
 * This is based on sample pixel filter from HDK docs
 */

#pragma once

#ifndef __VRAY_CvexFilter__
#define __VRAY_CvexFilter__

#include <VRAY/VRAY_PixelFilter.h>

class VRAY_Imager;
class VRAY_SampleBuffer;

namespace SKK_CvexFilter {

class VRAY_CvexFilter : public VRAY_PixelFilter {
public:
    VRAY_CvexFilter();
    virtual ~VRAY_CvexFilter();

    virtual VRAY_PixelFilter *clone() const;

    virtual void setArgs(int argc, const char *const argv[]);

    /// getFilterWidth is called after setArgs when Mantra needs to know
    /// how far to expand the render region.
    virtual void getFilterWidth(float &x, float &y) const;

    /// addNeededSpecialChannels is called after setArgs so that this filter
    /// can indicate that it depends on having special channels like z-depths
    /// or Op IDs.
    virtual void addNeededSpecialChannels(VRAY_Imager &imager);

    /// prepFilter is called after setArgs so that this filter can
    /// precompute data structures or values for use in filtering that
    /// depend on the number of samples per pixel in the x or y directions.
    virtual void prepFilter(int samplesperpixelx, int samplesperpixely);

    /// filter is called for each destination tile region with a source
    /// that is at least as large as is needed by this filter, based on
    /// the filter widths returned by getFilterWidth.
    virtual void filter(
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
        const VRAY_Imager &imager) const;

private:
    /// These must be saved in prepFilter.
    /// Each pixel has mySamplesPerPixelX*mySamplesPerPixelY samples.
    /// @{
    int mySamplesPerPixelX;
    int mySamplesPerPixelY;
    /// int myFilterWidth;
    //int myBucketsNumber;
    /// @}

};

} // End SKK_CvexFilter namespace

#endif
