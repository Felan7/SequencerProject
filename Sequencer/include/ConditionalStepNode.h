#ifndef CONDITIONAL_NODE_H
#define CONDITIONAL_NODE_H

#include <Node.h>

/**
 * @brief A enumeration of the different types of conditions to be checked to determine the next Node.
 *
 */
enum conditionalTypes
{
    X_EQUALS_Y,
    X_EQUALS_N,
    Y_EQUALS_N,
    X_SMALLER_Y,
    X_SMALLER_N,
    X_GREATER_N,
    X_GREATER_Y,
    Y_SMALLER_N,
    Y_GREATER_N
};

/**
 * @brief A Node that evaluates a certain condition and determines the next node out of two possible choices.
 */
class ConditionalStepNode : public Node
{
private:
    conditionalTypes conditionalType;
    double margin;
    double n; // internal number n for comparison

    bool evaluateCondition(double x, double y);

public:
    int getNextNode();

    /**
     * @brief Construct a new Conditional Step Node object.
     *
     * @param initGate The status of the gate output true = HIGH output
     * @param initTrigger The status of the trigger output true = HIGH output
     * @param initValueA The value for output A on a scale of -12 zo +12
     * @param initValueB The value for output A on a scale of -12 zo +12
     * @param initNextNodeA The next Nodes id in case the condition is true
     * @param initNextNodeB The next Nodes id in case the condition is false
     * @param initConditionalType The type of condition to check
     * @param initN The internal, fixed Number n. Can be used for comparisons.
     * @param initMargin The margin for equal checks (Calculated: abs(x - y) < margin)
     */
    ConditionalStepNode(int initId = -1, double initValueA = 0, double initValueB = 0, bool initGate = false, bool initTrigger = false, int initNextNodeA = -1, int initNextNodeB = -1, conditionalTypes initConditionalType = X_EQUALS_Y, double initN = 0, double initMargin = 0);
};

#endif