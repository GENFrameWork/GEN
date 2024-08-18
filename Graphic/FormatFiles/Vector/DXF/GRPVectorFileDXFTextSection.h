
#ifndef _GRPVECTORFILEDXFTEXTSECTION_h_
#define _GRPVECTORFILEDXFTEXTSECTION_h_

#pragma region INCLUDES

#include "XSubject.h"

#include "GRPVectorFile.h"
#include "GRPVectorFileDXFTextPart.h"


#pragma endregion


#pragma region DEFINES_ENUMS

enum GRPVECTORFILEDXFTEXTSECTION_BasicType
{
   GRPVECTORFILEDXFTEXTSECTION_BasicType_Unknown = 0,
   GRPVECTORFILEDXFTEXTSECTION_BasicType_Boolean,
   GRPVECTORFILEDXFTEXTSECTION_BasicType_Integer,
   GRPVECTORFILEDXFTEXTSECTION_BasicType_Double,
   GRPVECTORFILEDXFTEXTSECTION_BasicType_String,
};


enum GRPVECTORFILEDXFText_XDataCtrl_Status
{
   GRPVECTORFILEDXFTEXTSECTION_XDataCtrl_Status_Not = 0,
   GRPVECTORFILEDXFTEXTSECTION_XDataCtrl_Status_Ini,
   GRPVECTORFILEDXFTEXTSECTION_XDataCtrl_Status_End,
};


enum GRPVECTORFILEDXFTEXTSECTION_TYPESECTION
{
  GRPVECTORFILEDXFTEXTSECTION_TYPESECTION_UNKNOWN = 0,
  GRPVECTORFILEDXFTEXTSECTION_TYPESECTION_HEADER,
  GRPVECTORFILEDXFTEXTSECTION_TYPESECTION_CLASSES,
  GRPVECTORFILEDXFTEXTSECTION_TYPESECTION_TABLES, 
  GRPVECTORFILEDXFTEXTSECTION_TYPESECTION_BLOCKS, 
  GRPVECTORFILEDXFTEXTSECTION_TYPESECTION_ENTITIES,
  GRPVECTORFILEDXFTEXTSECTION_TYPESECTION_OBJECTS,
  GRPVECTORFILEDXFTEXTSECTION_TYPESECTION_ACDSDATA,
  GRPVECTORFILEDXFTEXTSECTION_TYPESECTION_THUMBNAILIMAGE,
};

#define GRPVECTORFILEDXFTEXTSECTION_MaxNDefSections 8
#define GRPVECTORFILEDXFTEXTSECTION_MaxNGenericDefTypes 93

#pragma endregion


#pragma region CLASSES

class XFILETXT;

typedef struct
{
   XCHAR* name;
   GRPVECTORFILEDXFTEXTSECTION_TYPESECTION type;
   
} GRPVECTORFILEDXFDefTextSection;


typedef struct
{  
   int type;
   int range;
   XCHAR* name;
   XCHAR* remark;

} GRPVECTORFILEDXFTEXTSECTIONGenericDefType;


class GRPVECTORFILEDXFTEXTSECTION : public XSUBJECT, public GRPVECTORFILEDXFTextPart
{
  public:

    GRPVECTORFILEDXFTEXTSECTION ();
    virtual ~GRPVECTORFILEDXFTEXTSECTION ();    
   
    GRPVECTORFILE* GetGRPVECTORFILE();  
    void SetGRPVECTORFILE(GRPVECTORFILE* vectorFile);  

    static GRPVECTORFILEDXFTEXTSECTION* CreateInstance(GRPVECTORFILEDXFTEXTSECTION_TYPESECTION type);   
    
    static GRPVECTORFILEDXFTEXTSECTION_TYPESECTION GetTypeSection (XSTRING& nameSection);
    
    static GRPVECTORFILEDXFTEXTSECTION_BasicType GetTypeBasic (int type);    
    GRPVECTORFILEDXFTEXTSECTIONGenericDefType* GetGenericDefType(int type);
    static void GetVariableFromLine(XCHAR* namevar, int type, XSTRING* line, XVARIANT& variant);  
    GRPVECTORFILEDXFText_XDataCtrl_Status IsXDataControl(int type, XSTRING& line);


    virtual GRPVECTORFILERESULT ParserTextSection (XFILETXT* fileTXT);
    
    GRPVECTORFILEDXFTEXTSECTION_TYPESECTION type;
    static GRPVECTORFILEDXFDefTextSection defsection[GRPVECTORFILEDXFTEXTSECTION_MaxNDefSections];
    static GRPVECTORFILEDXFTEXTSECTIONGenericDefType genericDef[GRPVECTORFILEDXFTEXTSECTION_MaxNGenericDefTypes];

  private:
    
    void Clean ();

    GRPVECTORFILE* vectorFile;
};

#pragma endregion


#pragma region EXTERN_INLINE_FUNCTIONS
#pragma endregion

#endif