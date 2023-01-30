#ifndef NODE_H
#define NODE_H
/**
 * @brief
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
 * @brief The root Node object. Conceptually abstract.
 *
 */
class Node
{

protected:
    valueStruct values;

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

    /**
     * @brief Get the Next Node object
     *
     * @return Node
     */
    virtual Node getNextNode(){
        return Node();
    };
};

#endif