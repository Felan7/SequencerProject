#ifndef CONDITIONAL_NODE_H
#define CONDITIONAL_NODE_H

#include <Node.h>

/**
 * @brief A enumaration of the diffrent types of conditions to be checked to determine the next Node.
 *
 */
enum conditonalTypes
{
    X_EQUALLS_Y,
    X_EQUALLS_N,
    Y_EQUALLS_N,
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
    Node nextNodeA;
    Node nextNodeB;
    conditonalTypes conditionalType;
    double margin;
    double n; // internal number n for comparison

    bool evaluateCondition(double x, double y);

public:
    Node getNextNode();

    /**
     * @brief Construct a new Conditional Step Node object.
     *
     * @param initGate The status of the gate output true = HIGH output
     * @param initTrigger The status of the trigger output true = HIGH output
     * @param initValueA The value for output A on a scale of -12 zo +12
     * @param initValueB The value for output A on a scale of -12 zo +12
     * @param initNextNodeA The next Node in case the condition is true
     * @param initNextNodeB The next Node in case the condition is false
     * @param initConditionalType The type of condition to check
     * @param initN The internal, fixed Number n. Can be used for comparrisons.
     * @param initMargin The margin for equal checks (Calculated: abs(x - y) < margin)
     */
    ConditionalStepNode(bool initGate, bool initTrigger, double initValueA, double initValueB, Node initNextNodeA, Node initNextNodeB, conditonalTypes initConditionalType, double initN, double initMargin);
};

#endif