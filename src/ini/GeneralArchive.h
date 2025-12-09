// from DearGPU.com, copyright allows to do anything

#pragma once
#include <vector>
#include <assert.h>
#include <filesystem> 

inline size_t IO_GetFileSize(const wchar_t* name)
{
	assert(name);

	return std::filesystem::file_size(name);
}


// 32bit max file size
// todo: better error handling (bool IsValid() so not every operation needs to be checked)
class CGeneralArchive
{
public:
	CGeneralArchive()
		: bIsLoading(false)
		, outHandle(0)
		, LoadPos(0)
	{
	}

	~CGeneralArchive()
	{
		if(IsSaving())
		{
			size_t FileSize = Data.size();

			if(fwrite(&Data[0], FileSize, 1, outHandle) != 1)
			{
				assert(0);
			}
			fclose(outHandle);
		}
	}

	// @ param bZeroTermination allows to a byte of 0 termination, could be made working for unicode as well
	bool OpenForLoading(const wchar_t* pFileName, bool bZeroTermination = false)
	{
		assert(pFileName);
		assert(!IsLoading());
		assert(!IsSaving());
		assert(!Data.size());

		FILE *in = 0;

		if(_wfopen_s(&in, pFileName, L"rb") != 0)
		{
			return false;
		}

		// todo: check to fit in 32bit
		size_t FileSize = IO_GetFileSize(pFileName);

		if(FileSize == -1)
		{
			// internal error 
			return false;
		}

		Data.resize(FileSize + bZeroTermination);

		if(FileSize)
		{
			if (fread(&Data[0], FileSize, 1, in) != 1)
			{
				assert(0);
			}
		}
		if(bZeroTermination)
		{
			Data[FileSize] = 0;
		}

		bIsLoading = true;

		fclose(in);
		return true;
	}

	bool OpenForSaving(const wchar_t* pFileName)
	{
		assert(pFileName);
		assert(!IsLoading());
		assert(!IsSaving());
		assert(!Data.size());

		// 1MB reserve to avoid reallocations
		Data.reserve(1024 * 1024);

		if(_wfopen_s(&outHandle, pFileName, L"wb") != 0)
		{
			return false;
		}

		assert(IsSaving());
		return true;
	}

	bool IsLoading() const { return bIsLoading; }

	bool IsSuccessfulLoading() const
	{
		return IsLoading() && LoadPos > 0 && LoadPos == Data.size();
	}

	bool IsSaving() const { return outHandle != 0; }


	// @param InChunkId e.g. 'Chk8' or 'BVH_'
	bool Chunk(const uint32_t InChunkId)
	{
		assert(InChunkId);

		uint32_t BackupPos = LoadPos;
		
		uint32_t ChunkId = InChunkId;

		Serialize(&ChunkId, sizeof(ChunkId));

		if(IsSaving())
		{
			return true;
		}

		if(ChunkId == InChunkId)
		{
			return true;
		}

		LoadPos = BackupPos;
		return false;
	}

	void Serialize(void* InData, uint32_t InSize)
	{
		assert(InData);
		assert(InSize);

		if(IsLoading())
		{
			size_t MaxSize = Data.size();
			assert(LoadPos + InSize <= MaxSize);
			memcpy(InData, &Data[LoadPos], InSize);
			LoadPos += InSize;
		}
		else
		{
			size_t OldSize = Data.size();
			Data.resize(OldSize + InSize);
			memcpy(&Data[OldSize], InData, InSize);
		}
	}

	size_t GetSize() const
	{
		return Data.size();
	}

	const uint8_t* GetDataPtr() const
	{
		return &Data[0];
	}

public: 
	bool bIsLoading;
	FILE *outHandle;

	std::vector<uint8_t> Data;
	uint32_t LoadPos;
};

// POD
inline void operator &(CGeneralArchive& ar, float& In) { ar.Serialize(&In, sizeof(In)); }
inline void operator &(CGeneralArchive& ar, uint32_t& In) { ar.Serialize(&In, sizeof(In)); }
inline void operator &(CGeneralArchive& ar, uint16_t& In) { ar.Serialize(&In, sizeof(In)); }
inline void operator &(CGeneralArchive& ar, uint8_t& In) { ar.Serialize(&In, sizeof(In)); }
inline void operator &(CGeneralArchive& ar, int32_t& In) { ar.Serialize(&In, sizeof(In)); }
inline void operator &(CGeneralArchive& ar, int16_t& In) { ar.Serialize(&In, sizeof(In)); }
inline void operator &(CGeneralArchive& ar, int8_t& In) { ar.Serialize(&In, sizeof(In)); }




template<class T>
void operator &(CGeneralArchive & ar, std::vector<T>& InVector)
{
	uint32_t VectorSize = InVector.size();
	ar & VectorSize;

	if(ar.IsLoading())
	{
		InVector.resize(VectorSize);
	}

	for(uint32_t i = 0; i < VectorSize; ++i)
	{
		T& el = InVector[i];

		ar & el;
	}
}
