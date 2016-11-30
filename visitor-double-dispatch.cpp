/*
 * visitor-double-dispatch
 *
 * A simple example of the visitor pattern combined with double dispatch
 * to allow individual handing of derived classes witohut having to
 * add the handling logic to each of the classes.
 */

#include <string>
#include <memory>
#include <vector>

// ---------------------------------------------------------------------
// Data object library - probably in some sort of header
// ---------------------------------------------------------------------

// Forward declare the visitor, because each derived class needs to
// know it exists for their access() functions.
class DataObjectVisitor;

class DataObject
{
public:
    virtual ~DataObject() = default;

    // Other shared concrete or virtual functions can go here, which
    // can be called on DataObject as normal.

    // Accept a visitor. The vistor could be modified by this call
    // (eg it might update how many objects it's processed), but it
    // can't modify the DataObject
    virtual void accept(DataObjectVisitor& visitor) const = 0;
};

class StringObject: public DataObject
{
public:
    StringObject(const std::string& strData, const std::string& encoding):
        strData(strData), encoding(encoding)
    {}

    // Unique interface for string data
    std::string getStringData() const { return strData; }
    std::string getEncoding()   const { return encoding; }

private:
    // The specific version of the vistor acceptance function that
    // will call the 'StringObject' vistation function
    void accept(DataObjectVisitor& visitor) const override;

    std::string strData, encoding;
};

class IntegerObject: public DataObject
{
public:
    IntegerObject(const int intValue, const int width):
        intValue(intValue), width(width)
    {}

    int getIntValue()  const { return intValue; }
    int getWidthBits() const { return width; }

private:
    void accept(DataObjectVisitor& visitor) const override;

    int intValue, width;
};

class FloatObject: public DataObject
{
public:
    FloatObject(const float floatValue, const std::string floatType):
        floatValue(floatValue), floatType(floatType)
    {}

    float       getFloatValue() const { return floatValue; }
    std::string getFloatType()  const { return floatType; }

private:
    void accept(DataObjectVisitor& visitor) const override;

    float floatValue;
    std::string floatType;
};

// The visitor base class - inherit from this to allow you to visit
// the DataObject derived classes
class DataObjectVisitor
{
public:
    // Implement a vistation function for each DataObject type
    virtual void visit(const StringObject& data) {};
    virtual void visit(const IntegerObject& data) {};
    virtual void visit(const FloatObject& data) {};
};

// Implementations of DataObject acceptance functions - these
// have to come after DataObjectVisitor because the need to see each
// vist function so they can get the relevant overload. These probably
// should go in the .cpp along with the rest of the implementation of
// the derived classes.

void StringObject::accept(DataObjectVisitor& visitor) const  { visitor.visit(*this); }
void IntegerObject::accept(DataObjectVisitor& visitor) const { visitor.visit(*this); }
void FloatObject::accept(DataObjectVisitor& visitor) const   { visitor.visit(*this); }

// ---------------------------------------------------------------------
// End of 'data library', start of a custom renderer
// ---------------------------------------------------------------------

class DataRenderer: public DataObjectVisitor
{
    // Implement a renderer for each derived type
    void visit(const StringObject& data) override
    {
        printf("String:     \"%s\"  (%s)\n", data.getStringData().c_str(), data.getEncoding().c_str());
    }

    void visit(const IntegerObject& data) override
    {
        printf("Integer:    %d  (%d bits)\n", data.getIntValue(), data.getWidthBits());
    }

    void visit(const FloatObject& data) override
    {
        printf("Float:      %f  (%s)\n", data.getFloatValue(), data.getFloatType().c_str());
    }
};

// Some container (not important which) of DataObjects, none of
// which can be modified
using DObjContainer = std::vector<std::unique_ptr<const DataObject>>;

// Some function that returns data objects (probably would be in a
// library somewhere)
static DObjContainer getObjects()
{
    DObjContainer objCont;
    objCont.push_back(std::make_unique<StringObject>("Hello", "utf-8"));
    objCont.push_back(std::make_unique<IntegerObject>(16, 32));
    objCont.push_back(std::make_unique<FloatObject>(3.14f, "ieee-754"));

    return objCont;
}

int main(int argc, char** argv)
{
    const auto objects = getObjects();

    // The renderer
    DataRenderer renderer;

    // Render each object in the container
    for (const auto& obj: objects)
    {
        obj->accept(renderer);
    }

    return 0;
}

