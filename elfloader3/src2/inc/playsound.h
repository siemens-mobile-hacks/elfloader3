#ifdef NEWSGOLD
typedef struct{
  int pid_from;
  int msg; //MSG_PLAYFILE_REPORT
  unsigned short cmd;
  short handler;
  void *data0;
  void *data1;
} GBS_PSOUND_MSG;
#else
typedef struct{
  short pid_from;
  short msg; //MSG_PLAYFILE_REPORT
  unsigned short cmd;
  short handler;
  void *data0;
  void *data1;
} GBS_PSOUND_MSG;
#endif


//cmd
#ifdef NEWSGOLD
#define M_SAE_EVENT_STARTED 0
#define M_SAE_EVENT_STOPPED 1
#define M_SAE_EVENT_DONE 2
#define M_SAE_EVENT_ERROR 3
#define M_SAE_PLAYBACK_STARTED 4
#define M_SAE_PLAYBACK_ERROR 5
#define M_SAE_PLAYBACK_NEXTCYCLE 6
#define M_SAE_PLAYBACK_DONE 7
#define M_SAE_VOLUME_UPDATED 8
#define M_SAE_VOLUME_ERROR 9
#define M_SAE_EQUALIZER_UPDATED 0xA
#define M_SAE_EQUALIZER_ERROR 0xB
#define M_SAE_3DAUDIO_UPDATED 0xC
#define M_SAE_3DAUDIO_ERROR 0xD
#define M_SAE_UNKNOWN_EVENT 0xE
#define M_SAE_UNKNOWN_EVENT_1 0xF
#define M_SAE_PLAYTIME_UPDATED 0x10
#define M_SAE_PLAYTIME_ERROR 0x11
#define M_SAE_HANDSFREE_UPDATED 0x12
#define M_SAE_HANDSFREE_ERROR 0x13
#define M_SAE_RECORD_STARTED 0x14
#define M_SAE_RECORD_ERROR 0x15
#define M_SAE_RECORD_DONE 0x16
#define M_SAE_STOP_OK 0x17
#define M_SAE_STOP_FAIL 0x18
#define M_SAE_CANCEL_OK 0x19
#define M_SAE_CANCEL_FAIL 0x1A
#define M_SAE_PAUSE_OK 0x1B
#define M_SAE_PAUSE_FAIL 0x1C
#define M_SAE_RESUME_OK 0x1D
#define M_SAE_RESUME_FAIL 0x1E
#define M_SAE_INFO_DATA_AVAILABLE 0x1F
#define M_SAE_INFO_REQUEST_FAIL 0x20
#define M_SAE_MAX_RECORDING_TIME_AVAILABLE 0x21
#define M_SAE_MAX_RECORDING_TIME_ERROR 0x22
#define M_SAE_UNKNOWN_EVENT_2 0x23
#define M_SAE_UNKNOWN_EVENT_3 0x24
#define M_SAE_AUDIO_EVENT_STOPPED 0x25
#define M_SAE_AUDIO_EVENT_PAUSED 0x26
#define M_SAE_AUDIO_EVENT_RESUMED 0x27
#define M_SAE_VISUALIZATION_STOPPED 0x28
#define M_SAE_VISUALIZATION_STARTED 0x29
#define M_SAE_UNKNOWN_EVENT_4 0x2A
#define M_SAE_VISUALIZATION_DATA 0x2B
#define M_SAE_STREAM_READY 0x2C
#define M_SAE_STREAM_STALLED 0x2D
#define M_SAE_STREAM_UNSTALLED 0x2E
#define M_SAE_STREAM_LOW_WATERMARK 0x2F
#define M_SAE_STREAM_HIGH_WATERMARK 0x30
#define M_SAE_BUFFER_CONSUMED 0x31
#define M_SAE_BUFFER_AVAILABLE 0x32
#define M_SAE_RESERVE_OK 0x33
#define M_SAE_RESERVE_FAIL 0x34
#define M_SAE_RELEASE_OK 0x35
#define M_SAE_RELEASE_FAIL 0x36
#define M_SAE_RELEASED 0x37
#define M_SAE_VOLUME_CHANGED 0x38
#define M_SAE_SOUNDSTATE_CHANGED 0x39
#define M_SAE_SOUNDSYSTEM_IDLE 0x3A
#define M_SAE_EVENT_IN_STACK 0x3B
#else
#define M_SAE_EVENT_STARTED 0
#define M_SAE_EVENT_STOPPED 1
#define M_SAE_EVENT_DONE 2
#define M_SAE_EVENT_ERROR 3
#define M_SAE_PLAYBACK_STARTED 4
#define M_SAE_PLAYBACK_ERROR 5
#define M_SAE_PLAYBACK_NEXTCYCLE 6
#define M_SAE_PLAYBACK_DONE 7
#define M_SAE_VOLUME_UPDATED 8
#define M_SAE_VOLUME_ERROR 9
#define M_SAE_PLAYTIME_UPDATED 0xA
#define M_SAE_PLAYTIME_ERROR 0xB
#define M_SAE_HANDSFREE_UPDATED 0xC
#define M_SAE_HANDSFREE_ERROR 0xD
#define M_SAE_RECORD_STARTED 0xE
#define M_SAE_RECORD_ERROR 0xF
#define M_SAE_RECORD_DONE 0x10
#define M_SAE_STOP_OK 0x11
#define M_SAE_STOP_FAIL 0x12
#define M_SAE_CANCEL_OK 0x13
#define M_SAE_CANCEL_FAIL 0x14
#define M_SAE_PAUSE_OK 0x15
#define M_SAE_PAUSE_FAIL 0x16
#define M_SAE_RESUME_OK 0x17
#define M_SAE_RESUME_FAIL 0x18
#define M_SAE_INFO_DATA_AVAILABLE 0x19
#define M_SAE_INFO_REQUEST_FAIL 0x1A
#define M_SAE_MAX_RECORDING_TIME_AVAILABLE 0x1B
#define M_SAE_MAX_RECORDING_TIME_ERROR 0x1C
#define M_SAE_AUDIO_EVENT_UPDATED 0x1D
#define M_SAE_AUDIO_EVENT_UPDATED_MS 0x1E
#define M_SAE_AUDIO_EVENT_STOPPED 0x1F
#define M_SAE_AUDIO_EVENT_PAUSED 0x20
#define M_SAE_AUDIO_EVENT_RESUMED 0x21
#endif

