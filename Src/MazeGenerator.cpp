#include "MazeGenerator.hpp"

#include <ctime>

namespace MG
{

static const double Eps = 0.00005;

const CellId InvalidCellId = -1;
const WallId InvalidWallId = -1;

class WallCollector
{
public:
	enum class Direction
	{
		Horizontal,
		Vertical
	};

	WallCollector (std::vector<WallGeometry>& wallGeometries, Direction direction, double elevation) :
		wallGeometries (wallGeometries),
		direction (direction),
		elevation (elevation),
		begPosition (0.0),
		endPosition (0.0),
		hasWall (false)
	{

	}

	void AddWall (double beg, double end)
	{
		if (hasWall && std::fabs (endPosition - beg) > Eps) {
			Flush ();
		}
		if (!hasWall) {
			begPosition = beg;
		}
		endPosition = end;
		hasWall = true;
	}

	void Flush ()
	{
		if (hasWall) {
			if (direction == Direction::Horizontal) {
				wallGeometries.push_back (WallGeometry (begPosition, elevation, endPosition, elevation));
			} else if (direction == Direction::Vertical) {
				wallGeometries.push_back (WallGeometry (elevation, begPosition, elevation, endPosition));
			}
			hasWall = false;
		}
	}

private:
	std::vector<WallGeometry>&	wallGeometries;
	Direction					direction;
	double						elevation;
	double						begPosition;
	double						endPosition;
	bool						hasWall;
};

static size_t GetDirectionIndex (Direction dir)
{
	switch (dir) {
	case Direction::Left:
		return 0;
	case Direction::Right:
		return 1;
	case Direction::Top:
		return 2;
	case Direction::Bottom:
		return 3;
	default:
		return (size_t) -1;
	}
}

static Direction GetOppositeDirection (Direction dir)
{
	if (dir == Direction::Left) {
		return Direction::Right;
	} else if (dir == Direction::Right) {
		return Direction::Left;
	} else if (dir == Direction::Top) {
		return Direction::Bottom;
	} else if (dir == Direction::Bottom) {
		return Direction::Top;
	}
	return Direction::Invalid;
}

Cell::Cell () :
	walls (4, InvalidWallId)
{

}

bool Cell::HasWall (Direction dir) const
{
	size_t index = GetDirectionIndex (dir);
	return walls[index] != InvalidWallId;
}

WallId Cell::GetWall (Direction dir) const
{
	size_t index = GetDirectionIndex (dir);
	return walls[index];
}

void Cell::EnumerateWalls (const std::function<void (WallId)>& processor) const
{
	for (WallId wallId : walls) {
		if (wallId != InvalidWallId) {
			processor (wallId);
		}
	}
}

void Cell::AddWall (Direction dir, WallId wallId)
{
	size_t index = GetDirectionIndex (dir);
	walls[index] = wallId;
}

void Cell::RemoveWall (WallId wallId)
{
	for (size_t i = 0; i < walls.size (); i++) {
		if (walls[i] == wallId) {
			walls[i] = InvalidWallId;
		}
	}
}

Wall::Wall () :
	Wall (InvalidCellId, InvalidCellId)
{

}

Wall::Wall (CellId cellId1, CellId cellId2) :
	cellId1 (cellId1),
	cellId2 (cellId2)
{

}

CellId Wall::GetCellId1 () const
{
	return cellId1;
}

CellId Wall::GetCellId2 () const
{
	return cellId2;
}

CellId Wall::GetOtherCellId (CellId cellId) const
{
	if (cellId == cellId1) {
		return cellId2;
	} else if (cellId == cellId2) {
		return cellId1;
	}
	return InvalidCellId;
}

WallGeometry::WallGeometry (double begX, double begY, double endX, double endY) :
	begX (begX),
	begY (begY),
	endX (endX),
	endY (endY)
{

}

Maze::Maze () :
	rows (0),
	cols (0),
	cells (),
	walls (),
	nextWallId (0)
{

}

Maze::Maze (int rowCount, int colCount) :
	rows (0),
	cols (0),
	cells (),
	walls (),
	nextWallId (0)
{
	Reset (rowCount, colCount);
}

void Maze::Reset (int rowCount, int colCount)
{
	rows = rowCount;
	cols = colCount;
	cells.clear ();
	walls.clear ();
	nextWallId = 0;

	cells.assign (rows * cols, Cell ());
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			AddWall (row, col, Direction::Left);
			AddWall (row, col, Direction::Top);
			if (row == rows - 1) {
				AddWall (row, col, Direction::Bottom);
			}
			if (col == cols - 1) {
				AddWall (row, col, Direction::Right);
			}
		}
	}
}

CellId Maze::GetCellId (int row, int col) const
{
	if (row < 0 || row >= rows) {
		return InvalidCellId;
	}
	if (col < 0 || col >= cols) {
		return InvalidCellId;
	}
	return (row * cols + col);
}

WallId Maze::GetWallId (int row, int col, Direction dir) const
{
	CellId cellId = GetCellId (row, col);
	if (cellId == InvalidCellId) {
		return InvalidWallId;
	}
	const Cell& cell = cells[cellId];
	return cell.GetWall (dir);
}

const Cell& Maze::GetCell (CellId cellId) const
{
	return cells[cellId];
}

const Wall& Maze::GetWall (WallId wallId) const
{
	return walls.at (wallId);
}

