#ifndef NODE_H
#define NODE_H

#include <stdlib.h>
#include <time.h> // time

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

enum nodeType
{
    SIMPLE_STEP,
    PROBABILITY,
    CONDITIONAL
};
/**
 * @brief The root Node object.
 *
 */
class Node
{

protected:
    valueStruct values;
    int nextNodes[2];
    nodeType type;
    int probability;

    int getRandom()
    {
        srand(time(NULL));
        int random = 1 + (rand() % 100);
        // Serial.println(random);
        return random;
    }

public:
    int id;

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
    void setValues(int newId, double newValueA, double newValueB, bool newTrigger, bool newGate)
    {
        id = newId;
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
    int getNextNode()
    {
        switch (type)
        {
        case SIMPLE_STEP:
            return nextNodes[0];
            break;
        case PROBABILITY:
            if (getRandom() <= probability)
            {
                return nextNodes[0];
            }
            else
            {
                return nextNodes[1];
            }
            break;

        case CONDITIONAL:
            return nextNodes[0];
            break;

        default:
            return nextNodes[0];
            break;
        }
    };

    /**
     * @brief Construct a new Simple Step Node object.
     *
     * @param initId The id of the new node
     * @param initGate The status of the gate output true = HIGH output
     * @param initTrigger The status of the trigger output true = HIGH output
     * @param initValueA The value for output A on a scale of -5 zo +5
     * @param initValueB The value for output A on a scale of -5 zo +5
     * @param initNextNode The id of the next Node in the sequence
     */
    Node(int initId = -1, double initValueA = 0, double initValueB = 0, bool initGate = false, bool initTrigger = false, int initNextNodeA = -1, int initNextNodeB = -1, nodeType initType = SIMPLE_STEP, int initProbability = 50)
    {
        id = initId;
        values.valueA = initValueA;
        values.valueB = initValueB;
        values.gate = initGate;
        values.trigger = initTrigger;
        nextNodes[0] = initNextNodeA;
        nextNodes[1] = initNextNodeB;
        type = initType;
        probability = initProbability;
    }
};

#endif