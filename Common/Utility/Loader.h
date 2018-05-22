#ifndef	LOADER_H
#define LOADER_H

#include <fstream>
#include <vector>

#include "Error.h"
#include "Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include <assimp/cimport.h>

#include "Math3D.h"

class Loader
{
public:
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

	struct Data3D
	{
		struct Mesh
		{
			std::string					name;

			Vertex::VERTEXTYPE::vertextype	vertexType;
			uint32_t			vertexSize;
			void*				vertexData;

			Vertex::INDEX::type	indexType;
			uint32_t			indexCount;
			void*				indexData;
		};
		std::vector<Mesh>	meshes;

		// armature
		//struct Bone
		//{
		//	std::string			name;
		//	Math3D::Mat4		mat;
		//	Math3D::Mat4		offset;
		//
		//	//Bone*				parent;
		//	std::vector<Bone>	childs;
		//};
		//std::vector<Bone>	armatures;
	};
	static void LoadData3D(const char* _filename, Data3D& _data3D, Vertex::VERTEXTYPE::vertextype _vertexType)
	{
		struct UV_NORMAL_COLOR3
		{
			float u, v, nx, ny, nz, r, g, b;
		} pointless = {};

		const aiScene* scene;
		Assimp::Importer Importer;
		static const int assimpFlags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_ImproveCacheLocality | aiProcess_SortByPType | aiProcess_FindInvalidData;

		scene = Importer.ReadFile(_filename, assimpFlags);

		_data3D.meshes.resize(scene->mNumMeshes);
		for (unsigned int iMesh = 0; iMesh != scene->mNumMeshes; ++iMesh)
		{
			// Name
			_data3D.meshes[iMesh].name = scene->mMeshes[iMesh]->mName.C_Str();

			// Vertex type
			_data3D.meshes[iMesh].vertexType = _vertexType;

			// Vertex size
			_data3D.meshes[iMesh].vertexSize = (uint32_t)Vertex::VERTEXTYPE::GetStride(_data3D.meshes[iMesh].vertexType) * scene->mMeshes[iMesh]->mNumVertices;

			// Vertex Data
			_data3D.meshes[iMesh].vertexData = Vertex::CreateVertexBuffer(_data3D.meshes[iMesh].vertexType, scene->mMeshes[iMesh]->mNumVertices);// new uint8_t[_data3D.meshes[iMesh].vertexSize * scene->mMeshes[iMesh]->mNumVertices];

			// Fill Vertex Data
			for (unsigned int iVert = 0; iVert != scene->mMeshes[iMesh]->mNumVertices; ++iVert)
			{
				size_t vertOffset = Vertex::VERTEXTYPE::GetStride(_data3D.meshes[iMesh].vertexType) * iVert;

				// POS1
				if (((_vertexType & Vertex::VERTEXTYPE::POS1) == Vertex::VERTEXTYPE::POS1) &&
					!((_vertexType & Vertex::VERTEXTYPE::POS2) == Vertex::VERTEXTYPE::POS2))
				{
					if (scene->mMeshes[iMesh]->HasPositions() == true)
						memcpy((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), &scene->mMeshes[iMesh]->mVertices[iVert].x, sizeof(float) * 1);
					else
						memset((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), 0, sizeof(float) * 1);

					vertOffset += sizeof(float) * 1;
				}
				// POS2
				else if (!((_vertexType & Vertex::VERTEXTYPE::POS1) == Vertex::VERTEXTYPE::POS1) &&
					((_vertexType & Vertex::VERTEXTYPE::POS2) == Vertex::VERTEXTYPE::POS2))
				{
					if (scene->mMeshes[iMesh]->HasPositions() == true)
						memcpy((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), &scene->mMeshes[iMesh]->mVertices[iVert].x, sizeof(float) * 2);
					else
						memset((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), 0, sizeof(float) * 2);

					vertOffset += sizeof(float) * 2;
				}
				// POS3
				else if ((_vertexType & Vertex::VERTEXTYPE::POS3) == Vertex::VERTEXTYPE::POS3)
				{
					if (scene->mMeshes[iMesh]->HasPositions() == true)
						memcpy((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), &scene->mMeshes[iMesh]->mVertices[iVert].x, sizeof(float) * 3);
					else
						memset((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), 0, sizeof(float) * 3);

					vertOffset += sizeof(float) * 3;
				}
				// UV
				if ((_vertexType & Vertex::VERTEXTYPE::UV) == Vertex::VERTEXTYPE::UV)
				{
					if (scene->mMeshes[iMesh]->HasTextureCoords(0) == true)
						memcpy((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), &scene->mMeshes[iMesh]->mTextureCoords[0][iVert].x, sizeof(float) * 2);
					else
						memset((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), 0, sizeof(float) * 2);

					vertOffset += sizeof(float) * 2;
				}
				// NORMAL
				if ((_vertexType & Vertex::VERTEXTYPE::NORMAL) == Vertex::VERTEXTYPE::NORMAL)
				{
					if (scene->mMeshes[iMesh]->HasNormals() == true)
						memcpy((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), &scene->mMeshes[iMesh]->mNormals[iVert].x, sizeof(float) * 3);
					else
						memset((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), 0, sizeof(float) * 3);

					vertOffset += sizeof(float) * 3;
				}
				// TANGENT
				if ((_vertexType & Vertex::VERTEXTYPE::TANGENT) == Vertex::VERTEXTYPE::TANGENT)
				{
					if (scene->mMeshes[iMesh]->HasTangentsAndBitangents() == true)
						memcpy((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), &scene->mMeshes[iMesh]->mTangents[iVert].x, sizeof(float) * 3);
					else
						memset((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), 0, sizeof(float) * 3);

					vertOffset += sizeof(float) * 3;
				}
				// BITANGENT
				if ((_vertexType & Vertex::VERTEXTYPE::BITANGENT) == Vertex::VERTEXTYPE::BITANGENT)
				{
					if (scene->mMeshes[iMesh]->HasTangentsAndBitangents() == true)
						memcpy((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), &scene->mMeshes[iMesh]->mBitangents[iVert].x, sizeof(float) * 3);
					else
						memset((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), 0, sizeof(float) * 3);

					vertOffset += sizeof(float) * 3;
				}
				// COLOR
				if (((_vertexType & Vertex::VERTEXTYPE::COLOR1) == Vertex::VERTEXTYPE::COLOR1) ||
					((_vertexType & Vertex::VERTEXTYPE::COLOR2) == Vertex::VERTEXTYPE::COLOR2) ||
					((_vertexType & Vertex::VERTEXTYPE::COLOR3) == Vertex::VERTEXTYPE::COLOR3) ||
					((_vertexType & Vertex::VERTEXTYPE::COLOR4) == Vertex::VERTEXTYPE::COLOR4))
				{
					uint8_t colorCount = 0;
					if ((_vertexType & Vertex::VERTEXTYPE::COLOR1) == Vertex::VERTEXTYPE::COLOR1)
						colorCount += 1;
					if ((_vertexType & Vertex::VERTEXTYPE::COLOR2) == Vertex::VERTEXTYPE::COLOR2)
						colorCount += 2;
					if ((_vertexType & Vertex::VERTEXTYPE::COLOR3) == Vertex::VERTEXTYPE::COLOR3)
						colorCount += 3;
					if ((_vertexType & Vertex::VERTEXTYPE::COLOR4) == Vertex::VERTEXTYPE::COLOR4)
						colorCount += 4;

					memcpy((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), &scene->mMeshes[iMesh]->mColors[0][iVert].r, sizeof(float) * colorCount);
					vertOffset += sizeof(float) * colorCount;
				}
				// BONE32
				if (((_vertexType & Vertex::VERTEXTYPE::BONE16_1) == Vertex::VERTEXTYPE::BONE16_1) ||
					((_vertexType & Vertex::VERTEXTYPE::BONE16_2) == Vertex::VERTEXTYPE::BONE16_2) ||
					((_vertexType & Vertex::VERTEXTYPE::BONE16_3) == Vertex::VERTEXTYPE::BONE16_3) ||
					((_vertexType & Vertex::VERTEXTYPE::BONE16_4) == Vertex::VERTEXTYPE::BONE16_4))
				{
					uint8_t boneCount = 0;
					if ((_vertexType & Vertex::VERTEXTYPE::BONE16_1) == Vertex::VERTEXTYPE::BONE16_1)
						boneCount += 1;
					if ((_vertexType & Vertex::VERTEXTYPE::BONE16_2) == Vertex::VERTEXTYPE::BONE16_2)
						boneCount += 2;
					if ((_vertexType & Vertex::VERTEXTYPE::BONE16_3) == Vertex::VERTEXTYPE::BONE16_3)
						boneCount += 3;
					if ((_vertexType & Vertex::VERTEXTYPE::BONE16_4) == Vertex::VERTEXTYPE::BONE16_4)
						boneCount += 4;

					memset((void*)((size_t)_data3D.meshes[iMesh].vertexData + vertOffset), 0, (sizeof(float) + sizeof(uint16_t)) * boneCount);
					vertOffset += (sizeof(float) + sizeof(uint16_t)) * boneCount;
				}
			}

			size_t weightCount = Vertex::VERTEXTYPE::GetBoneCount(_data3D.meshes[iMesh].vertexType);
			if (weightCount > 0)
			{
				for (unsigned int iBone = 0; iBone != scene->mMeshes[iMesh]->mNumBones; ++iBone)
				{
					scene->mMeshes[iMesh]->mBones[iBone]->mName;
					scene->mMeshes[iMesh]->mBones[iBone]->mOffsetMatrix;

					for (unsigned int iWeight = 0; iWeight != scene->mMeshes[iMesh]->mBones[iBone]->mNumWeights; ++iWeight)
					{
						unsigned int vertexIndex = scene->mMeshes[iMesh]->mBones[iBone]->mWeights[iWeight].mVertexId;
						float weight = scene->mMeshes[iMesh]->mBones[iBone]->mWeights[iWeight].mWeight;

						Vertex::BONE16::SetLeastRelevantAttribute(_data3D.meshes[iMesh].vertexData, _data3D.meshes[iMesh].vertexType, vertexIndex, iBone, weight);
					}
				}
			}

			// index type
			_data3D.meshes[iMesh].indexType = Vertex::INDEX::UINT32;

			// index count
			_data3D.meshes[iMesh].indexCount = 0;
			for (unsigned int iFace = 0; iFace != scene->mMeshes[iMesh]->mNumFaces; ++iFace)
			{
				if (scene->mMeshes[iMesh]->mFaces[iFace].mNumIndices == 3)
					_data3D.meshes[iMesh].indexCount += 3;
				else
				{
					Error::Report("BAD INDICE COUNT");
					return;
				}
			}

			// index data
			size_t triangleCount = 0;
			_data3D.meshes[iMesh].indexData = new uint8_t[Vertex::INDEX::GetSize(_data3D.meshes[iMesh].indexType) * _data3D.meshes[iMesh].indexCount];
			if (_data3D.meshes[iMesh].indexType == Vertex::INDEX::UINT32)
			{
				for (unsigned int iFace = 0; iFace != scene->mMeshes[iMesh]->mNumFaces; ++iFace)
				{
					size_t indexOffset = Vertex::INDEX::GetSize(_data3D.meshes[iMesh].indexType);
					size_t triangleOffset = indexOffset * 3 * triangleCount;

					*(uint32_t*)((size_t)_data3D.meshes[iMesh].indexData + triangleOffset + indexOffset * 0) = scene->mMeshes[iMesh]->mFaces[iFace].mIndices[0];
					*(uint32_t*)((size_t)_data3D.meshes[iMesh].indexData + triangleOffset + indexOffset * 1) = scene->mMeshes[iMesh]->mFaces[iFace].mIndices[1];
					*(uint32_t*)((size_t)_data3D.meshes[iMesh].indexData + triangleOffset + indexOffset * 2) = scene->mMeshes[iMesh]->mFaces[iFace].mIndices[2];

					++triangleCount;
				}
			}
			else
			{
				Error::Report("BAD INDICE TYPE");
				return;
			}
		}
	}
	static void UnloadData3D(Data3D& _data3D)
	{
		for (size_t iMesh = 0; iMesh != _data3D.meshes.size(); ++iMesh)
		{
			delete[] _data3D.meshes[iMesh].vertexData;
			delete[] _data3D.meshes[iMesh].indexData;
		}
	}

	struct DataImage
	{
		uint32_t width = 0;
		uint32_t height = 0;
		uint8_t channelCount;
		uint8_t bytesPerChannel;
		void* data = nullptr;
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

			size_t size = _dataImage.channelCount * _dataImage.bytesPerChannel * _dataImage.width * _dataImage.height;

			_dataImage.data = new uint8_t[size];
			if (fread(_dataImage.data, 1, size, fTGA) != size)
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
};

#endif