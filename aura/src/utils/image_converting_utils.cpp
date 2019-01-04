/// ==============================================================================
/// @file image_converting_utils.cpp
/// @brief Provides some useful functions for each schema backend node
/// @author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
/// @date 11.04.2018
/// ==============================================================================

/// INCLUDES

#include "aal_potential_field_navigation/utils/image_converting_utils.h"

/// NAMESPACES

using namespace std;

/// LOCAL FUNCTIONS

/// invertContrasts_v
/// \param src_mat
/// \param dst_mat
/// \brief Invert the contrasts like turn white of src to black in dst and vice versa
void invertContrasts_v(cv::Mat src_mat, cv::Mat dst_mat) {

#pragma omp parallel for
    for(int idy_i = 0; idy_i < src_mat.rows; ++idy_i) { // get a white map

        for (int idx_i = 0; idx_i < src_mat.cols; ++idx_i) {

            uchar &value_uch = src_mat.at<uchar>(idy_i,idx_i);

            if (value_uch != DYE_VALUE_MAX_i) {

                dst_mat.at<uchar>(idy_i,idx_i) = DYE_VALUE_MAX_i;

            }

        }

    }

}

/// closenessDetection_b
/// \param image_mat
/// \return true if closed else false
/// \brief Examine the hierarchical levels of the structure and determine if its a closed or opened structure
bool closenessDetection_b(cv::Mat image_mat) {

    bool closedContour_b = false; // necessary, because the contour check is not trivial
    std::vector< std::vector <cv::Point> > contours;
    std::vector< cv::Vec4i > hierarchy;

    findContours( image_mat, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

    for (int i = 0; i < hierarchy.size(); i = hierarchy[i][0]) {

        if ( hierarchy[i][FIRST_CHILD_CONTOUR_IDX_i] < 0 ) { //check if there is a child contour (magic line)

            closedContour_b = false;

        } else {

            closedContour_b = true;

        }

    }

    return closedContour_b;

}

/// cornerDetection_vec
/// \param image_mat
/// \return Vector of corner positions
/// \brief Locate all corners of a structure
std::vector<cv::Point> cornerDetection_vec(cv::Mat image_mat) {

    cv::Mat dst_mat, dst_norm, dst_norm_scaled;
    dst_mat = cv::Mat::zeros(image_mat.size(), CV_32FC1);
    std::vector<cv::Point> cornerList_vec;

    // detecting corners
    cv::cornerHarris(image_mat, dst_mat, BLOCK_SIZE_HARRIS_i, APERTURE_SIZE_HARRIS_i, HARRIS_PARAMETER_d, cv::BORDER_DEFAULT);

    // normalizing
    cv::normalize(dst_mat, dst_norm, 0, MAX_THRESHOLD_HARRIS_i, cv::NORM_MINMAX, CV_32FC1, cv::Mat());

    // store corner coordinates
    for (int idy_i = 0; idy_i < dst_norm.cols; idy_i++) { // caution: idx_i -> idy_i, idy_i -> idx_i

        for (int idx_i = 0; idx_i < dst_norm.rows; idx_i++) { // caution: idx_i -> idy_i, idy_i -> idx_i

            if ((int) dst_norm.at<float>(idx_i, idy_i) > THRESHOLD_HARRIS_i) { // caution: idx_i -> idy_i, idy_i -> idx_i

                cornerList_vec.emplace_back(cv::Point(idy_i, idx_i));

            }
        }
    }

//    cv::namedWindow( "dst_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "dst_mat", dst_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

    return cornerList_vec;

}

/// contourExtraction_v
/// \param src_mat
/// \param dst_mat
/// \brief Extract the contours of the src and copy them into the dst
void contourExtraction_v(cv::Mat src_mat, cv::Mat &dst_mat) {

    cv::Mat edgeMap_mat(src_mat.rows, src_mat.cols, CV_32FC2, cv::Scalar(0.0f));

    cv::blur(src_mat, dst_mat, cv::Size(KERNEL_SIZE_CANNY_i, KERNEL_SIZE_CANNY_i)); // reduce noise with a kernel

    cv::Canny(dst_mat, dst_mat, LOW_THRESHOLD_CANNY_i, LOW_THRESHOLD_CANNY_i * RATIO_CANNY_i, KERNEL_SIZE_CANNY_i); // canny detector

//    cv::namedWindow( "dst_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "dst_mat", dst_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

}

/// openContour_v
/// \param image_mat
/// \param cornerList_vec
/// \brief Open closed contours in dependency of the detected corners
void openContour_v(cv::Mat &image_mat, std::vector<cv::Point>cornerList_vec) {

    std::vector<cv::Point> processedPoints_vec;

    for(auto it1_ = cornerList_vec.begin(); it1_ < cornerList_vec.end(); it1_++) {

        cv::Point nearestNeighbour_pnt;
        cv::Point initialPoint_pnt;
        double minDist = std::numeric_limits<float>::max();

        if(processedPoints_vec.size() > 0) { // skip point if already processed

            if(std::find(processedPoints_vec.begin(), processedPoints_vec.end(), *it1_) != processedPoints_vec.end()) {

                continue;

            }

        }

        for (auto it2_ = cornerList_vec.begin(); it2_ < cornerList_vec.end(); it2_++) { // find the nearest corner

            if(*it1_ == *it2_) {

                continue;

            }

            double abs = sqrt((it1_->x - it2_->x) * (it1_->x - it2_->x) + (it1_->y -  it2_->y) * (it1_->y -  it2_->y));

            if(abs < minDist) {

                minDist = abs;
                nearestNeighbour_pnt = *it2_;

            }

        }

        initialPoint_pnt = *it1_;

        // store the processed point pair
        processedPoints_vec.emplace_back(nearestNeighbour_pnt);
        processedPoints_vec.emplace_back(initialPoint_pnt);

        cv::LineIterator lineIterator_c(image_mat, initialPoint_pnt, nearestNeighbour_pnt, LINE_THICKNESS_i);

        for(int i_i = 0; i_i < lineIterator_c.count; i_i++ , lineIterator_c++) { // move a kernel along the line

            cv::Point p1_pnt = cv::Point(lineIterator_c.pos().x - SIDE_LENGTH_i, lineIterator_c.pos().y - SIDE_LENGTH_i);
            cv::Point p2_pnt = cv::Point(lineIterator_c.pos().x + SIDE_LENGTH_i + 1, lineIterator_c.pos().y + SIDE_LENGTH_i + 1);
            cv::Mat kernel_mat = image_mat(cv::Rect(p1_pnt, p2_pnt));

            for(int idy_i = 0; idy_i < kernel_mat.rows; idy_i++) { // dye area

                for(int idx_i = 0; idx_i < kernel_mat.cols; idx_i++) {

                    kernel_mat.at<uchar>(idy_i, idx_i) = 0;

                }

            }

        }

    }

//    cv::namedWindow( "image_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "image_mat", image_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

}

/// contourClustering_mat
/// \param contourMap_mat
/// \return Number of labels
/// \brief Cluster the separate contours and label them
int contourClustering_mat(cv::Mat src_mat, cv::Mat &dst_mat) {

    return cv::connectedComponents(src_mat, dst_mat); // find connected components in the edgeMap_mat

}

/// redyeContour_v
/// \param dst_mat
/// \param whiteMap_mat
/// \param clusteredContourMap_mat
/// \brief Redye the separate clustered contours and copy them into the dst_mat
void redyeContour_v(cv::Mat &dst_mat, cv::Mat whiteMap_mat, cv::Mat clusteredContourMap_mat) {

#pragma omp parallel for
    for(int idy_i = 0; idy_i < dst_mat.rows; ++idy_i){ // dye the edges

        for(int idx_i = 0; idx_i < dst_mat.cols; ++idx_i) {

            int labelIdxEdgeMap_i = clusteredContourMap_mat.at<int>(idy_i, idx_i);

            cv::Vec3b &pixel_vec = dst_mat.at<cv::Vec3b>(idy_i, idx_i);

            if(labelIdxEdgeMap_i == OUTTER_LINE_IDX_CONNECTED_COMPONENT_i) { // leave the background (labelIdxEdgeMap_i == 0)

                if (INVERT_DIRECTION_b) {

                    pixel_vec = RED_vec; // ccw

                } else {

                    pixel_vec = BLUE_vec; // cw

                }

            } else if (labelIdxEdgeMap_i == INNER_LINE_IDX_CONNECTED_COMPONENT_i) {

                if (INVERT_DIRECTION_b) {

                    pixel_vec = BLUE_vec; // ccw

                } else {

                    pixel_vec = RED_vec; // cw

                }

            } else {

                if (whiteMap_mat.at<uchar>(idy_i,idx_i) == DYE_VALUE_MAX_i) { // burn the whiteMap_mat in

                    pixel_vec = WHITE_vec; // white

                }

            }

        }

    }

}

/// generateSmoothWhiteMap_mat
/// \param image_mat
/// \param kernelSizeDivX_i
/// \param kernelSizeDivY_i
/// \return Broaden white line
/// \brief Broaden the white line with a circular kernel
cv::Mat generateSmoothWhiteMap_mat(cv::Mat image_mat, int kernelSizeDivX_i, int kernelSizeDivY_i) {

    cv::Mat whiteMap_mat(image_mat.size(), image_mat.type(), cv::Scalar(0.0f, 0.0f)); // return image

    int kernelSizeX_i = image_mat.cols / kernelSizeDivX_i;
    int kernelSizeY_i = image_mat.rows / kernelSizeDivY_i;

#pragma omp parallel for
    for (int idy_i = 0; idy_i < whiteMap_mat.rows; ++idy_i) {

        for (int idx_i = 0; idx_i < whiteMap_mat.cols; ++idx_i) {

            uchar value_uch = image_mat.at<uchar>(idy_i, idx_i);

            if (value_uch == DYE_VALUE_MAX_i) { // case white

                int r_i = (kernelSizeX_i + kernelSizeY_i) / 2;

                if ((idy_i + r_i > whiteMap_mat.rows || idy_i - r_i < 0) || (idx_i + r_i > whiteMap_mat.cols || idx_i - r_i < 0)) { // check boundaries

                    cv::Point vertex1(idx_i - kernelSizeX_i / 2, idy_i - kernelSizeY_i / 2); // left top corner of the rectangle

                    cv::Point vertex2(idx_i + kernelSizeX_i / 2, idy_i + kernelSizeY_i / 2); // right bottom corner of the rectangle

                    cv::Rect rect_c(vertex1, vertex2);

                    rect_c = rect_c & cv::Rect(0, 0, whiteMap_mat.cols, whiteMap_mat.rows); // keep the rectangle in the ROI: cv::Rect(0, 0, whiteMap_mat.cols, whiteMap_mat.rows)


                } else {

                    cv::circle(whiteMap_mat, cv::Point(idx_i, idy_i), (kernelSizeX_i + kernelSizeY_i) / 2, (DYE_VALUE_MAX_i,DYE_VALUE_MAX_i,DYE_VALUE_MAX_i), cv::FILLED); // draw the kernel (circle) into the white map

                }

            }

        }

    }

//    cv::namedWindow( "whiteMap_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "whiteMap_mat", whiteMap_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

    return whiteMap_mat;

}

/// generateRoughWhiteMap_mat
/// \param image_mat
/// \param kernelSizeDivX_i
/// \param kernelSizeDivY_i
/// \return Broaden white line
/// \brief Broaden the white line with a rectangular kernel
cv::Mat generateRoughWhiteMap_mat(cv::Mat image_mat, int kernelSizeDivX_i, int kernelSizeDivY_i) {

    cv::Mat whiteMap_mat(image_mat.size(), image_mat.type(), cv::Scalar(0.0f, 0.0f)); // return image

    int kernelSizeX_i = image_mat.cols / kernelSizeDivX_i;
    int kernelSizeY_i = image_mat.rows / kernelSizeDivY_i;

#pragma omp parallel for
    for (int idy_i = 0; idy_i < whiteMap_mat.rows; ++idy_i) {

        for (int idx_i = 0; idx_i < whiteMap_mat.cols; ++idx_i) {

            uchar value_uch = image_mat.at<uchar>(idy_i, idx_i);

            if (value_uch == DYE_VALUE_MAX_i) { // case white

                cv::Point vertex1(idx_i - kernelSizeX_i / 2, idy_i - kernelSizeY_i / 2); // left top corner of the rectangle

                cv::Point vertex2(idx_i + kernelSizeX_i / 2, idy_i + kernelSizeY_i / 2); // right bottom corner of the rectangle

                cv::Rect rect_c(vertex1, vertex2);

                rect_c = rect_c & cv::Rect(0, 0, whiteMap_mat.cols, whiteMap_mat.rows); // keep the rectangle in the ROI: cv::Rect(0, 0, whiteMap_mat.cols, whiteMap_mat.rows)

                cv::rectangle(whiteMap_mat, rect_c, (DYE_VALUE_MAX_i,DYE_VALUE_MAX_i,DYE_VALUE_MAX_i), cv::FILLED); // draw the kernel (rectangle) into the white map

            }

        }

    }

//    cv::namedWindow( "whiteMap_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "whiteMap_mat", whiteMap_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

    return whiteMap_mat;

}

/// broadenLine_mat
/// \param image_mat
/// \param kernelSizeDivX_i
/// \param kernelSizeDivY_i
/// \param kernelShape_e
/// \return Broaden line image
/// \brief Broaden the all lines with a rectangular kernel
cv::Mat broadenLine_mat(cv::Mat image_mat, int kernelDilateX_i, int kernelDilateY_i, cv::MorphShapes kernelShape_e) {

    cv::Mat work_mat(image_mat.size(), image_mat.type(), cv::Scalar(0.0f, 0.0f)); // return image

    cv::dilate(image_mat, work_mat, cv::getStructuringElement(kernelShape_e, cv::Size(kernelDilateX_i, kernelDilateX_i)), DILATE_CENTER_POINT_c, DILATE_ITERATIONS_i,
               DILATE_BORDER_TYPE_i, cv::BORDER_DEFAULT);

//    cv::namedWindow( "work_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "work_mat", work_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

    return work_mat;

}

/// mergeAlignmentImages_v
/// \param dst_mat
/// \param imageBinary_mat
/// \param whiteMap_mat
/// \brief Convert the black contrast of the binary image to blue, merge it with the white map and copy the result into the dst
void mergeAlignmentImages_v(cv::Mat &dst_mat, cv::Mat imageBinary_mat, cv::Mat whiteMap_mat) {

#pragma omp parallel for
    for (int idy_i = 0; idy_i < dst_mat.rows; ++idy_i) {  // redye the image for charge

        for (int idx_i = 0; idx_i < dst_mat.cols; ++idx_i) {

            cv::Vec3b &pixel_vec = dst_mat.at<cv::Vec3b>(idy_i, idx_i);

            if (imageBinary_mat.at<uchar>(idy_i,idx_i) == DYE_VALUE_MAX_i) {

                pixel_vec = BLUE_vec;

            } else if (whiteMap_mat.at<uchar>(idy_i,idx_i) == DYE_VALUE_MAX_i) {

                pixel_vec = WHITE_vec;

            } else {

                pixel_vec = GRAY_vec;

            }

        }

    }

//    cv::namedWindow( "dst_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "dst_mat", dst_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

}

/// mergeDirectionImages_v
/// \param dst_mat
/// \param contourMap_mat
/// \param whiteMap_mat
/// \brief Convert the black contrast of the binary image to blue, merge it with the white map and copy the result into the dst
void mergeDirectionImages_v(cv::Mat &dst_mat, cv::Mat contourMap_mat, cv::Mat whiteMap_mat) {

#pragma omp parallel for
    for (int idy_i = 0; idy_i < dst_mat.rows; ++idy_i) {  // redye the image for direction

        for (int idx_i = 0; idx_i < dst_mat.cols; ++idx_i) {

            cv::Vec3b &pixel_vec = dst_mat.at<cv::Vec3b>(idy_i, idx_i);

            if (contourMap_mat.at<uchar>(idy_i, idx_i) == DYE_VALUE_MAX_i) {

                pixel_vec = BLACK_vec;

            } else if (whiteMap_mat.at<uchar>(idy_i, idx_i) == DYE_VALUE_MAX_i) {

                pixel_vec = WHITE_vec;

            } else {

                pixel_vec = GRAY_vec;

            }

        }

    }

}

/// subtractCenterLine_v
/// \param dst_mat
/// \brief Subtract the blue center line
void subtractCenterLine_v(cv::Mat &dst_mat) {

    cv::Mat invSharpend_mat;
    cv::GaussianBlur(dst_mat, invSharpend_mat, cv::Size(KERNEL_SIZE_GAUSS_BLUR_i, KERNEL_SIZE_GAUSS_BLUR_i), 0); // magic line

#pragma omp parallel for
    for (int idy_i = 0; idy_i < dst_mat.rows; ++idy_i) {  // redye the image for charge

        for (int idx_i = 0; idx_i < dst_mat.cols; ++idx_i) {

            cv::Vec3b &pixelOrig_vec = dst_mat.at<cv::Vec3b>(idy_i, idx_i);
            cv::Vec3b pixelBlur_vec = invSharpend_mat.at<cv::Vec3b>(idy_i, idx_i);

            if(pixelOrig_vec == BLUE_vec && pixelBlur_vec.val[2] > 0) { // magic condition

                pixelOrig_vec = WHITE_vec;

            }

        }

    }

}

/// clearContourGradients_v
/// \param dyedBGR_mat
/// \param vectorfield_mat
/// \param coloredPixel_vec
/// \brief Clear the gradients near the contours in the dyedBGR_mat
void clearContourGradients_v(cv::Mat dyedBGR_mat, cv::Mat &vectorField_mat, std::vector<cv::Point> coloredPixel_vec) {

    cv::Mat contourMap_mat(IMAGE_SIZE_sz, CV_32FC2, cv::Scalar(0.0f)); // contour image
    cv::Mat imageBinary_mat(IMAGE_SIZE_sz, CV_32FC2,  cv::Scalar(0.0f)); // work image
    cv::Mat dyedImageCopy_mat(IMAGE_SIZE_sz, CV_8UC3); // copy image

    dyedImageCopy_mat = dyedBGR_mat.clone();

    for (auto it = coloredPixel_vec.begin(); it != coloredPixel_vec.end(); ++it) {

        dyedImageCopy_mat.at<cv::Vec3b>(it->y, it->x) = WHITE_vec;

    }

    cv::cvtColor( dyedImageCopy_mat, imageBinary_mat, CV_BGR2GRAY );

    cv::Canny( imageBinary_mat, contourMap_mat, LOW_THRESHOLD_CANNY_i, LOW_THRESHOLD_CANNY_i * RATIO_CANNY_i, KERNEL_SIZE_CANNY_i ); // contour extraction

#pragma omp parallel for
    for(int idyC_i = 0; idyC_i < contourMap_mat.rows; ++idyC_i) { // get the edges

        for (int idxC_i = 0; idxC_i < contourMap_mat.cols; ++idxC_i) {

            uchar valueContour_uch = contourMap_mat.at<uchar>(idyC_i, idxC_i);

            if (valueContour_uch < DYE_VALUE_MAX_i) { // work only on the contours

                continue;

            }

            for (int idy_i = idyC_i - 2; idy_i <= idyC_i + 2; ++idy_i) { // get the cv::Scharr 3x3 kernel (4x4 to be sure) around a colored pixel

                for (int idx_i = idxC_i - 2; idx_i <= idxC_i + 2; ++idx_i) {

                    float &x_f = vectorField_mat.at<cv::Vec2f>(idy_i, idx_i)[0];
                    float &y_f = vectorField_mat.at<cv::Vec2f>(idy_i, idx_i)[1];

                    x_f = 0.0;
                    y_f = 0.0;

                }

            }

        }
    }

}

/// reduceCenterLine_v
/// \param imageBinary_mat
/// \param kernelErodeX_i
/// \param kernelErodeY_i
/// \brief Reduce all lines with an elliptical kernel
void reduceCenterLine_v(cv::Mat &imageBinary_mat, int kernelErodeX_i, int kernelErodeY_i) {

    cv::erode(imageBinary_mat, imageBinary_mat, cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, cv::Size(kernelErodeX_i, kernelErodeY_i)));

//    cv::namedWindow( "imageBinary_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "imageBinary_mat", imageBinary_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

}

