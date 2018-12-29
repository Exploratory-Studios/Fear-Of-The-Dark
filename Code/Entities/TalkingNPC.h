#include "../Entity.h"

#include "../Dialogue.h"

class TalkingNPC : public Entity
{
    public:
        TalkingNPC();
        ~TalkingNPC();

    private:
        virtual void update(Chunk* chunks[WORLD_SIZE], float timeStep) override;

        void drawText(); // Draws textBox and text inside
        void updateDialogue(); // Reads if you're trying to skip conversation, your response, etc.

        Dialogue dialogue;
};
