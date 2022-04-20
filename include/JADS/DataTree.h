#ifndef DATA_TREE_H_20220404
#define DATA_TREE_H_20220404

#include "JadsVal.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace jads 
{
	enum class DataType {
		Class,
		Array,
		Key,
		None
	};

	class DataTree
	{
		public:
			DataTree()
			{
				type = DataType::None;
				parent = nullptr;
			}
			DataTree(std::shared_ptr<DataTree> parent)
			{
				this->parent = parent;
				type = DataType::None;
			}
			~DataTree() {}

			std::shared_ptr<DataTree> parent;
			std::vector<std::shared_ptr<DataTree>> childs;
			
			DataType type;
			std::wstring name;
			JadsVal value;

	};
}

#endif
