#ifndef JADS_H_20220405
#define JADS_H_20220405

#include "JadsAnalyzer.h"
#include "DataTree.h"
#include "JadsVal.h"
#include "SaveJads.h"
#include "PrintJads.h"

#include <logger/Log.h>
#include <memory>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

namespace jads
{
	class Jads
	{
		public:
			Jads()
            {
                mTempVal = nullptr;
            }
			Jads(std::wstring_view fileName)
            {
                mFileName = std::wstring(fileName);
                mAnalyze.SetFile(mFileName);
                mAnalyze.ReadAndTokenize();
                mAnalyze.Analyze();
                mRoot = mAnalyze.GenerateStructure();
                mTempVal = mRoot;
                LOGI << fileName << " initialized.";
            }
			~Jads() {}
			
			//Initialize the file if not by the constructor.
			void OpenFile(std::wstring_view fileName)
            {
                mFileName = std::wstring(fileName);
                mAnalyze.SetFile(mFileName);
                mAnalyze.ReadAndTokenize();
                mAnalyze.Analyze();
                mRoot = mAnalyze.GenerateStructure();
                mTempVal = mRoot;
            }
			
			//returns the value in mTempVal if its type is DataType::Key
			JadsVal Value()
            {
                JadsVal value;

                if (mTempVal->type == DataType::Key)
                {
                    value = mTempVal->value;
                }
                else if (mTempVal->type == DataType::Class || mTempVal->type == DataType::Array)
                {
                    value = JadsVal();
                }

                mTempVal = mRoot;

                mTempIndex.clear();
                mTempIndex.str(std::wstring());

                return value;
            }

			template <typename VarStreamable>
			void Set(VarStreamable v)
            {
                // put value to stream
                // this makes it easier to operate on input
                std::wstringstream tValue;
                tValue << v;

                // if the not-found-array is empty
                // that means the intended operation is
                // changing the value of given key
                if (mTempIndex.rdbuf()->in_avail() == 0)
                {
                    mTempVal->value.SetValue(tValue.str());
                    mTempVal = mRoot;
                    return;
                }

                // fill operation for set leaves
                // single ' ' character at the end.
                // this operation is to eliminate 
                // that single space
                std::wstringstream tempSS;
                {
                    std::wstring tempStr = mTempIndex.str();
                    tempStr = tempStr.substr(0, tempStr.length() - 1);

                    tempSS.str(tempStr);
                }
                mTempIndex.clear();
                mTempIndex.str(std::wstring());

                // iterate until tempSS stream is empty
                while (!tempSS.eof())
                {
                    std::shared_ptr<DataTree> tempNode = std::make_shared<DataTree>();
                    std::wstring tempStr;

                    tempSS >> tempStr;
                    std::streampos tempPos = tempSS.tellp();
                    // if stream becomes empty after 
                    // the insert operation then the
                    // current token must be a key
                    // otherwise it may be an array
                    // or a class
                    if (tempSS.eof())
                    {
                        tempNode->type = DataType::Key;
                        tempNode->value = JadsVal(tValue.str());
                        tempNode->parent = mTempVal;

                        // if the key is in an array
                        // then it mustn't have a name
                        if (!Utils::IsInteger(tempStr))
                        {
                            tempNode->name = tempStr;
                        }
                    }
                    else
                    {
                        // if next token is an integer
                        // that means the current token is
                        // name of an array.
                        std::streampos current = tempSS.tellg();
                        std::wstring trash;
                        tempSS >> trash;
                        if (Utils::IsInteger(trash))
                        {
                            tempNode->type = DataType::Array;
                        }
                        else
                        {
                            tempNode->type = DataType::Class;
                        }

                        tempSS.seekg(current);

                        // if previous token points to an array
                        // then the new array|class shouldn't have a name
                        if (mTempVal->type != DataType::Array)
                        {
                            tempNode->name = tempStr;
                        }
                        tempNode->parent = mTempVal;
                    }
                    
                    mTempVal->childs.push_back(tempNode);
                    mTempVal = tempNode;
                }
                mTempVal = mRoot;
            }

            // Saves current state of file from scratch
            void Save()
            {
                LOGI << "Saving " << mFileName << "...";
                SaveJads es84 = SaveJads(mRoot, mFileName);
            }

			//Prints the data tree from root iterating recursively
			void PrintFromRoot()
            {
                LOGI << "PrintFromRoot() called.";
                PrintJads es84 = PrintJads(mRoot);
            }

            // this two overload searches for the given element
			Jads& operator[](std::wstring_view index)
			{
				bool found = false;
                if (mTempIndex.tellp() == std::streampos(0))
                {
                    for (auto it = mTempVal->childs.begin(); it != mTempVal->childs.end(); ++it)
                    {
                        if ((*it)->name.compare(index) == 0)
                        {
                            mTempVal = *it;
                            found = true;
                            break;
                        }
                    }
                }
				
				if (!found)
				{
					mTempIndex << index << ' ';
				}
				return *this;
			}
			Jads& operator[](size_t index)
			{
                if (mTempIndex.tellp() == std::streampos(0))
                {
                    if (index < mTempVal->childs.size())
                    {
                        mTempVal = mTempVal->childs.at(index);
                    }
                    else
                    {
                        mTempIndex << index << ' ';
                    }
                }
                else
                {
                    mTempIndex << index << ' ';
                }
                
				return *this;
			}

		private:
			std::shared_ptr<DataTree> mRoot;
			std::shared_ptr<DataTree> mTempVal;

			std::wstringstream mTempIndex;

			JadsAnalyzer mAnalyze;
			std::wstring mFileName;
	};
}
#endif