//volume flag (usually first paremeter in sound functions)
#define VOLUME_CALLS 0
#define VOLUME_SYSTEM 1
#define VOLUME_WALKY_TALKY 2
//#define VOLUME_SYSTEM 3
#define VOLUME_MESSAGES 4
//#define VOLUME_MESSAGES 5
#define VOLUME_CB_SERVICES 6
//#define VOLUME_SYSTEM 7
//#define VOLUME_SYSTEM 8
#define VOLUME_ALARM_CLOCK 9
#define VOLUME_ORGANISER 0xA
//#define VOLUME_SYSTEM 0xB
#define VOLUME_APPLICATIONS 0xC
//#define VOLUME_SYSTEM 0xD
//#define VOLUME_SYSTEM 0xE
//#define VOLUME_SYSTEM 0xF
//#define VOLUME_SYSTEM 0x10
//#define VOLUME_SYSTEM 0x11
//#define VOLUME_SYSTEM 0x12
//#define VOLUME_SYSTEM 0x13
//#define VOLUME_SYSTEM 0x14
//#define VOLUME_SYSTEM 0x15

//struct_types
#ifdef NEWSGOLD
#define Job_NoJob 0
#define Job_RecFile 1
#define Job_RecBuffer 2
#define Job_PlayFile 3
#define Job_PlayBuffer 5
#define Job_GetInfo 0xF
#define Job_GetRecordingTimeMax 0x10
#else
#define Job_NoJob 0
#define Job_RecFile 1
#define Job_RecBuffer 2
#define Job_PlayFile 3
#define Job_PlayBuffer 4
#define Job_GetInfo 5
#define Job_GetRecordingTimeMax 6
#endif
