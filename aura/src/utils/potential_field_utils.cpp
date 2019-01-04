/// ==============================================================================
/// \file potential_field_utils.cpp
/// \brief Provides some useful, generic functions
/// \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
/// ==============================================================================

/// INCLUDES

#include "aal_potential_field_navigation/utils/potential_field_utils.hpp"

/// LOCAL FUNCTIONS

/// INTERFACE FUNCTIONS

/// write_image_binary
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
void write_image_binary(cv::Mat &img, const std::string &name, int depth) {

    std::ofstream file(name, std::ios::out | std::ios::binary);

    file.write((char *) img.data, img.rows * img.cols * img.channels() * depth);

    file.close();

}

/// getAngleDiff
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
double getAngleDiff(double origin, double target) {

    // Get the angle between 0 .. 2 PI
    origin = fmod(origin + 2 * M_PI, 2 * M_PI);
    target = fmod(target + 2 * M_PI, 2 * M_PI);

    // Calculate the steering vector
    double angleDiff = target - origin;

    if (angleDiff > M_PI) {

        angleDiff = angleDiff - 2 * M_PI;

    } else if (angleDiff < -M_PI) {

        angleDiff = 2 * M_PI + angleDiff;

    }

    return angleDiff;

}

/// bgr_to_gray
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
cv::Mat bgr_to_gray(const cv::Mat &image) {

    cv::Mat gray(image.size(), CV_8UC1);

    for (int idx = 0; idx < image.cols * image.rows; ++idx) {

        gray.at<uchar>(idx) =
                (ushort(image.at<cv::Vec3b>(idx)[0]) +
                 ushort(image.at<cv::Vec3b>(idx)[1]) +
                 ushort(image.at<cv::Vec3b>(idx)[2])) / 3;

    }

    return gray;

}

/// rgb_to_gray
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
cv::Mat rgb_to_gray(const cv::Mat &image) {

    return bgr_to_gray(image);

}

/// get_abs_field
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
cv::Mat get_abs_field(cv::Mat &vectorfield) {

    cv::Mat absField(vectorfield.size(), CV_32FC1);

    for (int idx = 0; idx < vectorfield.rows * vectorfield.cols; ++idx) {

        const float x = vectorfield.at<cv::Vec2f>(idx)[0];

        const float y = vectorfield.at<cv::Vec2f>(idx)[1];

        absField.at<float>(idx) = sqrt(x * x + y * y);

    }

    return absField;

}

/// calc_normalized_field
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
void calc_normalized_field(cv::Mat &vectorfield) {

    float maxAbsValue = 0.0f;

    for (auto it = vectorfield.begin<cv::Vec2f>(); it != vectorfield.end<cv::Vec2f>(); ++it) {

        maxAbsValue = std::max((*it)[0] * (*it)[0] + (*it)[1] * (*it)[1], maxAbsValue);

    }

    maxAbsValue = sqrt(maxAbsValue);

    for (auto it = vectorfield.begin<cv::Vec2f>(); it != vectorfield.end<cv::Vec2f>(); ++it) {

        (*it)[0] /= maxAbsValue;

        (*it)[1] /= maxAbsValue;

    }

}

/// vectorfield_to_gray
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
cv::Mat vectorfield_to_gray(const cv::Mat &vectorfield) {

    cv::Mat gray(vectorfield.size(), CV_8UC1);
    cv::Mat vectorfieldCopy;
    vectorfield.copyTo(vectorfieldCopy);
    calc_normalized_field(vectorfieldCopy);
    cv::Mat vectorfieldAbs = get_abs_field(vectorfieldCopy);

    for (int idx = 0; idx < vectorfield.rows * vectorfield.cols; ++idx) {

        gray.at<uchar>(idx) = uchar(vectorfieldAbs.at<float>(idx) * 255.0f);

    }

    return gray;

}