/// INTERFACE FUNCTIONS

/// redyeDirection_mat
/// -----------------------------------------------------------------
/// additional information are given in the image_converting_utils.h
/// -----------------------------------------------------------------
cv::Mat redyeDirection_mat(cv::Mat rawImage_mat) {

    cv::Mat dyedBGR_mat(rawImage_mat.rows, rawImage_mat.cols, CV_8UC3, cv::Scalar(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i)); // return image
    cv::Mat imageBinary_mat(rawImage_mat.size(), rawImage_mat.type(), cv::Scalar(0.0f)); // work image
    cv::Mat contourMap_mat(imageBinary_mat.rows, imageBinary_mat.cols, CV_32FC2, cv::Scalar(0.0f)); // contour image
    std::vector<cv::Point> cornerList_vec;
    cv::Mat clusteredContourMap_mat;
    int clusteredContourLabels_i;

    invertContrasts_v(rawImage_mat, imageBinary_mat); // invert contrasts

//    imageBinary_mat = broadenLine_mat(imageBinary_mat, KERNEL_SIZE_DILATE_LD_X_i, KERNEL_SIZE_DILATE_LD_Y_i, cv::MorphShapes::MORPH_CROSS);
    imageBinary_mat = broadenLine_mat(imageBinary_mat, KERNEL_SIZE_DILATE_LD_X_i, KERNEL_SIZE_DILATE_LD_Y_i, cv::MorphShapes::MORPH_RECT);

    bool closedContour_b = closenessDetection_b(imageBinary_mat); // closeness detection

    if (!closedContour_b) {

        cornerList_vec = cornerDetection_vec(imageBinary_mat); // corner detection

    }

    contourExtraction_v(imageBinary_mat, contourMap_mat); // contour extraction

    if(!closedContour_b && !cornerList_vec.empty()) { // open the contour if it is not closed

        openContour_v(contourMap_mat, cornerList_vec);

    }

    clusteredContourLabels_i = contourClustering_mat(contourMap_mat, clusteredContourMap_mat); // find connected components in the edgeMap_mat

//    ROS_INFO("clusteredContourLabels_i = %i", clusteredContourLabels_i); // Debug only!

    redyeContour_v(dyedBGR_mat, imageBinary_mat, clusteredContourMap_mat); // redye the contours

//    cv::namedWindow( "dyedBGR_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "dyedBGR_mat", dyedBGR_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

    return dyedBGR_mat;

}

