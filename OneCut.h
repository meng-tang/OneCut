#pragma once
#include <assert.h>
#include <vector>
#include <time.h>
#include <string>

#include "ezi/Image2D.h"
#include "ezi/Table2D.h"
#include "maxflow/graph.h" // for BK algorithm
#include "ibfs/ibfs.h"     // for IBFS algorithm
#include "myutil.h"

template<class T>
struct Edge
{
	Edge(int p_ =0, int q_=0, T edgeweight_=0)
		:p(p_),q(q_),edgeweight(edgeweight_){}
	int p;
	int q;
	T edgeweight;
};
// which maxflow algorithm to use, either Boykov-Kolmogorov or IBFS
enum MAXFLOW {BK, IBFS};
int getl1penalty(Table2D<int> & colorlabel,Table2D<int> & box);

class OneCut{
public:
	OneCut();
	OneCut(Table2D<RGB> img_, double colorbinsize_, int GridConnectivity_ = 8, MAXFLOW maxflowoption = IBFS);
	~OneCut();
	// add smoothness term to the graph
	// weight_potts is the weight of smoothness or Potts term
	void addsmoothnessterm(double weight_potts);
	// add color separation term to the graph
	void addcolorseparation(const Table2D<int> &colorlabel,float weight_colorseparation);
	void constructbkgraph(Table2D<int> box, float weight_potts);
	Table2D<Label> run();

	void print();
	void computeedges();
	void computebinning();
private:
	Table2D<RGB> img;
	int img_w;
	int img_h;
	int GridConnectivity; // can be 4 or 8 or 16
	int numcolorbin;
	int colorbinsize;
	Table2D<int> colorbinning;

	vector<Edge<double> > edges;
	MAXFLOW maxflowoption;
	GraphType * bkgraph;
	IBFSGraph * ibfsgraph;
};

OneCut::OneCut():bkgraph(NULL),ibfsgraph(NULL)
{
}

OneCut::OneCut(Table2D<RGB> img_, double colorbinsize_, int GridConnectivity_, MAXFLOW maxflowoption_)
	:bkgraph(NULL), ibfsgraph(NULL), maxflowoption(maxflowoption_)
{
	Assert((GridConnectivity_==4)||(GridConnectivity_==8)||(GridConnectivity_==16), "grid connectivity can only be 4!");
	img = Table2D<RGB>(img_);
	img_w = img.getWidth();
	img_h = img.getHeight();

	GridConnectivity = GridConnectivity_;
	computeedges();

	colorbinsize = colorbinsize_;
	computebinning();
	numcolorbin = this->colorbinning.getMax()+1;
}

OneCut::~OneCut(){
	if(bkgraph !=NULL) 
		delete bkgraph;
	if(ibfsgraph !=NULL) 
		delete ibfsgraph;
}

void OneCut::constructbkgraph(Table2D<int> box, float weight_potts){
	// reset graph
	if(bkgraph!=NULL){
		delete bkgraph;
		bkgraph = NULL;
	}
	if(ibfsgraph!=NULL){
		delete ibfsgraph;
		ibfsgraph = NULL;
	}
	// construct graph
	if(maxflowoption == BK){
		bkgraph = new GraphType(/*estimated # of nodes*/ img_w*img_h+numcolorbin, 
			/*estimated # of edges*/ 5*img_w*img_h); 
		bkgraph->add_node(img_w*img_h+numcolorbin);    // adding nodes
	}else if(maxflowoption == IBFS){
		ibfsgraph = new IBFSGraph(IBFSGraph::IB_INIT_FAST);
		ibfsgraph->initSize(img_w*img_h+numcolorbin,6*img_w*img_h);
	}

	if(maxflowoption==BK){
		for(int x=0;x<img_w;x++)  
		{
			for(int y=0;y<img_h;y++)
				if(box[x][y]==255)
					bkgraph->add_tweights(x+y*img_w,0,INFTY);// Hard constraint outside the bounding box
				else
					bkgraph->add_tweights(x+y*img_w,1,0); // Linear foreground ballooning inside the box
		}
	}else if(maxflowoption==IBFS){
		for(int x=0;x<img_w;x++)  
		{
			for(int y=0;y<img_h;y++)
				if(box[x][y]==255)
					ibfsgraph->addNode(x+y*img_w,0,INFTY);// Hard constraint outside the bounding box
				else
					ibfsgraph->addNode(x+y*img_w,1*FLOATTOINTSCALE,0); // Linear foreground ballooning inside the box
		}
	}


	// weight of Potts term
	addsmoothnessterm(weight_potts);

	float beta_prime = 0.9; // for L1 color separation term
	float l1penalty = getl1penalty(colorbinning,box);

	int boxsize = countintable(box,0);
	float weight_colorseparation = (float)boxsize/l1penalty*beta_prime; // weight of L1 color separation term
	//outv(weight_colorseparation);

	addcolorseparation(colorbinning, weight_colorseparation);

}

Table2D<Label> OneCut::run(){
	Table2D<Label> segmentation(img_w,img_h,NONE);
	if(maxflowoption==BK){
		float flow = bkgraph->maxflow();
		outv(flow);
		getgraphlabeling(bkgraph, segmentation);
	}else if(maxflowoption==IBFS){
		ibfsgraph->initGraph();
		ibfsgraph->computeMaxFlow();
		outv(ibfsgraph->getFlow());
		getgraphlabelingIBFS(ibfsgraph, segmentation);
	}
	return segmentation;
}

void OneCut::print()
{
	cout<<"Image width: "<<img_w<<endl;
	cout<<"Image height: "<<img_h<<endl;
	cout<<"Color bin size: "<<colorbinsize<<endl;
	cout<<"number of non-empty color bins: "<<numcolorbin<<endl;
}

