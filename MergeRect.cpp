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
			���ʣ��rectΪleft��right������֧��
			��---------��-----------------
			| picture |   right        |
			|         |                |
			��---------------------------
			|                          |
			|        left              |
			|                          |
			|                          |
			|                          |
			----------------------------
			����.picture�Ѿ�ռ���ˡ����Ͻǡ�����ʣ�µ������Ϊleft��right���㣨�񣩵ĵط�����CGPoint�ˡ�
			�� �Ż��㣺���picture�Ǿ��Σ���ô������left��right��ʱ�򡣿���������ѡ���������죬�������졣
			ͨ������ֵС��һ�����죬������ֵ֤���һ�����ԷŽ�ȥ�����ͼƬ��
			�� ������������������졣
		*/
		node->left = new MNode;
		node->left->point = Point(node->point.x, node->point.y + m_currentRect.getHeight());
			
		
		node->right = new MNode;
		node->right->point = Point(node->point.x+ m_currentRect.getWidth(), node->point.y);

		node->left->picsize = Size(node->picsize.width, node->picsize.height - m_currentRect.getHeight());
		node->right->picsize = Size(node->picsize.width- m_currentRect.getWidth(), m_currentRect.getHeight());


		////�Ż�һ��
		//if (m_currentRect.getWidth() >= m_currentRect.getHeight())
		//{
		//	//left�Ŀ����parent�Ŀ�ȡ��߶���parent�߶� - ͼƬ�ĸ߶�
		//	node->left->picsize = Size(node->picsize.width, node->picsize.height - m_currentRect.getHeight());
		//	//right�Ŀ���ǣ�parent�Ŀ��-ͼƬ�Ŀ�ȡ��߶���ͼƬ�ĸ߶ȡ�
		//	node->right->picsize = Size(node->picsize.width- m_currentRect.getWidth(), m_currentRect.getHeight());
		//}else{
		//	//left�Ŀ����ͼƬ�Ŀ�ȡ��߶���parent�߶� - ͼƬ�ĸ߶ȣ����䣩
		//	node->left->picsize = Size(m_currentRect.getWidth(), node->picsize.height - m_currentRect.getHeight());
		//	//right�Ŀ���ǣ�parent�Ŀ��-ͼƬ�Ŀ�ȡ��߶�parent�ĸ߶ȡ�
		//	node->right->picsize = Size(node->picsize.width- m_currentRect.getWidth(), node->picsize.height);
		//}


		//�������pic����
		node->picpoint=Point(m_currentRect.left,m_currentRect.top);
		node->picsize=Size(m_currentRect.getWidth(),m_currentRect.getHeight());
		return true;
	}
	return false;
}









