#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <utility>
#include <algorithm>
#include <cmath>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

// Define the maze layout as a hardcoded 2D vector of chars
std::vector<std::string> MAZE_INPUT = {
     "###                 #########",
    "#   ###################   # #",
    "# ####                # # # #",
    "# ################### # # # #",
    "#                     # # # #",
    "##################### # # # #",
    "#   ##                # # # #",
    "# # ## ### ## ######### # # #",
    "# #    #   ##B#         # # #",
    "# # ## ################ # # #",
    "### ##             #### # # #",
    "### ############## ## # # # #",
    "###             ##    # # # #",
    "###### ######## ####### # # #",
    "###### ####             #   #",
    "A      ######################"
};

class Maze {
public:
    int width, height;
    std::pair<int, int> start, goal;
    std::vector<std::vector<char>> grid;
    std::vector<std::pair<int, int>> solution;
    std::vector<std::pair<int, int>> visitedNodes;

    Maze() {
        // Initialize maze dimensions
        height = MAZE_INPUT.size();
        width = MAZE_INPUT[0].size();

        // Parse the hardcoded maze input
        for (int i = 0; i < height; ++i) {
            std::vector<char> row;
            for (int j = 0; j < width; ++j) {
                char c = MAZE_INPUT[i][j];
                row.push_back(c);
                if (c == 'A') start = {i, j};
                if (c == 'B') goal = {i, j};
            }
            grid.push_back(row);
        }
    }

    bool solveBFS() {
        std::queue<std::pair<int, int>> q;
        q.push(start);

        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        visited[start.first][start.second] = true;

        std::vector<std::pair<int, int>> parents(height * width, {-1, -1});

        while (!q.empty()) {
            auto [r, c] = q.front();
            q.pop();
            visitedNodes.push_back({r, c});

            if (grid[r][c] == 'B') {
                // Trace back the path
                std::pair<int, int> p = goal;
                while (p != start) {
                    solution.push_back(p);
                    p = parents[p.first * width + p.second];
                }
                std::reverse(solution.begin(), solution.end());
                return true;
            }

            std::vector<std::pair<int, int>> directions = {{-1,0}, {1,0}, {0,-1}, {0,1}};
            for (auto [dr, dc] : directions) {
                int nr = r + dr, nc = c + dc;
                if (nr >= 0 && nr < height && nc >= 0 && nc < width && grid[nr][nc] != '#' && !visited[nr][nc]) {
                    visited[nr][nc] = true;
                    parents[nr * width + nc] = {r, c};
                    q.push({nr, nc});
                }
            }
        }

        return false;
    }

    bool solveDFS() {
        std::stack<std::pair<int, int>> s;
        s.push(start);

        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        visited[start.first][start.second] = true;

        std::vector<std::pair<int, int>> parents(height * width, {-1, -1});

        while (!s.empty()) {
            auto [r, c] = s.top();
            s.pop();
            visitedNodes.push_back({r, c});

            if (grid[r][c] == 'B') {
                // Trace back the path
                std::pair<int, int> p = goal;
                while (p != start) {
                    solution.push_back(p);
                    p = parents[p.first * width + p.second];
                }
                std::reverse(solution.begin(), solution.end());
                return true;
            }

            std::vector<std::pair<int, int>> directions = {{-1,0}, {1,0}, {0,-1}, {0,1}};
            for (auto [dr, dc] : directions) {
                int nr = r + dr, nc = c + dc;
                if (nr >= 0 && nr < height && nc >= 0 && nc < width && grid[nr][nc] != '#' && !visited[nr][nc]) {
                    visited[nr][nc] = true;
                    parents[nr * width + nc] = {r, c};
                    s.push({nr, nc});
                }
            }
        }

        return false;
    }

