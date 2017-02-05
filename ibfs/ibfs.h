/*
#########################################################
#                                                       #
#  IBFSGraph -  Software for solving                    #
#               Maximum s-t Flow / Minimum s-t Cut      #
#               using the IBFS algorithm                #
#                                                       #
#  http://www.cs.tau.ac.il/~sagihed/ibfs/               #
#                                                       #
#  Haim Kaplan (haimk@cs.tau.ac.il)                     #
#  Sagi Hed (sagihed@post.tau.ac.il)                    #
#                                                       #
#########################################################

This software implements the IBFS (Incremental Breadth First Search) maximum flow algorithm from
	"Faster and More Dynamic Maximum Flow
	by Incremental Breadth-First Search"
	Andrew V. Goldberg, Sagi Hed, Haim Kaplan, Pushmeet Kohli,
	Robert E. Tarjan, and Renato F. Werneck
	In Proceedings of the 23rd European conference on Algorithms, ESA'15
	2015
and from
	"Maximum flows by incremental breadth-first search"
	Andrew V. Goldberg, Sagi Hed, Haim Kaplan, Robert E. Tarjan, and Renato F. Werneck.
	In Proceedings of the 19th European conference on Algorithms, ESA'11, pages 457-468.
	ISBN 978-3-642-23718-8
	2011

Copyright Haim Kaplan (haimk@cs.tau.ac.il) and Sagi Hed (sagihed@post.tau.ac.il)

###########
# LICENSE #
###########
This software can be used for research purposes only.
If you use this software for research purposes, you should cite the aforementioned papers
in any resulting publication and appropriately credit it.

If you require another license, please contact the above.

*/


#ifndef _IBFS_H__
#define _IBFS_H__

#include <stdio.h>
#include <string.h>


#define IB_BOTTLENECK_ORIG 0
#define IBTEST 0
#define IB_MIN_MARGINALS_DEBUG 0
#define IB_MIN_MARGINALS_TEST 0
#define IBSTATS 0
#define IBDEBUG(X) fprintf(stdout, "\n"); fflush(stdout)
#define IB_ALTERNATE_SMART 1
#define IB_HYBRID_ADOPTION 1
#define IB_EXCESSES 1
#define IB_ALLOC_INIT_LEVELS 4096
#define IB_ADOPTION_PR 0
#define IB_DEBUG_INIT 0

class IBFSStats
{
public:
	IBFSStats() {
		reset();
	}
	void reset()
	{
		int C = (IBSTATS ? 0 : -1);
		augs=C;
		growthS=C;
		growthT=C;
		orphans=C;
		growthArcs=C;
		pushes=C;
		orphanArcs1=C;
		orphanArcs2=C;
		orphanArcs3=C;
		if (IBSTATS) augLenMin = (1 << 30);
		else augLenMin=C;
		augLenMax=C;
	}
	void inline incAugs() {if (IBSTATS) augs++;}
	double inline getAugs() {return augs;}
	void inline incGrowthS() {if (IBSTATS) growthS++;}
	double inline getGrowthS() {return growthS;}
	void inline incGrowthT() {if (IBSTATS) growthT++;}
	double inline getGrowthT() {return growthT;}
	void inline incOrphans() {if (IBSTATS) orphans++;}
	double inline getOrphans() {return orphans;}
	void inline incGrowthArcs() {if (IBSTATS) growthArcs++;}
	double inline getGrowthArcs() {return growthArcs;}
	void inline incPushes() {if (IBSTATS) pushes++;}
	double inline getPushes() {return pushes;}
	void inline incOrphanArcs1() {if (IBSTATS) orphanArcs1++;}
	double inline getOrphanArcs1() {return orphanArcs1;}
	void inline incOrphanArcs2() {if (IBSTATS) orphanArcs2++;}
	double inline getOrphanArcs2() {return orphanArcs2;}
	void inline incOrphanArcs3() {if (IBSTATS) orphanArcs3++;}
	double inline getOrphanArcs3() {return orphanArcs3;}
	void inline addAugLen(int len) {
		if (IBSTATS) {
			if (len > augLenMax) augLenMax = len;
			if (len < augLenMin) augLenMin = len;
		}
	}
	int inline getAugLenMin() {return augLenMin;}
	int inline getAugLenMax() {return augLenMax;}

private:
	double augs;
	double paugs;
	double growthS;
	double growthT;
	double orphans;
	double growthArcs;
	double pushes;
	double orphanArcs1;
	double orphanArcs2;
	double orphanArcs3;
	int augLenMin;
	int augLenMax;
};




