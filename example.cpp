#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "NeuronStructure.h"


int main()
{
	std::string nameT1 = "TG4037N21.txt";
	std::string nameT2 = "TG4037N22_matched.txt";
	
	NeuronStruct N1( nameT1.c_str() );
	NeuronStruct N2( nameT2.c_str() );

	std::cout<<N1.getNeuronSize()<<'\n';
	std::cout<<N2.getNeuronSize()<<'\n';

	
	std::vector< std::pair<int, double> > nonOverlapVarations;
	N1.NonOverlapVariation( N2, nonOverlapVarations);

	double sum = N1.getNeuronSize();
	for(int i=0; i<nonOverlapVarations.size(); ++i)
	{
		sum += nonOverlapVarations[i].second;
	}

	std::cout<<"Total Length N1 + sum( variations ) = "<<sum<<'\n';

	nonOverlapVarations.clear();
	N2.NonOverlapVariation( N1, nonOverlapVarations);

	sum = N2.getNeuronSize();
	for(int i=0; i<nonOverlapVarations.size(); ++i)
	{
		sum += nonOverlapVarations[i].second;
	}

	std::cout<<"Total Length N2 + sum( variations ) = "<<sum<<'\n';
	return 0;
}
