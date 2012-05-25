

#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include "ExecutableShell.h"

class PreProcessing : public ExecutableShell
{
   

public:
   PreProcessing();
   ~PreProcessing();
   bool getInputSpecification(PlugInArgList*& pInArgList);
   bool getOutputSpecification(PlugInArgList*& pOutArgList);
   bool execute(PlugInArgList* pInArgList, PlugInArgList* pOutArgList);


};

#endif
