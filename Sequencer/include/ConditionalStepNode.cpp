#include <ConditionalStepNode.h>
#include <stdlib.h> //TODO: <cmath.h> for float?

bool ConditionalStepNode::evaluateCondition(double x, double y)
{
    switch (conditionalType)
    {
    case X_EQUALS_Y:
        return (abs(x - y) < margin);

        break;

    case X_EQUALS_N:
        return (abs(x - n) < margin);
        break;

    case Y_EQUALS_N:
        return (abs(y - n) < margin);
        break;

    case X_SMALLER_Y:
        return (x < y);
        break;

    case X_SMALLER_N:
        return (x < n);
        break;

    case X_GREATER_N:
        return (x > n);
        break;

    case X_GREATER_Y:
        return (x < n);
        break;

    case Y_SMALLER_N:
        return (x < n);
        break;

    case Y_GREATER_N:
        return (x > n);
        break;

    default:
        return true;
        break;
    }
}

int ConditionalStepNode::getNextNode()
{
    // analogRead()

    double x;
    double y;

    if (evaluateCondition(x, y))
    {
        return nextNodes[0];
    }
    else
    {
        return nextNodes[1];
    }
}

ConditionalStepNode::ConditionalStepNode(int initId, double initValueA, double initValueB, bool initGate, bool initTrigger, int initNextNodeA, int initNextNodeB, conditionalTypes initConditionalType, double initN, double initMargin)
{
    id = initId;
    values.gate = initGate;
    values.trigger = initTrigger;
    values.valueA = initValueA;
    values.valueB = initValueB;
    nextNodes[0] = initNextNodeA;
    nextNodes[1] = initNextNodeB;
    conditionalType = initConditionalType;
    n = initN;
    margin = initMargin;
}
