#include "PrimMeshModel.h"
#include <vector>

PrimMeshModel::PrimMeshModel(const vector<triangle3d_t> &_triangles) : MeshModel("Pyramid")
{
    triangles = _triangles;
}

PrimMeshModel::PrimMeshModel(const vector<line3d_t> &_lines) : lines(_lines)
{
}

int i_bit(int num, int i)
{
    return (num >> i) & 1;
}
int turned_on_bits(int num)
{
    int ret = 0;
    while (num > 0)
    {
        ret += i_bit(num, 0);
        num = num >> 1;
    }
    return ret;
}

int hamming_dist(int a, int b)
{
    return turned_on_bits(a ^ b);
}
inline PrimMeshModel PrimMeshModel::CreateCube(const point3d_t &min_p, const point3d_t &max_p)
{
    vector<point3d_t> vertices;
    vector<line3d_t> lines;
    point3d_t both_points[] = {min_p, max_p};
    for (int i = 0; i < 8; i++)
    {
        point3d_t current_vertex;
        for (int j = 0; j < 3; j++)
        {
            current_vertex[j] = both_points[i_bit(i, j)][j];
        }
        vertices.push_back(current_vertex);
    }
 
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (hamming_dist(i, j) == 1)
            {
                lines.push_back(line3d_t(vertices[i], vertices[j]));
            }
        }
    }
    return PrimMeshModel(lines);
}



const hexahedron_t MeshModel::CalcBbox() const
{
	point3d_t min_p, max_p;
	for (const auto &tri : triangles)
	{
		for (const auto &p : tri.vertices)
		{

			for (size_t i = 0; i < 3; i++)
			{
				if (max_p[i]< p[i])
				{
					max_p[i] = p[i];
				}
				if (min_p[i] > p[i])
				{
					min_p[i] = p[i];
				}
			}
		}
	}
	return PrimMeshModel::CreateCube(min_p, max_p).lines;
}