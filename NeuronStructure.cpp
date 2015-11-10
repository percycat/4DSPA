#include <algorithm>

#include "NeuronStructure.h"


void NeuronStruct::SetUp( )
{
	//std::vector< VectorCoordinate > P;
	//NodesSetType S;

	int nSize = 0;

	for(int i=0; i<Branches.size(); ++i)
	{
		for(int j=0; j<Branches[i].points.size(); ++j)
		{
			VectorCoordinate v = Branches[i].points[j]; 
			
			int lz = v.z;
			HashTableType::iterator Itr = HashTable.find( lz );
			
			if( Itr == HashTable.end() )
			{
				NodeSearchListType tmpL;

				tmpL.push_back( NodeSearchType( v, Nodes.size() ) );
				Nodes.push_back( v );

				HashTable.insert( std::pair<int, int>(lz, NodeHashList.size() ) );

				NodeHashList.push_back( tmpL );
			}
			else
			{
				int slot = Itr->second;

				NodeSearchListType::iterator l_Itr = NodeHashList[ slot ].begin();
				bool HIT = false;
				for(;l_Itr != NodeHashList[ slot ].end() && !HIT; ++l_Itr)
				{
					if( l_Itr->first == v )
						HIT = true;
				}

				if( !HIT )
				{
					NodeHashList[ slot ].push_back( NodeSearchType( v, Nodes.size() ) );
					Nodes.push_back( v );
				}
			}
		}
	}


	numNodes = Nodes.size();//nSize;
	std::cout<<Nodes.size()<<'\n';

}



void NeuronStruct::BpCollector()
{
	int* Counter = new int [ numNodes ];
	memset( Counter, 0, sizeof( int ) * numNodes );

	int** NodeIdx = new int* [ Branches.size() ];
	for(int i=0; i<Branches.size(); ++i)
		NodeIdx[i] = new int [ Branches[i].points.size() ];
	
	//for each node, we record how many branches pass by.
	for(int i=0; i<Branches.size(); ++i)
	{
		for(int j=0; j<Branches[i].points.size(); ++j)
		{
			int idx = queryNodeIdx( Branches[i].points[j] );

			if( idx >= 0 && idx < numNodes )
			{
				NodeIdx[i][j] = idx;
				++Counter[ idx ]; 
			}
		}
	}

	for(int i=0; i<Branches.size(); ++i)
	{
		for(int j=Branches[i].points.size()-1; j>0; --j)	//Tracking from tip to soma, a branch point is the point where counts varied.
		{
			int preIdx = NodeIdx[ i ][ j ];
			int idx = NodeIdx[ i ][ j - 1 ];

			if( Counter[ idx ] > Counter[ preIdx ] )
			{
				Branches[i].bpLocalIdx.push_back( j - 1 );	//for each branch, record which node (array index) is branch point

				//Check whether it is discovered the very first time
				bool HIT = false;
				for(int k=0; k<BranchPoints.size(); ++k)
				{
					if( BranchPoints[k].getNode() == Branches[i].points[j - 1] ) //for each branch point, record which branch passes by
					{
						//BranchPoints[k].sharedBranchIdx.push_back( i );
						BranchPoints[k].sharedBranchIdx.push_back( Branches[i].getID() );
						HIT = true;
						break;
					}
				}

				if( !HIT )
					BranchPoints.push_back( BranchPoint( Branches[i].points[j - 1] ) );
			}
			
		}

	}
	
	for(int i=0; i<Branches.size(); ++i)
		delete [] NodeIdx[i];
	delete [] NodeIdx;

	delete [] Counter;


}
void NeuronStruct::Loader( const char* filename )
{
	//Load all complete branches info. from file
	std::ifstream infile( filename );
	if( !infile.is_open() )
	{
		std::cout<<"fail to open the file: "<<filename<<'\n';
		return;
	}
	
	char buf[256];
	memset( buf, 0, 256 );
	infile.getline(buf, 256);
	memset( buf, 0, 256 );
	infile.getline(buf, 256);

	char tmp;
	infile>>tmp;
	while( tmp != 'e' )
	{
		char tmpChar;

		if( tmp == 'P' )
		{
			int idx, size;
			infile>>idx>>tmpChar>>size;
			
			NeuronBranch tmpBranch( idx );
			for(int i=0; i<size; i++)
			{
				double x,y,z;
				infile>>x>>y>>z;
				tmpBranch.points.push_back( VectorCoordinate(x,y,z) );
				
			}

			BranchIdxTable.insert( std::pair<int, int>( idx, Branches.size() ) );
			Branches.push_back( tmpBranch );
		}

		infile>>tmp;
	}

	//Construct the search structure
	SetUp( );

	//Collect all branch points
	BpCollector();
}