class IBFSGraph
{
public:
	enum IBFSInitMode { IB_INIT_FAST, IB_INIT_COMPACT };
	IBFSGraph(IBFSInitMode initMode);
	~IBFSGraph();
	void setVerbose(bool a_verbose) {
		verbose = a_verbose;
	}
	bool readFromFile(char *filename);
	bool readFromFileCompile(char *filename);
	void initSize(int numNodes, int numEdges);
	void addEdge(int nodeIndexFrom, int nodeIndexTo, int capacity, int reverseCapacity);
	void addNode(int nodeIndex, int capFromSource, int capToSink);
	void incEdge(int nodeIndexFrom, int nodeIndexTo, int capacity, int reverseCapacity);
	void incNode(int nodeIndex, int deltaCapFromSource, int deltaCapToSink);
	bool incShouldResetTrees();
	struct Arc;
	void incArc(Arc *a, int deltaCap);
	void initGraph();
	int computeMaxFlow();
	int computeMaxFlow(bool allowIncrements);
	void resetTrees();
	void computeMinMarginals();
	void pushRelabel();


	inline bool getFileHasMore() {
		return fileHasMore;
	}
	inline IBFSStats getStats() {
		return stats;
	}
	inline int getFlow() {
		return flow;
	}
	inline int getNumNodes() {
		return nodeEnd-nodes;
	}
	inline int getNumArcs() {
		return arcEnd-arcs;
	}
	int isNodeOnSrcSide(int nodeIndex, int freeNodeValue = 0);


	struct Node;

	struct Arc
	{
		Node*		head;
		Arc*		rev;
		int			isRevResidual :1;
		int			rCap :31;
	};

	struct Node
	{
		int			lastAugTimestamp:30;
		int			isParentCurr:1;
		int			isIncremental:1;
		Arc			*firstArc;
		Arc			*parent;
		Node		*firstSon;
		Node		*nextPtr;
		int			label;	// label > 0: distance from s, label < 0: -distance from t
		int			excess;	 // excess > 0: capacity from s, excess < 0: -capacity to t
	};

private:
	Arc *arcIter;
	bool readFromFile(char *filename, bool checkCompile);
	bool readCompiled(FILE *pFile);
	void augment(Arc *bridge);
	template<bool sTree> int augmentPath(Node *x, int push);
	template<bool sTree> int augmentExcess(Node *x, int push);
	template<bool sTree> void augmentExcesses();
	template<bool sTree> void augmentDischarge(Node *x);
	template<bool sTree> void augmentExcessesDischarge();
	template<bool sTree> void augmentIncrements();
	template <bool sTree> void adoption(int fromLevel, bool toTop);
	template <bool sTree> void adoption3Pass(int minBucket);
	template <bool dirS> void growth();

	int computeMaxFlow(bool trackChanges, bool initialDirS);
	void resetTrees(int newTopLevelS, int newTopLevelT);

	// push relabel
	template<bool sTree> void pushRelabelDischarge(Node *x);
	template<bool sTree> void pushRelabelGlobalUpdate();
	template<bool sTree> void pushRelabelDir();
	void pushRelabelShelve(int fromLevel);

