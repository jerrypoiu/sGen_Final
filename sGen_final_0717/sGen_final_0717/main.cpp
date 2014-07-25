/********************************************************************
*																	*
*	sGen 4th project - 'see real'									*
*	�ȼ���, ���¿�, ������, �ڵ���, �̼ҿ�, �̰��, �����			*
*	3���� ������ �̿��Ͽ� perspective view�� �����ϴ� ���α׷�		*
*																	*
*	last modified : 17 / 07 / 2014									*
*																	*
*	Copyright(c) 2014												*
*	All rights reserved by Do-young Park							*
*																	*
********************************************************************/
// for open cv
#include "opencv/cv.h"
#include "opencv/highgui.h"

// for image processing
#include	"dy_lib.h"

#define NUM_OF_INTERPOLATIED_IMAGE 8

using namespace std;
///0. base - open cv lib ���, tcp���(��� ������)
	/// 0-1. open cv 2.4.8 ���, visual studio 2013 ��� 

INT main(INT argc, CHAR *argv[])
{
	//variable declaration
	register INT i, j;									//for loop
	IplImage *leftImg, *rightImg, *midImg;				//for image load
	IplImage *L, *R, *M;								//for color to gray image
	IplImage *disparity_img_L_M, *disparity_img_M_R;	//for disparity
	IplImage *resultSet[NUM_OF_INTERPOLATIED_IMAGE];	//interpolatied image 
	CvMat *disparity_L_M, *disparity_M_R;				//for normalized disparity

	//test
	IplImage *re,*re2;
	IplImage *disparity_test;
	IplImage *le, *ri;
	IplImage *view;


///1. ����� �̿��Ͽ� ������ ����. 3�� ���� ������ ����.
	
	//test - 3�� �ε� 
	if ((leftImg = cvLoadImage("TL.jpg", CV_LOAD_IMAGE_COLOR)) == NULL)
	{
		cerr << "left image is not open" << endl;
		exit(1);
	}
	if ((midImg = cvLoadImage("TR.jpg", CV_LOAD_IMAGE_COLOR)) == NULL)
	{
		cerr << "mid image is not open" << endl;
		exit(1);
	}
	if ((rightImg = cvLoadImage("TR.jpg", CV_LOAD_IMAGE_COLOR)) == NULL)
	{
		cerr << "right image is not open" << endl;
		exit(1);
	}

///2. ���� ������ (������ �ɼ�) raw data ���·� ���� (bmp, png�� �ɼ�) 
	//test -  ����� iplimage ���·� �س���. 


///3. ������ �����͸� ������ ����ó�� 
	/// 3-1. �����͸� ������ image rectification ����
	// �غ���...


	/// 3-2. rectified image�� �̿��Ͽ� stereo matching�Ͽ� disparity map ����
	// stereo matching prameter 
	CvStereoBMState *BMState = cvCreateStereoBMState();
	BMState->preFilterSize = 21;  // 5x5���� 21x21����
	BMState->preFilterCap = 21;
	BMState->SADWindowSize = 21;  // 5x5���� 21x21����
	BMState->minDisparity = 1;
	BMState->numberOfDisparities = 64;  //Searching�ϱ� ���� pixel���� ����
	BMState->textureThreshold = 10;    //minimum allowed
	BMState->uniquenessRatio = 5;

	//color to gray
	L = cvCreateImage(cvSize(leftImg->width, leftImg->height), leftImg->depth, 1);
	R = cvCreateImage(cvSize(rightImg->width, rightImg->height), rightImg->depth, 1);
	M = cvCreateImage(cvSize(midImg->width, midImg->height), midImg->depth, 1);

	cvCvtColor(leftImg, L, CV_RGB2GRAY);
	cvCvtColor(rightImg, R, CV_RGB2GRAY);
	cvCvtColor(midImg, M, CV_RGB2GRAY);

	//disparity 
	disparity_img_L_M = cvCreateImage(cvSize(midImg->width, midImg->height), IPL_DEPTH_8U, 1);
	disparity_img_M_R = cvCreateImage(cvSize(midImg->width, midImg->height), IPL_DEPTH_8U, 1);
	disparity_L_M = cvCreateMat(midImg->height, midImg->width, CV_16SC1);
	disparity_M_R = cvCreateMat(midImg->height, midImg->width, CV_16SC1);

	//stereo matching - left and mid
	cvFindStereoCorrespondenceBM(L, M, disparity_L_M, BMState);		//stereo matching
	cvNormalize(disparity_L_M, disparity_img_L_M, 0, 255, CV_MINMAX);	//normalize

	//mid and right
	cvFindStereoCorrespondenceBM(M, R, disparity_M_R, BMState);		//stereo matching
	cvNormalize(disparity_M_R, disparity_img_M_R, 0, 255, CV_MINMAX);	//normalize


	re = cvCreateImage(cvSize(midImg->width, midImg->height), IPL_DEPTH_8U, 1);
	for (i = 0; i < midImg->height; ++i)
	{
		for (j = 0; j < midImg->width/2; ++j)
		{
			re->imageData[i*midImg->width + j] = disparity_img_L_M->imageData[i*midImg->width + j + BMState->numberOfDisparities];
		}
		for (j = midImg->width / 2 + 1; j < midImg->width; ++j)
		{
			re->imageData[i*midImg->width + j] = disparity_img_L_M->imageData[i*midImg->width + j];
		}
	}//�ݹݹ�����? ��Ī����Ʈ ��ƾߵǳ�? ������? �ȼ���? 


	/// 3-3. disparity map�� raw data�� ������ view interpolation(backward projection)
	//test - disparity map�� �켱 �׽�Ʈ������ �������� 
	if ((disparity_test = cvLoadImage("flowerpots_disp1.pgm", CV_LOAD_IMAGE_GRAYSCALE)) == NULL)
	{
		cerr << "disparity not open" << endl;
		exit(1); 
	}

	if ((le = cvLoadImage("flowerpots_view1.png", CV_LOAD_IMAGE_COLOR)) == NULL)
	{
		cerr << "left not open" << endl;
		exit(1);
	}
	if ((ri = cvLoadImage("flowerpots_view5.png", CV_LOAD_IMAGE_COLOR)) == NULL)
	{
		cerr << "right not open" << endl;
		exit(1);
	}
	re2 = cvCreateImage(cvSize(ri->width, ri->height), IPL_DEPTH_8U, 1);


	for (i = 0; i < le->height; ++i)
	{
		for (j = 0; j < le->width; ++j)
		{
			if (j < le->width / 2)
				re2->imageData[i*le->width + j] = 0;
			else
				re2->imageData[i*le->width + j] = 255;
		}
	}





	/// 3-4. inpaint�Ͽ� ������ hole�� �Ųپ� ��. 
	/// 3-5. (���͸� �̿��Ͽ� ����ó�� - �ɼ�)

///4. ó���Ϸ��� ���� 4�徿 8�� ( left - mid ���� 4��, mid - right ���� 4�� ) ����(������ �ɼ�)





	


	// test - �߰� ����� �� �ϼ� ����� ������ ���� �����ֱ�
	cvNamedWindow("left original",1); cvShowImage("left original", leftImg);
	cvNamedWindow("mid original", 1); cvShowImage("mid original", midImg);
	cvNamedWindow("right original", 1); cvShowImage("right original", rightImg);

	cvNamedWindow("disparity left, mid"); cvShowImage("disparity left, mid", disparity_img_L_M);
	cvNamedWindow("disparity mid, right"); cvShowImage("disparity mid, right", disparity_img_M_R);

	cvNamedWindow("re", 1); cvShowImage("re", re2);

	while (1)
	{
		int c = cvWaitKey(10);
		if (c == 27)
			break;
	}

///5. ������ ���� 8��� ���� 3������ �ۿ��� show image ( ���� �� �κ��� �ƴ�. )





///fin. release 
	cvReleaseStereoBMState(&BMState);
	
	cvReleaseImage(&leftImg);
	cvReleaseImage(&rightImg);
	cvReleaseImage(&midImg);
	cvReleaseImage(&L);
	cvReleaseImage(&M);
	cvReleaseImage(&R);
	cvReleaseImage(&disparity_img_L_M);
	cvReleaseImage(&disparity_img_M_R);
	
	cvReleaseMat(&disparity_L_M);
	cvReleaseMat(&disparity_M_R);
	return 0;
}







/* ���� open cv �Լ� �� �ڷ� 
IplImage* pImage = cvLoadImage("img.jpg", CV_LOAD_IMAGE_UNCHANGED);
IplImage* R_Channel = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);
IplImage* G_Channel = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);
IplImage* B_Channel = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);

cvSplit(pImage, B_Channel, G_Channel, R_Channel, 0);


cvSaveImage



frame->imageData[i * frame->widthStep + j + 0];
frame->imageData[i * frame->widthStep + j + 1];
frame->imageData[i * frame->widthStep + j + 2];

BGR
*/