/// redyeAlignment_mat
/// -----------------------------------------------------------------
/// additional information are given in the image_converting_utils.h
/// -----------------------------------------------------------------
cv::Mat redyeAlignment_mat(cv::Mat rawImage_mat) {

    cv::Mat dyedBGR_mat(rawImage_mat.rows, rawImage_mat.cols, CV_8UC3, cv::Scalar(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i)); // return image
    cv::Mat imageBinary_mat(rawImage_mat.size(), rawImage_mat.type(), cv::Scalar(0.0f)); // work image
    cv::Mat whiteMap_mat(imageBinary_mat.size(), imageBinary_mat.type(), cv::Scalar(0.0f)); // white image

    invertContrasts_v(rawImage_mat, imageBinary_mat); // invert contrasts

    whiteMap_mat = broadenLine_mat(imageBinary_mat, KERNEL_SIZE_DILATE_WMA_X_i, KERNEL_SIZE_DILATE_WMA_Y_i, cv::MorphShapes::MORPH_ELLIPSE); // broaden the calculation area

    reduceCenterLine_v(imageBinary_mat, KERNEL_SIZE_ERODE_CL_X_i, KERNEL_SIZE_ERODE_CL_Y_i);

    mergeAlignmentImages_v(dyedBGR_mat, imageBinary_mat, whiteMap_mat); // merge the images

//    cv::namedWindow( "dyedBGR_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "dyedBGR_mat", dyedBGR_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

    return dyedBGR_mat;

}