WallId Maze::AddWall (int row, int col, Direction dir)
{
	CellId currCellId = GetCellId (row, col);
	if (currCellId == InvalidCellId) {
		return InvalidWallId;
	}

	Cell& currCell = cells[currCellId];
	if (currCell.HasWall (dir)) {
		return InvalidWallId;
	}

	CellId nextCellId = InvalidCellId;
	Direction nextDirection = GetOppositeDirection (dir);
	if (dir == Direction::Left) {
		nextCellId = GetCellId (row, col - 1);
	} else if (dir == Direction::Right) {
		nextCellId = GetCellId (row, col + 1);
	} else if (dir == Direction::Top) {
		nextCellId = GetCellId (row - 1, col);
	} else if (dir == Direction::Bottom) {
		nextCellId = GetCellId (row + 1, col);
	}

	if (nextCellId != InvalidCellId && cells[nextCellId].HasWall (nextDirection)) {
		return InvalidWallId;
	}

	WallId wallId = nextWallId++;
	walls.insert ({ wallId, Wall (currCellId, nextCellId) });

	currCell.AddWall (dir, wallId);
	if (nextCellId != InvalidCellId) {
		Cell& nextCell = cells[nextCellId];
		if (!nextCell.HasWall (nextDirection)) {
			nextCell.AddWall (nextDirection, wallId);
		}
	}

	return wallId;
}

void Maze::RemoveWall (WallId wallId)
{
	const Wall& wall = walls[wallId];
	CellId cellId1 = wall.GetCellId1 ();
	if (cellId1 != InvalidCellId) {
		cells[cellId1].RemoveWall (wallId);
	}
	CellId cellId2 = wall.GetCellId2 ();
	if (cellId2 != InvalidCellId) {
		cells[cellId2].RemoveWall (wallId);
	}
	walls.erase (wallId);
}

std::vector<WallGeometry> Maze::GetWallGeometries (double cellSize) const
{
	std::vector<WallGeometry> wallGeometries;
	std::vector<WallCollector> horizontalCollectors;
	std::vector<WallCollector> verticalCollectors;

	for (int row = 0; row <= rows; row++) {
		double top = row * cellSize;
		horizontalCollectors.push_back (WallCollector (wallGeometries, WallCollector::Direction::Horizontal, top));
	}

	for (int col = 0; col <= cols; col++) {
		double left = col * cellSize;
		verticalCollectors.push_back (WallCollector (wallGeometries, WallCollector::Direction::Vertical, left));
	}

	for (int row = 0; row < rows; row++) {
		double top = row * cellSize;
		double bottom = (row + 1) * cellSize;
		for (int col = 0; col < cols; col++) {
			CellId cellId = GetCellId (row, col);
			double left = col * cellSize;
			double right = (col + 1) * cellSize;
			const Cell& cell = cells[cellId];
			if (cell.HasWall (Direction::Top)) {
				horizontalCollectors[row].AddWall (left, right);
			}
			if (cell.HasWall (Direction::Left)) {
				verticalCollectors[col].AddWall (top, bottom);
			}
			if (row == rows - 1 && cell.HasWall (Direction::Bottom)) {
				horizontalCollectors[row + 1].AddWall (left, right);
			}
			if (col == cols - 1 && cell.HasWall (Direction::Right)) {
				verticalCollectors[col + 1].AddWall (top, bottom);
			}
		}
	}
	for (WallCollector& collector : horizontalCollectors) {
		collector.Flush ();
	}
	for (WallCollector& collector : verticalCollectors) {
		collector.Flush ();
	}

	return wallGeometries;
}

MazeGenerator::MazeGenerator (int rowCount, int colCount) :
	maze (),
	rowCount (rowCount),
	colCount (colCount)
{

}

bool MazeGenerator::Generate ()
{
	std::srand ((unsigned int) std::time (nullptr));
	
	maze.Reset (rowCount, colCount);
	visited.clear ();
	walls.clear ();

	CellId firstCellId = maze.GetCellId (0, 0);
	VisitCell (firstCellId);

	while (!walls.empty ()) {
		WallId wallId = SelectRandomWall ();
		const Wall& wall = maze.GetWall (wallId);
		CellId cellId1 = wall.GetCellId1 ();
		CellId cellId2 = wall.GetCellId2 ();
		bool cellVisited1 = (visited.find (cellId1) != visited.end ());
		bool cellVisited2 = (visited.find (cellId2) != visited.end ());
		if (cellVisited1 != cellVisited2) {
			CellId newCellId = (cellVisited1 ? cellId2 : cellId1);
			maze.RemoveWall (wallId);
			VisitCell (newCellId);
		}
		walls.erase (wallId);
	}

	WallId entrance = maze.GetWallId (0, 0, Direction::Top);
	WallId exit = maze.GetWallId (rowCount - 1, colCount - 1, Direction::Bottom);
	maze.RemoveWall (entrance);
	maze.RemoveWall (exit);

	return true;
}

const Maze& MazeGenerator::GetMaze () const
{
	return maze;
}

void MazeGenerator::VisitCell (CellId cellId)
{
	const Cell& cell = maze.GetCell (cellId);
	cell.EnumerateWalls ([&] (WallId wallId) {
		const Wall& wall = maze.GetWall (wallId);
		CellId otherCellId = wall.GetOtherCellId (cellId);
		if (otherCellId == InvalidCellId || visited.find (otherCellId) != visited.end ()) {
			return;
		}
		walls.insert (wallId);
	});
	visited.insert (cellId);
}

WallId MazeGenerator::SelectRandomWall ()
{
	if (walls.empty ()) {
		return InvalidWallId;
	}
	size_t random = std::rand () % walls.size ();
	auto it = walls.begin ();
	std::advance (it, random);
	return *it;
}

}
