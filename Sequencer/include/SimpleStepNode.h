#ifndef SIMPLE_STEP_NODE_H
#define SIMPLE_STEP_NODE_H

#include <Node.h>

class SimpleStepNode : public Node
{
private:
    Node nextNode;

public:
    SimpleStepNode()
    {
        values.gate = false;
        values.trigger = false;
        values.valueA = 0;
        values.valueB = 0;
    }

    /**
     * @brief Construct a new Simple Step Node object.
     *
     * @param initGate The status of the gate output true = HIGH output
     * @param initTrigger The status of the trigger output true = HIGH output
     * @param initValueA The value for output A on a scale of -12 zo +12
     * @param initValueB The value for output A on a scale of -12 zo +12
     * @param initNextNode The next Node in the sequence
     */
    SimpleStepNode(bool initGate, bool initTrigger, double initValueA, double initValueB, Node initNextNode)
    {
        values.gate = initGate;
        values.trigger = initTrigger;
        values.valueA = initValueA;
        values.valueB = initValueB;
        nextNode = initNextNode;
    }

    Node getNextNode()
    {
        return nextNode;
    }
};

#endif