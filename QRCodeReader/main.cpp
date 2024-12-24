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
    Mat crop = qr_code(Rect(bottom_left.x, top_right.y, top_right.x - bottom_left.x + 1, bottom_left.y - top_right.y + 1)); // Slicing to crop the imag
    return crop;
}

int get_pixel_width(Mat& cropped_code)
{
    int black_pixel_value = 127;
    int finder_pattern_width = 0;

    for (int col = 0; col < cropped_code.cols-1; col++)
    {
        int pixel_val = (int)cropped_code.at<uchar>(Point(col, cropped_code.rows-1));

        if (pixel_val > black_pixel_value)
        {
            finder_pattern_width = col;
            break;
        }
    }

    return finder_pattern_width / 7;
}

Mat get_bitified_code(Mat& cropped_code, int pixel_size)
{
    int black_pixel_threshold = 127;
    Mat bitified_qr_code = Mat(cropped_code.rows / pixel_size, cropped_code.cols / pixel_size, CV_8UC1);

    std::cout << bitified_qr_code.type() << std::endl;
    int row = 0;
    while (row <= cropped_code.rows - (pixel_size - 1))
    {
        int col = 0;
        while (col <= cropped_code.cols - (pixel_size-1))
        {
            Mat curr_pixel = cropped_code(Rect(col, row, pixel_size-1, pixel_size-1));
            Scalar asdf = mean(curr_pixel);
            int avg_pixel_val = asdf(0);

            int bit_row = row / pixel_size;
            int bit_col = col / pixel_size;
            bitified_qr_code.at<uchar>(bit_row, bit_col) = (avg_pixel_val > black_pixel_threshold) ? 255 : 0;

            col += pixel_size;
        }

        row += pixel_size;
    }

    return bitified_qr_code;
}

Mat reduce_code_to_single_pixels(Mat& qr_code_img)
{
    Mat cropped_code = get_code_square(qr_code_img);
    imwrite("crop.png", cropped_code);

    int pixel_width = get_pixel_width(cropped_code);

    Mat bitified_qr_code = get_bitified_code(cropped_code, pixel_width);

    return bitified_qr_code;
}

int main()
{
    std::string path1 = "C:/Users/richa/OneDrive/Documents/programming/QRcodeReader/frame.png";
    std::string path2 = "C:/Users/richa/Pictures/Screenshots/qrcode_test1.png";

    // Reading image
    Mat org_qr_code = imread(path1, IMREAD_GRAYSCALE);

    // Display original image
    imshow("original Image", org_qr_code);
    waitKey(0);

    Mat new_img = reduce_code_to_single_pixels(org_qr_code);
    
    imshow("Cropped Image", new_img);
    int k = waitKey(0); // Wait for a keystroke in the window

    if (k == 's')
    {
        imwrite("adsf.png", new_img);
    }

    destroyAllWindows();
    return 0;
}