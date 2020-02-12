#include "KeyboardClass.h"

KeyboardClass::KeyboardClass()
{
	for (int i = 0; i < 256; i++)
	{
		this->keyStates[i] = false;//Initialize all key states to off(false)
		this->keyStates_Trigger[i] = false;
	}
}

bool KeyboardClass::KeyIsPressed(const unsigned char keycode)
{
	return this->keyStates[keycode];
}
bool KeyboardClass::KeyIsTrigger(const unsigned char keycode)
{
	if (this->keyStates_Trigger[keycode])
	{
		this->keyStates_Trigger[keycode] = false;
		return true;
	}
	else
	{
		return false;
	}
}
bool KeyboardClass::KeyBufferIsEmpty()
{
	return this->keyBuffer.empty();
}

bool KeyboardClass::CharBufferIsEmpty()
{
	return this->charBuffer.empty();
}

KeyboardEvent KeyboardClass::ReadKey()
{
	if (this->keyBuffer.empty())					//If no keys to be read?
	{
		return KeyboardEvent();						//return empty keyboard event
	}
	else
	{
		KeyboardEvent e = this->keyBuffer.front();	//Get first Keyboard Event from queue
		this->keyBuffer.pop();						//Remove first item from queue
		return e;									//Return keyboard event
	}
}

unsigned char KeyboardClass::ReadChar()
{
	if (this->charBuffer.empty())					//If no keys to be read?
	{
		return 0;									//return 0(NULL char)
	}
	else
	{
		unsigned char e = this->charBuffer.front();	//Get first char from queue
		this->charBuffer.pop();						//Remove first char from queue
		return e;									//Returns char
	}
}

void KeyboardClass::OnKeyPressed(const unsigned char key)
{
	this->keyStates[key] = true;
	this->keyStates_Trigger[key] = true;

	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void KeyboardClass::OnKeyReleased(const unsigned char key)
{
	this->keyStates[key] = false;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void KeyboardClass::TriggerReleased(const unsigned char key)
{
	this->keyStates_Trigger[key] = false;
}

void KeyboardClass::OnChar(const unsigned char key)
{
	this->charBuffer.push(key);
}

void KeyboardClass::EnableAutoRepeatKeys()
{
	this->autoRepeatKeys = true;
}

void KeyboardClass::DisableAutoRepeatKeys()
{
	this->autoRepeatKeys = false;
}

void KeyboardClass::EnableAutoRepeatChars()
{
	this->autoRepearChars = true;
}

void KeyboardClass::DisableAutoRepeatChars()
{
	this->autoRepearChars = false;
}

bool KeyboardClass::IsKeysAutoRepeat()
{
	return this->autoRepeatKeys;
}

bool KeyboardClass::IsCharsAutoRepeat()
{
	return this->autoRepearChars;
}