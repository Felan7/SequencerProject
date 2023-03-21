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
        return nextNodes[0];
    }
    else
    {
        return nextNodes[1];
    }
}

ProbabilityStepNode::ProbabilityStepNode(double initValueA, double initValueB, bool initGate, bool initTrigger, int initNextNodeA, int initNextNodeB, double initProbability)
{
    values.gate = initGate;
    values.trigger = initTrigger;
    values.valueA = initValueA;
    values.valueB = initValueB;
    nextNodes[0] = initNextNodeA;
    nextNodes[1] = initNextNodeB;
    probability = initProbability;
}