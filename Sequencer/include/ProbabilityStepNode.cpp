#include <ProbabilityStepNode.h>

int ProbabilityStepNode::getRandom()
{
    return 50;
    // TODO: actually random
}

Node ProbabilityStepNode::getNextNode()
{
    if (getRandom() <= probability)
    {
        return nextNodeA;
    }
    else
    {
        return nextNodeB;
    }
}

ProbabilityStepNode::ProbabilityStepNode(bool initGate, bool initTrigger, double initValueA, double initValueB, Node initNextNodeA, Node initNextNodeB, double initProbability)
{
    values.gate = initGate;
    values.trigger = initTrigger;
    values.valueA = initValueA;
    values.valueB = initValueB;
    nextNodeA = initNextNodeA;
    nextNodeB = initNextNodeB;
    probability = initProbability;
}