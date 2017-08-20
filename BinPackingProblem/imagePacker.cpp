#include "imagePacker.h"
#include "node.h"
#include "util.hpp"
#include <iostream>
using namespace std;

namespace
{
	const u32 AtlasWidth = 1024;
	const u32 AtlasHeight = 1024;
}

ImagePacker::ImagePacker()
{
	mRootNodePtr = new Node();
	Rect rc;
	rc.l = 0;
	rc.r = static_cast<s32>(AtlasWidth) - 1;
	rc.t = 0;
	rc.b = static_cast<s32>(AtlasHeight) - 1;
	mRootNodePtr->setRect(rc);
	mImage = Image(AtlasWidth, AtlasHeight, 1, 3);

	unsigned char color[3];
	color[0] = 100;
	color[1] = 100;
	color[2] = 100;
	fillPixel(mImage, color);
}

ImagePacker::~ImagePacker()
{
	deleteNode(mRootNodePtr);
}

bool ImagePacker::insert(const Image& img, int& newId)
{
	auto* nodePtr = mRootNodePtr->insert(img);
	if (nodePtr != nullptr)
	{
		int id = 0;
		mAccessor.registerData(nodePtr, id);
		nodePtr->setID(id);
		copy(img, mImage, nodePtr->getRect().l, nodePtr->getRect().t);
		newId = id;
		return true;
	}

	cerr << "Cannot insert." << endl;
	return false;
}

void ImagePacker::save(const std::string& path)
{
	mImage.save(path.c_str());
}

bool ImagePacker::unregister(int id)
{
	auto* nodePtr = mAccessor.getData(id);
	if (nodePtr == nullptr)
	{
		return false;
	}

	mAccessor.unregisterData(id);
	SAFE_DELETE(nodePtr);
	return true;
}

void ImagePacker::deleteNode(Node* nodePtr)
{
	if (nodePtr == nullptr)
		return;

	auto* r = nodePtr->getRightChild();
	auto* l = nodePtr->getLeftChild();
	deleteNode(r);
	deleteNode(l);

	auto id = mAccessor.getID(nodePtr);
	mAccessor.unregisterData(id);
	SAFE_DELETE(nodePtr);
}