double NeuronStruct::getNeuronSize()
{

	double L = .0;

	bool* visited = new bool [ Nodes.size() ];
	for(int i=0; i<Nodes.size(); ++i)
		visited[i] = false;

	for(int i=0; i<Branches.size(); ++i)
	{
		
		for(int j=Branches[i].points.size()-1; j>0; --j)
		{
			int idx = queryNodeIdx( Branches[i].points[j] );
			if( !visited[idx] )
			{
				visited[ idx ] = true;
				L += (Branches[i].points[j] - Branches[i].points[j-1]).Length();
				//std::cout<<Branches[i].points[j]<<'\n';
			}
			else
			{
				//std::cout<<Branches[i].points[j]<<' '<<idx<<'\n';
				//system("pause");
				break;
			}
		}
	}
	
	
	delete [] visited;

	return L;
}

bool NeuronStruct::isNode( VectorCoordinate v )
{
	int idx = queryNodeIdx( v );
	
	return ( idx >= 0 );
}

bool NeuronStruct::isBranchPoint( VectorCoordinate v )
{
	bool HIT = false;
	for(int i=0; i<BranchPoints.size() && !HIT; ++i)
	{
		if( v == BranchPoints[i].getNode() )
			HIT = true;
	}

	return HIT;
}

bool NeuronStruct::isBranchID( int id )
{
	std::map<int, int>::iterator Itr = BranchIdxTable.find( id );
	
	return Itr != BranchIdxTable.end();
}

bool NeuronStruct::getNode( int i, VectorCoordinate& v )
{
	
	if( i >= 0 && i < Nodes.size() )
	{
		v = Nodes[i];
		return true;
	}
	
	return false;

}

bool NeuronStruct::getBranch( int id,  NeuronBranch& branch )
{
	
	return true;
}

bool NeuronStruct::getBranch( VectorCoordinate v,  NeuronBranch& branch )
{
	
	for(int i=0; i<Branches.size(); ++i)
	{
		if( v == Branches[ i ].points.back() )
		{
			branch = Branches[ i ];
			return true;
		}
	}

	return false;
}

double NeuronStruct::getBranchLength( int idx )	// if return value = -1, the query is invalid.
{
	double L = .0;

	if( idx < 0 || idx > Branches.size() )
		return -1.0;

	for(int i=1; i<Branches[ idx ].points.size(); ++i)
	{
		VectorCoordinate v = Branches[ idx ].points[ i ] - Branches[ idx ].points[ i - 1 ];
		L += v.Length();
	}

	return L;
}

double NeuronStruct::getBranchLength(int idx, int s, int e )	// if return value = -1, the query is invalid.
{
	double L = .0;

	if( idx < 0 || idx > Branches.size() )
		return -1.0;

	if( s > e || s < 0 || e >= Branches[idx].points.size() )
		return -1.0;

	for(int i=1; i<Branches[ idx ].points.size(); ++i)
	{
		VectorCoordinate v = Branches[ idx ].points[ i ] - Branches[ idx ].points[ i - 1 ];
		L += v.Length();
	}

	return L;
}

int NeuronStruct::queryNodeIdx( VectorCoordinate v )	//report the access index of v in Nodes, return -1 if v is not in Nodes.
{
	int lz = v.z;
	std::map<int, int>::iterator Itr = HashTable.find( lz );

	if( Itr != HashTable.end() )
	{
		int slot = Itr->second;

		NodeSearchListType::iterator l_Itr = NodeHashList[ slot ].begin();
		for( l_Itr; l_Itr != NodeHashList[ slot ].end(); ++l_Itr)
		{
			if( l_Itr->first == v )
			{
				return l_Itr->second;
			}
		}
	}
	
	return -1;
}

int NeuronStruct::getBranchID( int i )	
{
	if( i >= 0 && i < Branches.size() )
		return Branches[i].getID();
	
	return -1;
}

int NeuronStruct::getBranchIndex( int id )	
{
	std::map<int, int>::iterator Itr = BranchIdxTable.find( id );
	if( Itr != BranchIdxTable.end() )
		return Itr->second;

	return -1;
}

bool NeuronStruct::getBranchType( int BranchID, BranchType& type)
{
	std::map<int, int>::iterator Itr = BranchIdxTable.find( BranchID );

	if( Itr != BranchIdxTable.end() )
	{
		type = Branches[ Itr->second ].getType();
		return true;
	}

	return false;
}

void NeuronStruct::setBranchType( int BranchID, BranchType type )
{
	std::map<int, int>::iterator Itr = BranchIdxTable.find( BranchID );

	if( Itr != BranchIdxTable.end() )
		Branches[ Itr->second ].setType( type );
}

