// from DearGPU.com, copyright allows to do anything

#include "Parse.h"

void parseWhiteSpaceOrLF(const uint8_t* &p)
{
	while(*p != 0 && *p <= ' ')
	{
		++p;
	}
}

void parseWhiteSpaceNoLF(const uint8_t* &p)
{
	for(;;++p)
	{
		uint8_t c = *p;

		if (c == 0)
		{
			break;
		}
		
		if (!(c == ' ' || c == '\t'))
		{
			break;
		}
	}
}

bool parseStartsWith(const uint8_t* &_p, const char* Token)
{
	const uint8_t* p = _p;
	const uint8_t* t = (const uint8_t*)Token;

	while(*t)
	{
		if(*p != *t)
		{
			return false;
		}
		++p;
		++t;
	}

	_p = p;
	return true;
}

// @return true if a return was found
bool ParseToEndOfLine(const uint8_t* &p)
{
	while (*p)
	{
		if (*p == 13)		// CR
		{
			++p;

			if (*p == 10)	// CR+LF
				++p;

			return true;
		}
		if (*p == 10)		// LF
		{
			++p;
			return true;
		}
		++p;
	}

	return false;
}

void parseLine(const uint8_t* &p, std::string &Out)
{
	Out.clear();

	// can be optimized, does a lot of resize
	while(*p)
	{
		if(*p == 13)		// CR
		{
			++p;

			if (*p == 10)	// CR+LF
				++p;

			break;
		}
		if(*p == 10)		// LF
		{
			++p;
			break;
		}

		Out += *p++;
	}
}

// without Numbers
bool isNameCharacter(uint8_t Value)
{
	return (Value >= 'a' && Value <= 'z') || (Value >= 'A' && Value <= 'Z') || Value == '_';
}

bool isDigitCharacter(uint8_t Value)
{
	return Value >= '0' && Value <= '9';
}

bool parseName(const uint8_t* &p, std::string &Out)
{
	bool Ret = false;

	Out.clear();

	// can be optimized, does a lot of resize

	if (isNameCharacter(*p))
	{
		Out += *p++;
		Ret = true;
	}

	while(isNameCharacter(*p ) || isDigitCharacter(*p))
	{
		Out += *p++;
	}

	return Ret;
}

SPushStringA<MAX_PATH> parsePath(const uint8_t* &p)
{
	SPushStringA<MAX_PATH> Ret;

	for (;;)
	{
		uint8_t c = *p++;

		if ((c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			(c >= '0' && c <= '9') ||
			c == '\\' ||
			c == '/' ||
			c == ' ' ||
			c == '@' ||
			c == '.' ||
			c == '_')
		{
			Ret.push(c);
		}
		else
		{
			break;
		}
	}

	return Ret;
}

