#ifndef PRINT_JADS_20220420
#define PRINT_JADS_20220420

#include "DataTree.h"

#include <logger/Log.h>
#include <memory>

namespace jads
{
	class PrintJads
	{
		public:
			PrintJads(std::shared_ptr<DataTree> root)
			{
				mRoot = root;
			}
			~PrintJads()
			{
				RecursivePrint(mRoot, L"");
			}

		private:
			std::shared_ptr<DataTree> mRoot;

			void RecursivePrint(std::shared_ptr<DataTree> tRoot, std::wstring_view path)
			{
				LOGI << "RecursivePrint() for " << path;
				std::wstringstream tempStream;
				tempStream << path;

				if (tRoot->type == DataType::Key)
				{
					PrintKey(tRoot, tempStream.str());
					return;
				}

				if (tRoot->type == DataType::Class)
				{
					PrintClass(tRoot, tempStream);
					tempStream << '.' << tRoot->name;
				}

				if (tRoot->type == DataType::Array)
				{
					PrintArray(tRoot, tempStream.str());
					return;
				}

				for (auto it = tRoot->childs.begin(); it != tRoot->childs.end(); ++it)
				{
					RecursivePrint(*it, tempStream.str());
				}
			}
			void PrintKey(std::shared_ptr<DataTree> tRoot, std::wstring_view path)
			{
				LOGI << path << '.' << tRoot->name << " = " << tRoot->value.AsString();
			}
			void PrintClass(std::shared_ptr<DataTree> tRoot, std::wstringstream& path)
			{
				LOGI << path.str() << '.' << tRoot->name << " as Class";
			}
			void PrintArray(std::shared_ptr<DataTree> tRoot, std::wstring_view path)
			{
				std::wstringstream tempStream;
				tempStream << path;
				tempStream << '.' << tRoot->name;
				LOGI << tempStream.str() << " as Array";

				for (auto it = tRoot->childs.begin(); it != tRoot->childs.end(); ++it)
				{
					if ((*it)->type == DataType::Key)
					{
						PrintKey((*it), tempStream.str());
					}
					if ((*it)->type == DataType::Array)
					{
						PrintArray((*it), tempStream.str());
					}
					if ((*it)->type == DataType::Class)
					{
						PrintClass((*it), tempStream);
					}
				}
			}
	};
}

#endif