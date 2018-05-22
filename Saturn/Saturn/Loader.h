#ifndef	LOADER_H
#define LOADER_H

#include "../../Common/Utility/Error.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>
#include <vector>

#include "../../Common/Utility/Structure.h"

namespace Loader
{
	static void LoadText(const char* _filename, std::vector<char>& _text)
	{
		size_t fileSize = 0;

		std::ifstream file(_filename, std::ios::ate | std::ios::binary);

		if (file.good())
		{
			fileSize = (size_t)file.tellg();
			file.seekg(0);

			if (file.good())
			{
				_text.resize(fileSize);
				file.read(_text.data(), fileSize);
				file.close();
			}
			else
			{
				file.close();
				Error::Report((std::string("Error reading \"") + _filename + std::string("\"")).c_str());
				return;
			}
		}
		else
		{
			file.close();
			Error::Report((std::string("Error reading \"") + _filename + std::string("\"")).c_str());
			return;
		}
	}

	struct MeshData
	{
		std::string						name;

		enum DATATYPE
		{
			UNKNOWN = 0,
			POS3,
			UV_NORMAL,
		};

		static inline Structure DATATYPEToStructure(DATATYPE _datatype)
		{
			Structure structure;
			switch (_datatype)
			{
			case POS3:
				structure.SetDatas({
					Structure::Data::Get("Position3", Structure::DATATYPE::FLOAT, 3),
				});
			case UV_NORMAL:
				structure.SetDatas({
					Structure::Data::Get("Position3", Structure::DATATYPE::FLOAT, 3),
					Structure::Data::Get("Normal", Structure::DATATYPE::FLOAT, 3),
				});
			case UNKNOWN:
				Error::Report("UNKNOWN datatype");
			default:
				Error::Report("Missing datatype");
			}

			return {};
		}

		struct Vertex
		{
			DATATYPE	type;
			Structure	structure;
			size_t		size;
			void*		data;
		};
		std::vector<Vertex>			vertices;

		Structure::DATATYPE::datatype	indexType;
		size_t							indexSize;
		uint32_t						indexCount;
		void*							indexData;

