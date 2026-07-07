/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPVectorFileDXFEntityObjPolyLine.cpp
* 
* @class      GRPVECTORFILEDXFENTITYOBJPOLYLINE
* @brief      Graphic Vector File DXF Entity Obj PolyLine class
* @ingroup    GRAPHIC
* 
* @copyright  EndoraSoft. All rights reserved.
* 
* @cond
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
* documentation files(the "Software"), to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense, and/ or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of
* the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
* THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* @endcond
* 
* --------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"



/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "GRPVectorFileDXFEntityObjPolyLine.h"

#include "GRPVectorFileDXFTextSectionEntities.h"



/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/

#include "GEN_Control.h"




/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/



/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITYOBJPOLYLINE::GRPVECTORFILEDXFENTITYOBJPOLYLINE(XCHAR* nametype) : GRPVECTORFILEDXFENTITYOBJ(nametype)
* @brief      Constructor of class
* @ingroup    GRAPHIC
* 
* @param[in]  nametype : Nametype pointer to use.
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITYOBJPOLYLINE::GRPVECTORFILEDXFENTITYOBJPOLYLINE(XCHAR* nametype) : GRPVECTORFILEDXFENTITYOBJ(nametype)
{
  Clean();   

  type = GRPVECTORFILEDXFENTITYOBJTYPE_POLYLINE;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITYOBJPOLYLINE::~GRPVECTORFILEDXFENTITYOBJPOLYLINE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITYOBJPOLYLINE::~GRPVECTORFILEDXFENTITYOBJPOLYLINE()
{ 
  Clean();
}

 
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double GRPVECTORFILEDXFENTITYOBJPOLYLINE::GetThickness()
* @brief      Get thickness
* @ingroup    GRAPHIC
* 
* @return     double : Requested value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILEDXFENTITYOBJPOLYLINE::GetThickness()
{
  return thickness;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITYOBJPOLYLINE::SetThickness(double thickness)
* @brief      Set thickness
* @ingroup    GRAPHIC
* 
* @param[in]  thickness : Thickness value.
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJPOLYLINE::SetThickness(double thickness)
{
  this->thickness = thickness;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITYDEFPOINT* GRPVECTORFILEDXFENTITYOBJPOLYLINE::GetDirExtrusion()
* @brief      Get dir extrusion
* @ingroup    GRAPHIC
* 
* @return     GRPVECTORFILEDXFENTITYDEFPOINT* : Pointer to the requested object; NULL if it is not available.
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITYDEFPOINT* GRPVECTORFILEDXFENTITYOBJPOLYLINE::GetDirExtrusion()
{
  return &dirextrusion;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFENTITYOBJPOLYLINE::SetDirExtrusion(GRPVECTORFILEDXFENTITYDEFPOINT* dirextrusion)
* @brief      Set dir extrusion
* @ingroup    GRAPHIC
* 
* @param[in]  dirextrusion : Dirextrusion pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFENTITYOBJPOLYLINE::SetDirExtrusion(GRPVECTORFILEDXFENTITYDEFPOINT* dirextrusion)
{
  if(!dirextrusion) return false;

  this->dirextrusion = dirextrusion;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFENTITYOBJPOLYLINE::ApplyData(GRPVECTORFILEDXFENTITY* entity)
* @brief      Apply data
* @ingroup    GRAPHIC
* 
* @param[in]  entity : Entity pointer to use.
* 
* @return     bool : true if the operation is successful; otherwise false.
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFENTITYOBJPOLYLINE::ApplyData(GRPVECTORFILEDXFENTITY* entity)
{
  GRPVECTORFILEDXFVALUE* value;

  GRPVECTORFILEDXFENTITYOBJ::ApplyData(entity);

  // INCOMPLETE

  value = GetDataValue(VFDXF_THICKNESS, entity);  
  if(value) 
    {
      thickness = (*value->GetData());
    }

  value = GetDataValue(VFDXF_EXTRUSION_DIRECTION_X, entity);  
  if(value) 
    {
      dirextrusion.SetX((*value->GetData()));      
    }

  value = GetDataValue(VFDXF_EXTRUSION_DIRECTION_Y, entity);  
  if(value) 
    {
      dirextrusion.SetY((*value->GetData()));      
    }

  value = GetDataValue(VFDXF_EXTRUSION_DIRECTION_Z, entity);  
  if(value) 
    {
      dirextrusion.SetY((*value->GetData()));      
    }
      
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITYOBJPOLYLINE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJPOLYLINE::Clean()
{
   
}