	class ActiveList
	{
	public:
		inline ActiveList() {
			list = NULL;
			len = 0;
		}
		inline void init(Node **mem) {
			list = mem;
			len = 0;
		}
		inline void clear() {
			len = 0;
		}
		inline void add(Node* x) {
			list[len] = x;
			len++;
		}
		inline Node* pop() {
			len--;
			return list[len];
		}
		inline Node** getEnd() {
			return list+len;
		}
		inline static void swapLists(ActiveList *a, ActiveList *b) {
			ActiveList tmp = (*a);
			(*a) = (*b);
			(*b) = tmp;
		}
		Node **list;
		int len;
	};


#define IB_PREVPTR_EXCESS(x) (ptrs[(((x)-nodes)<<1) + 1])
#define IB_NEXTPTR_EXCESS(x) (ptrs[((x)-nodes)<<1])
#define IB_PREVPTR_3PASS(x) ((x)->firstSon)



	class BucketsOneSided
	{
	public:
		inline BucketsOneSided() {
			buckets = NULL;
			maxBucket = 0;
			nodes = NULL;
			allocLevels = 0;
		}
		inline void init(Node *a_nodes, int numNodes) {
			nodes = a_nodes;
			allocLevels = numNodes/8;
			if (allocLevels < IB_ALLOC_INIT_LEVELS) {
				if (numNodes < IB_ALLOC_INIT_LEVELS) allocLevels = numNodes;
				else allocLevels = IB_ALLOC_INIT_LEVELS;
			}
			buckets = new Node*[allocLevels+1];
			memset(buckets, 0, sizeof(Node*)*(allocLevels+1));
			maxBucket = 0;
		}
		inline void allocate(int numLevels) {
			if (numLevels > allocLevels) {
				allocLevels <<= 1;
				Node **alloc = new Node*[allocLevels+1];
				memset(alloc, 0, sizeof(Node*)*(allocLevels+1));
				delete []buckets;
				buckets = alloc;
			}
		}
		inline void free() {
			delete []buckets;
			buckets = NULL;
		}
		template <bool sTree> inline void add(Node* x) {
			int bucket = (sTree ? (x->label) : (-x->label));
			x->nextPtr = buckets[bucket];
			buckets[bucket] = x;
			if (bucket > maxBucket) maxBucket = bucket;
		}
		inline Node* popFront(int bucket) {
			Node *x;
			if ((x = buckets[bucket]) == NULL) return NULL;
			buckets[bucket] = x->nextPtr;
			return x;
		}

		Node **buckets;
		int maxBucket;
		Node *nodes;
		int allocLevels;
	};



	class Buckets3Pass
	{
	public:
		inline Buckets3Pass() {
			buckets = NULL;
			nodes = NULL;
			maxBucket = allocLevels = -1;
		}
		inline void init(Node *a_nodes, int numNodes) {
			nodes = a_nodes;
			allocLevels = numNodes/8;
			if (allocLevels < IB_ALLOC_INIT_LEVELS) {
				if (numNodes < IB_ALLOC_INIT_LEVELS) allocLevels = numNodes;
				else allocLevels = IB_ALLOC_INIT_LEVELS;
			}
			buckets = new Node*[allocLevels+1];
			memset(buckets, 0, sizeof(Node*)*(allocLevels+1));
			maxBucket = 0;
		}
		inline void allocate(int numLevels) {
			if (numLevels > allocLevels) {
				allocLevels <<= 1;
				Node **alloc = new Node*[allocLevels+1];
				memset(alloc, 0, sizeof(Node*)*(allocLevels+1));
				//memcpy(alloc, buckets, sizeof(Node*)*(allocLevels+1));
				delete []buckets;
				buckets = alloc;
			}
		}
		inline void free() {
			delete []buckets;
			buckets = NULL;
		}
		template <bool sTree> inline void add(Node* x) {
			int bucket = (sTree ? (x->label) : (-x->label));
			if ((x->nextPtr = buckets[bucket]) != NULL) IB_PREVPTR_3PASS(x->nextPtr) = x;
			buckets[bucket] = x;
			if (bucket > maxBucket) maxBucket = bucket;
		}
		inline Node* popFront(int bucket) {
			Node *x = buckets[bucket];
			if (x == NULL) return NULL;
			buckets[bucket] = x->nextPtr;
			IB_PREVPTR_3PASS(x) = NULL;
			return x;
		}
		template <bool sTree> inline void remove(Node *x) {
			int bucket = (sTree ? (x->label) : (-x->label));
			if (buckets[bucket] == x) {
				buckets[bucket] = x->nextPtr;
			} else {
				IB_PREVPTR_3PASS(x)->nextPtr = x->nextPtr;
				if (x->nextPtr != NULL) IB_PREVPTR_3PASS(x->nextPtr) = IB_PREVPTR_3PASS(x);
			}
			IB_PREVPTR_3PASS(x) = NULL;
		}
		inline bool isEmpty(int bucket) {
			return buckets[bucket] == NULL;
		}

