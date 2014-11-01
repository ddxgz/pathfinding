
/*  
 * This problem is from a company's hiring exam. If the appearence in here
 * is not appropriate, please inform me to remove.
 *
 * Here is the problem description:
 * Find a shortest path from a start point to a goal point, and probably
 * there will be 0, 1 or more checkpoints need to be arrived.
 *
 * The map of which is like:
 *
 *  11 5
 *	###########
 *	#...G.....#
 *	#.#.#.#...#
 *	#..@@@..S.#
 *	###########
 *
 * The first line contains two numbers to indicate the width and height of
 * the map. Each characters in the map stands for different kinds of objects:
 *
 * 'S' means the orienteering start.
 * 'G' means the orienteering goal.
 * '@' means an orienteering checkpoint.
 * '.' means an opened-block that players can pass.
 * '#' means a closed-block that players cannot pass.
 *
 * 1 <= width <= 100
 * 1 <= height <= 100
 * The maximum number of checkpoints is 18.
 *
 * Rules:
 * *It is allowed to move only by one step vertically or horizontally (up,
 * 	down, left, or right) to the next block. Other types of movements, such
 * 	as moving diagonally (left up, right up, left down and right down) and
 * 	skipping one or more blocks, are NOT permitted.
 * *You MUST NOT get out of the map.
 * *Distance is to be defined as the number of movements to the different blocks.
 * *You CAN pass opened-blocks, checkpoints, the start, and the goal more than
 *  once if necessary. * You can assume that parameters satisfy following
 *  conditions.
 *
 *
 */

/*
 * *******************My solution so far*************************
 *
 * To my point, the most difficult part is how to find a best path to go
 * through every checkpoint. At present, I just use a stupid method which
 * is "calculate every sequence". If you are intrested in this problem,
 * please kindly share your thoughts with me.
 *
 * In this piece of code, A* was utilized to find the path. The h(n) is
 * calculated by Manhattan * distance instead of Euclidean. This is referenced
 * from Patrick Lester (http://www.policyalmanac.org/games/aStarTutorial.htm)
 *
 * For the path finding with @(checkpoint), a dummy method was used, which
 * just try all the possible paths with different sequence of checkpoints,
 * this is the most cost part of this program. There should be a much more
 * clever method to deal with this.
 *
 * Dynamic programming is probably a ideal method to deal with this.
 *
 * Besides about the solution, any suggestion on the code is also very welcome.
 *
 * Run: ./a.out < example1.txt
 *
 **********************************************************/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <iterator>

#include <cmath>

using namespace std;

#define DEBUG 1


typedef struct coordination {
	int x;
	int y;
} coord;

class Orienteering {
public:
	//stands for different kind of points in the map
	enum Map {
		Wall, Road, Checkpoint, Start, Goal
	};
	void main();

private:
	Map **_map;
	int _map_row;
	int _map_col;
	//the coord of the start and goal, (0,0)-left top
	coord _start;
	coord _goal;
	coord _start_forNow;
	coord _goal_forNow;
	vector<coord> _checkpoints;
	int _shortest_distance;
	//g(n)
	float **_gn;
	float **_hn;
	float **_fn;
	bool **_in_open_list;
	bool **_in_closed_list;
	coord **_parent;
	int _checkpoints_sequence_remain;

	vector<coord> _open_list, _closed_list;
	vector<vector<int> > _checkpoints_sequences;
	vector<int> getCheckpoints_sequence();
	void update_list(coord, int);
	void read_map();
	int shortest_distance();
	int calculate_path(coord, coord);
	int calculate_path(vector<int>);
	float euclideanOf(int &, int &, int &, int &);
	int manhattanOf(int &, int &, int &, int &);
	void process_next_point(int, int, coord, const int, coord);
	int trace_parent(coord, coord);
	void generate_sequences();
	void sorted_insert(coord, vector<coord> &);
	bool check_researchable(coord);
	int check_direction(int, int);

