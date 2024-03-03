//#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
//#include<opencv2/opencv.hpp>
//#include<opencv2/face.hpp>

#include <iostream>
using namespace cv;
using namespace std;

Mat OriginalImg, GrayImg, CannyImg,ThreImg,BlurImg,DilImg,ErodImg, WarpImg,CropImg;
vector<Point> initialPoint,docPoints;
int maxArea;

float w = 420, h = 596;

Mat preprocessing(Mat img)
{
	cvtColor(OriginalImg, GrayImg, COLOR_BGR2GRAY);
	GaussianBlur(OriginalImg, BlurImg, Size(3,3), 3, 0);
	imshow("Image blur", BlurImg);
	Canny(BlurImg, CannyImg, 25, 75);
	imshow("Canny blur", CannyImg);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(CannyImg, DilImg, kernel);
	imshow("Dil Image", DilImg);
	return DilImg;
	//erode(DilImg, ErodImg, kernel);
}

vector<Point> getContours(Mat image) {

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours (img, contours, -1, Scalar(255, 0, 255),
	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	vector<Point> biggest;


	for (int i = 0; i < contours.size(); i++)
	{

		int area = contourArea(contours[i]);

		//cout << area << endl;
		string objectType;

		if (area > 1000)

		{

			float peri = arcLength(contours[i], true);

			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			if (area > maxArea && conPoly[i].size()==4)
			{
			    //drawContours(OriginalImg, conPoly, i, Scalar(255, 0, 255), 5);
				biggest = { conPoly[i][0],conPoly[i][1],conPoly[i][2],conPoly[i][3]};
				maxArea = area;
			}


			//rectangle(OriginalImg, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
		}
	}
	return biggest;
}

void drawPoints(vector<Point> points)
{
	for (int i = 0; i < points.size(); i++)
	{
		circle(OriginalImg, points[i], 5, Scalar(0,0,255), FILLED);
		putText(OriginalImg, to_string(i), points[i], FONT_HERSHEY_PLAIN, 2, Scalar(0,255,0), 2);

	}
}

vector<Point> reorder(vector<Point> points)
{
	vector<Point> newPoints;
	vector<int> sumPoints, subPoints;

	for (int i = 0; i < 4; i++)
	{
		sumPoints.push_back(points[i].x + points[i].y);
		subPoints.push_back(points[i].x - points[i].y);

	}

	newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);
	newPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
	newPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
	newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);

	return newPoints;
}

Mat getWarp(Mat img, vector<Point> points, float w, float h)
{
	Point2f src[4] = { points[0],points[1],points[2],points[3] };
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };

	Mat matrix = getPerspectiveTransform(src, dst);
	warpPerspective(img, WarpImg, matrix, Point(w, h));

	return WarpImg;
}

void main()
{
	string path = "resources/test7.jpg";
	OriginalImg = imread(path);
	resize(OriginalImg, OriginalImg, Size(360,740));

	ThreImg = preprocessing(OriginalImg);
    initialPoint = getContours(ThreImg);
	//drawPoints(initialPoint, Scalar(67, 136, 255));
	docPoints = reorder(initialPoint);
	//drawPoints(initialPoint, Scalar(67,255,255));
	drawPoints(docPoints);

	WarpImg = getWarp(OriginalImg, docPoints, w, h);

	//Rect roi(5, 5, w - (2 * 5), h - (2 * 5));
	//CropImg = WarpImg(roi);
	imshow("image",OriginalImg);

	imshow("Threshold Image", ThreImg);
	imshow("Warp Image", WarpImg);
	//imshow("Crop Image", CropImg);
	//imshow("Image blur", blurimg);
	//imshow("Canny blur", cannyimg);

	
	waitKey(0);
}