/// vectorfield_to_hsv
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
cv::Mat vectorfield_to_hsv(const cv::Mat &vectorField, std::string id) {

    cv::Mat hsv(vectorField.size(), CV_8UC3);

    float l_max_square = 0.0;

    for (int y = 0; y < vectorField.rows; y++) {

        for (int x = 0; x < vectorField.cols; x++) {

            const cv::Point2f point(vectorField.at<cv::Vec2f>(y, x)[0], vectorField.at<cv::Vec2f>(y, x)[1]);

            const float l_square = point.x * point.x + point.y * point.y;

            l_max_square = std::max(l_square, l_max_square);

        }

    }

#pragma omp parallel for
    for (int y = 0; y < vectorField.rows; y++) {

        for (int x = 0; x < vectorField.cols; x++) {

            const cv::Point2f point(vectorField.at<cv::Vec2f>(y, x)[0], vectorField.at<cv::Vec2f>(y, x)[1]);
            double angle = atan2(point.y, point.x) / M_PI * 180;
            angle = fmod(angle + 360.0, 360.0);

            const float l = sqrt((point.x * point.x + point.y * point.y) / l_max_square);

            hsv.at<cv::Vec3b>(y, x)[0] = angle / 2; // H

            hsv.at<cv::Vec3b>(y, x)[1] = 255;       // S

            hsv.at<cv::Vec3b>(y, x)[2] = l * 255;   // V

        }

    }

    return hsv;

}

/// potentialfield_to_vectorfield
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
cv::Mat potentialfield_to_vectorfield(const cv::Mat &potentialField,
                              const int ddepth,
                              const double delta,
                              int borderType) {

    cv::Mat vectorFieldX(potentialField.rows, potentialField.cols, CV_32FC1, cv::Scalar(0.0f)),
            vectorFieldY(potentialField.rows, potentialField.cols, CV_32FC1, cv::Scalar(0.0f)),
            vectorField(potentialField.rows, potentialField.cols, CV_32FC2, cv::Scalar(0.0f));

    cv::Scharr(potentialField, vectorFieldX, ddepth, 1, 0, -1.0, delta, borderType); // multiply the resulting vectors by -1 because of gradient descent
    cv::Scharr(potentialField, vectorFieldY, ddepth, 0, 1, -1.0, delta, borderType); // multiply the resulting vectors by -1 because of gradient descent

    cv::Mat vectorFieldChannels[2] = {vectorFieldX, vectorFieldY};
    cv::merge(vectorFieldChannels, 2, vectorField);

    return vectorField; // x: [0], y: [1]

}

/// rot90
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
void rot90(cv::Mat &matImage, int rotflag) {

    //1=CW, 2=CCW, 3=180
    if (rotflag == 1) {

        transpose(matImage, matImage);
        flip(matImage, matImage, 1); // transpose + flip(1) = CW

    } else if (rotflag == 2) {

        transpose(matImage, matImage);
        flip(matImage, matImage, 0); // transpose + flip(0) = CCW

    } else if (rotflag == 3) {

        flip(matImage, matImage, -1); // flip(-1) = 180

    } else if (rotflag != 0) { // if not 0, 1, 2, 3:

        std::cerr << "Unknown rotation flag(" << rotflag << ")" << std::endl;

    }

}

/// pose2pixel
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
cv::Point2i pose2pixel(const geometry_msgs::Pose &pose_c, const int imageWidth_i, const int imageHeight_i,
                       const double meterPerPixelWidth_d, const double meterPerPixelHeight_d) {

    return cv::Point2i(
            (int) (pose_c.position.x / meterPerPixelWidth_d + imageWidth_i / 2.0),
            (int) (-1.0 * pose_c.position.y / meterPerPixelHeight_d + imageHeight_i / 2.0));

}

/// rot90Vector
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
void rot90Vector_v(cv::Vec2f &vector) {

    float xBuffer_f = vector[0];
    vector[0] = (float)(-1.0 * vector[1]); // x
    vector[1] = (float)(1.0 * xBuffer_f); // y

}

/// flipVectors_v
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
void flipVectors_v(cv::Mat &vectorField_mat) {

#pragma omp parallel for
    for (int idy_i = 0; idy_i < vectorField_mat.rows; idy_i++) {

        for (int idx_i = 0; idx_i < vectorField_mat.cols; idx_i++) {

            double abs_d = cv::norm(vectorField_mat.at<cv::Vec2f>(idy_i, idx_i));

            if (abs_d > 0.0) {

                float &y_f = vectorField_mat.at<cv::Vec2f>(idy_i, idx_i)[1];

                y_f = -1.0f * y_f;

            }

        }

    }

}

/// flipVector_v
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
void flipVector_v(cv::Point2f &vector_c) {

    float &y_f = vector_c.y;

    y_f = -1.0f * y_f;

}