	void set_closed(coord point) {
		_in_closed_list[point.y][point.x] = true;
	}
	;

	void set_open(coord point) {
		_in_open_list[point.y][point.x] = true;
	}
	;

	void unset_open(coord point) {
		_in_open_list[point.y][point.x] = false;
	}
	;

};

void Orienteering::generate_sequences() {
	//_checkpoints_sequence_remain and _checkpoints_sequences will be set
	const size_t length = _checkpoints.size();
	vector<int> sequence, tmp;
	for (size_t i = 0; i < length; ++i) {
		sequence.push_back(i);
	}

	_checkpoints_sequences.push_back(sequence);
	while (next_permutation(sequence.begin(), sequence.end())) {
		_checkpoints_sequences.push_back(sequence);
	}
	_checkpoints_sequence_remain = _checkpoints_sequences.size();
}

int Orienteering::trace_parent(coord start_forNow, coord goal_forNow) {
	coord tracor = goal_forNow;
	int cnt = 0;
	while (tracor.x != start_forNow.x || tracor.y != start_forNow.y) {
		tracor = _parent[tracor.y][tracor.x];
		++cnt;
	}
	return cnt;
}

vector<int> Orienteering::getCheckpoints_sequence() {
	vector<int> seq;
	seq = _checkpoints_sequences.back();
	_checkpoints_sequences.pop_back();
	--_checkpoints_sequence_remain;
	return seq;
}


void Orienteering::sorted_insert(coord new_elem, vector<coord> &vec) {
	//the openlist starts from bigger fn to smaller fn
	vector<coord>::iterator iter = vec.begin();
	vector<coord>::iterator it_end = vec.end();
	int new_x = new_elem.x, new_y = new_elem.y;
	if (vec.size() != 0) {
		for (; iter != it_end; ++iter) {
			if (_fn[new_y][new_x] >= _fn[(*iter).y][(*iter).x]) {
				vec.insert(iter, new_elem);
				break;
			} else {
				vec.push_back(new_elem);
				break;
			}
		}
	} else {
		vec.push_back(new_elem);
	}
}

void Orienteering::process_next_point(int given_x, int given_y, coord current,
		const int stepCnt, coord goal_forNow) {
	coord nextP;
	nextP.x = given_y;
	nextP.y = given_x;

	if (_map[given_x][given_y] == 0
			|| _in_closed_list[given_x][given_y] == true) {
	} else if (_in_open_list[given_x][given_y] == false) {

		_hn[given_x][given_y] = manhattanOf(nextP.x, nextP.y, goal_forNow.x,
				goal_forNow.y);
		_gn[given_x][given_y] = stepCnt + 1;
		_fn[given_x][given_y] = _gn[given_x][given_y] + _hn[given_x][given_y];
		_parent[given_x][given_y] = current;

		//instead of keep the vector sorted insert
		//_open_list.push_back(nextP);
		sorted_insert(nextP, _open_list);
		set_open(nextP);
	} else if (_in_open_list[given_x][given_y] == true) {

		if (stepCnt + 1 < _gn[given_x][given_y]) {
			_parent[given_x][given_y] = current;
			//_hn[given_x][given_y] = manhattanOf(nextP.x, nextP.y, _goal.x, _goal.y);
			_gn[given_x][given_y] = stepCnt + 1;
			_fn[given_x][given_y] = _gn[given_x][given_y]
					+ _hn[given_x][given_y];
		}
	} else {
		cout << "problem occurs" << endl;
	}
}

int Orienteering::check_direction(int x, int y) {
	if (_map[y][x] > 0)
		return 1;
	else
		return 0;
}

