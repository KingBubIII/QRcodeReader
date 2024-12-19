#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
using namespace cv;


Mat get_code_square(Mat& qr_code)
{
    bool found = false;
    int pixel_threshold = 127;

    Point top_right;
    for (int row = 0; row < qr_code.rows; row++)
    {
        for (int col = qr_code.cols - 1; col > 0; col--)
        {
            int pixel_val = (int)qr_code.at<uchar>(row, col);
            if (pixel_val <= pixel_threshold)
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
    for (int row = qr_code.rows - 1; row > 0; row--)
    {
        for (int col = 0; col < qr_code.cols - 1; col++)
        {
            int pixel_val = (int)qr_code.at<uchar>(Point(col, row));
            if (pixel_val <= pixel_threshold)
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

    //return std::pair<Point, Point>(top_right, bottom_left);
    Mat crop = qr_code(Rect(bottom_left.x, top_right.y, top_right.x - bottom_left.x, bottom_left.y - top_right.y)); // Slicing to crop the imag
    return crop;
}

int get_pixel_width(Mat& cropped_code)
{
    int black_pixel_value = 127;
    float finder_pattern_width = 0;

    for (int col = 0; col < cropped_code.cols-1; col++)
    {
        int pixel_val = (int)cropped_code.at<uchar>(Point(col, cropped_code.rows-1));

        if (pixel_val > black_pixel_value)
        {
            finder_pattern_width = col-1;
            break;
        }
    }

    return finder_pattern_width / 7;
}

std::vector<std::vector<bool>> get_bitified_code(Mat& cropped_code, int pixel_size)
{
    int black_pixel_threshold = 127;
    std::vector<std::vector<bool>> bitified_qr_code = {};

    for (int row = 0; row < cropped_code.rows; row++)
    {
        bitified_qr_code.push_back({});
        for (int col = 0; col < cropped_code.cols; col++)
        {
            Mat curr_pixel = cropped_code(Rect(col * pixel_size, row * pixel_size, pixel_size, pixel_size));
            Scalar asdf = mean(curr_pixel);
            int avg_pixel_val = asdf(0);

            bitified_qr_code[row].push_back(avg_pixel_val > black_pixel_threshold);
        }
    }

    return bitified_qr_code;
}

Mat reduce_code_to_single_pixels(Mat& qr_code_img)
{
    Mat cropped_code = get_code_square(qr_code_img);
    //imwrite("crop.png", cropped_code);

    float pixel_width = get_pixel_width(cropped_code);

    std::vector<std::vector<bool>> bitified_qr_code = get_bitified_code(cropped_code, pixel_width);

    float new_size = cropped_code.rows / pixel_width;

    Mat resized_crop;
    //resize(cropped_code, resized_crop, Size(new_size, new_size), INTER_LINEAR);

    return resized_crop;
}

int main()
{
    std::string path1 = "C:/Users/richa/Downloads/website_qr.png";
    std::string path2 = "C:/Users/richa/Pictures/Screenshots/qrcode_test1.png";

    // Reading image
    Mat img = imread(path2, IMREAD_GRAYSCALE);

    // Display original image
    imshow("original Image", img);
    waitKey(0);

    Mat new_img = reduce_code_to_single_pixels(img);
    imshow("Cropped Image", new_img);
    int k = waitKey(0); // Wait for a keystroke in the window

    if (k == 's')
    {
        imwrite("adsf.png", new_img);
    }

    destroyAllWindows();
    return 0;
}