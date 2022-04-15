/**-------------------------------------------------------------------------------------------------------------------
*
* @file       XWINDOWSDriveImageManager .cpp
*
* @class      XWINDOWSDRIVEIMAGEMANAGER
* @brief      eXtended WINDOWS Drive Image Manager class
* @ingroup    PLATFORM_WINDOWS
*
* @copyright  GEN Group. All right reserved.
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
*---------------------------------------------------------------------------------------------------------------------*/

/*---- PRECOMPILATION CONTROL ----------------------------------------------------------------------------------------*/

#include "GEN_Defines.h"

/*---- INCLUDES ------------------------------------------------------------------------------------------------------*/

#include "XBuffer.h"
#include "XDriveImageManager_XEvent.h"

#include "XWINDOWSDriveImageManager.h"

#include "XMemory_Control.h"


/*---- GENERAL VARIABLE ----------------------------------------------------------------------------------------------*/


/*---- CLASS MEMBERS -------------------------------------------------------------------------------------------------*/



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWINDOWSDRIVEIMAGEMANAGER::XWINDOWSDRIVEIMAGEMANAGER()
* @brief      Constructor
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XWINDOWSDRIVEIMAGEMANAGER::XWINDOWSDRIVEIMAGEMANAGER()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XWINDOWSDRIVEIMAGEMANAGER::~XWINDOWSDRIVEIMAGEMANAGER()
* @brief      Destructor
* @note       VIRTUAL
* @ingroup    UTILS
*
* @return     Does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
XWINDOWSDRIVEIMAGEMANAGER::~XWINDOWSDRIVEIMAGEMANAGER()
{
  Clean();
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::GetDrives(XVECTOR<XDRIVEIMAGEMANAGER_DRIVE*>& drives)
* @brief      GetDrives
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  drives :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::GetDrives(XVECTOR<XDRIVEIMAGEMANAGER_DRIVE*>& drives)
{
  drives.DeleteContents();
  drives.DeleteAll();

  XPATH   path;
  XDWORD  drivemask = GetLogicalDrives();
  XQWORD  ID        = 0;
  int     c         = 0;

  // GetLogicalDrives returns 0 on failure, or a bitmask representing the drives available on the system (bit 0 = A:, bit 1 = B:, etc)

  while(drivemask != 0)
    {
      if(drivemask & 1)
        {
          // the "A" in path will get incremented by the # of bits
          // we've shifted
          path.Format( __L("\\\\.\\%c:\\"), __C('A') + c);
          if(CheckDriveType(path, ID))
            {
              XDRIVEIMAGEMANAGER_DRIVE* drive = new XDRIVEIMAGEMANAGER_DRIVE();
              if(drive)
                {
                  XSTRING label;

                  drive->GetName()->Format(__L("%c:"), __C('A') + c);
                  drive->GetPath()->Set(path);
                  drive->SetVolume(c);
                  drive->GetLabel()->Set(label);
                  drive->SetID(ID);

                  if(GetDriveLabel(drive->GetName()->Get(), label))  drive->GetLabel()->Set(label);

                  drives.Add(drive);
                }
            }
        }

      drivemask >>= 1;

      ++c;
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::Open(XDRIVEIMAGEMANAGER_DRIVE& drive)
* @brief      Open
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  drive :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::Open(XDRIVEIMAGEMANAGER_DRIVE& drive)
{
  HANDLE handlevolume = GetHandleFromVolume(drive.GetVolume(), GENERIC_READ | GENERIC_WRITE);
  if(handlevolume == INVALID_HANDLE_VALUE) return false;

  drive.SetHandle((XDWORD)handlevolume);

  XDWORD deviceID = 0;
  if(!GetDeviceID(handlevolume, deviceID)) return false;
  drive.SetDeviceID(deviceID);

  if(!LockVolume(handlevolume, true))  return false;

  if(!UnMountVolume(handlevolume))    return false;

  HANDLE rawhandle = GetHandleFromDevice(deviceID, GENERIC_READ | GENERIC_WRITE);
  if(rawhandle == INVALID_HANDLE_VALUE) return false;

  drive.SetRawHandle((XDWORD)rawhandle);

  XQWORD sectorsize    = 0;
  XQWORD numbersectors = 0;

  if(!GetNumberOfSectors(rawhandle, sectorsize, numbersectors)) return false;

  drive.SetSectorSize(sectorsize);
  drive.SetNumberSectors(numbersectors);

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::Read(XDRIVEIMAGEMANAGER_DRIVE& drive, XQWORD startsector, XQWORD numbersectors, XBUFFER& buffer)
* @brief      Read
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  drive :
* @param[in]  startsector :
* @param[in]  numbersectors :
* @param[in]  buffer :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::Read(XDRIVEIMAGEMANAGER_DRIVE& drive, XQWORD startsector, XQWORD numbersectors, XBUFFER& buffer)
{
  XBYTE* _buffer;
  XDWORD maxbuffer         = ((XDWORD)numbersectors * (XDWORD)drive.GetSectorSize());
  XQWORD maxnsectorsblock  = (maxbuffer / drive.GetSectorSize());
  XQWORD readnsectorsblock = 0;
  XQWORD sizeread          = 0;
  bool   status            = false;

  buffer.Delete();

  _buffer = new XBYTE[maxbuffer];
  if(!_buffer) return false;

  if(maxnsectorsblock > numbersectors) maxnsectorsblock = numbersectors;

  while(readnsectorsblock < numbersectors)
    {
      XDRIVEIMAGEMANAGER_XEVENT xevent(this, XDRIVEIMAGEMANAGER_XEVENT_TYPE_READSECTORS);

      xevent.SetDrive(&drive);
      xevent.SetIndexSector(startsector);
      xevent.SetNumberSectorsInOperation(maxnsectorsblock);

      sizeread = (maxnsectorsblock * drive.GetSectorSize());

      memset(_buffer, 0, maxbuffer);

      status = ReadSectorDataFromHandle((HANDLE)drive.GetRawHandle(), startsector, drive.GetSectorSize(), _buffer, sizeread);
      if(status)
        {
          xevent.SetError(XDRIVEIMAGEMANAGER_ERROR_NONE);

          buffer.Add(_buffer, (XDWORD)sizeread);

          readnsectorsblock += maxnsectorsblock;

          if((readnsectorsblock + maxnsectorsblock) > numbersectors)             maxnsectorsblock = (numbersectors - readnsectorsblock);
          if((readnsectorsblock + maxnsectorsblock) > drive.GetNumberSectors())  maxnsectorsblock = (drive.GetNumberSectors() - readnsectorsblock);
        }
       else
        {
          xevent.SetError(XDRIVEIMAGEMANAGER_ERROR_READSECTORS);
        }

      PostEvent(&xevent);

      if(!status) break;

    }

  delete [] _buffer;

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::Write(XDRIVEIMAGEMANAGER_DRIVE& drive, XQWORD startsector, XQWORD numbersectors, XBUFFER& buffer)
* @brief      Write
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  drive :
* @param[in]  startsector :
* @param[in]  numbersectors :
* @param[in]  buffer :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::Write(XDRIVEIMAGEMANAGER_DRIVE& drive, XQWORD startsector, XQWORD numbersectors, XBUFFER& buffer)
{
  XBYTE* _buffer;
  XDWORD maxbuffer          = ((XDWORD)numbersectors * (XDWORD)drive.GetSectorSize());
  XQWORD maxnsectorsblock   = (maxbuffer / drive.GetSectorSize());
  XQWORD writensectorsblock = 0;
  XQWORD sizewrite          = 0;
  bool   status             = false;

  _buffer = new XBYTE[maxbuffer];
  if(!_buffer) return false;

  if(maxnsectorsblock > numbersectors) maxnsectorsblock = numbersectors;

  while(writensectorsblock < numbersectors)
    {
      XDRIVEIMAGEMANAGER_XEVENT xevent(this, XDRIVEIMAGEMANAGER_XEVENT_TYPE_WRITESECTORS);

      xevent.SetDrive(&drive);
      xevent.SetIndexSector(startsector);
      xevent.SetNumberSectorsInOperation(maxnsectorsblock);

      sizewrite = (maxnsectorsblock * drive.GetSectorSize());

      memset(_buffer, 0, maxbuffer);

      memcpy(_buffer, buffer.Get(), (XDWORD)sizewrite);

      status = WriteSectorDataToHandle((HANDLE)drive.GetRawHandle(), startsector, drive.GetSectorSize(), _buffer, sizewrite);
      if(status)
        {
          xevent.SetError(XDRIVEIMAGEMANAGER_ERROR_NONE);

          writensectorsblock += maxnsectorsblock;

          if((writensectorsblock + maxnsectorsblock) > numbersectors)             maxnsectorsblock = (numbersectors - writensectorsblock);
          if((writensectorsblock + maxnsectorsblock) > drive.GetNumberSectors())  maxnsectorsblock = (drive.GetNumberSectors() - writensectorsblock);

        }
       else
        {
          xevent.SetError(XDRIVEIMAGEMANAGER_ERROR_WRITESECTORS);
        }

      PostEvent(&xevent);

      if(!status) break;
    }

  delete [] _buffer;

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::Close(XDRIVEIMAGEMANAGER_DRIVE& drive)
* @brief      Close
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  drive :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::Close(XDRIVEIMAGEMANAGER_DRIVE& drive)
{
  HANDLE vol_handle = (HANDLE)drive.GetHandle();
  HANDLE raw_handle = (HANDLE)drive.GetRawHandle();

  if(raw_handle != INVALID_HANDLE_VALUE)
    {
      CloseHandle(raw_handle);
    }

  if(vol_handle != INVALID_HANDLE_VALUE)
    {
      LockVolume(vol_handle, false);
      CloseHandle(vol_handle);
    }

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         HANDLE XWINDOWSDRIVEIMAGEMANAGER::GetHandleFromDevice(int device, XDWORD access)
* @brief      GetHandleFromDevice
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  device :
* @param[in]  access :
*
* @return     HANDLE :
*
*---------------------------------------------------------------------------------------------------------------------*/
HANDLE XWINDOWSDRIVEIMAGEMANAGER::GetHandleFromDevice(int device, XDWORD access)
{
  XSTRING devicename;
  devicename.Format(__L("\\\\.\\PhysicalDrive%d"), device);

  return CreateFile(devicename.Get(), access, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         HANDLE XWINDOWSDRIVEIMAGEMANAGER::GetHandleFromVolume(int volume, XDWORD access)
* @brief      GetHandleFromVolume
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  volume :
* @param[in]  access :
*
* @return     HANDLE :
*
*---------------------------------------------------------------------------------------------------------------------*/
HANDLE XWINDOWSDRIVEIMAGEMANAGER::GetHandleFromVolume(int volume, XDWORD access)
{
  XSTRING  volumename;

  volumename.Format(__L("\\\\.\\%c:"), __C('A') + volume);

  return CreateFile(volumename.Get(), access, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         XDWORD XWINDOWSDRIVEIMAGEMANAGER::GetDeviceID(HANDLE handlevolume);
* @brief      GetDeviceID
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  handlevolume) :
*
* @return     XDWORD :
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::GetDeviceID(HANDLE handlevolume, XDWORD& deviceID)
{
  VOLUME_DISK_EXTENTS   vol_ext;
  DWORD                 bytesreturned;

  deviceID = 0;

  if(!DeviceIoControl(handlevolume, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &vol_ext, sizeof(vol_ext), &bytesreturned, NULL))
    {
      return false;
    }

  deviceID  = vol_ext.Extents[0].DiskNumber;

  return true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::LockVolume(HANDLE handle, bool islock)
* @brief      LockVolume
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  handle :
* @param[in]  islock :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::LockVolume(HANDLE handle, bool islock)
{
  DWORD junk;

  return DeviceIoControl(handle, islock?FSCTL_LOCK_VOLUME: FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &junk, NULL)?true:false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::UnMountVolume(HANDLE handle)
* @brief      UnMountVolume
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  handle :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::UnMountVolume(HANDLE handle)
{
  DWORD junk;

  return DeviceIoControl(handle, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &junk, NULL)?true:false;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::IsVolumeUnmounted(HANDLE handle)
* @brief      IsVolumeUnmounted
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  handle :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::IsVolumeUnmounted(HANDLE handle)
{
  DWORD junk;

  return DeviceIoControl(handle, FSCTL_IS_VOLUME_MOUNTED, NULL, 0, NULL, 0, &junk, NULL)?false:true;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::GetNumberOfSectors(HANDLE handle, XQWORD& sectorsize, XQWORD& numbersectors)
* @brief      GetNumberOfSectors
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  handle :
* @param[in]  sectorsize :
* @param[in]  numbersectors :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::GetNumberOfSectors(HANDLE handle, XQWORD& sectorsize, XQWORD& numbersectors)
{
  DWORD             junk;
  DISK_GEOMETRY_EX  diskgeometry;
  bool              status;

  sectorsize    = 0;
  numbersectors = 0;

  status = DeviceIoControl(handle, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, &diskgeometry, sizeof(diskgeometry), &junk, NULL)?true:false;
  if(status)
    {
      sectorsize    = (XQWORD)diskgeometry.Geometry.BytesPerSector;
      numbersectors = ((XQWORD)diskgeometry.DiskSize.QuadPart / (XQWORD)diskgeometry.Geometry.BytesPerSector);
    }

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::GetSpaceAvailable(XCHAR* location, XQWORD& spaveavailable)
* @brief      GetSpaceAvailable
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  location :
* @param[in]  spaveavailable :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::GetSpaceAvailable(XCHAR* location, XQWORD& spaveavailable)
{
  ULARGE_INTEGER freespace;
  bool           status;

  spaveavailable = 0L;

  status = GetDiskFreeSpaceEx(location, NULL, NULL, &freespace)?true:false;
  if(!status) return false;

  spaveavailable = freespace.QuadPart;

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::GetDriveLabel(XCHAR* location, XSTRING& label)
* @brief      GetDriveLabel
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  location :
* @param[in]  label :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::GetDriveLabel(XCHAR* location, XSTRING& label)
{
  bool status;

  label.AdjustSize(_MAXSTR);
  status =  GetVolumeInformation(location, label.Get(), label.GetSize(), NULL, NULL, NULL, NULL, 0);
  label.AdjustSize();

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::CheckDriveType(XPATH& path, XQWORD& ID)
* @brief      CheckDriveType
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  path :
* @param[in]  ID :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::CheckDriveType(XPATH& path, XQWORD& ID)
{
  bool                              status = false;

  #ifndef BUILDER

  HANDLE                            hdevice;
  PSTORAGE_DEVICE_DESCRIPTOR        pdevdesc;
  XWINDOWSDRIVEIMAGE_DEVICE_NUMBER  deviceinfo;
  XPATH                             path_noslash;
  int                               drivetype;
  DWORD                             nbytesreturned;


  path_noslash = path.Get();

  path_noslash.Slash_Delete();

    // some calls require no tailing slash, some require a trailing slash...

  drivetype = GetDriveType(path.Get());
  switch(drivetype)
    {
      case DRIVE_REMOVABLE  : // The media can be removed from the drive.

      case DRIVE_FIXED      : { // The media cannot be removed from the drive. Some USB drives report as this.
                                hdevice = CreateFile(path_noslash.Get(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                                if(hdevice == INVALID_HANDLE_VALUE) break;

                                int sizearray  = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;
                                pdevdesc       = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizearray];

                                pdevdesc->Size = sizearray;

                                // get the device number if the drive is removable or (fixed AND on the usb bus, SD, or MMC (undefined in XP/mingw))

                                if(GetMediaType(hdevice))
                                  {
                                    status = true;

                                    STORAGE_PROPERTY_QUERY  query;        // input param for query
                                    DWORD                   noutbytes;    // IOCTL output length
                                    BOOL                    result;       // IOCTL return val
                                    DWORD                   nreturned;

                                    // specify the query type
                                    query.PropertyId = StorageDeviceProperty;
                                    query.QueryType  = PropertyStandardQuery;

                                    // query using IOCTL_STORAGE_QUERY_PROPERTY
                                    result = ::DeviceIoControl(hdevice, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(STORAGE_PROPERTY_QUERY), pdevdesc, pdevdesc->Size, &noutbytes, (LPOVERLAPPED)NULL);
                                    if(result)
                                      {
                                        result = ::DeviceIoControl(hdevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &deviceinfo, sizeof(XWINDOWSDRIVEIMAGE_DEVICE_NUMBER), &noutbytes, (LPOVERLAPPED)NULL);
                                        if(!result)
                                          {
                                            status = false;

                                          } else status = true;
                                      }
                                     else
                                      {
                                        if(DeviceIoControl(hdevice, IOCTL_STORAGE_CHECK_VERIFY2, NULL, 0, NULL, 0, &nreturned, (LPOVERLAPPED) NULL))
                                          {

                                          }

                                        status = false;
                                      }

                                    if(status)
                                      {
                                        status = false;

                                        if(((drivetype == DRIVE_REMOVABLE) && (pdevdesc->BusType != BusTypeSata))  ||
                                           ((drivetype == DRIVE_FIXED)     && ((pdevdesc->BusType == BusTypeUsb)   || (pdevdesc->BusType == BusTypeSd ) || (pdevdesc->BusType == BusTypeMmc ))))
                                          {
                                            // ensure that the drive is actually accessible  multi-card hubs were reporting "removable" even when empty

                                            if(DeviceIoControl(hdevice, IOCTL_STORAGE_CHECK_VERIFY2, NULL, 0, NULL, 0, &nbytesreturned, (LPOVERLAPPED) NULL))
                                              {
                                                ID = deviceinfo.devicenumber;
                                                status = true;
                                              }
                                             else
                                              {
                                                // IOCTL_STORAGE_CHECK_VERIFY2 fails on some devices under XP/Vista, try the other (slower) method, just in case.

                                                CloseHandle(hdevice);
                                                hdevice = CreateFile(path_noslash.Get(), FILE_READ_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                                                if(DeviceIoControl(hdevice, IOCTL_STORAGE_CHECK_VERIFY, NULL, 0, NULL, 0, &nbytesreturned, (LPOVERLAPPED) NULL))
                                                  {
                                                    ID = deviceinfo.devicenumber;
                                                    status = true;
                                                  }
                                              }
                                          }
                                      }
                                  }

                                delete[] pdevdesc;

                                CloseHandle(hdevice);
                              }

                              break;

             default       :  break;

    }

  #endif

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::GetMediaType(HANDLE hdevice)
* @brief      GetMediaType
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  hdevice :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::GetMediaType(HANDLE hdevice)
{
  DISK_GEOMETRY diskgeometry;
  DWORD         nbytesreturned;

  if(DeviceIoControl(hdevice, IOCTL_DISK_GET_DRIVE_GEOMETRY,NULL, 0, &diskgeometry, sizeof(diskgeometry), &nbytesreturned, NULL))
    {
      if((diskgeometry.MediaType == FixedMedia) || (diskgeometry.MediaType == RemovableMedia)) return true; // Not a floppy
    }

  return false;
}




/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::ReadSectorDataFromHandle(HANDLE handle, XQWORD startsector, XQWORD sectorsize, XBYTE* buffer, XQWORD& size)
* @brief      ReadSectorDataFromHandle
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  handle :
* @param[in]  startsector :
* @param[in]  sectorsize :
* @param[in]  buffer :
* @param[in]  size :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::ReadSectorDataFromHandle(HANDLE handle, XQWORD startsector, XQWORD sectorsize, XBYTE* buffer, XQWORD& size)
{
  LARGE_INTEGER li;
  DWORD         bytesread;
  bool          status;

  li.QuadPart = startsector * sectorsize;

  SetFilePointer(handle, li.LowPart, &li.HighPart, FILE_BEGIN);

  status = ReadFile(handle, buffer, (DWORD)size, &bytesread, NULL)?true:false;
  if(status)
    {
      status = (bytesread == size)?true:false;
      size = bytesread;
    }

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         bool XWINDOWSDRIVEIMAGEMANAGER::WriteSectorDataToHandle(HANDLE handle, XQWORD startsector, XQWORD sectorsize, XBYTE* buffer, XQWORD& size)
* @brief      WriteSectorDataToHandle
* @ingroup    PLATFORM_WINDOWS
*
* @param[in]  handle :
* @param[in]  startsector :
* @param[in]  sectorsize :
* @param[in]  buffer :
* @param[in]  size :
*
* @return     bool : true if is succesful.
*
*---------------------------------------------------------------------------------------------------------------------*/
bool XWINDOWSDRIVEIMAGEMANAGER::WriteSectorDataToHandle(HANDLE handle, XQWORD startsector, XQWORD sectorsize, XBYTE* buffer, XQWORD& size)
{
  LARGE_INTEGER li;
  DWORD         byteswrite;
  bool          status;

  li.QuadPart = startsector * sectorsize;

  SetFilePointer(handle, li.LowPart, &li.HighPart, FILE_BEGIN);

  status = WriteFile(handle, buffer, (DWORD)size, &byteswrite, NULL)?true:false;
  if(status)
    {
      status = (byteswrite == size)?true:false;
      size = byteswrite;
    }

  return status;
}



/**-------------------------------------------------------------------------------------------------------------------
*
* @fn         void XWINDOWSDRIVEIMAGEMANAGER::Clean()
* @brief      Clean the attributes of the class: Default initialice
* @note       INTERNAL
* @ingroup    UTILS
*
* @return     void : does not return anything.
*
*---------------------------------------------------------------------------------------------------------------------*/
void XWINDOWSDRIVEIMAGEMANAGER::Clean()
{
  handle = NULL;
}