/// imageToVectorfield_pmat
/// -----------------------------------------------------------------
/// additional information are given in the image_converting_utils.h
/// -----------------------------------------------------------------
std::pair<cv::Mat, cv::Mat> imageToVectorfield_pmat(cv::Mat dyedImage_mat, std::string imageType_st, bool applyHeuristics_b) {

    cv::Mat bgr_mat[3];
    cv::split(dyedImage_mat, bgr_mat);

    cv::Mat potentialFieldRed_mat(dyedImage_mat.size(), CV_32FC1, cv::Scalar(0.0f)),
            potentialFieldBlue_mat(dyedImage_mat.size(), CV_32FC1, cv::Scalar(0.0f));

    cv::Mat vectorFieldRed_mat(dyedImage_mat.size(), CV_32FC2, cv::Scalar(0.0f, 0.0f)),
            vectorFieldBlue_mat(dyedImage_mat.size(), CV_32FC2, cv::Scalar(0.0f, 0.0f));

    cv::Mat vectorField_mat(dyedImage_mat.size(), CV_32FC2, cv::Scalar(0.0f, 0.0f));

    // listing the red and blue pixels
    std::vector<cv::Point> redPixel_vec, bluePixel_vec, mvPixel_vec;
    for (int y_i = 0; y_i < dyedImage_mat.rows; y_i++) { // get a list of red and blue pixels

        for (int x_i = 0; x_i < dyedImage_mat.cols; x_i++) {

            if (bgr_mat[0].at<uchar>(y_i, x_i) > DYE_VALUE_HALF_i && bgr_mat[2].at<uchar>(y_i, x_i) < DYE_VALUE_HALF_i) { // get blue pixels

                bluePixel_vec.emplace_back(cv::Point(x_i, y_i));

            }

            if (bgr_mat[2].at<uchar>(y_i, x_i) > DYE_VALUE_HALF_i && bgr_mat[0].at<uchar>(y_i, x_i) < DYE_VALUE_HALF_i) { // get red pixels

                redPixel_vec.emplace_back(cv::Point(x_i, y_i));

            }

            if (bgr_mat[2].at<uchar>(y_i, x_i) > DYE_VALUE_HALF_i && bgr_mat[0].at<uchar>(y_i, x_i) > DYE_VALUE_HALF_i &&
                bgr_mat[1].at<uchar>(y_i, x_i) > DYE_VALUE_HALF_i) { // get white pixels

                mvPixel_vec.emplace_back(cv::Point(x_i, y_i));

            }

        }

    }

    // merge the red and blue pixel vectors
    std::vector<cv::Point> coloredPixel_vec = bluePixel_vec;
    coloredPixel_vec.insert(coloredPixel_vec.end(), redPixel_vec.begin(), redPixel_vec.end());

    // potentialfield calculation
    if (imageType_st == IMAGE_TYPE_CURRENT_st) {

#pragma omp parallel for
        for (auto itMv_a = mvPixel_vec.begin(); itMv_a < mvPixel_vec.end(); ++itMv_a) {

            int y_i = itMv_a->y;
            int x_i = itMv_a->x;

            double minDist_d = numeric_limits<double>::max();

            uchar value_uch = 0.0;

            for (auto it_a = bluePixel_vec.begin(); it_a < bluePixel_vec.end(); ++it_a) { // calculate normalized potential field for the blue channel

                if (it_a->y == y_i && it_a->x == x_i) {

                    continue;

                }

                double dist_d = sqrt((y_i - it_a->y) * (y_i - it_a->y) + (x_i - it_a->x) * (x_i - it_a->x));

                if (dist_d < minDist_d) {

                    minDist_d = dist_d;
                    value_uch = bgr_mat[0].at<uchar>(it_a->y, it_a->x);

                }

            }

            // attracting field
            potentialFieldBlue_mat.at<float>(y_i, x_i) = -1.0f * (value_uch / 255.0f) / minDist_d;

        }

#pragma omp parallel for
        for (auto itMv_a = mvPixel_vec.begin(); itMv_a < mvPixel_vec.end(); ++itMv_a) {

            int y_i = itMv_a->y;
            int x_i = itMv_a->x;

            double minDist_d = numeric_limits<double>::max();

            uchar value_uch = 0;

            for (auto it_a = redPixel_vec.begin(); it_a < redPixel_vec.end(); ++it_a) { // calculate normalized potential field for the red channel

                if (it_a->y == y_i && it_a->x == x_i) {

                    continue;

                }

                double dist_d = sqrt((y_i - it_a->y) * (y_i - it_a->y) + (x_i - it_a->x) * (x_i - it_a->x));

                if (dist_d < minDist_d) {

                    minDist_d = dist_d;
                    value_uch = bgr_mat[2].at<uchar>(it_a->y, it_a->x);

                }

            }

            // repelling field
            potentialFieldRed_mat.at<float>(y_i, x_i) = (value_uch / 255.0f) / minDist_d;

        }

    } else { // if (msg->header.frame_id.compare("charge") == 0)

#pragma omp parallel for
            for (auto itMv_a = mvPixel_vec.begin(); itMv_a < mvPixel_vec.end(); ++itMv_a) {

                int y_i = itMv_a->y;
                int x_i = itMv_a->x;

                double minDist_d = numeric_limits<double>::max();

                uchar value_uch = 0;

                for (auto it_a = bluePixel_vec.begin(); it_a < bluePixel_vec.end(); ++it_a) { // calculate normalized potential field for the blue channel

                    if (it_a->y == y_i && it_a->x == x_i) {

                        continue;

                    }

                    double dist_d = sqrt((y_i - it_a->y) * (y_i - it_a->y) + (x_i - it_a->x) * (x_i - it_a->x));

                    if (dist_d < minDist_d) {

                        minDist_d = dist_d;
                        value_uch = bgr_mat[0].at<uchar>(it_a->y, it_a->x);

                    }

                }

                // inverted attracting field
                potentialFieldBlue_mat.at<float>(y_i, x_i) = ALPHA_f * (value_uch / 255.0f) * pow(minDist_d,2); // linear


            }

    }

    // vectorfield calculation
    vectorFieldRed_mat = potentialfield_to_vectorfield(potentialFieldRed_mat);
    vectorFieldBlue_mat = potentialfield_to_vectorfield(potentialFieldBlue_mat);

    if (imageType_st == IMAGE_TYPE_CURRENT_st) { // postprocessing for currents, s.t. rotate around a charge with velocities which become weaker with closer distance to the current

        // rotate all vectors by 90 degree to get a tangential vectorfield
        //      ∧           |
        //     _|_         _∨_
        // <––| r |––> ––>| b |<––
        //     ‾|‾         ‾∧‾
        //      ∨           |
        //
        //     <––        ––>
        //     _ _  ∧  ∧  _ _
        // |  | r | |  | | b | |
        // ∨   ‾ ‾        ‾ ‾  ∨
        //     ––>        <––
        //
        // r: red pixel, b: blue pixel
        // –– >: vector
        rotVectors_v(vectorFieldBlue_mat, RotateFlags::ROTATE_90_COUNTERCLOCKWISE);
        rotVectors_v(vectorFieldRed_mat, RotateFlags::ROTATE_90_COUNTERCLOCKWISE);

        cv::add(vectorFieldRed_mat, vectorFieldBlue_mat, vectorField_mat);

#pragma omp parallel for
        for (int idy_i = 0; idy_i < vectorField_mat.rows; idy_i++) {

            for (int idx_i = 0; idx_i < vectorField_mat.cols; idx_i++) {

                float &x_f = vectorField_mat.at<cv::Vec2f>(idy_i, idx_i)[0];
                float &y_f = vectorField_mat.at<cv::Vec2f>(idy_i, idx_i)[1];
                float abs_f = cv::norm(vectorField_mat.at<cv::Vec2f>(idy_i, idx_i));

                // skip gray pixels
                if(abs_f == 0.0) {

                    continue;

                }

                // remove value_uch if on current
                if ((bgr_mat[0].at<uchar>(idy_i, idx_i) > 0 || bgr_mat[2].at<uchar>(idy_i, idx_i) > 0) && bgr_mat[1].at<uchar>(idy_i, idx_i) == 0) { // only red and blue pixels

                    x_f = 0.0f;
                    y_f = 0.0f;

                } else if (abs_f < HEURISTIC_ABS_MIN_CURRENT_f) { // keep vector which are far away constant

                    const float factor_f = HEURISTIC_FACTOR_CURRENT_f;
                    x_f = factor_f * x_f / abs_f;
                    y_f = factor_f * y_f / abs_f;


                } else {  // degenerate vector as closer they are to the charge

                    const float factor_f = HEURISTIC_FACTOR_CURRENT_f * HEURISTIC_ABS_MIN_CURRENT_f / abs_f;
                    x_f = factor_f * x_f / abs_f;
                    y_f = factor_f * y_f / abs_f;

                }

            }

        }

    } else { // postprocessing for charges

        for (auto it = bluePixel_vec.begin(); it != bluePixel_vec.end(); ++it) {

            // remove value_uch if on charge
            if (bgr_mat[0].at<uchar>(it->y, it->x) > 0 && bgr_mat[2].at<uchar>(it->y, it->x) == 0) { // only blue pixels

                float &x_f = vectorFieldBlue_mat.at<cv::Vec2f>(it->y, it->x)[0];
                float &y_f = vectorFieldBlue_mat.at<cv::Vec2f>(it->y, it->x)[1];

                x_f = 0.0f;
                y_f = 0.0f;

            }

        }

        cv::add(vectorFieldBlue_mat, vectorFieldRed_mat, vectorField_mat); // subsum the vector fields of the charges

#pragma omp parallel for
        for (int idy_i = 0; idy_i < vectorField_mat.rows; idy_i++) { // check the gradients

            for (int idx_i = 0; idx_i < vectorField_mat.cols; idx_i++) {

                float abs_f = cv::norm(vectorField_mat.at<cv::Vec2f>(idy_i, idx_i));

                // skip gray pixels
                if(abs_f == 0.0) {

                    continue;

                }

                float &x_f = vectorField_mat.at<cv::Vec2f>(idy_i, idx_i)[0];
                float &y_f = vectorField_mat.at<cv::Vec2f>(idy_i, idx_i)[1];

                if (abs_f > 1.5f * HEURISTIC_FACTOR_CURRENT_f) { // keep vector which are far away constant

                    const float factor_f = 1.5f * HEURISTIC_FACTOR_CURRENT_f;
                    x_f = factor_f * x_f / abs_f;
                    y_f = factor_f * y_f / abs_f;


                }

            }

        }

    }

    neutralizeVectorsOnGrayPx_v(vectorField_mat, bgr_mat); // push all vectors on gray pixels to 0

    clearContourGradients_v(dyedImage_mat, vectorField_mat, coloredPixel_vec); // get rid of the gradient spikes near the gray border

    if (vectorField_mat.rows > DESIRED_VECTORFIELD_WIDTH_i && vectorField_mat.cols > DESIRED_VECTORFIELD_HEIGTH_i) { // sanity check for vectorfield size

        if (vectorField_mat.rows != DESIRED_VECTORFIELD_HEIGTH_i || vectorField_mat.cols != DESIRED_VECTORFIELD_WIDTH_i) {

            ROS_INFO("[%s] Resize vectorField_mat", ros::this_node::getName().c_str());

            cv::resize(vectorField_mat, vectorField_mat, cv::Size(DESIRED_VECTORFIELD_WIDTH_i, DESIRED_VECTORFIELD_HEIGTH_i), 0, 0,
                       cv::INTER_LINEAR);

        }

    }

    drawGradientOnImg_v(vectorField_mat, dyedImage_mat); // print the vectorfield over the dyedImage_mat

//    cv::namedWindow( "image", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "image", dyedImage_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

    // prepare the return
    cv::Mat potentialField_mat = potentialFieldBlue_mat + potentialFieldRed_mat;

    return std::make_pair(vectorField_mat, potentialField_mat);

}

