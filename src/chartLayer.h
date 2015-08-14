#ifndef CHARTLAYER_H
#define CHARTLAYER_H

#include <QString>

class ChartLayer
{
public:
	//creates a new chart with the given name and unique id
	ChartLayer(unsigned int uid, const QString& name);
	//creates a new chart with a given name and an automatically generated id
	ChartLayer(const QString& name);
	~ChartLayer();
	
	const QString& name();
	void setName(const QString& name);
	
	unsigned int uid();
	
	bool visible();
	void setVisible(bool visible);

private:
	static unsigned int nextuid;
	
	bool mVisible;
	unsigned int mUid;
	QString mName;
};

#endif // CHARTLAYER_H