    bool solveAStar() {
        auto heuristic = [&](std::pair<int, int> a, std::pair<int, int> b) {
            return std::abs(a.first - b.first) + std::abs(a.second - b.second);
        };

        std::priority_queue<std::tuple<int, int, int>, std::vector<std::tuple<int, int, int>>, std::greater<>> pq;
        pq.push({0, start.first, start.second});

        std::vector<std::vector<int>> cost(height, std::vector<int>(width, INT_MAX));
        cost[start.first][start.second] = 0;

        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        std::vector<std::pair<int, int>> parents(height * width, {-1, -1});

        while (!pq.empty()) {
            auto [currentCost, r, c] = pq.top();
            pq.pop();

            if (visited[r][c]) continue;
            visited[r][c] = true;
            visitedNodes.push_back({r, c});

            if (grid[r][c] == 'B') {
                // Trace back the path
                std::pair<int, int> p = goal;
                while (p != start) {
                    solution.push_back(p);
                    p = parents[p.first * width + p.second];
                }
                std::reverse(solution.begin(), solution.end());
                return true;
            }

            std::vector<std::pair<int, int>> directions = {{-1,0}, {1,0}, {0,-1}, {0,1}};
            for (auto [dr, dc] : directions) {
                int nr = r + dr, nc = c + dc;
                if (nr >= 0 && nr < height && nc >= 0 && nc < width && grid[nr][nc] != '#' && !visited[nr][nc]) {
                    int newCost = cost[r][c] + 1;
                    if (newCost < cost[nr][nc]) {
                        cost[nr][nc] = newCost;
                        parents[nr * width + nc] = {r, c};
                        int priority = newCost + heuristic({nr, nc}, goal);
                        pq.push({priority, nr, nc});
                    }
                }
            }
        }

        return false;
    }

    void drawMaze() {
        float cellWidth = 2.0 / width;
        float cellHeight = 2.0 / height;

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                float x = -1.0 + j * cellWidth;
                float y = 1.0 - i * cellHeight;

                if (grid[i][j] == '#') {
                    glColor3f(0.0, 0.0, 0.0);  // Wall (Black)
                } else if (grid[i][j] == 'A') {
                    glColor3f(1.0, 0.0, 0.0);  // Start (Red)
                } else if (grid[i][j] == 'B') {
                    glColor3f(0.0, 1.0, 0.0);  // Goal (Green)
                } else {
                    glColor3f(0.9, 0.9, 0.9);  // Empty Space
                }

                glBegin(GL_POLYGON);
                glVertex2f(x, y);
                glVertex2f(x + cellWidth, y);
                glVertex2f(x + cellWidth, y - cellHeight);
                glVertex2f(x, y - cellHeight);
                glEnd();
            }
        }

        // Draw the visited nodes in red
        glColor3f(1.0, 0.0, 0.0);  // Visited Path (Red)
        for (const auto& [r, c] : visitedNodes) {
            float x = -1.0 + c * cellWidth;
            float y = 1.0 - r * cellHeight;
            glBegin(GL_POLYGON);
            glVertex2f(x, y);
            glVertex2f(x + cellWidth, y);
            glVertex2f(x + cellWidth, y - cellHeight);
            glVertex2f(x, y - cellHeight);
            glEnd();
        }

        // Draw the solution path in blue
        if (!solution.empty()) {
            glColor3f(0.0, 0.0, 1.0);  // Final Path (Blue)
            for (const auto& [r, c] : solution) {
                float x = -1.0 + c * cellWidth;
                float y = 1.0 - r * cellHeight;
                glBegin(GL_POLYGON);
                glVertex2f(x, y);
                glVertex2f(x + cellWidth, y);
                glVertex2f(x + cellWidth, y - cellHeight);
                glVertex2f(x, y - cellHeight);
                glEnd();
            }
        }
    }
};

Maze* maze;

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    maze->drawMaze();
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    std::string method;
    std::cout << "Choose method (bfs/dfs/a*): ";
    std::cin >> method;

    maze = new Maze();
    bool solved = false;

    if (method == "bfs") {
        solved = maze->solveBFS();
    } else if (method == "dfs") {
        solved = maze->solveDFS();
    } else if (method == "a*") {
        solved = maze->solveAStar();
    } else {
        std::cerr << "Invalid method." << std::endl;
        delete maze;
        return 1;
    }

    if (!solved) {
        std::cerr << "No solution found." << std::endl;
        delete maze;
        return 1;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Maze Solver");

    glClearColor(1.0, 1.0, 1.0, 1.0);  // White background
    glutDisplayFunc(display);

    glutMainLoop();

    delete maze;
    return 0;
}