/// redyeDirectionExtended_mat
/// -----------------------------------------------------------------
/// additional information are given in the image_converting_utils.h
/// -----------------------------------------------------------------
cv::Mat redyeDirectionExtended_mat(cv::Mat rawImage_mat) {

    cv::Mat dyedBGR_mat(rawImage_mat.rows, rawImage_mat.cols, CV_8UC3, cv::Scalar(DYE_VALUE_HALF_i, DYE_VALUE_HALF_i, DYE_VALUE_HALF_i)); // return image
    cv::Mat imageBinary_mat(rawImage_mat.size(), rawImage_mat.type(), cv::Scalar(0.0f)); // work image
    cv::Mat contourMap_mat(imageBinary_mat.rows, imageBinary_mat.cols, CV_32FC2, cv::Scalar(0.0f)); // contour image
    cv::Mat whiteMap_mat(imageBinary_mat.size(), imageBinary_mat.type(), cv::Scalar(0.0f)); // white image
    std::vector<cv::Point> cornerList_vec;
    cv::Mat clusteredContourMap_mat;
    int clusteredContourLabels_i;

    invertContrasts_v(rawImage_mat, imageBinary_mat); // invert contrasts

    bool closedContour_b = closenessDetection_b(imageBinary_mat); // closeness detection

    if (!closedContour_b) {

        cornerList_vec = cornerDetection_vec(imageBinary_mat); // corner detection

    }

    whiteMap_mat = generateRoughWhiteMap_mat(imageBinary_mat, KERNEL_SIZE_DILATE_LD_X_i, KERNEL_SIZE_DILATE_LD_Y_i); // broaden the calculation area

    if (!closedContour_b) {

        cornerList_vec = cornerDetection_vec(whiteMap_mat); // corner detection

    }

    contourExtraction_v(whiteMap_mat, contourMap_mat); // contour extraction

    if(!closedContour_b && !cornerList_vec.empty()) { // open the contour if it is not closed

        openContour_v(contourMap_mat, cornerList_vec);

    }

    clusteredContourLabels_i = contourClustering_mat(contourMap_mat, clusteredContourMap_mat); // find connected components in the edgeMap_mat

    if (clusteredContourLabels_i != EXPECTED_LABEL_NUMBER_i) { // check if the number of detected labels equals the expected number

        ROS_WARN("[%s] redyeDirection_mat: Unexpected number (%i) of detected contours. Please check the input image.",
                 ros::this_node::getName().c_str(), clusteredContourLabels_i);

        return rawImage_mat; // return the input image

    }

    mergeDirectionImages_v(dyedBGR_mat, contourMap_mat, whiteMap_mat); // merge the images

    redyeContour_v(dyedBGR_mat, imageBinary_mat, clusteredContourMap_mat); // redye the contours

//    cv::namedWindow( "dyedBGR_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "dyedBGR_mat", dyedBGR_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

    return dyedBGR_mat;

}