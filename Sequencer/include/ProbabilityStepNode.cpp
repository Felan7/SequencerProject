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

ProbabilityStepNode::ProbabilityStepNode(double initValueA, double initValueB, bool initGate, bool initTrigger, Node *initNextNodeA, Node *initNextNodeB, double initProbability)
{
    values.gate = initGate;
    values.trigger = initTrigger;
    values.valueA = initValueA;
    values.valueB = initValueB;
    nextNodes[0] = initNextNodeA;
    nextNodes[1] = initNextNodeB;
    probability = initProbability;
}