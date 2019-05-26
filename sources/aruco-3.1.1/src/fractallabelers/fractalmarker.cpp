#include "fractalmarker.h"

#include <bitset>


namespace aruco
{
    FractalMarker::FractalMarker()
    {

    }

    FractalMarker::FractalMarker(int id, cv::Mat m, std::vector<cv::Point3f> corners, std::vector<int> id_submarkers)
    {
        this->id = id;
        this->_M = m;
        for(auto p:corners)
            push_back(p);

        _submarkers = id_submarkers;
        _mask = cv::Mat::ones(m.size(), CV_8UC1);
    }

    void FractalMarker::addSubFractalMarker(FractalMarker submarker)
    {
        float bitSize = (at(1).x - at(0).x) / int(mat().cols+2);
        float nsubBits = (submarker.at(1).x - submarker.at(0).x) / bitSize;

        int x_min = int(round(submarker[0].x / bitSize + mat().cols/2));
        int x_max = x_min + nsubBits;
        int y_min = int(round(-submarker[0].y / bitSize + mat().cols/2));
        int y_max = y_min + nsubBits;

        for(int y=y_min; y<y_max; y++){
            for(int x=x_min; x<x_max; x++){
                _mask.at<uchar>(y,x)=0;
            }
        }
    }

    std::vector<cv::Point3f> FractalMarker::getInnerCorners()
    {
        int nBitsSquared = int(sqrt(mat().total()));
        float bitSize =  getMarkerSize() / (nBitsSquared+2);

        //Add border bits
        cv::Mat markerBorder;
        copyMakeBorder(mat(), markerBorder, 1,1,1,1,cv::BORDER_CONSTANT,0);

        std::vector<cv::Point3f> innerCorners;
        for(int y=0; y< markerBorder.rows-1; y++)
        {
            for(int x=0; x< markerBorder.cols-1; x++)
            {

                if(     ((markerBorder.at<uchar>(y, x) == markerBorder.at<uchar>(y+1, x+1)) and
                         (markerBorder.at<uchar>(y, x) != markerBorder.at<uchar>(y, x+1) or
                         markerBorder.at<uchar>(y, x) != markerBorder.at<uchar>(y+1, x)))

                        or

                        ((markerBorder.at<uchar>(y, x+1) == markerBorder.at<uchar>(y+1, x)) and
                        (markerBorder.at<uchar>(y, x+1) != markerBorder.at<uchar>(y, x) or
                         markerBorder.at<uchar>(y, x+1) != markerBorder.at<uchar>(y+1, x+1)))
                    )
                    innerCorners.push_back(cv::Point3f(x-nBitsSquared/2.f, -(y-nBitsSquared/2.f), 0) * bitSize);
            }
        }
        return innerCorners;
    }
}

