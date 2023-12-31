#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include "keymapper.h"

#define SHIFTED		0x8000 
#define PRESSED		0xFF80
#define KF_ALTDOWN  0x2000
#define CHAR_OFFSET_DIKALA  -1

const int DEFAULT_CHAR_OFFSET = 5;  
const int CTRL = 1;
const int SHIFT = VK_SHIFT;

//enum ModifierKey {NONE=0, CTRL=1, SHIFT=2};
typedef struct tagKeyMap *LPKeyMap;

typedef struct tagKeyMap{
	 int             first;			// first mapped character (amharic)
     int             offset;	// 5 ... the sixth letter in the set
     int			 modifier;		// any modifier key combinations
	 int			 dikala;		// offset of the 'Dikala' version of  this base letter
	 LPKeyMap		 sub_char;
	 LPKeyMap		 sub_char2;
}KeyMap, *PKeyMap, *LPKeyMap;

LPKeyMap MapKey(int firstCharId) {
	LPKeyMap km = (LPKeyMap)malloc(sizeof(KeyMap));
	km->first = firstCharId;
	km->modifier = 0;
	km->offset = DEFAULT_CHAR_OFFSET;
	km->dikala = 7;
	km->sub_char = NULL;
	km->sub_char2 = NULL;

	return km;
}

LPKeyMap MapKey2(int firstCharId, int charOffet, int modifier) {
	LPKeyMap km = (LPKeyMap)malloc(sizeof(KeyMap));
	km->first = firstCharId;
	km->modifier = modifier;
	km->offset = charOffet;
	km->dikala = 7;
	km->sub_char = NULL;
	km->sub_char2 = NULL;

	return km;
}

LPKeyMap MapKey3(int firstCharId, int modifier) {
	LPKeyMap km = (LPKeyMap)malloc(sizeof(KeyMap));
	km->first = firstCharId;
	km->modifier = modifier;
	km->offset = 5;
	km->dikala = 7;
	km->sub_char = NULL;
	km->sub_char2 = NULL;

	return km;
}

LPKeyMap  key_map[26];
BOOL isEnabled = 1;

int			_prevKey = NULL;
int			_prevVowel = NULL;
LPKeyMap	_prevKmi ;

/**************************************************************** 
  WH_CALLWNDPROC hook procedure 
 ****************************************************************/ 

int			MapChar(int);
int			GetKeyOffset(int key, int prevVowelKey);
int			KeyMapperProc(int key);
LPKeyMap	GetKeyChar(int key);
BOOL		IsVowel(int key);
void		SendKey(int key);
void		SendKeyUnicode(int key);

int ProcessKey(int vkKey) 
{
	if (vkKey >= 0x41 && vkKey <= 0x5A){
		
		SendKey(VK_BACK);	// delete 'english' character

		int _key = KeyMapperProc(vkKey);
		if(_key == 0) return 1;
		
		if (_key < 0){
			SendKey(VK_BACK);		// delete previous amharic letter
			SendKeyUnicode(-_key);	// put new amharic letter
		}
		else{
			SendKeyUnicode(_key);	// put new amharic letter
		}
		
		return 1;
	}
	else if (vkKey == VK_SPACE) {
		_prevKey = NULL;
        _prevVowel = NULL;
		_prevKmi = NULL;
	}

	return 0;
}


int KeyMapperProc(int key)
{
	int isVowel = IsVowel(key);
    if (!isVowel)
    {
        _prevKey = key;
        LPKeyMap kmi = GetKeyChar(key);
        
        _prevKmi = kmi;
        _prevVowel = 0;
        return kmi->first + kmi->offset;
    }
    else
    {
		// if the previous key is also Vowel and both the current 
		// vowel and the previous one do not make one of the allowed
		// vowel sequences, then take the current vowel as it is
		if(_prevVowel) {
			BOOL specialVowelSequence = 
				((_prevVowel == 'A' && key == 'A') /*dikala: 'bwa' etc*/ || 
				(_prevVowel == 'E' && key == 'I') || 
				(_prevVowel == 'I' && key == 'E') );
			if(!specialVowelSequence) {
				_prevKey = NULL;
			}
		}

        if (!_prevKey)
        {
            LPKeyMap kmi = GetKeyChar(key);
            _prevVowel = key;
            return kmi->first + kmi->offset;
        }
        else
        {
            int offset = GetKeyOffset(key, _prevVowel);
            _prevVowel = key;
            if (offset == CHAR_OFFSET_DIKALA) 
				offset = _prevKmi->dikala;
            int charId = _prevKmi->first + offset;
            			
            return -charId;	// remove previous letter and insert the new one
        }
    }
}