		void SetVertexStructures(std::vector<MeshData::DATATYPE> _datatype)
		{
			vertices.resize(_datatype.size());
			for (size_t i = 0; i != _datatype.size(); ++i)
			{
				switch (_datatype[i])
				{
				case POS3:
					vertices[i].structure.SetDatas({
						Structure::Data::Get("Position", Structure::DATATYPE::FLOAT, 3),
					});
					break;
				case UV_NORMAL:
					vertices[i].structure.SetDatas({
						Structure::Data::Get("Uv", Structure::DATATYPE::FLOAT, 2),
						Structure::Data::Get("Normal", Structure::DATATYPE::FLOAT, 3),
					});
					break;
				default:
					Error::Report(std::string("Missing Mesh Datatype") + std::to_string(i) + "on Loader::MeshData::SetStructure()");
				}
			}
		}
	};
	static MeshData LoadMeshData(const char* _filename, std::vector<MeshData::DATATYPE> _datatype)
	{
		const aiScene* scene;
		Assimp::Importer Importer;
		static const int assimpFlags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_ImproveCacheLocality | aiProcess_SortByPType | aiProcess_FindInvalidData;

		scene = Importer.ReadFile(_filename, assimpFlags);
		if (scene == nullptr)
		{
			Error::Report(std::string("While attempting to load mesh from file:") + _filename + " file was not found.");
			return {};
		}

		aiMesh* mesh = scene->mMeshes[0];

		MeshData meshData;

		meshData.name = mesh->mName.C_Str();

		// Index
		size_t indexCount = 0;
		for (unsigned int iFace = 0; iFace != mesh->mNumFaces; ++iFace)
		{
			if (mesh->mFaces[iFace].mNumIndices == 3)
				indexCount += 3;
			else
			{
				Error::Report("BAD INDICE COUNT");
				return {};
			}
		}

		if (indexCount <= 0xFFFF)
			meshData.indexType = Structure::DATATYPE::UINT16;
		else
			meshData.indexType = Structure::DATATYPE::UINT32;
		meshData.indexSize = Structure::DATATYPE::GetSize(meshData.indexType) * indexCount;
		meshData.indexCount = (uint32_t)indexCount;
		meshData.indexData = new uint8_t[meshData.indexSize];

		// Fill Index
		size_t triangleCount = 0;
		if (meshData.indexType == Structure::DATATYPE::UINT16)
		{
			for (unsigned int iFace = 0; iFace != mesh->mNumFaces; ++iFace)
			{
				size_t indexOffset = Structure::DATATYPE::GetSize(Structure::DATATYPE::UINT16);
				size_t triangleOffset = indexOffset * 3 * triangleCount;

				*(uint16_t*)((size_t)meshData.indexData + triangleOffset + indexOffset * 0) = mesh->mFaces[iFace].mIndices[0];
				*(uint16_t*)((size_t)meshData.indexData + triangleOffset + indexOffset * 1) = mesh->mFaces[iFace].mIndices[1];
				*(uint16_t*)((size_t)meshData.indexData + triangleOffset + indexOffset * 2) = mesh->mFaces[iFace].mIndices[2];

				++triangleCount;
			}
		}
		else if (meshData.indexType == Structure::DATATYPE::UINT32)
		{
			for (unsigned int iFace = 0; iFace != mesh->mNumFaces; ++iFace)
			{
				size_t indexOffset = Structure::DATATYPE::GetSize(Structure::DATATYPE::UINT32);
				size_t triangleOffset = indexOffset * 3 * triangleCount;

				*(uint32_t*)((size_t)meshData.indexData + triangleOffset + indexOffset * 0) = mesh->mFaces[iFace].mIndices[0];
				*(uint32_t*)((size_t)meshData.indexData + triangleOffset + indexOffset * 1) = mesh->mFaces[iFace].mIndices[1];
				*(uint32_t*)((size_t)meshData.indexData + triangleOffset + indexOffset * 2) = mesh->mFaces[iFace].mIndices[2];

				++triangleCount;
			}
		}

		// Vertex
		meshData.SetVertexStructures(_datatype);
		for (size_t i = 0; i != _datatype.size(); ++i)
		{
			// Mesh
			meshData.vertices[i].type = _datatype[i];

			// Vertex
			meshData.vertices[i].structure.SetCount(mesh->mNumVertices);
			meshData.vertices[i].size = meshData.vertices[i].structure.GetSize();
			meshData.vertices[i].data = new uint8_t[meshData.vertices[i].size];

			// Fill Vertex
			switch (meshData.vertices[i].type)
			{
			case MeshData::DATATYPE::POS3:
				for (unsigned int iVert = 0; iVert != mesh->mNumVertices; ++iVert)
				{
					size_t vertOffset = meshData.vertices[i].structure.GetIndexAddressOffset(iVert);

					// POS
					if (mesh->HasPositions() == true)
						memcpy((void*)((size_t)meshData.vertices[i].data + vertOffset), &mesh->mVertices[iVert].x, sizeof(float) * 3);
					else
						memset((void*)((size_t)meshData.vertices[i].data + vertOffset), 0, sizeof(float) * 3);
					vertOffset += sizeof(float) * 3;
				}
				break;
			case MeshData::DATATYPE::UV_NORMAL:
				for (unsigned int iVert = 0; iVert != mesh->mNumVertices; ++iVert)
				{
					size_t vertOffset = meshData.vertices[i].structure.GetIndexAddressOffset(iVert);

					// UV
					if (mesh->HasTextureCoords(0) == true)
						memcpy((void*)((size_t)meshData.vertices[i].data + vertOffset), &mesh->mTextureCoords[0][iVert].x, sizeof(float) * 2);
					else
						memset((void*)((size_t)meshData.vertices[i].data + vertOffset), 0, sizeof(float) * 2);
					vertOffset += sizeof(float) * 2;

					// NORM
					if (mesh->HasNormals() == true)
						memcpy((void*)((size_t)meshData.vertices[i].data + vertOffset), &mesh->mNormals[iVert].x, sizeof(float) * 3);
					else
						memset((void*)((size_t)meshData.vertices[i].data + vertOffset), 0, sizeof(float) * 3);
					vertOffset += sizeof(float) * 3;
				}
				break;
			default:
				Error::Report("Missing Mesh Datatype on Loader::LoadMeshData()");
			}
		}

		return meshData;
	}
	static void UnloadMeshData(MeshData& _meshData)
	{
		for (size_t i = 0; i != _meshData.vertices.size(); ++i)
			delete[] _meshData.vertices[i].data;

		delete[] _meshData.indexData;
	}