bool Orienteering::check_researchable(coord point) {
	int researchable = 0;
	if ((point.x > 0 && point.x < _map_col)
			&& (point.y > 0 && point.y < _map_row)) {
		if (_map[point.y][point.x] == Goal || _map[point.y][point.x] == Start
				|| _map[point.y][point.x] == Checkpoint) {

			//}
			researchable = check_direction(point.x, point.y + 1);
			researchable += check_direction(point.x + 1, point.y);
			researchable += check_direction(point.x, point.y - 1);
			researchable += check_direction(point.x - 1, point.y);
			if (researchable > 0)
				return true;
		}
	}
	return false;
}

/*
 * 1. get the points which can be stepped into next, and not in closed list
 * 2. calculate their g(n), h(n), f(n)=g(n)+h(n)
 * 3-1. if not in open list, push these points to open list, sort open list
 * 3-2. if in open list, and new g(n) is bigger than then g(n), update g(n),
 * 			push these points to open list, sort open list
 *
 * delete 4. push current point to closed list
 */

int Orienteering::calculate_path(coord start_forNow, coord goal_forNow) {
	int stepCnt = 0, distance = 0;
	coord current = start_forNow;
	_open_list.push_back(current);
	set_open(current);
	while (current.x != goal_forNow.x || current.y != goal_forNow.y) {
//		_open_list.pop_back();
//		unset_open(current);
//		set_closed(current);

		process_next_point(current.y, current.x + 1, current, stepCnt,
				goal_forNow);
		process_next_point(current.y + 1, current.x, current, stepCnt,
				goal_forNow);
		process_next_point(current.y - 1, current.x, current, stepCnt,
				goal_forNow);
		process_next_point(current.y, current.x - 1, current, stepCnt,
				goal_forNow);

		//instead of keep the vector sorted insert
//		if (_open_list.size() > 1) {
//			//longest to shortest
//			sort_openList(_open_list);
//		}

		current = _open_list.back();
		_open_list.pop_back();
		unset_open(current);
		set_closed(current);
		stepCnt++;

		if (_open_list.empty())
			return -1;
	}
	distance = trace_parent(start_forNow, goal_forNow);

	//clear related data for the next round
	for (int i = 0; i < _map_row; ++i) {
		for (int j = 0; j < _map_col; ++j) {
			_gn[i][j] = 1000;
			_hn[i][j] = 1000;
			_fn[i][j] = 0;
			_in_open_list[i][j] = false;
			_in_closed_list[i][j] = false;
		}
	}
	_open_list.clear();
	_closed_list.clear();

	return distance;
}

int Orienteering::calculate_path(vector<int> sequence) {
	int best_path, distance = 0;
	distance = calculate_path(_start, _checkpoints[sequence[0]]);
	if (distance == -1)
		return -1;
	for (int i = 1; i < sequence.size(); ++i) {
		distance += calculate_path(_checkpoints[sequence[i - 1]],
				_checkpoints[sequence[i]]);
	}
	distance += calculate_path(_checkpoints[sequence.back()], _goal);

	return distance;
}

/*
 * a coord array to store all the checkpoints,
 * a vector to store all the possible sequences of those checkpoints
 * a getCheckpoints_sequence() returns one sequence each time
 */
int Orienteering::shortest_distance() {
	bool valid;
	if (!check_researchable(_start))
		return -1;
	if (!check_researchable(_goal))
		return -1;
	if (_checkpoints.size() != 0) {
		vector<coord>::iterator iter = _checkpoints.begin();
		for (; iter != _checkpoints.end(); ++iter) {
			if (!check_researchable(*iter))
				return -1;
		}
	}

	int distance = 1000, tmpdistance = 1000;
	if (_checkpoints.size() == 0) {
		distance = calculate_path(_start, _goal);
	} else {
		vector<int> checkpoints_sequence;
		generate_sequences();
		while (_checkpoints_sequence_remain) {
			checkpoints_sequence = getCheckpoints_sequence();
			tmpdistance = calculate_path(checkpoints_sequence);
			if (tmpdistance < distance) {
				distance = tmpdistance;
			}
		}
	}
	return distance;
}