void OneCut::computeedges()
{
	int node_id = 0;
	int img_w = img.getWidth();
	int img_h = img.getHeight();
	double sigma_sum = 0;
	double sigma_square_count = 0;
	Point kernelshifts [] = {Point(1,0),Point(0,1),Point(1,1),Point(1,-1),
		Point(2,-1),Point(2,1),Point(1,2),Point(-1,2),};
	for (int y=0; y<img_h; y++) // adding edges (n-links)
	{
		for (int x=0; x<img_w; x++) 
		{ 
			Point p(x,y);
			for(int i=0;i<GridConnectivity/2;i++)
			{
				Point q = p + kernelshifts[i];
				if(img.pointIn(q))
				{
					sigma_sum += dI(img[p],img[q]);
					sigma_square_count ++;
				}
			}
		}
	}

	double sigma_square = sigma_sum/sigma_square_count;
	for (int y=0; y<img_h; y++) // adding edges (n-links)
	{
		for (int x=0; x<img_w; x++) 
		{ 
			Point p(x,y);
			for(int i=0;i<GridConnectivity/2;i++)
			{
				Point q = p + kernelshifts[i];
				if(img.pointIn(q))
				{
					edges.push_back(Edge<double>(p.x+p.y*img_w,q.x+q.y*img_w,Gaussian(dI(img[p],img[q]),1.0,sigma_square)/(p-q).norm()));
				}
			}
		}
	}

}


void OneCut::computebinning(){
	colorbinning= Table2D<int>(img_w,img_h);
	RGB rgb_v;
	int r_idx =0, g_idx = 0, b_idx = 0, idx =0;
	int binperchannel = (int)ceil(256.0/colorbinsize);
	for(unsigned int j=0;j<img_h;j++)
	{
		for(unsigned int i=0;i<img_w;i++)
		{
			rgb_v = img[i][j];
			r_idx = (int)(rgb_v.r/colorbinsize);
			g_idx = (int)(rgb_v.g/colorbinsize);
			b_idx = (int)(rgb_v.b/colorbinsize);
			idx = r_idx + g_idx*binperchannel+b_idx*binperchannel*binperchannel;
			colorbinning[i][j] = idx;
		}
	}
	// sparse binning
	vector<int> colorhist(binperchannel*binperchannel*binperchannel,0);
	for(unsigned int j=0;j<img_h;j++)
	{
		for(unsigned int i=0;i<img_w;i++)
		{
			colorhist[colorbinning[i][j]] = colorhist[colorbinning[i][j]]+1;
		}
	}
	
	vector<int> correspondence(colorhist.size(),-1);
	int compactcount = 0;
	for(int i=0;i<colorhist.size();i++)
	{
		if(colorhist[i]!=0)
		{
			correspondence[i] = compactcount;
			compactcount++;
		}
	}
	for(int j=0;j<img_h;j++)
	{
		for(int i=0;i<img_w;i++)
		{
			colorbinning[i][j] = correspondence[colorbinning[i][j]];
		}
	}
	
}

// Color histogram overlap of box and its outside region based on L1 metric
int getl1penalty(Table2D<int> & colorbinning,Table2D<int> & box)
{
	int returnv = 0;
	int bin_num = colorbinning.getMax()+1;
	vector<int> obj_vector(bin_num,0);
	vector<int> bkg_vector(bin_num,0);
	for(int j=0;j<box.getHeight();j++)
	{
		for(int i=0;i<box.getWidth();i++)
		{
			if(box[i][j] == 0)
			{
				obj_vector[colorbinning[i][j]]++;
			}
			else
				bkg_vector[colorbinning[i][j]]++;
		}
	}
	for(int i=0;i<bin_num;i++)
	{
		returnv+=min(obj_vector[i],bkg_vector[i]);
	}
	return returnv;
}

// add L1 color separation term to the graph
// ROI is the region of interest
// separation_w is the weight of the color separation term
void OneCut::addcolorseparation(const Table2D<int> &colorlabel,float separation_w)
{
	int node_id = 0;
	int img_w = colorlabel.getWidth();
	int img_h = colorlabel.getHeight();
	for(int y=0; y<img_h; y++) // adding links to auxiliary nodes
	{
		for(int x=0; x<img_w; x++) 
		{ 
			node_id = x+y*img_w;
			if(maxflowoption == BK)
				bkgraph->add_edge( node_id, colorlabel[x][y]+img_w*img_h,separation_w, separation_w);
			else if(maxflowoption == IBFS)
				ibfsgraph->addEdge(node_id, colorlabel[x][y]+img_w*img_h,(int)(separation_w*FLOATTOINTSCALE), (int)(separation_w*FLOATTOINTSCALE));
		}
	}
}


// add smoothness term to the graph
// lambda is the weight of the smoothness term
// ROI is the region of interest
void OneCut::addsmoothnessterm(double lambda)
{
	// number of neighboring pairs of pixels
	int numNeighbor = edges.size();
	// n-link - smoothness term
	int node_id1 =0, node_id2 =0;
	
	for(int i=0;i<numNeighbor;i++)
	{
		Edge<double> edge = edges[i];
		double v = lambda*edges[i].edgeweight;
		if(maxflowoption == BK)
			bkgraph->add_edge(edge.p,edge.q,v,v);
		else if(maxflowoption == IBFS)
			ibfsgraph->addEdge(edge.p,edge.q,(int)(v*FLOATTOINTSCALE),(int)(v*FLOATTOINTSCALE));
	}
}