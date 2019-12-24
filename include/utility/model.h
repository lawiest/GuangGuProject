#pragma once
#include "preconfig.h" 
UTILITY_NS
	 

#pragma pack(push)
#pragma pack(1)
/// \brief 四叉树瓦片Key
struct GsQuadKey
{
	GsQuadKey()
	{
		Level = 0;
		Row = 0;
		Col = 0;
	}
	GsQuadKey(int l, int r, int c)
	{
		Level = l;
		Row = r;
		Col = c;
	}
	GsQuadKey(const GsQuadKey& rhs)
	{
		Level = rhs.Level;
		Row = rhs.Row;
		Col = rhs.Col;
	}
	bool operator<(const GsQuadKey& rhs) const
	{
		return (Level != rhs.Level) ? (Level < rhs.Level) :
			(Row != rhs.Row) ? (Row < rhs.Row) :
			(Col < rhs.Col);

	}
	bool operator !=(const GsQuadKey& rhs) const
	{
		return Level != rhs.Level || Row != rhs.Row || Col != rhs.Col;
	}
	bool operator ==(const GsQuadKey& rhs) const
	{
		return Level == rhs.Level && Row == rhs.Row && Col == rhs.Col;
	}

	/// \brief 获取瓦片在父级瓦片中的行索引
	int ParentRow(const GsQuadKey& grand) const
	{
		//如果传入的瓦片不是其父级瓦片则返回-1
		if (!IsParent(grand))
			return -1;

		int r = RowIndex();
		GsQuadKey p = Parent();
		while (p.Level != grand.Level)
		{
			r += p.RowIndex() * 2;
			p = p.Parent();
		}
		return r;
	}
	/// \brief 获取同一父亲的第i个兄弟节点
	GsQuadKey Sibling(int i) const
	{
		i = i % 4;
		
		int r = Row  - (Row % 2);
		int c = Col - (Col % 2);
		if (i % 2) c++;
		if (i > 1) r++;
		return GsQuadKey(Level, r, c);

	}
	/// \brief 获取瓦片在父级瓦片中的列索引
	int ParentCol(const GsQuadKey& grand) const
	{
		//如果传入的瓦片不是其父级瓦片则返回-1
		if (!IsParent(grand))
			return -1;

		int c = ColIndex();
		GsQuadKey p = Parent();
		while (p.Level != grand.Level)
		{
			c += p.ColIndex() * 2;
			p = p.Parent();
		}
		return c;
	}
	/// \brief 获取瓦片在父瓦片中的索引
	int Index() const
	{
		return RowIndex() * 2 + ColIndex();
	}
	/// \brief 获取瓦片在父瓦片中的行索引
	int RowIndex() const
	{
		return Row % 2;
	}
	/// \brief 获取瓦片在父瓦片中的列索引
	int ColIndex() const
	{
		return Col % 2;
	}

	/// \brief 根据子瓦片的索引获取子瓦片的Key
	/// \brief [0][1]
	/// \brief [2][3]
	GsQuadKey Child(int i) const
	{
		i = i % 4;

		int r = Row * 2;
		int c = Col * 2;
		if (i % 2) c++;
		if (i > 1) r++;
		return GsQuadKey(Level + 1, r, c);
	}
	/// \brief 判断一个瓦片是否是自身的父瓦片Key
	bool IsParent(const GsQuadKey& rhs) const
	{
		if (rhs.Level >= Level) return false;
		GsQuadKey p = Parent();
		while (p.Level > rhs.Level)
		{
			p = p.Parent();
		}
		return p.Row == rhs.Row && p.Col == rhs.Col;
	}
	/// \brief 获取瓦片的父瓦片Key
	GsQuadKey Parent() const
	{
		return GsQuadKey(Level - 1, Row / 2, Col / 2);
	}
	
	GsString ChildQuadKey(char first)
	{
		std::string str;
		str += first;
		str[0] += abs(Level + Row + Col) % 3;
		return str;
	}

	GsString QuadStringKey(char first)
	{
		GsString str;
		str.reserve(Level);
		GsQuadKey p = *this;
		while (p.Level > 0)
		{
			str.push_back(first + p.Index());
			p = p.Parent();
		}
		return GsString(str.rbegin(), str.rend());
	}
	GsString OddEven(int  odd = 7)
	{
		int n = abs(Row + Col) % odd;
		return GsStringHelp::ToString(n);
	}
	/// \brief 瓦片级别
	int Level;
	/// \brief 瓦片行
	int Row;
	/// \brief 瓦片列
	int Col;
};
#pragma pack(pop)
UTILITY_ENDNS
