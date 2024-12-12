#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

Mat crop_code(Mat& qr_code_img)
{
    bool found = false;

    Point top_right;
    for (int row = 0; row < qr_code_img.rows; row++)
    {
        for (int col = qr_code_img.cols - 1; col > 0; col--)
        {
            int pixel_val = (int)qr_code_img.at<uchar>(row, col);
            if (pixel_val == 0)
            {
                top_right = Point(col, row);
                found = true;
                break;
            }
        }

        if (found)
        {
            break;
        }
    }

    found = false;
    Point bottom_left;
    for (int row = qr_code_img.rows - 1; row > 0; row--)
    {
        for (int col = 0; col < qr_code_img.cols - 1; col++)
        {
            int pixel_val = (int)qr_code_img.at<uchar>(Point(col, row));
            if (pixel_val == 0)
            {
                bottom_left = Point(col, row);
                found = true;
                break;
            }
        }

        if (found)
        {
            break;
        }
    }

    Mat crop = qr_code_img(Rect(bottom_left.x, top_right.y, top_right.x - bottom_left.x, bottom_left.y - top_right.y)); // Slicing to crop the imag
    return crop;
}

int main()
{
    // Reading image
    Mat img = imread("C:/Users/richa/Downloads/website_qr.png", IMREAD_GRAYSCALE);
    // Display original image
    imshow("original Image", img);
    waitKey(0);

    Mat new_img = crop_code(img);
    imshow("Cropped Image", new_img);
    waitKey(0);

    Mat resized_crop;
    resize(new_img, resized_crop, Size(29, 29), INTER_LINEAR);

    imshow("Resized Crop", resized_crop);
    int k = waitKey(0); // Wait for a keystroke in the window

    if (k == 's')
    {
        imwrite("adsf.png", resized_crop);
    }

    destroyAllWindows();
    return 0;
}