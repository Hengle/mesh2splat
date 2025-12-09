// from DearGPU.com, copyright allows to do anything

#pragma once

#include <string>
#include <map>
#include "GeneralArchive.h"

class CIniArchive
{
public:
	struct SValueEx
	{
//		bool bLoadedFromFile = false;
		// comment line before value, may be empty, lines are separated by "\r\n"
		std::string CommentBlock;
		std::string Value;

		void Get(float& inoutValue, float Default)
		{
			inoutValue = Default;
			sscanf_s(Value.c_str(), "%f", &inoutValue);
		}
		void Get(int& inoutValue, int Default)
		{
			inoutValue = Default;
			sscanf_s(Value.c_str(), "%d", &inoutValue);
		}
	};

	bool Load(const wchar_t* pFileName);

	bool Save(const wchar_t* pFileName);

	void Get(const char* Key, float& inoutValue, float Default) { if(auto p = GetByKey(Key))p->Get(inoutValue, Default); }
	void Get(const char* Key, int& inoutValue, int Default) { if (auto p = GetByKey(Key))p->Get(inoutValue, Default); }

	// @return 0 if not found
	SValueEx* GetByKey(const std::string& Key);

	// @param InCommentBlock may be 0, data is copied
	void AddKeyValue(const char *Key, const std::string& Value, const char* InCommentBlock = 0);
	void AddKeyValue(const char *Key, float Value, const char* InCommentBlock = 0);
	void AddKeyValue(const char* Key, int Value, const char* InCommentBlock = 0);
	
	bool IsLoading() const { return bIsLoading; }
	bool IsSaving() const { return bIsSaving; }

private:
	bool bIsLoading = false;
	bool bIsSaving = false;
	

	// todo: support sections
	std::map<std::string, SValueEx> KeyValuePairs;
};

