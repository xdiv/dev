#pragma once

#ifndef _OBJREADER_H_
#define _OBJREADER_H_

#include <fstream>
#include <vector>
#include <D3DX10math.h>

using namespace std;

typedef struct Vertices
{
	float x, y, z;//v
	float tu, tv;//vt
	float nx, ny, nz;//vn
}Vertices;


class ObjReader
{
private:
	typedef struct Point3D
	{
		int v, vt, vn;
	}Point3D;
public:
	ObjReader(void);
	~ObjReader(void);
	bool GetTriangeleList(Vertices ** , int &, bool remake = false);
	bool ReadFileStructure(WCHAR *);

private:
	int vertexCount, textureCount, normalCount, faceCount,
		total;
	vector<D3DXVECTOR3> * v;
	vector<D3DXVECTOR2> * vt;
	vector<D3DXVECTOR3> * vn;
	vector<vector<Point3D>> * f;

	char garbage;
	Vertices *m_vertices;
};
#endif