void NeuronStruct::NonOverlapVariation( NeuronStruct& N, std::vector< std::pair< int, double> >& Variation )
{
	//In order make the sum of all brnahces variations equals the difference of neuron sizes,  
	//The variation is defined as the partial branch staring at the tip and end at the farthest branch point or soma.
	//The sum of the variation is well-defined; however, the mapping between branchs and variations is ambiguous.
	
	//The results were "std::pairs" stored in a vector, Variation, and the "first" represents the branch ID and the "second" is the length variation


	bool* visited = new bool [ N.getNumBranches() ];
	memset( visited, 0, sizeof( bool ) * N.getNumBranches() );

	std::vector< std::pair<int, int> > matchingPairs;	//store the matched branches' array index
	std::vector< int > retractedIDs;
	std::vector< int > addedIDs;

	for(int i=0; i<Branches.size(); ++i)
	{
		int idxThis = Branches[i].getID();
		bool HIT = false;
		for(int j=0; j<N.getNumBranches() && !HIT; ++j)
		{
			int idxN = N.getBranchID( j );

			if( idxThis == idxN )
			{
				matchingPairs.push_back( std::pair<int, int>(i, j) );

				Branches[ i ].setType( BranchType::TRANSIENT );
				N.setBranchType( idxN, BranchType::TRANSIENT );

				visited[ j ] = true;
				HIT = true;
			}
		}

		if( !HIT )
		{
			retractedIDs.push_back( i );
			Branches[i].setType( BranchType::RETRACT );
		}
	}

	for(int i=0; i<N.getNumBranches(); ++i)
	{
		if( !visited[i] )
		{
			addedIDs.push_back( i );
			N.setBranchType( N.getBranchID( i ), BranchType::NEW );
		}
	}
	delete [] visited;

	bool* visitedThis = new bool [ Nodes.size() ];
	bool* visitedN = new bool [ N.getNumNodes() ];

	memset( visitedN, 0, sizeof( bool ) * N.getNumNodes() );
	memset( visitedThis, 0, sizeof( bool ) * Nodes.size() );
	
	//Compute the variations of the remain branches first
	for(int i=0; i<matchingPairs.size(); ++i)
	{
		int idxThis = matchingPairs[i].first;
		int idxN = matchingPairs[i].second;

		double lengthThis = .0;
		double lengthN = .0;
		for(int j=Branches[ idxThis ].points.size() - 1; j > 0; --j)	//backtrack from tip to soma
		{
			VectorCoordinate v1 = Branches[ idxThis ].points[ j ];
			VectorCoordinate v2 = Branches[ idxThis ].points[ j - 1 ];

			int nodeIdx1 = queryNodeIdx( v1 );
			
			if( nodeIdx1 >= 0 )
			{
				if( !visitedThis[ nodeIdx1 ] )
				{
					visitedThis[ nodeIdx1 ] = true;
					lengthThis += ( v2 - v1 ).Length();
				}
				else
					break;
			}
		}

		NeuronBranch B;
		N.getBranch( idxN, B );

		for(int j=B.points.size() - 1 ; j > 0; --j)
		{
			VectorCoordinate v1 = B.points[ j ];
			VectorCoordinate v2 = B.points[ j - 1 ];

			int nodeIdx1 = N.queryNodeIdx( v1 );
		
			if( nodeIdx1 >= 0 )
			{
				if( !visitedN[ nodeIdx1 ] )
				{
					visitedN[ nodeIdx1 ] = true;
					lengthN += ( v2 - v1 ).Length();
				}
				else
					break;
			}
		}

		Variation.push_back( std::pair<int, double>( idxThis, lengthN - lengthThis ) );
	}

	//Compute the variations of the retracted branches
	for(int i=0; i<retractedIDs.size(); ++i)
	{
		int idxThis = retractedIDs[i];

		double lengthThis = .0;
		for(int j=Branches[ idxThis ].points.size() - 1; j > 0; --j)	//backtrack from tip to soma
		{
			VectorCoordinate v1 = Branches[ idxThis ].points[ j ];
			VectorCoordinate v2 = Branches[ idxThis ].points[ j - 1 ];

			int nodeIdx1 = queryNodeIdx( v1 );

			if( nodeIdx1 >= 0 )
			{
				if( !visitedThis[ nodeIdx1 ]  )
				{
					visitedThis[ nodeIdx1 ] = true;
					lengthThis += ( v2 - v1 ).Length();
				}
			}
		}

		Variation.push_back( std::pair<int, double>( idxThis, -lengthThis ) );
	}

	//Compute the variations of the added branches
	for(int i=0; i<addedIDs.size(); ++i)
	{
		int idxN = addedIDs[i];

		NeuronBranch B;
		N.getBranch( idxN, B );
		double lengthN = .0;
		for(int j=B.points.size() - 1 ; j > 0; --j)
		{
			VectorCoordinate v1 = B.points[ j ];
			VectorCoordinate v2 = B.points[ j - 1 ];

			int nodeIdx1 = N.queryNodeIdx( v1 );
			
			if( nodeIdx1 >= 0 )
			{
				if( !visitedN[ nodeIdx1 ]  )
				{
					visitedN[ nodeIdx1 ] = true;
					lengthN += ( v2 - v1 ).Length();
				}
			}
		}

		Variation.push_back( std::pair<int, double>( idxN, lengthN ) );
	}

	delete [] visitedN;
	delete [] visitedThis;


}


