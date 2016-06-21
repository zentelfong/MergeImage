#pragma once
#include <vector>
#include "Rect.h"

typedef std::vector<Rect> RectVector;

class MergeRect
{
public:
	MergeRect();
	~MergeRect();

	struct MNode
	{
		MNode()
			:left(NULL),right(NULL),isFull(false)
		{
		}

		//�ϲ����ͼƬ��λ��
		Point point;

		//ԭͼ
		Size picsize;
		Point picpoint;
		MNode* left;
		MNode* right;
		bool isFull;
	};

	void Merge(RectVector* rects,int maxWidth,int maxHeight);
	void GetRects(RectVector* rectsSrc,RectVector* rectsMerge);
private:
	void freeNode(MNode* node);
	void getTreeNodes(MNode* node,RectVector* rectsSrc,RectVector* rectsMerge);
	bool lastTraveralTree(MNode* node);
	bool insertPicture(MNode* node);
	Rect m_currentRect;
	MNode* m_root;
};