#include "chartLayer.h"

unsigned int ChartLayer::nextuid = 0;

ChartLayer::ChartLayer(const QString& name)
{
	mName = name;
	mUid = nextuid;
	nextuid++;
}

ChartLayer::ChartLayer(unsigned int uid, const QString& name)
{
	mName = name;
	mUid = uid;
	if (mUid >= nextuid)
		nextuid = mUid+1;
}

ChartLayer::~ChartLayer()
{
}

const QString& ChartLayer::name()
{
	return mName;
}

void ChartLayer::setName(const QString& name)
{
	mName = name;
}

unsigned int ChartLayer::uid()
{
	return mUid;
}

bool ChartLayer::visible()
{
	return mVisible;
}

void ChartLayer::setVisible(bool visible)
{
	mVisible = visible;
}