#include "objreader.h"


ObjReader::ObjReader(void)
{
	m_vertices = 0;
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;
	v = 0;
	vn = 0;
	vt = 0;
	f = 0;
}


ObjReader::~ObjReader(void)
{
	delete[] m_vertices;
	m_vertices = 0;
}

bool ObjReader::ReadFileStructure(WCHAR * filename)
{
	char input, prev = 0;
	char *line;
	int location;
	ifstream fs;

	// Initialize the counts.
	
	float a, b, c;
	Point3D poin;

	fs.open(filename);

	if(fs.fail())
	{
		fs.close();
		return false;
	}

	v = new vector<D3DXVECTOR3>();
	vt = new vector<D3DXVECTOR2>();
	vn = new vector<D3DXVECTOR3>();
	f = new vector<vector<Point3D>>();
	

	// Read from the file and continue to read until the end of the file is reached.
	fs.get(input);
	while(!fs.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if(prev == 'v' && input == 't' ) //
		{
			textureCount++;
			fs >> a >> b;
			vt->push_back(D3DXVECTOR2(a, b));
		} else if(prev == 'v' && input == ' ' ) //
		{
			vertexCount++; 
			fs >> a >> b >> c;
			v->push_back(D3DXVECTOR3(a, b, c * -1.0f));
		} else if(prev == 'v' && input == 'n' )
		{
			normalCount++; 
			fs >> a >> b >> c;
			vn->push_back(D3DXVECTOR3(a , b, c));
		}else if(input == 'f')
		{
			faceCount++;
			vector<Point3D> list;
			while(input != '\n')
			{
				fs >> poin.v;
				fs >> input;
				fs >> poin.vt;
				fs >> input;
				fs >> poin.vn;

				fs.get(input);
				list.push_back(poin);
			}
			f->push_back(list);
		}else if(input == '#')
		{
			while (input != '\n')
			{
				fs.get(input);
			}
		}

		// Start reading the beginning of the next line.
		prev = input;
		fs.get(input);
	}

	// Close the file.
	fs.close();

	return true;
}

//ið turimø duomenø sukuria TrinagleList masyvà, ir gràþina pinteri á já.
//Jei masyvas jau sukurtas, tiesiog gràþina masyvà.
//jei "remake" true, masyvà perkruria
void ObjReader::GetTriangeleList(Vertices * list, int & count, bool remake)
{
	if(m_vertices == 0 || remake)
	{
		total = 0;
		vector<Vertices> invList;
		
		for (int i = 0; i< faceCount; i++)
		{
			for (int j = 0, size = (f->at(i)).size(); j + 3 < size; j++)
			{
				Vertices vertice;
				total++;
				vertice.nx = vn->at((f->at(i)).at(j).vn).x;
				vertice.ny = vn->at((f->at(i)).at(j).v).y;
				vertice.nz = vn->at((f->at(i)).at(j).v).z;

				vertice.x = v->at((f->at(i)).at(j).v).x;
				vertice.y = v->at((f->at(i)).at(j).v).y;
				vertice.z = v->at((f->at(i)).at(j).v).z;

				vertice.tu = vt->at((f->at(i)).at(j).vt).x;
				vertice.tv = vt->at((f->at(i)).at(j).vt).y;
				invList.push_back(vertice);
			}
		}

		if(!m_vertices)
		{
			delete[] m_vertices;
			m_vertices = 0;
		}

		m_vertices = new Vertices[total];
		for(int i = total-1; i>0; i--)
		{
			m_vertices[total-i] = invList[i];
		}

	}
	count = total;
	list = m_vertices;
}
