/*
 * The information in this file is
 * subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "AppConfig.h"
#include "AppVerify.h"
#include "DataAccessor.h"
#include "DataAccessorImpl.h"
#include "DataRequest.h"
#include "MessageLogResource.h"
#include "ObjectResource.h"
#include "PlugInArgList.h"
#include "PlugInManagerServices.h"
#include "PlugInRegistration.h"
#include "PlugInResource.h"
#include "Progress.h"
#include <vector>
#include "RasterDataDescriptor.h"
#include "RasterElement.h"
#include "StringUtilities.h"
#include "switchOnEncoding.h"
#include <limits>
#include "PreProcessing.h"

REGISTER_PLUGIN_BASIC(OpticksOBIAPlugin, PreProcessing);
namespace
{
   template<typename T>
   void updateStatistics(T* pData, double& total, std::vector<double>& data)
   {

	  total += *pData;
	  data.push_back(static_cast<double>(*pData));
   }
};
PreProcessing::PreProcessing() 
{
 setDescriptorId("{DF7B5AEF-B156-40ae-8DF6-6B3D5BC964E1}");
   setName("Reading Image Data");
   setCreator("Mohit Kumar");
   setType("Sample");
   setCopyright("Copyright (C) 2012, Lab For Spatial Informatics");
   setProductionStatus(false);
   setDescription("Verifies Requirement 324.");
   setShortDescription("Verifies Requirement 324.");
   setMenuLocation("[OBIA]\\Read Data");
   setWizardSupported(false);
}

PreProcessing::~PreProcessing()
{
}

bool PreProcessing::getInputSpecification(PlugInArgList* &pInArgList)
{
   pInArgList = Service<PlugInManagerServices>()->getPlugInArgList();
   VERIFY(pInArgList != NULL);
   pInArgList->addArg<Progress>(Executable::ProgressArg(), NULL, "Progress reporter");
   pInArgList->addArg<RasterElement>(Executable::DataElementArg(), "Generate statistics for this raster element");
   return true;
}

bool PreProcessing::getOutputSpecification(PlugInArgList*& pOutArgList)
{
   pOutArgList = Service<PlugInManagerServices>()->getPlugInArgList();
   VERIFY(pOutArgList != NULL);
   pOutArgList->addArg<std::vector<double> >("values Red","The values of data");
   pOutArgList->addArg<std::vector<double> >("values Green","The values of data");
   pOutArgList->addArg<std::vector<double> >("values Blue","The values of data");
   return true;
}


bool PreProcessing::execute(PlugInArgList* pInArgList, PlugInArgList* pOutArgList)
{
StepResource pStep("Reading Image Data", "app", "27170298-10CE-4E6C-AD7A-97E8058C29FF");
   if (pInArgList == NULL || pOutArgList == NULL)
   {
      return false;
   }
   Progress* pProgress = pInArgList->getPlugInArgValue<Progress>(Executable::ProgressArg());
   RasterElement* pCube = pInArgList->getPlugInArgValue<RasterElement>(Executable::DataElementArg());
   if (pCube == NULL)
   {
      std::string msg = "A raster cube must be specified.";
      pStep->finalize(Message::Failure, msg);
      if (pProgress != NULL)
      {
         pProgress->updateProgress(msg, 0, ERRORS);
      }

      return false;
   }
   RasterDataDescriptor* pDesc = static_cast<RasterDataDescriptor*>(pCube->getDataDescriptor());
   VERIFY(pDesc != NULL);
   
   FactoryResource<DataRequest> pRequestR, pRequestG, pRequestB;
   pRequestR->setInterleaveFormat(BSQ);
   pRequestG->setInterleaveFormat(BSQ);
   pRequestB->setInterleaveFormat(BSQ);

   
   

   

   pRequestR->setBands(pDesc->getActiveBand(0),pDesc->getActiveBand(0));
   DataAccessor pAccR = pCube->getDataAccessor(pRequestR.release());
   pRequestG->setBands(pDesc->getActiveBand(1),pDesc->getActiveBand(1));
   DataAccessor pAccG = pCube->getDataAccessor(pRequestG.release());
   pRequestB->setBands(pDesc->getActiveBand(2),pDesc->getActiveBand(2));
   DataAccessor pAccB = pCube->getDataAccessor(pRequestB.release());

   double totalR = 0.0;
   double totalG = 0.0;
   double totalB = 0.0;
   std::vector<double> dataR;
   std::vector<double> dataG;
   std::vector<double> dataB;
   for (unsigned int row = 0; row < pDesc->getRowCount(); ++row)
   {
      if (isAborted())
      {
         std::string msg = getName() + " has been aborted.";
         pStep->finalize(Message::Abort, msg);
         if (pProgress != NULL)
         {
            pProgress->updateProgress(msg, 0, ABORT);
         }

         return false;
      }
      if (!pAccR.isValid() || !pAccG.isValid() || !pAccB.isValid())
      {
         std::string msg = "Unable to access the cube data.";
         pStep->finalize(Message::Failure, msg);
         if (pProgress != NULL)
         {
            pProgress->updateProgress(msg, 0, ERRORS);
         }

         return false;
      }

      if (pProgress != NULL)
      {
         pProgress->updateProgress("Calculating statistics", row * 100 / pDesc->getRowCount(), NORMAL);
      }

      for (unsigned int col = 0; col < pDesc->getColumnCount(); ++col)
      {
         switchOnEncoding(pDesc->getDataType(), updateStatistics, pAccR->getColumn(), totalR,dataR);
         pAccR->nextColumn();
	 switchOnEncoding(pDesc->getDataType(), updateStatistics, pAccG->getColumn(), totalG,dataG);
	 pAccG->nextColumn();
	 switchOnEncoding(pDesc->getDataType(), updateStatistics, pAccB->getColumn(), totalB,dataB);
	 pAccB->nextColumn();
      }
      pAccR->nextRow();
	  pAccG->nextRow();
	  pAccB->nextRow();
   }
   unsigned int count = pDesc->getColumnCount() * pDesc->getRowCount();
   
   double meanR = totalR / count;
   double meanG = totalG / count;
   double meanB = totalB / count;

   if (pProgress != NULL)
   {
      std::string msg = "Number of pixels: " + StringUtilities::toDisplayString(count) + "\n"
			+ "Number of bands : "+ StringUtilities::toDisplayString(pDesc->getBandCount()) +"\n"
                        + "Averages: " + StringUtilities::toDisplayString(meanR) +" : "+ StringUtilities::toDisplayString(meanG) +" : "+ StringUtilities::toDisplayString(meanB) +"\n"
					  + "Values of first pixel: " + StringUtilities::toDisplayString(dataR.front())+" : "+ StringUtilities::toDisplayString(dataG.front())+" : "+ StringUtilities::toDisplayString(dataB.front());
      pProgress->updateProgress(msg, 100, NORMAL);
   }
   pStep->addProperty("values Red", dataR);
   pStep->addProperty("values Green", dataG);
   pStep->addProperty("values Blue", dataB);
   

   pOutArgList->setPlugInArgValue("values Red", &dataR);
   pOutArgList->setPlugInArgValue("values Green", &dataG);
   pOutArgList->setPlugInArgValue("values Blue", &dataB);

   pStep->finalize();
   return true;

	

   

}