		Node **buckets;
		int maxBucket;
		Node *nodes;
		int allocLevels;
	};

	class ExcessBuckets
	{
	public:
		inline ExcessBuckets() {
			buckets = ptrs = NULL;
			nodes = NULL;
			allocLevels = maxBucket = minBucket = -1;
		}
		inline void init(Node *a_nodes, Node **a_ptrs, int numNodes) {
			nodes = a_nodes;
			allocLevels = numNodes/8;
			if (allocLevels < IB_ALLOC_INIT_LEVELS) {
				if (numNodes < IB_ALLOC_INIT_LEVELS) allocLevels = numNodes;
				else allocLevels = IB_ALLOC_INIT_LEVELS;
			}
			buckets = new Node*[allocLevels+1];
			memset(buckets, 0, sizeof(Node*)*(allocLevels+1));
			ptrs = a_ptrs;
			reset();
		}
		inline void allocate(int numLevels) {
			if (numLevels > allocLevels) {
				allocLevels <<= 1;
				Node **alloc = new Node*[allocLevels+1];
				memset(alloc, 0, sizeof(Node*)*(allocLevels+1));
				//memcpy(alloc, buckets, sizeof(Node*)*(allocLevels+1));
				delete []buckets;
				buckets = alloc;
			}
		}
		inline void free() {
			delete []buckets;
			buckets = NULL;
		}

		template <bool sTree> inline void add(Node* x) {
			int bucket = (sTree ? (x->label) : (-x->label));
			IB_NEXTPTR_EXCESS(x) = buckets[bucket];
			if (buckets[bucket] != NULL) {
				IB_PREVPTR_EXCESS(buckets[bucket]) = x;
			}
			buckets[bucket] = x;
			if (bucket > maxBucket) maxBucket = bucket;
			if (bucket != 0 && bucket < minBucket) minBucket = bucket;
		}
		inline Node* popFront(int bucket) {
			Node *x = buckets[bucket];
			if (x == NULL) return NULL;
			buckets[bucket] = IB_NEXTPTR_EXCESS(x);
			return x;
		}
		template <bool sTree> inline void remove(Node *x) {
			int bucket = (sTree ? (x->label) : (-x->label));
			if (buckets[bucket] == x) {
				buckets[bucket] = IB_NEXTPTR_EXCESS(x);
			} else {
				IB_NEXTPTR_EXCESS(IB_PREVPTR_EXCESS(x)) = IB_NEXTPTR_EXCESS(x);
				if (IB_NEXTPTR_EXCESS(x) != NULL) IB_PREVPTR_EXCESS(IB_NEXTPTR_EXCESS(x)) = IB_PREVPTR_EXCESS(x);
			}
		}
		inline void incMaxBucket(int bucket) {
			if (maxBucket < bucket) maxBucket = bucket;
		}
		inline bool empty() {
			return maxBucket < minBucket;
		}
		inline void reset() {
			maxBucket = 0;
			minBucket = -1 ^ (1<<31);
		}

		Node **buckets;
		Node **ptrs;
		int maxBucket;
		int minBucket;
		Node *nodes;
		int allocLevels;
	};

