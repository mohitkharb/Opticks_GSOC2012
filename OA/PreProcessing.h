

#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include "ExecutableShell.h"

class PreProcessing : public ExecutableShell
{

public:
		int Rows;
	int Cols;
	std::vector<double> dataR;
	std::vector<double> dataG;
	std::vector<double> dataB;

	void CalculateMeanshift();
   PreProcessing();
   ~PreProcessing();
   bool getInputSpecification(PlugInArgList*& pInArgList);
   bool getOutputSpecification(PlugInArgList*& pOutArgList);
   bool execute(PlugInArgList* pInArgList, PlugInArgList* pOutArgList);


};

#endif
