/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       GRP2DRebuildAreas.cpp
* 
* @class      GRP2DREBUILDAREAS
* @brief      Graphics 2D Rebuild Areas class
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

#include "GRP2DRebuildAreas.h"

#include "GRPCanvas.h"

#include "XMemory_Control.h"

#pragma endregion


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/
#pragma region GENERAL_VARIABLE

#pragma endregion


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/
#pragma region CLASS_MEMBERS


#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRP2DREBUILDAREA::GRP2DREBUILDAREA()
* @brief      Constructor
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DREBUILDAREA::GRP2DREBUILDAREA()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRP2DREBUILDAREA::~GRP2DREBUILDAREA()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DREBUILDAREA::~GRP2DREBUILDAREA()
{
  if(bitmap)
    {
      delete bitmap;
      bitmap = NULL;
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double GRP2DREBUILDAREA::GetXPos()
* @brief      GetXPos
* @ingroup    GRAPHIC
*
* @return     double :
*
* --------------------------------------------------------------------------------------------------------------------*/
double GRP2DREBUILDAREA::GetXPos()
{
  return xpos;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         double GRP2DREBUILDAREA::GetYPos()
* @brief      GetYPos
* @ingroup    GRAPHIC
*
* @return     double :
*
* --------------------------------------------------------------------------------------------------------------------*/
double GRP2DREBUILDAREA::GetYPos()
{
  return ypos;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRP2DREBUILDAREA::SetXPos(double xpos)
* @brief      SetXPos
* @ingroup    GRAPHIC
*
* @param[in]  xpos :
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DREBUILDAREA::SetXPos(double xpos)
{
  this->xpos = xpos;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRP2DREBUILDAREA::SetYPos(double ypos)
* @brief      SetYPos
* @ingroup    GRAPHIC
*
* @param[in]  ypos :
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DREBUILDAREA::SetYPos(double ypos)
{
  this->ypos = ypos;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAP* GRP2DREBUILDAREA::GetBitmap()
* @brief      GetBitmap
* @ingroup    GRAPHIC
*
* @return     GRPBITMAP* :
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAP* GRP2DREBUILDAREA::GetBitmap()
{
  return bitmap;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRP2DREBUILDAREA::SetBitmap(GRPBITMAP* bitmap)
* @brief      SetBitmap
* @ingroup    GRAPHIC
*
* @param[in]  bitmap :
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DREBUILDAREA::SetBitmap(GRPBITMAP* bitmap)
{
  this->bitmap = bitmap;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void* GRP2DREBUILDAREA::GetExtraData()
* @brief      GetExtraData
* @ingroup    GRAPHIC
*
* @return     void* : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void* GRP2DREBUILDAREA::GetExtraData()
{
  return extra_data;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void GRP2DREBUILDAREA::SetExtraData(void* extra_data)
* @brief      SetExtraData
* @ingroup    GRAPHIC
*
* @param[in]  extra_data : 
* 
* ---------------------------------------------------------------------------------------------------------------------*/
void GRP2DREBUILDAREA::SetExtraData(void* extra_data)
{
  this->extra_data = extra_data;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRP2DREBUILDAREA::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DREBUILDAREA::Clean()
{
  xpos        = 0;
  ypos        = 0;

  bitmap      = NULL;

  extra_data  = NULL;
}


#pragma endregion


#pragma region CLASS_MEMBERS


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRP2DREBUILDAREAS::GRP2DREBUILDAREAS()
* @brief      Constructor
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DREBUILDAREAS::GRP2DREBUILDAREAS()
{
  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRP2DREBUILDAREAS::~GRP2DREBUILDAREAS()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
GRP2DREBUILDAREAS::~GRP2DREBUILDAREAS()
{
  DeleteAllRebuildAreas();

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD GRP2DREBUILDAREAS::GetExcessEdge()
* @brief      GetExcessEdge
* @ingroup    GRAPHIC
*
* @return     XDWORD :
*
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD GRP2DREBUILDAREAS::GetExcessEdge()
{
  return excessedge;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRP2DREBUILDAREAS::SetExcessEdge(XDWORD excessedge)
* @brief      SetExcessEdge
* @ingroup    GRAPHIC
*
* @param[in]  excessedge :
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DREBUILDAREAS::SetExcessEdge(XDWORD excessedge)
{
  this->excessedge = excessedge;
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XVECTOR<GRP2DREBUILDAREA*>* GRP2DREBUILDAREAS::GetRebuildAreas()
* @brief      GetRebuildAreas
* @ingroup    GRAPHIC
*
* @return     XVECTOR<GRP2DREBUILDAREA*>* :
*
* --------------------------------------------------------------------------------------------------------------------*/
XVECTOR<GRP2DREBUILDAREA*>* GRP2DREBUILDAREAS::GetRebuildAreas()
{
  return &areas;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRP2DREBUILDAREAS::RebuildAllAreas()
* @brief      RebuildAllAreas
* @ingroup    GRAPHIC
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DREBUILDAREAS::RebuildAllAreas()
{
  for(XDWORD c = areas.GetSize(); c>0;  c--)
    {
      GRP2DREBUILDAREA* area = areas.Get(c-1);
      if(area) PutBitmapNoAlpha(area->GetXPos(), area->GetYPos(), area->GetBitmap());
    }

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool GRP2DREBUILDAREAS::DeleteAllRebuildAreas()
* @brief      DeleteAllRebuildAreas
* @ingroup    GRAPHIC
*
* @return     bool : true if is succesful.
*
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DREBUILDAREAS::DeleteAllRebuildAreas()
{
  if(areas.IsEmpty()) return false;

  areas.DeleteContents();
  areas.DeleteAll();

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool GRP2DREBUILDAREAS::CreateRebuildArea(double x, double y, double width, double height, void* extra_data)
* @brief      CreateRebuildArea
* @ingroup    GRAPHIC
* 
* @param[in]  x : 
* @param[in]  y : 
* @param[in]  width : 
* @param[in]  height : 
* @param[in]  extra_data : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool GRP2DREBUILDAREAS::CreateRebuildArea(double x, double y, double width, double height, void* extra_data)
{
  double _x      = (x      - excessedge);
  double _y      = (y      - excessedge);
  double _width  = (width  + (excessedge*2));
  double _height = (height + (excessedge*2));

  if(_x < 0)  _x = 0;
  if(_y < 0)  _y = 0;

  GRPBITMAP* bitmap = GetBitmap(_x, _y, _width, _height);
  if(!bitmap) return false;

  GRP2DREBUILDAREA* newarea = new GRP2DREBUILDAREA();
  if(!newarea)
    {
      delete bitmap;
      return false;
    }

  newarea->SetXPos(_x);
  newarea->SetYPos(_y);
  newarea->SetBitmap(bitmap);
  newarea->SetExtraData(extra_data);

  return areas.Add(newarea);
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         GRPBITMAP* GRP2DREBUILDAREAS::GetBitmap(double x, double y, double width, double height)
* @brief      GetBitmap
* @ingroup    GRAPHIC
*
* @param[in]  x :
* @param[in]  y :
* @param[in]  width :
* @param[in]  height :
*
* @return     GRPBITMAP* :
*
* --------------------------------------------------------------------------------------------------------------------*/
GRPBITMAP* GRP2DREBUILDAREAS::GetBitmap(double x, double y, double width, double height)
{
  return NULL;
}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRP2DREBUILDAREAS::PutBitmapNoAlpha(double x, double y, GRPBITMAP* bitmap)
* @brief      PutBitmapNoAlpha
* @ingroup    GRAPHIC
*
* @param[in]  x :
* @param[in]  y :
* @param[in]  bitmap :
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DREBUILDAREAS::PutBitmapNoAlpha(double x, double y, GRPBITMAP* bitmap)
{

}


/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void GRP2DREBUILDAREAS::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    GRAPHIC
*
* --------------------------------------------------------------------------------------------------------------------*/
void GRP2DREBUILDAREAS::Clean()
{
  excessedge = GRP2DREBUILDAREAS_DEFAULT_EXCESSEDGE;
}


#pragma endregion


#pragma endregion