BOOL IsVowel(int key){
	return (key == 0x41 || key == 'E' || key == 'I' || key == 'O' || key == 'U');
}

 LPKeyMap GetKeyChar(int key)
{
    LPKeyMap kmi = key_map[key-0x41];
    if (kmi->sub_char)
    {
        LPKeyMap kmi2 = kmi->sub_char;
        if (GetKeyState(kmi2->modifier) & PRESSED)
        {
            kmi = kmi2;
        }
    }
    /*if (kmi->sub_char2)
    {
        LPKeyMap kmi2 = kmi->sub_char2;
        if (GetKeyState(kmi2->modifier) & PRESSED)
        {
            kmi = kmi2;
        }
    }*/

    return kmi; 
}

 int GetKeyOffset(int key, int prevVowelKey)
{
    if ((prevVowelKey == 'E' && key == 'I') ||
        (prevVowelKey == 'I' && key == 'E'))
    {
        return 4;
    }

    if (prevVowelKey == 'A' && key == 'A')
    {
        return CHAR_OFFSET_DIKALA;
    }

	switch (key) 
    {
        case 'A':
            return 3;
        case 'U':
            return 1;
        case 'I':
            return 2;
        case 'O':
            return 6;
        case 'E':
            return 0;
        default:
            return DEFAULT_CHAR_OFFSET; // 5
    }
}
 
 // Simulates a standard key stroke (Up and Down)
 // 
 void SendKey(int key) {
	
	int sz = sizeof(INPUT);	
	int extra  = GetMessageExtraInfo();
	INPUT lpi;

	KEYBDINPUT ki;
	ki.dwFlags = 0; // Keydown
	ki.wVk = key;
	ki.wScan = 0; 
	ki.time = 0;
	ki.dwExtraInfo= extra;	
	lpi.type = INPUT_KEYBOARD;
	lpi.ki = ki;
 
	SendInput(1, &lpi, sz);
	
	INPUT lpi2;
	KEYBDINPUT ki2;
	ki2.dwFlags = KEYEVENTF_KEYUP; // KeyUp
	ki2.wVk = key;
	ki2.wScan = 0; 
	ki2.time = 0;
	ki2.dwExtraInfo = extra;
	lpi2.type = INPUT_KEYBOARD;
	lpi2.ki = ki2;
 
	SendInput(1, &lpi2, sz);
 }

  // Simulate a key press for a unicode letter
 void SendKeyUnicode(int key){
	
	int sz = sizeof(INPUT);	
	int extra  = GetMessageExtraInfo();
	INPUT lpi;

	KEYBDINPUT ki;
	ki.dwFlags = KEYEVENTF_UNICODE;
	ki.wVk = 0;
	ki.wScan = key; 
	ki.time = 0;
	ki.dwExtraInfo = extra;
	
	lpi.type = INPUT_KEYBOARD;
	lpi.ki = ki;

	SendInput(1, &lpi, sz);
 }

 void InitKeyMapper() {
	//fileLog = fopen("sknl.txt", "a+");
	// initialize keymap
	key_map[0] = MapKey2(0x12A0, 0, 0);		// A : 
	key_map[0]->sub_char = MapKey2(0x12A3, 0, SHIFT);
	key_map[1] = MapKey(0x1260);		// B
	key_map[2] = MapKey(0x1278);		// C
	key_map[2]->sub_char = MapKey3(0x1328, SHIFT); // CHE
	key_map[3] = MapKey(0x12F0);		// D
	key_map[3]->sub_char = MapKey3(0x12F8, SHIFT);		// D
	key_map[4] = MapKey2(0x12A5,0,0);		// E
	key_map[4]->sub_char = MapKey2(0x12A7, 0, SHIFT);	// ስ
	key_map[5] = MapKey(0x1348);		// F
	key_map[5]->sub_char = MapKey3(0x1280, SHIFT);	 // **
	key_map[5]->sub_char->dikala = 11;
	key_map[6] = MapKey(0x1308);		// G
	key_map[6]->dikala =11;
	key_map[6]->sub_char = MapKey3(0x1318, SHIFT);	 // SHIFT + G	
	key_map[7] = MapKey(0x1200);		// H
	key_map[7]->sub_char = MapKey3(0x1210, SHIFT);	 // SHIFT + H	
	//key_map[7]->sub_char2 = MapKey3(0x1280, CTRL);	 // CTRL + H
	key_map[8] = MapKey2(0x12A2,0,0);		// I  **
	key_map[8]->sub_char = MapKey2(0x12A4, 0, SHIFT);	 // SHIFT + I	
	key_map[9] = MapKey(0x1300);		// J
	key_map[10] = MapKey(0x12A8);		// K   ** 
	key_map[10]->dikala = 11;
	key_map[10]->sub_char = MapKey3(0x12B8, SHIFT);	 // **
	key_map[10]->sub_char->dikala=11;
	key_map[11] = MapKey(0x1208);		// L
	key_map[12] = MapKey(0x1218);		// M
	key_map[13] = MapKey(0x1290);		// N
	key_map[13]->sub_char = MapKey3(0x1298, SHIFT);	 // **
	key_map[14] = MapKey2(0x12A6, 0, 0);		// O
	key_map[15] = MapKey(0x1350);		// P
	key_map[15]->sub_char = MapKey3(0x1330, SHIFT);	 // **
	key_map[16] = MapKey(0x1240);		// Q **
	key_map[16]->dikala = 11;
	key_map[16]->sub_char = MapKey3(0x1250, SHIFT);	 //SHIFT + Q
	key_map[16]->sub_char->dikala = 11;
	key_map[17] = MapKey(0x1228);		// R
	key_map[17]->sub_char = MapKey3(0x1340, SHIFT);	 //SHIFT + R
	key_map[18] = MapKey(0x1230);		// S
	key_map[18]->sub_char = MapKey3(0x1338, SHIFT);	 // **
	key_map[19] = MapKey(0x1270);		// T
	key_map[19]->sub_char = MapKey3(0x1320, SHIFT);	 // **
	key_map[20] = MapKey2(0x12A1, 0, 0);		// U
	key_map[21] = MapKey(0x1268);		// V
	key_map[22] = MapKey(0x12C8);		// W
	key_map[22]->sub_char = MapKey3(0x1220, SHIFT);	 // **
	key_map[23] = MapKey(0x1238);		// X
	key_map[23]->sub_char = MapKey3(0x12D0, SHIFT);	 // **
	key_map[24] = MapKey(0x12E8);		// Y
	key_map[25] = MapKey(0x12D8);		// Z
	key_map[25]->sub_char = MapKey3(0x12E0, SHIFT);	 // *
};
