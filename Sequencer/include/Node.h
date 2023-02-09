#ifndef NODE_H
#define NODE_H

#include <stdlib.h>

/**
 * @brief The structured musical data of a node.
 *
 */
struct valueStruct
{
    double valueA;
    double valueB;
    bool trigger;
    bool gate;
};
/**
 * @brief The root Node object.
 *
 */
class Node
{

protected:
    valueStruct values;
    Node *nextNodes[2];

public:
    /**
     * @brief Get the Values struct
     *
     * @return valueStruct
     */
    valueStruct getValues()
    {
        return values;
    }

    /**
     * @brief Set the Values struct
     *
     */
    void setValues(valueStruct)
    {
    }
    void setValues(double newValueA, double newValueB, bool newTrigger, bool newGate)
    {
        values.gate = newGate;
        values.trigger = newTrigger;
        values.valueA = newValueA;
        values.valueB = newValueB;
    }

    /**
     * @brief Get the Next Node object
     *
     * @return Node
     */
    Node getNextNode()
    {
        return *nextNodes[0];
    };

    /**
     * @brief Construct a new Simple Step Node object.
     *
     * @param initGate The status of the gate output true = HIGH output
     * @param initTrigger The status of the trigger output true = HIGH output
     * @param initValueA The value for output A on a scale of -12 zo +12
     * @param initValueB The value for output A on a scale of -12 zo +12
     * @param initNextNode The pointer to the next Node in the sequence
     */
    Node(double initValueA = 0, double initValueB = 0, bool initGate = false, bool initTrigger = false, Node *initNextNode = NULL)
    {
        values.gate = initGate;
        values.trigger = initTrigger;
        values.valueA = initValueA;
        values.valueB = initValueB;
        nextNodes[0] = initNextNode;
    }
};

#endif