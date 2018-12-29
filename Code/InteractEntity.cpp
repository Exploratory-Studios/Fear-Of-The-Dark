#include "InteractEntity.h"

InteractEntity::InteractEntity(glm::vec2 position, Categories::Entity_Type type, unsigned int id) {
    init(position, type, id);
}

InteractEntity::InteractEntity() {
    //ctor
}

InteractEntity::~InteractEntity()
{
    //dtor
}
