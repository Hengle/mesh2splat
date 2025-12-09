// from DearGPU.com, copyright allows to do anything

#include "IniArchive.h"
#include "Parse.h"

bool CIniArchive::Load(const wchar_t* pFileName)
{
	assert(pFileName);
	assert(!bIsLoading);
	assert(!bIsSaving);
	
	// includes 0 termination
	CGeneralArchive LoadedArchive;

	if(!LoadedArchive.OpenForLoading(pFileName, true))
	{
		return false;
	}

	const uint8_t *pFile = LoadedArchive.GetDataPtr();

	std::string Key, Value;
	std::string Line;
	// lines separated by "\r\n"
	std::string PendingCommentBlock;

	while(*pFile)
	{
		parseLine(pFile, Line);

		// parse line
		{
			const uint8_t *p = (const uint8_t *)Line.c_str();

			parseWhiteSpaceOrLF(p);
			if (!parseStartsWith(p, ";"))
			{
				if (parseName(p, Key))
				{
					parseWhiteSpaceOrLF(p);

					if (parseStartsWith(p, "="))
					{
						parseWhiteSpaceOrLF(p);
						parseLine(p, Value);

						SValueEx ValueEx;

						ValueEx.Value = Value;
						ValueEx.CommentBlock = PendingCommentBlock;
						PendingCommentBlock.clear();
//						ValueEx.bLoadedFromFile = true;

						// duplicates override the former settings
						KeyValuePairs[Key] = ValueEx;
						continue;
					}
					else
					{
						// bad syntax in ini, ignoring line
						assert(0);
					}
				}
			}
			else
			{
				if (!PendingCommentBlock.empty())
				{
					PendingCommentBlock += "\r\n";
				}
				PendingCommentBlock += Line;
			}
		}
	}

	bIsLoading = true;

	return true;
}

bool StartsWithSeparatorLine(const char* CommentBlock)
{
	// e.g. "; -------------------------\r\n"

	const uint8_t* p = (const uint8_t*)CommentBlock;

	parseWhiteSpaceOrLF(p);
	if (*p == ';')
	{
		++p;
		parseWhiteSpaceOrLF(p);
		if (*p == '-')
		{
			return true;
		}
	}

	return false;
}

bool CIniArchive::Save(const wchar_t* pFileName)
{
	assert(pFileName);
	assert(!bIsSaving);

	bIsLoading = false;

	FILE* outHandle = 0;
	if(_wfopen_s(&outHandle, pFileName, L"wb") != 0)
	{
		return false;
	}

	fprintf(outHandle, "; This file is auto generated and can be user edited.\r\n");
	fprintf(outHandle, "; Note: * Comments get regenerated from code.\r\n\r\n");
	fprintf(outHandle, ";       * If a default changes (in code) and the value exists in the ini it will not get updated there.\r\n\r\n");

	bool bFirst = true;

	// append data that was missing
	for (auto it = KeyValuePairs.begin(), end = KeyValuePairs.end(); it != end; ++it)
	{
		if (!bFirst)
		{
			bool bStartsWithSeparatorLine = StartsWithSeparatorLine(it->second.CommentBlock.c_str());

			if(!bStartsWithSeparatorLine)
			{
				fprintf(outHandle, "; -------------------------\r\n");
			}
		}
		bFirst = false;

		if (!it->second.CommentBlock.empty())
		{
			fprintf(outHandle, "; %s\r\n", it->second.CommentBlock.c_str());
		}
		fprintf(outHandle, "%s = %s\r\n", it->first.c_str(), it->second.Value.c_str());
	}

	fclose(outHandle);
	
	bIsSaving = true;

	return true;
}

CIniArchive::SValueEx* CIniArchive::GetByKey(const std::string &Key)
{
	auto it = KeyValuePairs.find(Key);
	
	if(it != KeyValuePairs.end())
	{
		return &(it->second);
	}

	return nullptr;
}

void CIniArchive::AddKeyValue(const char *Key, const std::string& Value, const char *InCommentBlock)
{
	SValueEx ValueEx;

	ValueEx.Value = Value;
//	ValueEx.bLoadedFromFile = false;

	if(InCommentBlock)
	{
		ValueEx.CommentBlock = InCommentBlock;
	}

	auto dataPair = std::pair<std::string, SValueEx>(Key, ValueEx);

	// duplicate key, use the last one but warn in debug
//	assert(KeyValuePairs.find(dataPair) == KeyValuePairs.end());

	KeyValuePairs.insert(dataPair);
}

void CIniArchive::AddKeyValue(const char *Key, float Value, const char *OptionalHelp)
{
	char str[80];
	sprintf_s(str, sizeof(str) / sizeof(str[0]), "%g", Value);
	AddKeyValue(Key, str, OptionalHelp);
}

void CIniArchive::AddKeyValue(const char* Key, int Value, const char* OptionalHelp)
{
	char str[80];
	sprintf_s(str, sizeof(str) / sizeof(str[0]), "%d", Value);
	AddKeyValue(Key, str, OptionalHelp);
}