void NeuronStruct::OverlapVariation( NeuronStruct& N, std::vector< std::pair< int, double> >& Variation )
{
	//The sum of all brnahces variations will not equal the difference of neuron sizes, unless there is no overlap partial branches.  
	//The variation is defined as the line segment sum from tip to soma.
	
	//The results were "std::pairs" stored in a vector, Variation, and the "first" represents the branch ID and the "second" is the length variation


	bool* visited = new bool [ N.getNumBranches() ];
	memset( visited, 0, sizeof( bool ) * N.getNumBranches() );

	std::vector< std::pair<int, int> > matchingPairs;	//store the matched branches' array index
	std::vector< int > retractedIDs;
	std::vector< int > addedIDs;

	for(int i=0; i<Branches.size(); ++i)
	{
		int idxThis = Branches[i].getID();
		bool HIT = false;
		for(int j=0; j<N.getNumBranches() && !HIT; ++j)
		{
			int idxN = N.getBranchID( j );

			if( idxThis == idxN )
			{
				matchingPairs.push_back( std::pair<int, int>(i, j) );

				Branches[ i ].setType( BranchType::TRANSIENT );
				N.setBranchType( idxN, BranchType::TRANSIENT );

				visited[ j ] = true;
				HIT = true;
			}
		}

		if( !HIT )
		{
			retractedIDs.push_back( i );
			Branches[i].setType( BranchType::RETRACT );
		}
	}

	for(int i=0; i<N.getNumBranches(); ++i)
	{
		if( !visited[i] )
		{
			addedIDs.push_back( i );
			N.setBranchType( N.getBranchID( i ), BranchType::NEW );
		}
	}
	delete [] visited;


	//Compute the variations of the remain branches first
	for(int i=0; i<matchingPairs.size(); ++i)
	{
		int idxThis = matchingPairs[i].first;
		int idxN = matchingPairs[i].second;

		double lengthThis = .0;
		double lengthN = .0;
		for(int j=1; j<Branches[ idxThis ].points.size(); ++j)	//backtrack from soma to tip
		{
			VectorCoordinate v1 = Branches[ idxThis ].points[ j ];
			VectorCoordinate v2 = Branches[ idxThis ].points[ j - 1 ];

			lengthThis += ( v2 - v1 ).Length();
		}

		NeuronBranch B;
		N.getBranch( idxN, B );

		for(int j=1 ; j < B.points.size(); ++j)
		{
			VectorCoordinate v1 = B.points[ j ];
			VectorCoordinate v2 = B.points[ j - 1 ];
			
			lengthN += ( v2 - v1 ).Length();
		}

		Variation.push_back( std::pair<int, double>( idxThis, lengthN - lengthThis ) );
	}

	//Compute the variations of the retracted branches
	for(int i=0; i<retractedIDs.size(); ++i)
	{
		int idxThis = retractedIDs[i];

		double lengthThis = .0;
		for(int j=1; j < Branches[ idxThis ].points.size(); ++j)	//track from soma to tip
		{
			VectorCoordinate v1 = Branches[ idxThis ].points[ j ];
			VectorCoordinate v2 = Branches[ idxThis ].points[ j - 1 ];

			lengthThis += ( v2 - v1 ).Length();
			
		}

		Variation.push_back( std::pair<int, double>( idxThis, -lengthThis ) );
	}

	//Compute the variations of the added branches
	for(int i=0; i<addedIDs.size(); ++i)
	{
		int idxN = addedIDs[i];

		NeuronBranch B;
		N.getBranch( idxN, B );
		double lengthN = .0;
		for(int j=1; j < B.points.size(); ++j)	//track from soma to tip
		{
			VectorCoordinate v1 = B.points[ j ];
			VectorCoordinate v2 = B.points[ j - 1 ];

			lengthN += ( v2 - v1 ).Length();
		}

		Variation.push_back( std::pair<int, double>( idxN, lengthN ) );
	}

}
