#ifndef _4DSPA_NEURON_H
#define _4DSPA_NEURON_H

#include <vector>
#include <list>
#include <fstream>
#include <utility>

#include <map>

#include "Branch.h"
class BranchPoint
{
public:
	BranchPoint( VectorCoordinate v ):node(v){};
	std::vector< int > sharedBranchIdx; //All BranchIDs of branches passing by the branch point will be recorded;
	inline VectorCoordinate getNode() { return node; };

private:
	VectorCoordinate node;
};


typedef std::map<int, int> HashTableType;
typedef std::pair< VectorCoordinate, int > NodeSearchType;
typedef std::vector< std::list< NodeSearchType > > NodeHashType;
typedef std::list< NodeSearchType > NodeSearchListType;

class NeuronStruct
{
public:

	NeuronStruct( const char* filename )
	{
		Loader( filename );
		timeStamp = -1;
	}

	NeuronStruct( const char* filename, int t )
	{
		Loader( filename );
		timeStamp = t;
	}

	
	//==================================//
	//	functions related to branches	//
	//==================================//
	bool getBranch( int i, NeuronBranch& branch  ); //query a branch by branch ID;
	bool getBranch( VectorCoordinate v, NeuronBranch& branch ); //query a branch by a tip;
	
	double getBranchLength( int idx );
	double getBranchLength( int idx, int s, int e );
	
	int getBranchID(int i);		//given the access index of a branch, query it's id 
	int getBranchIndex(int id);	//given a branch's id, query it's access index.

	bool getBranchType( int branchID, BranchType& type );
	void setBranchType( int branchID, BranchType type );

	bool isBranchID( int id );

	//==================================//
	//	functions related to Nodes		//
	//==================================//
	int queryNodeIdx( VectorCoordinate v );	//report the access index of v in Nodes, return -1 if v is not in Nodes.
	bool getNode(int i, VectorCoordinate& v );
	bool isNode( VectorCoordinate v );	

	//==================================//
	//	functions related to branch		//
	//	point							//
	//==================================//
	bool isBranchPoint( VectorCoordinate v );
	
	//==================================//
	//	functions related to whole		//
	//	neuron							//
	//==================================//
	double getNeuronSize();
	void NonOverlapVariation( NeuronStruct& N, std::vector< std::pair< int, double> >& Variation );
	void OverlapVariation( NeuronStruct& N, std::vector< std::pair< int, double> >& Variation );
	
	//==================================//
	//	Utility function				//
	//==================================//
	void Loader(const char* filename );
	inline int getTimeStamp( ) { return timeStamp; }
	inline int getNumNodes(){ return numNodes; }
	inline int getNumBranches(){ return Branches.size(); }

private:
	std::vector< NeuronBranch > Branches;
	std::vector< BranchPoint > BranchPoints;
	std::vector< VectorCoordinate > Nodes;	//store all non duplicate nodes

	void SetUp( ); //Set up the Hash_List;
	void BpCollector(); //Collect all branch points

	std::map<int, int> HashTable;	//record the mapping between floor(z) (key) to vector index (value) in HashList
	NodeHashType NodeHashList;	// Search structure for any query with the vector coordinate format

	std::map<int, int> BranchIdxTable; //record the mapping between branchID (key) to vector index (value) in Branches

	int numNodes;
	int timeStamp;

};

#endif