	struct DataImage
	{
		uint32_t width = 0;
		uint32_t height = 0;
		uint8_t channelCount;
		uint8_t bytesPerChannel;
		void* data = nullptr;
		size_t dataSize;
	};
	static void LoadTGA(const char* _filename, DataImage& _dataImage)
	{
		_dataImage.width = 0;
		_dataImage.height = 0;
		_dataImage.channelCount = 0;
		_dataImage.bytesPerChannel = 0;
		if (_dataImage.data != nullptr)
			delete[] _dataImage.data;
		_dataImage.data = nullptr;

		uint8_t header[12];

		FILE* fTGA;
		fTGA = fopen(_filename, "rb");

		uint8_t cTGAcompare[12] = { 0,0,10,0,0,0,0,0,0,0,0,0 };
		uint8_t uTGAcompare[12] = { 0,0, 2,0,0,0,0,0,0,0,0,0 };

		if (fTGA == NULL)
			Error::Report((std::string(_filename) + " Missing").c_str());

		if (fread(&header, sizeof(header), 1, fTGA) == 0)
			return;
		if (memcmp(cTGAcompare, &header, sizeof(header)) == 0)
			Error::Report((std::string(_filename) + " compressed TGA not supported").c_str());
		else if (memcmp(uTGAcompare, &header, sizeof(header)) == 0)
		{
			uint8_t header[6];

			if (fread(header, sizeof(header), 1, fTGA) == 0)
				return;

			_dataImage.width = header[1] * 256 + header[0];
			_dataImage.height = header[3] * 256 + header[2];
			uint8_t bpp = header[4];

			if (_dataImage.width == 0 || _dataImage.height == 0 || (bpp != 24 && bpp != 32))
				Error::Report((std::string(_filename) + " TGA has invalid data").c_str());

			if (bpp == 24)
			{
				_dataImage.channelCount = 3;
				_dataImage.bytesPerChannel = 1;
			}
			else if (bpp == 32)
			{
				_dataImage.channelCount = 4;
				_dataImage.bytesPerChannel = 1;
			}

			_dataImage.dataSize = _dataImage.channelCount * _dataImage.bytesPerChannel * _dataImage.width * _dataImage.height;

			_dataImage.data = new uint8_t[_dataImage.dataSize];
			if (fread(_dataImage.data, 1, _dataImage.dataSize, fTGA) != _dataImage.dataSize)
			{
				_dataImage.width = 0;
				_dataImage.height = 0;
				_dataImage.channelCount = 0;
				_dataImage.bytesPerChannel = 0;
				delete[] _dataImage.data;
				_dataImage.data = nullptr;

				Error::Report((std::string(_filename) + " fread failed").c_str());

				return;
			}
		}
		else
		{
			_dataImage.width = 0;
			_dataImage.height = 0;
			_dataImage.channelCount = 0;
			_dataImage.bytesPerChannel = 0;
			delete[] _dataImage.data;
			_dataImage.data = nullptr;

			Error::Report((std::string(_filename) + " TGA header has invalid data").c_str());
			return;
		}
	}
	static void UnloadTGA(DataImage& _dataImage)
	{
		_dataImage.width = 0;
		_dataImage.height = 0;
		delete[] _dataImage.data;
		_dataImage.data = nullptr;
	}
}

#endif