	// members
	IBFSStats stats;
	Node	*nodes, *nodeEnd;
	Arc		*arcs, *arcEnd;
	Node	**ptrs;
	int 	numNodes;
	int		flow;
	short 	augTimestamp;
	int topLevelS, topLevelT;
	ActiveList active0, activeS1, activeT1;
	Node **incList;
	int incLen;
	int incIteration;
	Buckets3Pass orphan3PassBuckets;
	BucketsOneSided orphanBuckets;
	ExcessBuckets excessBuckets;
	Buckets3Pass &prNodeBuckets;
	FILE *file;
	FILE *fileCompiled;
	bool fileIsCompiled;
	bool fileHasMore;
	bool verbose;
	int testFlow;
	double testExcess;

	//
	// Orphans
	//
	unsigned int uniqOrphansS, uniqOrphansT;
	template <bool sTree> inline void orphanFree(Node *x) {
		if (IB_EXCESSES && x->excess) {
			x->label = (sTree ? -topLevelT : topLevelS);
			if (sTree) activeT1.add(x);
			else activeS1.add(x);
			x->isParentCurr = 0;
		} else {
			x->label = 0;
		}
	}

	//
	// Initialization
	//
	struct TmpEdge
	{
		int		head;
		int		tail;
		int		cap;
		int		revCap;
	};
	struct TmpArc
	{
		TmpArc		*rev;
		int			cap;
	};
	char	*memArcs;
	TmpEdge	*tmpEdges, *tmpEdgeLast;
	TmpArc	*tmpArcs;
	bool isInitializedGraph() {
		return memArcs != NULL;
	}
	IBFSInitMode initMode;
	void initGraphFast();
	void initGraphCompact();
	void initNodes();

	//
	// Testing
	//
	void testTree();
	void testPrint();
	void testExit() {
		exit(1);
	}
	inline void testNode(Node *x) {
		if (IBTEST && x-nodes == 110559) {
			IBDEBUG("*");
		}
	}
};





inline void IBFSGraph::addNode(int nodeIndex, int capSource, int capSink)
{
	int f = nodes[nodeIndex].excess;
	if (f > 0) {
		capSource += f;
	} else {
		capSink -= f;
	}
	if (capSource < capSink) {
		flow += capSource;
	} else {
		flow += capSink;
	}
	nodes[nodeIndex].excess = capSource - capSink;
}


// @pre: activeS1.len == 0 && activeT1.len == 0
inline void IBFSGraph::resetTrees()
{
	resetTrees(1,1);
}

// @pre: activeS1.len == 0 && activeT1.len == 0
inline void IBFSGraph::resetTrees(int newTopLevelS, int newTopLevelT)
{
	uniqOrphansS = uniqOrphansT = 0;
	topLevelS = newTopLevelS;
	topLevelT = newTopLevelT;
	for (Node *y=nodes; y != nodeEnd; y++)
	{
		if (y->label < topLevelS && y->label > -topLevelT) continue;
		y->firstSon = NULL;
		if (y->label == topLevelS) activeS1.add(y);
		else if (y->label == -topLevelT) activeT1.add(y);
		else {
			y->parent = NULL;
			if (y->excess == 0) {
				y->label = 0;
			} else if (y->excess > 0) {
				y->label = topLevelS;
				activeS1.add(y);
			} else {
				y->label = -topLevelT;
				activeT1.add(y);
			}
		}
	}
}

inline bool IBFSGraph::incShouldResetTrees()
{
	return (uniqOrphansS + uniqOrphansT) >= (unsigned int)(2*numNodes);
}

inline void IBFSGraph::incNode(int nodeIndex, int deltaCapSource, int deltaCapSink)
{
	Node *x = (nodes+nodeIndex);

	// Initialize Incremental Phase
	if (incList == NULL)
	{
		// init list
		incList = active0.list;
		incLen = 0;

		// reset checks
		if (incShouldResetTrees()) {
			resetTrees(1, 1);
		}
	}

	// turn both deltas to positive
	if (deltaCapSource < 0) {
		flow += deltaCapSource;
		deltaCapSink -= deltaCapSource;
		deltaCapSource = 0;
	}
	if (deltaCapSink < 0) {
		flow += deltaCapSink;
		deltaCapSource -= deltaCapSink;
		deltaCapSink = 0;
	}

	// add delta
	addNode(nodeIndex, deltaCapSource, deltaCapSink);

	// add to incremental list
	if (!x->isIncremental) {
		incList[incLen++] = x;
		x->isIncremental=1;
	}
}

