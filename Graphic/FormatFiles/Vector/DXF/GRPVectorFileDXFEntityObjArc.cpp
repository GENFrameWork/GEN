/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRPVectorFileDXFEntityObjArc.cpp
* 
* @class      GRPVECTORFILEDXFENTITYOBJARC
* @brief      Graphic Vector File DXF Entity Obj Arc class
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
#pragma region PRECOMPILATION_DEFINES_INCLUDE

#include "GEN_Defines.h"

#pragma endregion


/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/
#pragma region INCLUDES

#include "GRPVectorFileDXFEntityObjArc.h"

#include "GRPVectorFileDXFTextSectionEntities.h"

#pragma endregion


/*---- PRECOMPILATION INCLUDES ---------------------------------------------------------------------------------------*/
#pragma region PRECOMPILATION_CONTROL_INCLUDE

#include "GEN_Control.h"

#pragma endregion



/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITYOBJARC::GRPVECTORFILEDXFENTITYOBJARC(XCHAR* nametype)
* @brief      Constructor of class
* @ingroup    GRAPHIC
* 
* @param[in]  nametype : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITYOBJARC::GRPVECTORFILEDXFENTITYOBJARC(XCHAR* nametype) : GRPVECTORFILEDXFENTITYOBJ(nametype)
{
  Clean();   

  type = GRPVECTORFILEDXFENTITYOBJTYPE_ARC;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITYOBJARC::~GRPVECTORFILEDXFENTITYOBJARC()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITYOBJARC::~GRPVECTORFILEDXFENTITYOBJARC()
{ 
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITYDEFPOINT* GRPVECTORFILEDXFENTITYOBJARC::GetCenter()
* @brief      Get center
* @ingroup    GRAPHIC
* 
* @return     GRPVECTORFILEDXFENTITYDEFPOINT* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITYDEFPOINT* GRPVECTORFILEDXFENTITYOBJARC::GetCenter()
{
  return &center;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFENTITYOBJARC::SetCenter(GRPVECTORFILEDXFENTITYDEFPOINT* center)
* @brief      Set center
* @ingroup    GRAPHIC
* 
* @param[in]  center : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFENTITYOBJARC::SetCenter(GRPVECTORFILEDXFENTITYDEFPOINT* center)
{
  if(!center) 
    {
      return false;
    }

  this->center = center;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double GRPVECTORFILEDXFENTITYOBJARC::GetIniAngle()
* @brief      Get ini angle
* @ingroup    GRAPHIC
* 
* @return     double : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILEDXFENTITYOBJARC::GetIniAngle()
{
  return iniangle;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITYOBJARC::SetIniAngle(double angle)
* @brief      Set ini angle
* @ingroup    GRAPHIC
* 
* @param[in]  angle : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJARC::SetIniAngle(double angle)
{
  this->iniangle = angle;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double GRPVECTORFILEDXFENTITYOBJARC::GetEndAngle()
* @brief      Get end angle
* @ingroup    GRAPHIC
* 
* @return     double : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILEDXFENTITYOBJARC::GetEndAngle()
{
  return endangle;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITYOBJARC::SetEndAngle(double angle)
* @brief      Set end angle
* @ingroup    GRAPHIC
* 
* @param[in]  angle : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJARC::SetEndAngle(double angle)
{
  this->endangle = angle;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double GRPVECTORFILEDXFENTITYOBJARC::GetRadius()
* @brief      Get radius
* @ingroup    GRAPHIC
* 
* @return     double : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILEDXFENTITYOBJARC::GetRadius()
{
  return radius;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITYOBJARC::SetRadius(double radius)
* @brief      Set radius
* @ingroup    GRAPHIC
* 
* @param[in]  radius : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJARC::SetRadius(double radius)
{
  this->radius = radius;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         double GRPVECTORFILEDXFENTITYOBJARC::GetThickness()
* @brief      Get thickness
* @ingroup    GRAPHIC
* 
* @return     double : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
double GRPVECTORFILEDXFENTITYOBJARC::GetThickness()
{
  return thickness;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRPVECTORFILEDXFENTITYOBJARC::SetThickness(double thickness)
* @brief      Set thickness
* @ingroup    GRAPHIC
* 
* @param[in]  thickness : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJARC::SetThickness(double thickness)
{
  this->thickness = thickness;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         GRPVECTORFILEDXFENTITYDEFPOINT* GRPVECTORFILEDXFENTITYOBJARC::GetDirExtrusion()
* @brief      Get dir extrusion
* @ingroup    GRAPHIC
* 
* @return     GRPVECTORFILEDXFENTITYDEFPOINT* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
GRPVECTORFILEDXFENTITYDEFPOINT* GRPVECTORFILEDXFENTITYOBJARC::GetDirExtrusion()
{
  return &dirextrusion;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRPVECTORFILEDXFENTITYOBJARC::SetDirExtrusion(GRPVECTORFILEDXFENTITYDEFPOINT* dirextrusion)
* @brief      Set dir extrusion
* @ingroup    GRAPHIC
* 
* @param[in]  dirextrusion : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFENTITYOBJARC::SetDirExtrusion(GRPVECTORFILEDXFENTITYDEFPOINT* dirextrusion)
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
* @fn         bool GRPVECTORFILEDXFENTITYOBJARC::ApplyData(GRPVECTORFILEDXFENTITY* entity)
* @brief      Apply data
* @ingroup    GRAPHIC
* 
* @param[in]  entity : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRPVECTORFILEDXFENTITYOBJARC::ApplyData(GRPVECTORFILEDXFENTITY* entity)
{
  GRPVECTORFILEDXFVALUE* value;

  GRPVECTORFILEDXFENTITYOBJ::ApplyData(entity);
   
  value = GetDataValue( VFDXF_CENTER_POINT_X, entity); 
  if(value) 
    {
      center.SetX((*value->GetData()));         
    }

  value = GetDataValue( VFDXF_CENTER_POINT_Y, entity); 
  if(value) 
    {
      center.SetY((*value->GetData()));      
    }

  value = GetDataValue( VFDXF_CENTER_POINT_Z, entity); 
  if(value) 
    {
      center.SetZ((*value->GetData()));             
    }
 
  value = GetDataValue( VFDXF_RADIOUS, entity); 
  if(value) 
    {
      radius =(*value->GetData());      
    }
   
  value = GetDataValue(VFDXF_INI_ANGLE , entity); 
  if(value) 
    {
      iniangle =(*value->GetData());      
    }

  value = GetDataValue(VFDXF_END_ANGLE , entity); 
  if(value) 
    {
      endangle =(*value->GetData());      
    }
   
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
* @fn         void GRPVECTORFILEDXFENTITYOBJARC::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    GRAPHIC
* 
* --------------------------------------------------------------------------------------------------------------------*/
void GRPVECTORFILEDXFENTITYOBJARC::Clean()
{
  radius = 0.0f;

  iniangle = 0.0f; 
  endangle = 0.0f;   

  thickness= 0.0f;
}


#pragma endregion

