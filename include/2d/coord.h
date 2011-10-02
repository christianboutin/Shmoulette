#ifndef ___DEF___COORD_H___
#define ___DEF___COORD_H___

namespace Shmoulette{

typedef struct {
	int x;
	int y;
	int w;
	int h;
}XYWH;

typedef struct {
	float x;
	float y;
	float w;
	float h;
}RXYWH;

typedef enum {
	REL_RULE_ABSOLUTE,
	REL_RULE_KEEP_RATIO_UL,
	REL_RULE_KEEP_RATIO_UR,
	REL_RULE_KEEP_RATIO_DL,
	REL_RULE_KEEP_RATIO_DR
}REL_RULE;

class _ShmouletteExport Coordinate{
private:
	static Coordinate* mCoord;
	float mAspectRatio;
	
	Coordinate();
public:
	static Coordinate* getSingleton();
	void setAspectRatio(float a);

	XYWH getPixelCoord(string XmlFile, string context, string GraphicsFile);
	RXYWH getRelativeCoord(string XmlFile, string context, string GraphicsFile, REL_RULE rr=REL_RULE_ABSOLUTE);
};
}

#endif