/// rotVectors_v
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
void rotVectors_v(cv::Mat &vectorField_mat, int rotFlag) {

    float rotFactor11_f;
    float rotFactor12_f;
    float rotFactor21_f;
    float rotFactor22_f;

    if(rotFlag == RotateFlags::ROTATE_90_COUNTERCLOCKWISE) { // ccw

        rotFactor11_f = 0.0f;
        rotFactor12_f = 1.0f;
        rotFactor21_f = -1.0f;
        rotFactor22_f = 0.0f;

    } else if (rotFlag == RotateFlags::ROTATE_90_CLOCKWISE) { // cw

        rotFactor11_f = 0.0f;
        rotFactor12_f = -1.0f;
        rotFactor21_f = 1.0f;
        rotFactor22_f = 0.0f;

    } else if (rotFlag == RotateFlags::ROTATE_180) { // cw

        rotFactor11_f = -1.0f;
        rotFactor12_f = 0.0f;
        rotFactor21_f = 0.0f;
        rotFactor22_f = -1.0f;

    } else { // no rotation

        return;

    }

#pragma omp parallel for
    for (int idy_i = 0; idy_i < vectorField_mat.rows; idy_i++) {

        for (int idx_i = 0; idx_i < vectorField_mat.cols; idx_i++) {

            float abs_f = cv::norm(vectorField_mat.at<cv::Vec2f>(idy_i, idx_i));

            if (abs_f > 0.0) {

                float &x_f = vectorField_mat.at<cv::Vec2f>(idy_i, idx_i)[0];
                float &y_f = vectorField_mat.at<cv::Vec2f>(idy_i, idx_i)[1];

                float xBuffer_f = x_f;
                float yBuffer_f = y_f;

                x_f = rotFactor11_f * xBuffer_f + rotFactor12_f * yBuffer_f;
                y_f = rotFactor21_f * xBuffer_f + rotFactor22_f * yBuffer_f;

            }

        }

    }

}

/// neutralizeVectorsOnGrayPx_v
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
void neutralizeVectorsOnGrayPx_v(cv::Mat &vectorField_mat, cv::Mat* bgrImage_mat) {

    uchar valueB_uch;
    uchar valueR_uch;

#pragma omp parallel for
    for (int idy_i = 0; idy_i < vectorField_mat.rows; idy_i++) { // push all vectors on gray pixels to 0

        for (int idx_i = 0; idx_i < vectorField_mat.cols; idx_i++) {

            valueB_uch = bgrImage_mat[0].at<uchar>(idy_i, idx_i); // check the blue channel
            valueR_uch = bgrImage_mat[2].at<uchar>(idy_i, idx_i); // check the red channel

            if (valueB_uch > DYE_VALUE_HALF_i && valueR_uch > DYE_VALUE_HALF_i) { // case white

            } else { // every other case

                float &x_f = vectorField_mat.at<cv::Vec2f>(idy_i, idx_i)[0];
                float &y_f = vectorField_mat.at<cv::Vec2f>(idy_i, idx_i)[1];

                x_f = 0.0;
                y_f = 0.0;

            }

        }

    }

}

/// drawGradientOnImg_v
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
void drawGradientOnImg_v(cv::Mat vectorField_mat, cv::Mat& image_mat) {

    int flowRes_i = 5; // resolution

    for (int i = 0 ; i < vectorField_mat.rows ; i += flowRes_i){

        for (int j = 0 ; j < vectorField_mat.cols ; j += flowRes_i){

            cv::Point2f pStart_c(j, i); // start point

            double absGrad_d = cv::norm(vectorField_mat.at<cv::Vec2f>(i, j));

            if (absGrad_d > 0) {

                cv::Point2f pGradEnd_c(vectorField_mat.at<cv::Vec2f>(i,j)[0], vectorField_mat.at<cv::Vec2f>(i,j)[1]);

                cv::Point2f pEnd_c(pStart_c.x + pGradEnd_c.x, pStart_c.y + pGradEnd_c.y); // end point

                cv::line(image_mat, pStart_c, pEnd_c, cv::Scalar(0, DYE_VALUE_MAX_i, 0), 1, cv::LINE_4, 0); // print line

                image_mat.at<cv::Vec3b>(pStart_c) = BLACK_vec; // dye the start point

            } else {

                continue;

            }

        }

    }

}

/// transformVector_v
/// -----------------------------------------------------------------
/// additional information are given in the potential_field_utils.hpp
/// -----------------------------------------------------------------
void transformVector_v(cv::Point2f &vector_c) {

    // transform the vector from the image frame to the world frame
    // image frame:  –– x ––>
    //              |
    //              y
    //              |
    //              ∨
    //
    // world frame: ∧
    //              |
    //              y
    //              |
    //               –– x ––>
    flipVector_v(vector_c);

}