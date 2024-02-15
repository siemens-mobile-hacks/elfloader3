#ifndef _GPSLOGGER_IPC_H_
#define _GPSLOGGER_IPC_H_

//IPC const for GPSLogger interface system
  #define IPC_GPSL_NAME "GPSLogger"
  #define IPC_GPSL_UPD_TMO 1
  #define IPC_GPSL_REFRESH 2

typedef struct
{
  char msg_version;   // Версия структуры обмена
  unsigned short cid; // CID, LAC
  unsigned short lac; 
}GPSL_IPC_MSG_UPD_TMO;

#endif
//EOL,EOF
