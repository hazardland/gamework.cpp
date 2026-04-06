#include "game/unit.h"

#include "game/position.h"
#include "game/map.h"
#include "game/cell.h"
#include "game/terrain.h"
#include "game/camera.h"
#include "game/minimap.h"
#include <algorithm>


Unit* Unit::setMap(Map* map) {
    this->map = map;
    return this;
}

void Unit::render(State* state) {
    // Implement your rendering logic here
}

SDL_FRect* Unit::getRenderPosition() {
    return renderPosition->getPosition();
}

void Unit::addPosition(float x, float y) {
    Object::addPosition(x, y);
    updateGrid();
}

void Unit::setPosition(float x, float y) {
    Object::setPosition(x, y);
    updateGrid();
}

bool Unit::isSelected() {
    return selected;
}

void Unit::select() {
    selected = true;
}

// Input dirX and dirY are directions -1, 0 or 1
bool Unit::move (Uint64 deltaTime, float dirX, float dirY, float &deltaX, float &deltaY) {
    float moveX = dirX;
    float moveY = dirY;

    if (moveX != 0 && moveY != 0) {
        moveX *= 0.7071f;
        moveY *= 0.7071f;
    }

    float diagonalX = (moveX * (float)deltaTime) * (speed/100);
    float diagonalY = (moveY * (float)deltaTime) * (speed/100);

    deltaX = diagonalX;
    deltaY = diagonalY;

    if (diagonalX!=0 && diagonalY!=0 && canMove(diagonalX, diagonalY)) {
        rotate(diagonalX, diagonalY);
        addPosition(diagonalX, diagonalY);
        return true;
    }

    float straightX = (dirX * (float)deltaTime) * (speed/100);
    if (straightX!=0 && canMove(straightX, 0)) {
        rotate(straightX, 0);
        addPosition(straightX, 0);
        deltaX = straightX;
        deltaY = 0;
        return true;
    }

    float straightY = (dirY * (float)deltaTime) * (speed/100);
    if (straightY!=0 && canMove(0, straightY)) {
        rotate(0, straightY);
        addPosition(0, straightY);
        deltaX = 0;
        deltaY = straightY;
        return true;
    }

    return false;
}

bool Unit::move(Uint64 deltaTime, float dirX, float dirY) {
    float dx = 0, dy = 0;
    return move(deltaTime, dirX, dirY, dx, dy);
}

// Implement your rotation if needed
// You might need to update sprite to
// Face the direction unit just moved
void Unit::rotate(float deltaX, float deltaY) {

};

bool Unit::canMove(float deltaX, float deltaY) {
    if (map==nullptr) {
        return true;
    }

    if (deltaX==0 && deltaY==0){
        return false;
    }
    // Compute the target position based on the movement delta
    float targetX = getX() + deltaX;
    float targetY = getY() + deltaY;
    float targetWidth = getWidth();
    float targetHeight = getHeight();

    // Check if the unit can legally occupy the target position
    return canOccupy(targetX, targetY, targetWidth, targetHeight);
}

bool Unit::canOccupy(float newX, float newY, float newWidth, float newHeight) {

    if (map==nullptr) {
        return true;
    }

    // Fail fast if out of map bounds
    if (newX < 0 || (newX + newWidth) > map->gridWidth * map->cellWidth ||
        newY < 0 || (newY + newHeight) > map->gridHeight * map->cellHeight) {
        return false;
    }

    // Compute occupied grid cells
    int newGridFromX = std::clamp(static_cast<int>(newX / map->cellWidth), 0, map->gridWidth - 1);
    int newGridFromY = std::clamp(static_cast<int>(newY / map->cellHeight), 0, map->gridHeight - 1);
    int newGridToX = std::clamp(static_cast<int>((newX + newWidth) / map->cellWidth), 0, map->gridWidth - 1);
    int newGridToY = std::clamp(static_cast<int>((newY + newHeight) / map->cellHeight), 0, map->gridHeight - 1);

    // First check terrain (Fail fast)
    for (int i = newGridFromX; i <= newGridToX; i++) {
        for (int j = newGridFromY; j <= newGridToY; j++) {
            Cell* cell = map->grid[i][j];
            // if (!cell) continue;  // Avoid null pointer crash

            if (cell->terrain->layer != getLayer() && getLayer() != 0) {
                return false;
            }
            if (!ignoresTerrain && !isTerrainAllowed(cell->terrain->id)) {
                return false;
            }
        }
    }

    // Now check for unit collisions only if terrain check passed
    for (int i = newGridFromX; i <= newGridToX; i++) {
        for (int j = newGridFromY; j <= newGridToY; j++) {
            Cell* cell = map->grid[i][j];
            // if (!cell) continue;  // Prevent null pointer crash

            auto& objects = cell->units[getLayer()];
            for (const auto& object : objects) {
                // Skip self-check
                if (object == this) continue;

                // Now do collision check inside cell
                if (newX < object->getX() + object->getWidth() &&
                    newX + newWidth > object->getX() &&
                    newY < object->getY() + object->getHeight() &&
                    newY + newHeight > object->getY()) {
                return false;
            }

            }
        }
    }

    // No collisions
    return true;
}