//be instead of manhattan distance
float Orienteering::euclideanOf(int &x1, int &y1, int &x2, int &y2) {
	float distance = 0.0;
	distance = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);

	return sqrt(distance);
}

int Orienteering::manhattanOf(int &x1, int &y1, int &x2, int &y2) {
	int distance = 0;
	distance = abs(x1 - x2) + abs(y1 - y2);
	return distance;
}

void Orienteering::read_map() {
//
	istream_iterator<string> instream(cin);
	istream_iterator<string> eof;

		// ifstream infile;
		// char filename[20] = "example1.txt";
		// //try {
		// infile.open(filename);
		// //} catch (runtime_error &err) {
		// //
		// //	cerr << err.what() << "try again" << endl;
		// //}
		// istream_iterator<string> instream(infile), eof;

	_map_col = atoi((*instream++).c_str());
	int col = _map_col;
	_map_row = atoi((*instream++).c_str());
	int row = _map_row;

	_map = (Map **) malloc(row * sizeof(Map*));
	for (int i = 0; i < row; ++i) {
		_map[i] = (Map *) malloc(col * sizeof(Map*));
	}
	string tmp;
	int col_cnt = 0, row_cnt = 0, chktmp[2];
	while (instream != eof) {
		tmp = *instream;
		coord chkptTmp;
		for (col_cnt = 0; col_cnt < col; ++col_cnt) {
			switch (tmp[col_cnt]) {
			case '#':
				_map[row_cnt][col_cnt] = Wall;
				break;
			case '.':
				_map[row_cnt][col_cnt] = Road;
				break;
			case '@': {
				_map[row_cnt][col_cnt] = Checkpoint;
				chkptTmp.x = col_cnt;
				chkptTmp.y = row_cnt;
				_checkpoints.push_back(chkptTmp);
			}
				break;
			case 'S':
				_map[row_cnt][col_cnt] = Start;
				_start.x = col_cnt;
				_start.y = row_cnt;
				break;
			case 'G':
				_map[row_cnt][col_cnt] = Goal;
				_goal.x = col_cnt;
				_goal.y = row_cnt;
				break;
			}
		}
		++instream;
		++row_cnt;
	}
}

void Orienteering::main() {
	// TODO: Implement this function

	//notice to free the memory of these points
	read_map();

	_gn = (float **) malloc(_map_row * sizeof(float*));
	_fn = (float **) malloc(_map_row * sizeof(float*));
	_hn = (float **) malloc(_map_row * sizeof(float*));
	_in_open_list = (bool **) malloc(_map_row * sizeof(bool*));
	_in_closed_list = (bool **) malloc(_map_row * sizeof(bool*));
	_parent = (coord **) malloc(_map_row * sizeof(coord*));
	for (int i = 0; i < _map_row; ++i) {
		_gn[i] = (float *) malloc(_map_col * sizeof(float*));
		_fn[i] = (float *) malloc(_map_col * sizeof(float*));
		_hn[i] = (float *) malloc(_map_col * sizeof(float*));
		_in_open_list[i] = (bool *) malloc(_map_col * sizeof(bool*));
		_in_closed_list[i] = (bool *) malloc(_map_col * sizeof(bool*));
		_parent[i] = (coord *) malloc(_map_col * sizeof(coord*));
		for (int j = 0; j < _map_col; ++j) {
			_gn[i][j] = 1000;
			_fn[i][j] = 0;
			_hn[i][j] = 1000;
			_in_open_list[i][j] = false;
			_in_closed_list[i][j] = false;
			_parent[i][j].x = -1;
			_parent[i][j].y = -1;
		}
	}

	_shortest_distance = shortest_distance();
	cout << _shortest_distance << endl;

	//finalize();
//	free(_map);
//	free(_gn);
//	free(_hn);
//	free(_fn);
//	free(_in_open_list);
//	free(_in_closed_list);
//	free(_parent);

	return;
}

int main(int argc, char* argv[]) {
	Orienteering o;
	o.main();
	return 0;
}
