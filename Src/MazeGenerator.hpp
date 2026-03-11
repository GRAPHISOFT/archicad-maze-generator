#ifndef MAZEGENERATOR_HPP
#define MAZEGENERATOR_HPP

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace MG
{

using CellId = int;
using WallId = int;
extern const CellId InvalidCellId;
extern const WallId InvalidWallId;

enum class Direction
{
	Left,
	Right,
	Top,
	Bottom,
	Invalid
};

class Cell
{
public:
	Cell ();

	bool	HasWall (Direction dir) const;
	WallId	GetWall (Direction dir) const;
	void	EnumerateWalls (const std::function<void (WallId)>& processor) const;
	void	AddWall (Direction dir, WallId wallId);
	void	RemoveWall (WallId wallId);

private:
	std::vector<WallId>		walls;
};

class Wall
{
public:
	Wall ();
	Wall (CellId cellId1, CellId cellId2);

	CellId	GetCellId1 () const;
	CellId	GetCellId2 () const;
	CellId	GetOtherCellId (CellId cellId) const;

private:
	CellId	cellId1;
	CellId	cellId2;
};

class WallGeometry
{
public:
	WallGeometry (double begX, double begY, double endX, double endY);

	double begX;
	double begY;
	double endX;
	double endY;
};

class Maze
{
public:
	Maze ();
	Maze (int rowCount, int colCount);

	void						Reset (int rowCount, int colCount);

	CellId						GetCellId (int row, int col) const;
	WallId						GetWallId (int row, int col, Direction dir) const;

	const Cell&					GetCell (CellId cellId) const;
	const Wall&					GetWall (WallId wallId) const;

	WallId						AddWall (int row, int col, Direction dir);
	void						RemoveWall (WallId wallId);

	std::vector<WallGeometry>	GetWallGeometries (double cellSize) const;

private:
	int									rows;
	int									cols;
	std::vector<Cell>					cells;
	std::unordered_map<WallId, Wall>	walls;
	WallId								nextWallId;
};

class MazeGenerator
{
public:
	MazeGenerator (int rowCount, int colCount);

	bool			Generate ();
	const Maze&		GetMaze () const;

private:
	void			VisitCell (CellId cellId);
	WallId			SelectRandomWall ();

	Maze						maze;
	int							rowCount;
	int							colCount;

	std::unordered_set<CellId>	visited;
	std::unordered_set<WallId>	walls;
};

}

#endif