void Unit::updateGrid() {
    if (map==nullptr || !position->isReady()) {
        return;
    }

    // Calculate the new cells this object should occupy
    int newGridFromX = std::clamp(static_cast<int>(getX() / map->cellWidth), 0, map->gridWidth - 1);
    int newGridFromY = std::clamp(static_cast<int>(getY() / map->cellHeight), 0, map->gridHeight - 1);
    int newGridToX = std::clamp(static_cast<int>((getX() + getWidth()) / map->cellWidth), 0, map->gridWidth - 1);
    int newGridToY = std::clamp(static_cast<int>((getY() + getHeight()) / map->cellHeight), 0, map->gridHeight - 1);

    if (gridSet) {

        if(newGridFromX == gridFromX && newGridFromY == gridFromY && newGridToX == gridToX && newGridToY == gridToY){
            return;
        }

        // If old grid cells does not intersect new grid cells remove them from grid
        // Aka we moved from there
        for (int i = gridFromX; i<=gridToX; i++) {
            for (int j = gridFromY; j<=gridToY; j++) {
                // Check if i, j is outside of rectangle newGridFromX, newGridFromY, newGridToX, newGridToY
                if (i < newGridFromX || i > newGridToX || j < newGridFromY || j > newGridToY) {
                    // Remove the unit from this grid cell
                    // printf("Remove gridToX:%d, %d,%d ", gridToX, i , j);

                    map->grid[i][j]->units[getLayer()].remove(this);
                }
            }
        }
    }


    // If new grid cells does not intersect new grid cells add them from grind
    for (int i = newGridFromX; i<=newGridToX; i++) {
        for (int j = newGridFromY; j<=newGridToY; j++) {
            // Check if i, j is outside of rectangle newGridFromX, newGridFromY, newGridToX, newGridToY
            if (!gridSet || i < gridFromX || i > gridToX || j < gridFromY || j > gridToY) {
                // Add new cells to grid cell
                // printf("Add %d,%d ", i , j);
                map->grid[i][j]->units[getLayer()].push_back(this);
            }
        }
    }

    map->markModified();

    gridFromX = newGridFromX;
    gridFromY = newGridFromY;
    gridToX = newGridToX;
    gridToY = newGridToY;
    gridSet = true;


}

void Unit::drawPosition(State* state) {
    SDL_SetRenderDrawColor(state->renderer, 161, 195, 69, 255);
    SDL_RenderRect(state->renderer, state->camera->translate(getPosition()));
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 0);
}

int Unit::getLayer() {
    return layer;
}

void Unit::setLayer(int layer) {
    this->layer = layer;
}


// Allow a single terrain
void Unit::allowTerrain(int terrainId) {
    ignoresTerrain = false;
    allowedTerrains |= (1 << terrainId);
}

// Allow multiple terrains
void Unit::allowTerrains(std::initializer_list<int> terrains) {
    for (int terrainId : terrains) {
        allowTerrain(terrainId);
    }
}

// Remove a terrain
void Unit::removeTerrain(int terrainId) {
    allowedTerrains &= ~(1 << terrainId);
}

// Mark the unit as flying (ignores terrain)
void Unit::ignoreTerrain() {
    ignoresTerrain = true;
}

// Check if terrain is allowed (Flying units ignore terrain)
bool Unit::isTerrainAllowed(int terrainId) const {
    return ignoresTerrain || (allowedTerrains & (1 << terrainId));
}