inline void IBFSGraph::incArc(Arc *a, int deltaCap)
{
	if (deltaCap == 0) return;
	if (a->rCap + a->rev->rCap + deltaCap < 0) {fprintf(stdout, "ERROR\n"); exit(1);}
	Node *x, *y;
	int push;

	if (deltaCap > -a->rCap)
	{
		// there is enough residual capacity
		a->rCap += deltaCap;
		if (a->rCap == deltaCap) {
			// we added capcity (deltaCap > 0) and arc was just made residual
			x = a->rev->head;
			y = a->head;
			if (x->label > 0 && y->label == (x->label+1) && y->isParentCurr && a->rev < y->parent) {
				y->isParentCurr = 0;
			} else if (x->label < 0 && y->label == (x->label+1) && x->isParentCurr && a < x->parent) {
				x->isParentCurr = 0;
			} else if ((x->label > 0 && y->label <= 0)
					|| (x->label >= 0 && y->label < 0)
					|| (x->label > 0 && y->label > (x->label+1))
					|| (x->label < (y->label-1) && y->label < 0)) {
				// arc invalidates invariants - must saturate it
				a->rev->rCap += deltaCap;
				a->rCap = 0;
				flow -= deltaCap;
				incNode(x-nodes, 0, deltaCap);
				incNode(y-nodes, deltaCap, 0);
			}
		}
	}
	else
	{
		// there is not enough residual capacity
		// saturate the reverse arc
		x = a->rev->head;
		y = a->head;
		push = -(deltaCap+a->rCap);
		a->rev->rCap -= push;
		a->rCap = 0;
		flow -= push;
		incNode(y-nodes, 0, push);
		incNode(x-nodes, push, 0);
	}

	a->rev->isRevResidual = (a->rCap ? 1 : 0);
	a->isRevResidual = (a->rev->rCap ? 1 : 0);
}

inline void IBFSGraph::addEdge(int nodeIndexFrom, int nodeIndexTo, int capacity, int reverseCapacity)
{
	tmpEdgeLast->tail = nodeIndexFrom;
	tmpEdgeLast->head = nodeIndexTo;
	tmpEdgeLast->cap = capacity;
	tmpEdgeLast->revCap = reverseCapacity;
	tmpEdgeLast++;

	// use label as a temporary storage
	// to count the out degree of nodes
	nodes[nodeIndexFrom].label++;
	nodes[nodeIndexTo].label++;
}

inline void IBFSGraph::incEdge(int nodeIndexFrom, int nodeIndexTo, int capacity, int reverseCapacity)
{
	Node *x = nodes + nodeIndexFrom;
	Node *y = nodes + nodeIndexTo;
	if (arcIter == NULL || arcIter->rev->head != x) {
		arcIter = x->firstArc;
	}
	Arc *end = (x+1)->firstArc;
	Arc *initIter = arcIter;
	if (arcIter->head != y)
		for ((++arcIter) == end && (arcIter=x->firstArc);
			arcIter != initIter;
			(++arcIter) == end && (arcIter=x->firstArc))
	{
		if (arcIter->head == y) break;
	}
	if (arcIter->head != y) {
		fprintf(stdout, "Cannot increment arc (%d, %d)!\n", (int)(x-nodes), (int)(y-nodes));
		exit(1);
	}
	incArc(arcIter, capacity);
	incArc(arcIter->rev, reverseCapacity);
}


inline int IBFSGraph::isNodeOnSrcSide(int nodeIndex, int freeNodeValue)
{
	if (nodes[nodeIndex].label == 0) {
		return freeNodeValue;
	}
	return (nodes[nodeIndex].label > 0 ? 1 : 0);
}




#endif


