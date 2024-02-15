// Функции управления медиаплеером
// (c) Kibab
// Версия 0.4
// (r) All S-C Community

/* Лог изменений 
0.4 - inline, добавлен комплект функций //BoBa
0.3 - добавлены функции выключения плеера, выключения звука
0.2 - добавлена функция изменения громкости
0.1 - первая публичная версия
*/

#ifndef _MPLAYER_H_
  #define _MPLAYER_H_

// Команды плеера
#define PLAYER_PREV	0x01
#define PLAYER_NEXT	0x02
#define PLAYER_RESTART	0x03
//0x04 а хз... часики рисует... БД??
//0x05 хрень какая-то с кнопками...
//0x06 роняет
//0x07 рестарт
//0x08 рестарт
//0x09 рестарт
#define PLAYER_MUTE	0x0A
//0x0b kill?
#define PLAYER_KILL	0x0C
//0x0d
#define PLAYER_PLAY	0x0E
#define PLAYER_STOP	0x0F
#define PLAYER_PAUSE	0x10
#define PLAYER_TOGGLE	0x11
#define PLAYER_FWD	0x12
#define PLAYER_RWN	0x13
#define PLAYER_NRM	0x14
#define PLAYER_VOL_UP	0x15
#define PLAYER_VOL_DN	0x16
//0x17 mute?
#define PLAYER_REPEAT1	0x18
//0x19 error?
//0x20
//0x21 роняет

void Send_MPlayer_Command(unsigned short cmd, short substract){		// Отправить команду плееру
	GBS_SendMessage(0x4209, 0x8053, 0, *((unsigned int*)RamMPlayer_CtrlCode())-substract,cmd);
}

void MPlayer_VolChange(short dir){					// Громкость вверх/вниз (dir=1 / dir=-1)
	int cmd;
	if(dir==1){
    		cmd = PLAYER_VOL_UP;
  	}else{
    		cmd = PLAYER_VOL_DN;
  	}
	Send_MPlayer_Command(cmd,2);
	Send_MPlayer_Command(cmd,0);
}

inline void MPlayer_Stop()	{Send_MPlayer_Command(PLAYER_STOP,0);}	// Остановить игру
inline void MPlayer_Start()	{Send_MPlayer_Command(PLAYER_PLAY,0);}	// Запуск проигрывания
inline void MPlayer_Next()	{Send_MPlayer_Command(PLAYER_NEXT,0);}	// Переключиться на следующую песню
inline void MPlayer_Prev()	{Send_MPlayer_Command(PLAYER_PREV,0);}	// Переключиться на предыдущую песню
inline void MPlayer_Toggle()	{Send_MPlayer_Command(PLAYER_TOGGLE,0);}// Пауза/воспроизведение
inline void MPlayer_Shutdown()	{Send_MPlayer_Command(PLAYER_KILL,0);}	// Завершить работу плеера
inline void MPlayer_Mute()	{Send_MPlayer_Command(PLAYER_MUTE,0);}	// Выключить звук. При повторном вызове звук включается
inline void MPlayer_Pause()	{Send_MPlayer_Command(PLAYER_PAUSE,0);}	// Пауза


#endif
