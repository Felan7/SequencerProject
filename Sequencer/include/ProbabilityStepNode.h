#ifndef PROBAILITY_NODE_H
#define PROBAILITY_NODE_H

#include <Node.h>

/**
 * @brief A Node that employs a random number generator too determine the next node out of two possible choices.
 *
 */
class ProbabilityStepNode : public Node
{
private:
    int probability; // Probability of A on a scale of 0 to 100

    /**
     * @brief Get a random integer from 0 to 100 (inclusive).
     *
     * @return int
     */
    int getRandom();

public:
    /**
     * @brief Get the Next Node object.
     *
     * @return Node
     */
    Node getNextNode();

    /**
     * @brief Construct a new Probability Step Node object.
     *
     * @param initGate The status of the gate output true = HIGH output
     * @param initTrigger The status of the trigger output true = HIGH output
     * @param initValueA The value for output A on a scale of -12 zo +12
     * @param initValueB The value for output A on a scale of -12 zo +12
     * @param initNextNodeA The next Nodes id in case the condition is true
     * @param initNextNodeB The next Nodes id in case the condition is false
     * @param initProbability The likeliness of initValueA beeing the next Node
     *
     */
    ProbabilityStepNode(double initValueA = 0, double initValueB = 0, bool initGate = false, bool initTrigger = false, int initNextNodeA = NULL, int initNextNode = NULL, double initProbability = 50);
};

#endif