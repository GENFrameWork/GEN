/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPVectorFileDXFEntityObjLWPolyLine.cpp
* 
* @class      GRPVECTORFILEDXFENTITYOBJLWPOLYLINE
* @brief      Graphic Vector File DXF Entity Obj LWPolyLine class
* @ingroup    GRAPHIC
* 
* @copyright  GEN Group. All rights reserved.
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
#pragma region PRECOMPILATION_INCLUDES

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "GRPVectorFileDXFEntityObjLWPolyLine.h"

#include "GRPVectorFileDXFTextSectionEntities.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GRPVECTORFILEDXFENTITYOBJLWPOLYLINE(XCHAR* nametype)
* @brief      Constructor
* @ingroup    GRAPHIC
* 
* @param[in]  XCHAR* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GRPVECTORFILEDXFENTITYOBJLWPOLYLINE(XCHAR* nametype) : GRPVECTORFILEDXFENTITYOBJ (nametype)
{
  Clean();   

  type = GRPVECTORFILEDXFENTITYOBJTYPE_LWPOLYLINE;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::~GRPVECTORFILEDXFENTITYOBJLWPOLYLINE()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::~GRPVECTORFILEDXFENTITYOBJLWPOLYLINE()
{ 
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetNVertex()
* @brief      GetNVertex
* @ingroup    GRAPHIC
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetNVertex()
{
  return nvertex;
}
      
      
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetNVertex(int nvertex)
* @brief      SetNVertex
* @ingroup    GRAPHIC
* 
* @param[in]  nVertex : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetNVertex(int nvertex)
{
  this->nvertex = nvertex;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         int GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetPolyLineFlag()
* @brief      GetPolyLineFlag
* @ingroup    GRAPHIC
* 
* @return     int : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
int GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetPolyLineFlag()
{
  return polylineflag;
}
      
      
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetPolyLineFlag(int polylineflag)
* @brief      SetPolyLineFlag
* @ingroup    GRAPHIC
* 
* @param[in]  polyLineFlag : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetPolyLineFlag(int polylineflag)
{
  this->polylineflag = polylineflag;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetConstantWidth()
* @brief      GetConstantWidth
* @ingroup    GRAPHIC
* 
* @return     double : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetConstantWidth()
{
  return constantwidth;
}
      
      
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetConstantWidth(double constantwidth)
* @brief      SetConstantWidth
* @ingroup    GRAPHIC
* 
* @param[in]  constantWidth : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetConstantWidth(double constantwidth)
{
  this->constantwidth = constantwidth;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetElevation()
* @brief      GetElevation
* @ingroup    GRAPHIC
* 
* @return     double : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetElevation()
{
  return elevation;
}
      
      
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetElevation(double elevation)
* @brief      SetElevation
* @ingroup    GRAPHIC
* 
* @param[in]  elevation : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetElevation(double elevation)
{
  this->elevation = elevation;
}
      
      
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetThickness()
* @brief      GetThickness
* @ingroup    GRAPHIC
* 
* @return     double : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetThickness()
{
  return thickness;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetThickness(double thickness)
* @brief      SetThickness
* @ingroup    GRAPHIC
* 
* @param[in]  thickness : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetThickness(double thickness)
{
  this->thickness = thickness;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITYDEFPOINT* GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetDirExtrusion()
* @brief      GetDirExtrusion
* @ingroup    GRAPHIC
* 
* @return     GRPVECTORFILEDXFENTITYDEFPOINT* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITYDEFPOINT* GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::GetDirExtrusion()
{
  return &dirextrusion;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetDirExtrusion(GRPVECTORFILEDXFENTITYDEFPOINT* dirextrusion)
* @brief      SetDirExtrusion
* @ingroup    GRAPHIC
* 
* @param[in]  dirextrusion : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::SetDirExtrusion(GRPVECTORFILEDXFENTITYDEFPOINT* dirextrusion)
{
  if(!dirextrusion) 
    {
      return false;
    }
  
  this->dirextrusion = dirextrusion;
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::ApplyData(GRPVECTORFILEDXFENTITY* entity)
* @brief      ApplyData
* @ingroup    GRAPHIC
* 
* @param[in]  entity : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::ApplyData(GRPVECTORFILEDXFENTITY* entity)
{
  GRPVECTORFILEDXFVALUE* value;

  GRPVECTORFILEDXFENTITYOBJ::ApplyData(entity);

  value = GetDataValue( 90, entity);  if(value) nvertex       = (*value->GetData());

  value = GetDataValue( 70, entity);  if(value) polylineflag  = (*value->GetData());
  value = GetDataValue( 43, entity);  if(value) constantwidth = (*value->GetData());
  value = GetDataValue( 38, entity);  if(value) elevation     = (*value->GetData());

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
* @fn         void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJLWPOLYLINE::Clean()
{
  thickness = 0.0f;    
}

#pragma endregion
