#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include<reorder.hpp>
using namespace cv;
using namespace std;

class Scan
{
public:
	Mat OriginalImg, GrayImg, CannyImg, ThreImg, BlurImg, DilImg, ErodImg, WarpImg, CropImg;
	vector<Point> initialPoint, docPoints;
	int maxArea = 0;
	float w = 420, h = 596;

	void start(string path)
	{
		OriginalImg = imread(path);
		resize(OriginalImg, OriginalImg, Size(360, 740));
		//if we find the edges of an image we aould easily analyze the shape of it
		ThreImg = preprocessing(OriginalImg);
		initialPoint = getContours(ThreImg);
		//Random numbering will be given to each edges so we have to reorder them before drawing the contours
		docPoints = reord(initialPoint);
		drawPoints(docPoints);

		WarpImg = getWarp(OriginalImg, docPoints, w, h);
		Rect roi(0, 0, w, h);

		CropImg = WarpImg(roi);
		display();
	}

	Mat preprocessing(Mat img)
	{
		//To detect an edge we first find the canny of an image
		//We apply guassian blur to reduce the noise of an image
		//The canny image edges will be very thin and might go undetected so we dilate the canny to accurately detect the edges
		//cvtColor(OriginalImg, GrayImg, COLOR_BGR2GRAY);
		GaussianBlur(OriginalImg, BlurImg, Size(3, 3), 3, 0);
		Canny(BlurImg, CannyImg, 25, 75);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
		dilate(CannyImg, DilImg, kernel);
		return DilImg;
	}

	vector<Point> getContours(Mat image)
	{
		// Vector to store the contours found in the image
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		// Vector to store the approximated polygons
		vector<vector<Point>> conPoly(contours.size());
		// Vector to store the four corner points of the largest contour
		vector<Point> biggest;

		// Iterate through all the contours found in the image
		for (int i = 0; i < contours.size(); i++)
		{
			// Calculate the area of the current contour
			int area = contourArea(contours[i]);

			// If the area is greater than a threshold (1000), proceed

			if (area > 1000)
			{
				// Approximate the current contour with a polygon

             approxPolyDP(contours[i], conPoly[i], 375, true);
			 // Check if the area of the polygon is greater than the maximum area 
			 // and if the polygon has four vertices (a quadrilateral)

				if (area > maxArea && conPoly[i].size() == 4)
				{
					// Update the 'biggest' vector with the four corner points of the current polygon
					biggest = { conPoly[i][0],conPoly[i][1],conPoly[i][2],conPoly[i][3] };
					maxArea = area;
				}
			}
		}
		return biggest;
	}

	void drawPoints(vector<Point> points)
	{
		for (int i = 0; i < points.size(); i++)
		{
			circle(OriginalImg, points[i], 5, Scalar(0, 0, 255), FILLED);
		}
	}


	Mat getWarp(Mat img, vector<Point> points, float w, float h)
	{
		//It holds only floating point values
		//The source and destination poins are defined
		Point2f src[4] = { points[0],points[1],points[2],points[3] };
		Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };
		//The below function makes a matrix corresponding to the source file
		Mat matrix = getPerspectiveTransform(src, dst);
		//Now the image is transformed using the specified matrix
		warpPerspective(img, WarpImg, matrix, Point(w, h));

		return WarpImg;
	}
	void display()
	{
		imshow("image", OriginalImg);
		//imshow("Threshold Image", ThreImg);
		imshow("Warp Image", WarpImg);
		//imshow("Crop Image", CropImg);
		//imshow("Canny", CannyImg);
		waitKey(0);
	}
};


void main()
{
	Scan obj, ob2;
	//obj.start("resources/testimg.jpg");
	ob2.start("resources/testpap.jpg");

	return;
}