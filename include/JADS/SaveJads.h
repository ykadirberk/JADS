#ifndef SAVE_JADS_20220420
#define SAVE_JADS_20220420

#include "DataTree.h"

#include <fstream>
#include <logger/Log.h>
#include <memory>

namespace jads
{
	class SaveJads
	{
		public:
			SaveJads(std::shared_ptr<DataTree> root, std::wstring_view fileName) 
			{
				mRoot = root;
				mFileName = std::wstring(fileName);
				mFile.open(fileName);
			}
			~SaveJads() 
			{
				for (auto it = mRoot->childs.begin(); it != mRoot->childs.end(); ++it)
				{
					if ((*it)->type == DataType::Key)
					{
						SaveKey(*it, 0);
					}

					if ((*it)->type == DataType::Class)
					{
						SaveClass(*it, 0);
					}

					if ((*it)->type == DataType::Array)
					{
						SaveArray(*it, 0);
					}
				}
				LOGI << mFileName << " saved.";
			}
		private:

			std::shared_ptr<DataTree> mRoot;
			std::wstring mFileName;
			std::wofstream mFile;

			void SaveKey(std::shared_ptr<DataTree> tRoot, int indent) 
			{
				for (int i = 0; i < indent; ++i)
				{
					mFile << '\t';
				}
				if (tRoot->name.compare(L"") != 0)
				{
					mFile << tRoot->name << L' ';
				}
				mFile << tRoot->value.AsString() << L'\n';
			}
			void SaveClass(std::shared_ptr<DataTree> tRoot, int indent)
			{
				for (int i = 0; i < indent; ++i)
				{
					mFile << '\t';
				}
				if (tRoot->name.compare(L"") != 0)
				{
					mFile << tRoot->name << L' ';
				}
				mFile << "{\n";
				for (auto it = tRoot->childs.begin(); it != tRoot->childs.end(); ++it)
				{
					if ((*it)->type == DataType::Key)
					{
						SaveKey(*it, indent + 1);
					}

					if ((*it)->type == DataType::Class)
					{
						SaveClass(*it, indent + 1);
					}

					if ((*it)->type == DataType::Array)
					{
						SaveArray( *it, indent + 1);
					}
				}
				for (int i = 0; i < indent; ++i)
				{
					mFile << '\t';
				}
				mFile << "}\n";
			}
			void SaveArray(std::shared_ptr<DataTree> tRoot, int indent) 
			{
				for (int i = 0; i < indent; ++i)
				{
					mFile << '\t';
				}
				if (tRoot->name.compare(L"") != 0)
				{
					mFile << tRoot->name << L' ';
				}
				mFile << "[\n";

				for (auto it = tRoot->childs.begin(); it != tRoot->childs.end(); ++it)
				{
					if ((*it)->type == DataType::Key)
					{
						SaveKey(*it, indent + 1);
					}

					if ((*it)->type == DataType::Class)
					{
						SaveClass(*it, indent + 1);
					}

					if ((*it)->type == DataType::Array)
					{
						SaveArray(*it, indent + 1);
					}
				}

				for (int i = 0; i < indent; ++i)
				{
					mFile << '\t';
				}
				mFile << "]\n";
			}
	};
}


#endif
