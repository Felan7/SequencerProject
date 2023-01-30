#include <ConditionalStepNode.h>
#include <stdlib.h> //TODO: <cmath.h> for float?

bool ConditionalStepNode::evaluateCondition(double x, double y)
{
    switch (conditionalType)
    {
    case X_EQUALLS_Y:
        return (abs(x - y) < margin);

        break;

    case X_EQUALLS_N:
        return (abs(x - n) < margin);
        break;

    case Y_EQUALLS_N:
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

Node ConditionalStepNode::getNextNode()
{
    // analogRead()

    double x;
    double y;

    if (evaluateCondition(x, y))
    {
        return nextNodeA;
    }
    else
    {
        return nextNodeB;
    }
}

ConditionalStepNode::ConditionalStepNode(bool initGate, bool initTrigger, double initValueA, double initValueB, Node initNextNodeA, Node initNextNodeB, conditonalTypes initConditionalType, double initN, double initMargin)
{
    values.gate = initGate;
    values.trigger = initTrigger;
    values.valueA = initValueA;
    values.valueB = initValueB;
    nextNodeA = initNextNodeA;
    nextNodeB = initNextNodeB;
    conditionalType = initConditionalType;
    n = initN;
    margin = initMargin;
}
