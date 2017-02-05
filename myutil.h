#pragma once

#define INFTY 1e+10
#define EPS 1e-10
#define FLOATTOINTSCALE 1000

#define outv(A) cout << #A << ": " << (double)(A) << endl; // output a Variable
#define outs(S) cout<<S<<endl; // output a String

// Gaussian function for constrast sensitive smoothness
inline double Gaussian(const double dI, double lambda,double sigma_square);

enum Label {NONE=0, OBJ=1, BKG=2}; 
typedef Graph<double,double,double> GraphType;

// count element key in table
template<typename T>
int countintable(const Table2D<T> & table, T key);

// save binary labeling as image
void savebinarylabeling(const Table2D<RGB> & img, const Table2D<Label> & labeling, string savefilename, bool BW = false);

// error rate of segmentation
double geterrorrate(Table2D<Label> & segmentation,Table2D<int> & groundtruth, int boxsize, int gtOBJcolor=0);

// get segmentation from maxflow instances (BK)
bool getgraphlabeling(GraphType * g, Table2D<Label> & segmentation);

// get segmentation from maxflow instances (IBFS)
bool getgraphlabelingIBFS(GraphType * g, Table2D<Label> & segmentation);


inline double Gaussian(const double dI, double lambda,double sigma_square) 
{
	return lambda*(exp(-dI/2/sigma_square));
}

template<typename T>
int countintable(const Table2D<T> & table, T key)
{
	int table_w = table.getWidth();
	int table_h = table.getHeight();
	int tsize = 0;
	for(int y=0; y<table_h; y++) 
	{
		for(int x=0; x<table_w; x++) 
		{ 
			if(table[x][y]==key) // certain element t
				tsize++;
		}
	}
	return tsize;
}

void savebinarylabeling(const Table2D<RGB> & img, const Table2D<Label> & labeling, string savefilename, bool BW)
{
	int img_w = labeling.getWidth();
	int img_h = labeling.getHeight();
	Table2D<RGB> tmp(img_w,img_h);
	for(int i=0;i<img_w;i++)
	{
		for(int j=0;j<img_h;j++)
		{
			if(labeling[i][j]==OBJ)
			{
				if(BW) tmp[i][j] = black;
				else tmp[i][j] = img[i][j];
			}
			else
				tmp[i][j] = white;
		}
	}
	if(saveImage(tmp, savefilename.c_str()))
	    cout<<"saved into: "<<savefilename<<endl;
}

double geterrorrate(Table2D<Label> & segmentation, Table2D<int> & groundtruth, int boxsize, int gtOBJcolor)
{
	double errorrate = 0 ;
	int errornum = 0;
	for(int j=1;j<groundtruth.getHeight()-1;j++)
	{
		for(int i=1;i<groundtruth.getWidth()-1;i++)
		{
			if((groundtruth[i][j]==gtOBJcolor)&&(segmentation[i][j]==OBJ))
				errornum++;
			else if((groundtruth[i][j]==(255-gtOBJcolor))&&(segmentation[i][j]==BKG))
				errornum++;
		}
	}
	cout<<"errornum / boxsize "<<errornum<<' '<<boxsize<<endl;
	errorrate = (double)errornum / boxsize;
	return errorrate;
}

bool getgraphlabeling(GraphType * graph, Table2D<Label> & segmentation)
{
	int img_w = segmentation.getWidth();
	int img_h = segmentation.getHeight();
	segmentation.reset(NONE);
	for (int y=0; y<img_h; y++) 
	{
		for (int x=0; x<img_w; x++) 
		{ 
			int n = x+y*img_w;
			if(graph->what_segment(n) == GraphType::SOURCE)
			{
				segmentation[x][y]=OBJ;
			}
			else if(graph->what_segment(n) == GraphType::SINK)
			{
				segmentation[x][y]=BKG;
			}
		}
	}
	if((countintable(segmentation, OBJ)==0)||(countintable(segmentation, BKG)==0))
		return false;
	else
		return true;
}

bool getgraphlabelingIBFS(IBFSGraph * ibfsgraph, Table2D<Label> & segmentation)
{
	int img_w = segmentation.getWidth();
	int img_h = segmentation.getHeight();
	segmentation.reset(NONE);
	for (int y=0; y<img_h; y++) 
	{
		for (int x=0; x<img_w; x++) 
		{ 
			int n = x+y*img_w;
			if(ibfsgraph->isNodeOnSrcSide(n, 999))
			{
				segmentation[x][y]=OBJ;
			}
			else
			{
				segmentation[x][y]=BKG;
			}
		}
	}
	if((countintable(segmentation, OBJ)==0)||(countintable(segmentation, BKG)==0))
		return false;
	else
		return true;
}
