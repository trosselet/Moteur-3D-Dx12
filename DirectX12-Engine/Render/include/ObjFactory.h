#ifndef OBJFACTORY__H
#define OBJFACTORY__H

struct Geometry;
class Color;

class ObjFactory
{
public:
	static Geometry* LoadObjFile(const char* filePath, Color color);

private:
	static std::vector<std::string> SplitString(const std::string& input, char separator);
};


#endif // !OBJFACTORY__H