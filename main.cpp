/***********************************************************************************/
/*          OneCut - software for interactive image segmentation                   */
/*          "Grabcut in One Cut"                                                   */
/*          Meng Tang, Lena Gorelick, Olga Veksler, Yuri Boykov,                   */
/*          In IEEE International Conference on Computer Vision (ICCV), 2013       */
/*          https://github.com/meng-tang/OneCut                                    */
/*          Contact Author: Meng Tang (mtang73@uwo.ca)                             */
/***********************************************************************************/

#include "OneCut.h"
#include "myutil.h"
#include <iostream>

int main(int argc, char * argv[])
{
	// set energy parameters and hyperparameters
	int ColorBinSize = 8; // size of color bin
	int GridConnectivity = 8; // 4, 8 or 16 connect Grid
	double WeightPotts = 9.0; // weight of Potts term
	MAXFLOW maxflowoption = IBFS; // either use BK or IBFS algorithm. BK is NOT recommended here.

	outs("load input image");
	Table2D<RGB> image = loadImage<RGB>("images/326038.bmp");
	clock_t start = clock(); // Timing
	OneCut onecut(image, ColorBinSize, GridConnectivity, maxflowoption); // 8 connect 32 bins per channel
	onecut.print();
	
	outs("load bounding box");
	Table2D<int> box = loadImage<RGB>("images/326038_box.bmp");
	
	onecut.constructbkgraph(box, WeightPotts);

	outs("run maxflow/mincut");
	Table2D<Label> segmentation = onecut.run();

	outs("save segmentation");
	savebinarylabeling(image, segmentation, "images/326038_result.bmp");

	// timing
	cout<<"\nIt takes "<<(double)(clock()-start)/CLOCKS_PER_SEC<<" seconds!"<<endl;

	// segmentation error rate
	Table2D<int> groundtruth = loadImage<RGB>("images/326038_gt.bmp"); // ground truth
	double errorrate = geterrorrate(segmentation, groundtruth, countintable(box, 0));
	outv(errorrate);

	return -1;
}