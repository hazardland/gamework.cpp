#include "game/unit.h"

#include "game/context.h"
#include "game/position.h"
#include "game/world.h"
#include "game/cell.h"
#include "game/camera.h"
#include <functional>
#include <algorithm>


Unit* Unit::setWorld(World* world) {
    this->world = world;
    updateGrid();
    return this;
}

bool Unit::isVisible(Context* context) {
    return context->camera->isVisible(getRenderPosition());
}

void Unit::render(Context* context) {
    // Implement your rendering logic here
}

SDL_FRect* Unit::getRenderPosition() {
    if (renderPosition != nullptr) {
        return renderPosition->getPosition();
    }
    return getPosition();
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

bool Unit::canCrossUnit(Unit* target) const {
    return false;
}

bool Unit::canCrossTile(int type) const {
    return isTileAllowed(type);
}

int Unit::getType() const {
    return 0;
}

bool Unit::canMove(float deltaX, float deltaY) {
    if (world==nullptr) {
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
    return canPlace(targetX, targetY, targetWidth, targetHeight);
}

bool Unit::canPlace(float newX, float newY, float newWidth, float newHeight) {

    if (world==nullptr) {
        return true;
    }

    // Fail fast if out of world bounds
    if (newX < 0 || (newX + newWidth) > world->gridWidth * world->cellWidth ||
        newY < 0 || (newY + newHeight) > world->gridHeight * world->cellHeight) {
        return false;
    }

    // Compute occupied grid cells
    int newGridFromX = std::clamp(static_cast<int>(newX / world->cellWidth), 0, world->gridWidth - 1);
    int newGridFromY = std::clamp(static_cast<int>(newY / world->cellHeight), 0, world->gridHeight - 1);
    int newGridToX = std::clamp(static_cast<int>((newX + newWidth) / world->cellWidth), 0, world->gridWidth - 1);
    int newGridToY = std::clamp(static_cast<int>((newY + newHeight) / world->cellHeight), 0, world->gridHeight - 1);

    // First check tile crossing (Fail fast)
    for (int i = newGridFromX; i <= newGridToX; i++) {
        for (int j = newGridFromY; j <= newGridToY; j++) {
            Cell* cell = world->grid[i][j];
            if (!canCrossTile(cell->type)) {
                return false;
            }
        }
    }

    // Now check for unit collisions only if terrain check passed
    for (int i = newGridFromX; i <= newGridToX; i++) {
        for (int j = newGridFromY; j <= newGridToY; j++) {
            Cell* cell = world->grid[i][j];
            // if (!cell) continue;  // Prevent null pointer crash

            auto& objects = cell->units[getLayer()];
            for (const auto& object : objects) {
                if (object == this) continue;
                if (canCrossUnit(object)) continue;

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
    if (world==nullptr || !position->isReady()) {
        return;
    }

    // Calculate the new cells this object should occupy
    int newGridFromX = std::clamp(static_cast<int>(getX() / world->cellWidth), 0, world->gridWidth - 1);
    int newGridFromY = std::clamp(static_cast<int>(getY() / world->cellHeight), 0, world->gridHeight - 1);
    int newGridToX = std::clamp(static_cast<int>((getX() + getWidth()) / world->cellWidth), 0, world->gridWidth - 1);
    int newGridToY = std::clamp(static_cast<int>((getY() + getHeight()) / world->cellHeight), 0, world->gridHeight - 1);

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

                    world->grid[i][j]->units[getLayer()].remove(this);
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
                world->grid[i][j]->units[getLayer()].push_back(this);
            }
        }
    }

    gridFromX = newGridFromX;
    gridFromY = newGridFromY;
    gridToX = newGridToX;
    gridToY = newGridToY;
    gridSet = true;


}

int Unit::getLayer() {
    return layer;
}

void Unit::setLayer(int layer) {
    this->layer = layer;
}


void Unit::allowTile(int tileId) {
    ignoresTiles = false;
    allowedTiles |= (1 << tileId);
}

void Unit::allowTiles(std::initializer_list<int> tiles) {
    for (int tileId : tiles) {
        allowTile(tileId);
    }
}

void Unit::removeTile(int tileId) {
    allowedTiles &= ~(1 << tileId);
}

void Unit::ignoreTiles() {
    ignoresTiles = true;
}

bool Unit::isTileAllowed(int tileId) const {
    return ignoresTiles || (allowedTiles & (1 << tileId));
}

bool Unit::touchesTile(int type, float offsetX, float offsetY, float offsetWidth, float offsetHeight) {
    if (world == nullptr || !position->isReady()) {
        return false;
    }

    float x = getX() + offsetX;
    float y = getY() + offsetY;
    float width = getWidth() + offsetWidth;
    float height = getHeight() + offsetHeight;

    if (width <= 0 || height <= 0) {
        return false;
    }

    if (x < 0 || (x + width) > world->gridWidth * world->cellWidth ||
        y < 0 || (y + height) > world->gridHeight * world->cellHeight) {
        return false;
    }

    int fromX = std::clamp(static_cast<int>(x / world->cellWidth), 0, world->gridWidth - 1);
    int fromY = std::clamp(static_cast<int>(y / world->cellHeight), 0, world->gridHeight - 1);
    int toX = std::clamp(static_cast<int>((x + width) / world->cellWidth), 0, world->gridWidth - 1);
    int toY = std::clamp(static_cast<int>((y + height) / world->cellHeight), 0, world->gridHeight - 1);

    for (int x = fromX; x <= toX; x++) {
        for (int y = fromY; y <= toY; y++) {
            if (world->grid[x][y]->type == type) {
                return true;
            }
        }
    }

    return false;
}

bool Unit::touchesUnit(int kind, float right, float top, float left, float bottom) {
    bool found = false;
    scanUnits(right, top, left, bottom, [&](Unit* unit) {
        if (unit->getType() != kind) {
            return true;
        }

        found = true;
        return false;
    });
    return found;
}

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

void Unit::updateJobs(Context* context) {
    std::vector<Job*> newJobs;
    if (jobs.size() > 2) {
        newJobs.reserve(jobs.size());
    }

    for (auto it = jobs.begin(); it != jobs.end();) {
        Job* job = *it;

        if (!job->update(context)) {
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

void Unit::renderJobs(Context* context) {
    for (Job* job : jobs) {
        job->render(context);
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



