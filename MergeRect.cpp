#include "StdAfx.h"
#include "MergeRect.h"
#include <algorithm>

MergeRect::MergeRect()
	:m_root(NULL)
{
}


MergeRect::~MergeRect()
{
	freeNode(m_root);
	m_root=NULL;
}


static bool SortRectByArea( const Rect &v1, const Rect &v2)
{  
	return v1.getWidth()* v1.getHeight() > v2.getWidth()* v2.getHeight();
}

static bool SortRectByWidth( const Rect &v1, const Rect &v2)
{  
	return v1.getWidth() > v2.getWidth();
}

static bool SortRectByHeight( const Rect &v1, const Rect &v2)
{  
	return v1.getHeight() > v2.getHeight();
}

void MergeRect::Merge(RectVector* rects,int maxWidth,int maxHeight)
{
	freeNode(m_root);
	m_root=NULL;

	std::sort(rects->begin(),rects->end(),SortRectByHeight);
	m_root = new MNode;
	m_root->picsize=Size(maxWidth,maxHeight);

	for (RectVector::iterator i=rects->begin();i!=rects->end();++i)
	{
		m_currentRect= *i;
		lastTraveralTree(m_root);
	}
}

void MergeRect::GetRects(RectVector* rectsSrc,RectVector* rectsMerge)
{
	getTreeNodes(m_root,rectsSrc,rectsMerge);
}



void MergeRect::freeNode(MNode* node)
{
	if (node)
	{
		freeNode(node->left);
		freeNode(node->right);
		delete node;
	}
}

void MergeRect::getTreeNodes(MNode* node,RectVector* rectsSrc,RectVector* rectsMerge)
{
	if (node)
	{
		getTreeNodes(node->left,rectsSrc,rectsMerge);
		if (node->isFull)
		{
			Rect src(node->picpoint.x,node->picpoint.y,node->picpoint.x+node->picsize.width,node->picpoint.y+node->picsize.height);
			Rect merge(node->point.x,node->point.y,node->point.x+node->picsize.width,node->point.y+node->picsize.height);
			rectsSrc->push_back(src);
			rectsMerge->push_back(merge);
		}
		getTreeNodes(node->right,rectsSrc,rectsMerge);
	}
}


bool MergeRect::lastTraveralTree(MNode* node)
{
	if (node)
	{
		if (lastTraveralTree(node->right))
			return true;

		if (insertPicture(node))
			return true;

		if(lastTraveralTree(node->left))
			return true;
	}
	return false;
}

bool MergeRect::insertPicture(MNode* node)
{
	if (!node->isFull
		&& node->picsize.width >= m_currentRect.getWidth()
		&& node->picsize.height >= m_currentRect.getHeight())
	{
		node->isFull = true;

		/*
			拆分剩余rect为left和right两个分支。
			●---------●-----------------
			| picture |   right        |
			|         |                |
			●---------------------------
			|                          |
			|        left              |
			|                          |
			|                          |
			|                          |
			----------------------------
			例如.picture已经占据了【左上角】区域。剩下的区域分为left和right。点（●）的地方就是CGPoint了。
			● 优化点：如果picture是矩形，那么在生成left和right的时候。可以有两个选择：向下延伸，向右延伸。
			通常是向值小的一方延伸，这样保证值大的一方可以放进去更多的图片。
			● 上面的例子是向右延伸。
		*/
		node->left = new MNode;
		node->left->point = Point(node->point.x, node->point.y + m_currentRect.getHeight());
			
		
		node->right = new MNode;
		node->right->point = Point(node->point.x+ m_currentRect.getWidth(), node->point.y);

		node->left->picsize = Size(node->picsize.width, node->picsize.height - m_currentRect.getHeight());
		node->right->picsize = Size(node->picsize.width- m_currentRect.getWidth(), m_currentRect.getHeight());


		////优化一下
		//if (m_currentRect.getWidth() >= m_currentRect.getHeight())
		//{
		//	//left的宽度是parent的宽度。高度是parent高度 - 图片的高度
		//	node->left->picsize = Size(node->picsize.width, node->picsize.height - m_currentRect.getHeight());
		//	//right的宽度是，parent的宽度-图片的宽度。高度是图片的高度。
		//	node->right->picsize = Size(node->picsize.width- m_currentRect.getWidth(), m_currentRect.getHeight());
		//}else{
		//	//left的宽度是图片的宽度。高度是parent高度 - 图片的高度（不变）
		//	node->left->picsize = Size(m_currentRect.getWidth(), node->picsize.height - m_currentRect.getHeight());
		//	//right的宽度是，parent的宽度-图片的宽度。高度parent的高度。
		//	node->right->picsize = Size(node->picsize.width- m_currentRect.getWidth(), node->picsize.height);
		//}


		//将虚拟的pic复制
		node->picpoint=Point(m_currentRect.left,m_currentRect.top);
		node->picsize=Size(m_currentRect.getWidth(),m_currentRect.getHeight());
		return true;
	}
	return false;
}









