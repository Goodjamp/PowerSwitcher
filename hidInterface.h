#ifndef USERHIDINTERFACES_H
#define USERHIDINTERFACES_H

#include "stdint.h"

#include <QObject>
#include <string.h>

#include <windows.h>

class hidInterface
{
public:
    hidInterface();

public:
   void initUSB(void);
   bool getDInterfaceInfo(uint index ,std::wstring &VID, std::wstring &PID, std::wstring &Manufacturer, std::wstring &Product );
   bool getInterfaceVidPid(uint index ,uint &VID, uint &PID);
   bool openInterface(uint index);
   bool openInterface(uint VID, uint PID);
   bool closeInterface(void);
   bool isHIDOpen(void);
   uint32_t read(uint8_t *buff, uint32_t numToRead, uint32_t timeout);
   uint32_t write(uint8_t *buff, uint32_t numToWrite, uint32_t timeout);

public:
   std::vector<std::wstring> hidWPathList;

private:
    HANDLE     currentHID;
    HANDLE     hEventObject;
    OVERLAPPED HIDOverlapped;

};

#endif // USERHIDINTERFACES_H