bool Unit::touchesTerrain(int terrainId, float offsetX, float offsetY, float offsetWidth, float offsetHeight) {
    if (map == nullptr || !position->isReady()) {
        return false;
    }

    float x = getX() + offsetX;
    float y = getY() + offsetY;
    float width = getWidth() + offsetWidth;
    float height = getHeight() + offsetHeight;

    if (width <= 0 || height <= 0) {
        return false;
    }

    if (x < 0 || (x + width) > map->gridWidth * map->cellWidth ||
        y < 0 || (y + height) > map->gridHeight * map->cellHeight) {
        return false;
    }

    int fromX = std::clamp(static_cast<int>(x / map->cellWidth), 0, map->gridWidth - 1);
    int fromY = std::clamp(static_cast<int>(y / map->cellHeight), 0, map->gridHeight - 1);
    int toX = std::clamp(static_cast<int>((x + width) / map->cellWidth), 0, map->gridWidth - 1);
    int toY = std::clamp(static_cast<int>((y + height) / map->cellHeight), 0, map->gridHeight - 1);

    for (int x = fromX; x <= toX; x++) {
        for (int y = fromY; y <= toY; y++) {
            Terrain* terrain = map->grid[x][y]->terrain;
            if (terrain != nullptr && terrain->id == terrainId) {
                return true;
            }
        }
    }

    return false;
}

bool Unit::hasMinimap() {
    return false;
}

// Uint32 Unit::getMinimapColor(const SDL_PixelFormatDetails* format) {
//     if (minimapColorCache==0) {
//         minimapColorCache = SDL_MapRGBA(format, nullptr, minimapColor.r, minimapColor.g, minimapColor.b, minimapColor.a);
//     }
//     return minimapColorCache;
// }

// void Unit::setMinimapColor(SDL_Color color) {
//     minimapColorCache = 0;
//     minimapColor = color;
// }


SDL_Color* Unit::getColor() {
    return &color;
}

void Unit::setColor(SDL_Color color) {
    this->color = color;
}

void Unit::addJob(Job* job) {
    if (!job) return;

    if (job->isUnique()) {
        for (auto it = jobs.begin(); it != jobs.end(); ++it) {
            if ((*it)->isUnique() && (*it)->getType() == job->getType()) {
                delete *it;
                jobs.erase(it); // remove the old job
                break;          // exit loop, then add the new one
            }
        }
    }

    jobs.push_back(job);
}

void Unit::addJobs(std::initializer_list<Job*> jobList) {
    for (Job* job : jobList) {
        addJob(job);
    }
}

void Unit::onJobFinished (Job* job) {

}

void Unit::updateJobs(State* state) {
    std::vector<Job*> newJobs;
    if (jobs.size() > 2) {
        newJobs.reserve(jobs.size());
    }

    for (auto it = jobs.begin(); it != jobs.end();) {
        Job* job = *it;

        if (!job->update(state)) {
            auto childJobs = job->finish();

            if (!childJobs.empty()) {
                newJobs.insert(newJobs.end(),
                    std::make_move_iterator(childJobs.begin()),
                    std::make_move_iterator(childJobs.end()));
            }

            onJobFinished(job);

            delete job;
            it = jobs.erase(it);
        } else {
            ++it;
        }
    }

    if (!newJobs.empty()) {
        for (Job* job : newJobs) {
            addJob(job); // ensures uniqueness still applies
        }
    }
}

void Unit::renderJobs(State* state) {
    for (Job* job : jobs) {
        job->render(state); // no check needed — base is empty
    }
}

void Unit::removeJob(int type) {
    for (auto it = jobs.begin(); it != jobs.end(); ) {
        if ((*it)->getType() == type) {
            delete *it;
            it = jobs.erase(it);
        } else {
            ++it;
        }
    }
}

void Unit::removeJobs(std::initializer_list<int> types) {
    for (int type : types) {
        removeJob(type);
    }
}

void Unit::pauseJobs() {
    for (Job* job : jobs) {
        job->pause();
    }
}

void Unit::resumeJobs() {
    for (Job* job : jobs) {
        job->resume();
    }
}

void Unit::pause() {
    if (paused) return;
    paused = true;

    for (Job* job : jobs) {
        job->pause();
    }
}

void Unit::resume() {
    if (!paused) return;
    paused = false;

    for (Job* job : jobs) {
        job->resume();
    }
}

Unit::~Unit() {
    for (Job* job : jobs) {
        delete job;
    }
    delete renderPosition;
    delete selectPosition;
}
