/**-------------------------------------------------------------------------------------------------------------------
* 
* @file       DIONodeItemValue.cpp
* 
* @class      DIONODEITEMVALUE
* @brief      Data Input/Output Node Item Value class
* @ingroup    DATAIO
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

#include "DIONodeItemValue.h"

#include "XFactory.h"

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
* @fn         DIONODEITEMVALUE::DIONODEITEMVALUE()
* @brief      Constructor of class
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIONODEITEMVALUE::DIONODEITEMVALUE()
{
  Clean();

  datetime = GEN_XFACTORY.CreateDateTime();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIONODEITEMVALUE::~DIONODEITEMVALUE()
* @brief      Destructor of class
* @note       VIRTUAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIONODEITEMVALUE::~DIONODEITEMVALUE()
{
  if(datetime) 
    {
      GEN_XFACTORY.DeleteDateTime(datetime);
      datetime = NULL;
    }

  Clean();
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XDWORD DIONODEITEMVALUE::GetType()
* @brief      Get type
* @ingroup    DATAIO
* 
* @return     XDWORD : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XDWORD DIONODEITEMVALUE::GetType()
{
  return type;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIONODEITEMVALUE::SetType(DIONODEITEMVALUE_TYPE type)
* @brief      Set type
* @ingroup    DATAIO
* 
* @param[in]  type : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIONODEITEMVALUE::SetType(XDWORD type)
{
  this->type = type;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIONODEITEMVALUE::GetDescription(XSTRING& description)
* @brief      Get description
* @ingroup    DATAIO
* 
* @param[in]  description : 
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONODEITEMVALUE::GetDescription(XSTRING& description)
{
  description.Empty();

  switch(type)
    {
      case DIONODEITEMVALUE_TYPE_UNKNOWN        : description = __L("unknown"); 
                                                  break;
           
      case DIONODEITEMVALUE_TYPE_TEMPERATURE    : description = __L("temperature");     
                                                  break;

      case DIONODEITEMVALUE_TYPE_HUMIDITY       : description = __L("humidity");        
                                                  break; 

      case DIONODEITEMVALUE_TYPE_MOTIONDETECT   : description = __L("Motion Detect");        
                                                  break;

      case DIONODEITEMVALUE_TYPE_LIGHTSWITCH    : description = __L("Light Switch");        
                                                  break;

      case DIONODEITEMVALUE_TYPE_LIGHTLEVEL     : description = __L("light level");     
                                                  break;

                                      default   : break; 
    }

  if(description.IsEmpty()) return false;

  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIONODEITEMVALUE_MODE DIONODEITEMVALUE::GetMode()
* @brief      Get mode
* @ingroup    DATAIO
* 
* @return     DIONODEITEMVALUE_MODE : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIONODEITEMVALUE_MODE DIONODEITEMVALUE::GetMode()
{
  return mode;
}
    

/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIONODEITEMVALUE::SetMode(DIONODEITEMVALUE_MODE mode)
* @brief      Set mode
* @ingroup    DATAIO
* 
* @param[in]  mode : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIONODEITEMVALUE::SetMode(DIONODEITEMVALUE_MODE mode)
{
  this->mode = mode;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIONODEITEMVALUE::GetModeString(XSTRING& modestring)
* @brief      Get mode string
* @ingroup    DATAIO
* 
* @param[in]  modestring : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIONODEITEMVALUE::GetModeString(XSTRING& modestring)
{
  modestring.Empty();

  switch(mode)
    {
      case DIONODEITEMVALUE_MODE_NONE       :
                            default         : modestring = __L("none");
                                              break;

      case DIONODEITEMVALUE_MODE_READ       : modestring = __L("only read");
                                              break;

      case DIONODEITEMVALUE_MODE_WRITE      : modestring = __L("only write");
                                              break;

      case DIONODEITEMVALUE_MODE_READWRITE  : modestring = __L("read/write");
                                              break;
    }
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVARIANT* DIONODEITEMVALUE::GetDateTimeLastUpdate()
* @brief      Get date time last update
* @ingroup    DATAIO
* 
* @return     XVARIANT* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVARIANT* DIONODEITEMVALUE::GetDateTimeLastUpdate()
{
  return &datetimelastupdate;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVARIANT* DIONODEITEMVALUE::GetValue()
* @brief      Get value
* @ingroup    DATAIO
* 
* @return     XVARIANT* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVARIANT* DIONODEITEMVALUE::GetValue()
{
  return &value;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVARIANT* DIONODEITEMVALUE::GetMinValue()
* @brief      Get min value
* @ingroup    DATAIO
* 
* @return     XVARIANT* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVARIANT* DIONODEITEMVALUE::GetMinValue()
{
  return &minvalue;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVARIANT* DIONODEITEMVALUE::GetMaxValue()
* @brief      Get max value
* @ingroup    DATAIO
* 
* @return     XVARIANT* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVARIANT* DIONODEITEMVALUE::GetMaxValue()
{
  return &maxvalue;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         XVARIANT* DIONODEITEMVALUE::GetDifferenceForChange()
* @brief      Get difference for change
* @ingroup    DATAIO
* 
* @return     XVARIANT* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
XVARIANT* DIONODEITEMVALUE::GetDifferenceForChange()
{
  return &differenceforchange;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIONODEITEMVALUE::ValueHasChanged()
* @brief      Value has changed
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONODEITEMVALUE::ValueHasChanged()
{
  return haschanged;
}

    
/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIONODEITEMVALUE::SetValueHasChanged(bool haschanged)
* @brief      Set value has changed
* @ingroup    DATAIO
* 
* @param[in]  haschanged : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIONODEITEMVALUE::SetValueHasChanged(bool haschanged)
{
  this->haschanged = haschanged;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         DIONODEITEMVALUEUNITFORMAT* DIONODEITEMVALUE::GetUnitFormat()
* @brief      Get unit format
* @ingroup    DATAIO
* 
* @return     DIONODEITEMVALUEUNITFORMAT* : 
* 
* --------------------------------------------------------------------------------------------------------------------*/
DIONODEITEMVALUEUNITFORMAT* DIONODEITEMVALUE::GetUnitFormat()
{
  return &unitformat;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIONODEITEMVALUE::ReadUpdateDateTime()
* @brief      Read update date time
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONODEITEMVALUE::ReadUpdateDateTime()
{
  if(!datetime)
    {
      return false;
    }

  datetime->Read(false);

  datetimelastupdate = (*datetime);

  return datetime;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIONODEITEMVALUE::Serialize()
* @brief      Serialize
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONODEITEMVALUE::Serialize()
{   
  XSTRING typedescription;
  XSTRING formatvaluestr;
  XSTRING modestr;
  
  GetDescription(typedescription);
  value.GetType(formatvaluestr);
  GetModeString(modestr);

  Primitive_Add<int>(type, __L("type")); 

  Primitive_Add<XSTRING*>(&typedescription, __L("description"));

  Primitive_Add<XSTRING*>(&formatvaluestr, __L("formatvalue"));
  Primitive_Add<XDWORD>((XDWORD)mode, __L("mode"));

  GetModeString(modestr);
  Primitive_Add<XSTRING*>(&modestr, __L("modestr"));

  Primitive_Add<XVARIANT*>(&datetimelastupdate, __L("lastupdate"));
  Primitive_Add<XVARIANT*>(&value, __L("value")); 
  Primitive_Add<XVARIANT*>(&minvalue, __L("minvalue"));
  Primitive_Add<XVARIANT*>(&maxvalue, __L("maxvalue")); 
  Primitive_Add<XVARIANT*>(&differenceforchange, __L("differenceforchange"));
  Class_Add<DIONODEITEMVALUEUNITFORMAT>(&unitformat, __L("unitformat"));
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         bool DIONODEITEMVALUE::Deserialize()
* @brief      Deserialize
* @ingroup    DATAIO
* 
* @return     bool : true if is succesful. 
* 
* --------------------------------------------------------------------------------------------------------------------*/
bool DIONODEITEMVALUE::Deserialize()
{   
  Primitive_Extract<XDWORD>(type                  , __L("type"));  
  Primitive_Extract<XDWORD>((XDWORD&)mode         , __L("mode"));
  
//Primitive_Extract<XVARIANT>(datetimelastupdate, __L("lastupdate"));

  Primitive_Extract<XVARIANT>(value               , __L("value")); 
  Primitive_Extract<XVARIANT>(minvalue            , __L("minvalue"));
  Primitive_Extract<XVARIANT>(maxvalue            , __L("maxvalue"));  
  Primitive_Extract<XVARIANT>(differenceforchange , __L("differenceforchange"));
  
  Class_Extract<DIONODEITEMVALUEUNITFORMAT>(&unitformat, __L("unitformat"));
  
  return true;
}


/**-------------------------------------------------------------------------------------------------------------------
* 
* @fn         void DIONODEITEMVALUE::Clean()
* @brief      Clean the attributes of the class: Default initialize
* @note       INTERNAL
* @ingroup    DATAIO
* 
* --------------------------------------------------------------------------------------------------------------------*/
void DIONODEITEMVALUE::Clean()
{
  type        = DIONODEITEMVALUE_TYPE_UNKNOWN;  
  mode        = DIONODEITEMVALUE_MODE_NONE;
 
  haschanged  = false;

  datetime    = NULL;
}


#pragma endregion

