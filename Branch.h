#ifndef _4DSPA_BRANCH_H
#define _4DSPA_BRANCH_H

#include <vector>
#include "Basic.h"

enum BranchType{
	NEUTRAL, RETRACT, TRANSIENT, NEW 
};

class NeuronBranch
{

public:
	NeuronBranch(int id):type( BranchType::NEUTRAL ), ID(id){  };
	NeuronBranch( ):type( BranchType::NEUTRAL ), ID(-1){  };

	std::vector< VectorCoordinate > points;
	std::vector< int > bpLocalIdx;	//record the array indices of branch points
	
	inline BranchType getType() { return type; }
	inline void setType( BranchType t ) { type = t; }
	inline int getID(){ return ID; }
	inline void setID(int id){ ID = id; }

	double getLength()
	{
		double L = .0;
		for(int i=1; i<points.size(); ++i)
		{
			VectorCoordinate v = points[i] - points[i-1];
			L += v.Length();
		}
		return L;
	}

private:
	BranchType type;
	int ID;
};



#endif
