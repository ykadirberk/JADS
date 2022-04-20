#ifndef JADS_ANALYZER_H_20220404
#define JADS_ANALYZER_H_20220404

#include "DataTree.h"
#include "Utils.h"

#include <fstream>
#include <iostream>
#include <logger/Log.h>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

namespace jads 
{
	enum class TokenType : char {
		None,
		Key,
		Value,
		ClassKey,
		ClassOpener,
		ClassCloser,
		ArrayKey,
		ArrayOpener,
		ArrayCloser,
		EndOfStatement
	};

	class JadsAnalyzer
	{
		public:
			JadsAnalyzer() {}
			JadsAnalyzer(std::wstring_view fileName)
			{
				mFileName = std::wstring(fileName);
				mFile.open(mFileName);
				LOGI << fileName << " set for analyzer.";
			}
			~JadsAnalyzer() {}

			void SetFile(std::wstring_view fileName)
			{
				mFileName = std::wstring(fileName);
				mFile.open(mFileName);
				LOGI << fileName << " set for analyzer.";
			}
			void ReadAndTokenize()
			{
				std::vector<wchar_t> tempToken;
				wchar_t currentChar;
				wchar_t nextChar;
				bool isInsideQuote = false;

				while (!mFile.eof())
				{
					currentChar = static_cast<wchar_t>(mFile.get());
					nextChar = static_cast<wchar_t>(mFile.peek());

					if (isInsideQuote && Utils::IsBackSlash(currentChar))
					{
						currentChar = Utils::GetEscapeSequence(nextChar);
						tempToken.push_back(currentChar);
						mFile.get();
						continue;
					}

					if (Utils::IsQuote(currentChar))
					{
						isInsideQuote = !isInsideQuote;
						continue;
					}

					if (!isInsideQuote && Utils::IsWhiteSpace(currentChar))
					{
						if (!tempToken.empty())
						{
							AddToTokens(tempToken);
						}

						if (Utils::IsDeadEnd(currentChar))
						{
							mTempTokenList.push_back(endOfStatement);
						}
						continue;
					}

					if (!isInsideQuote && Utils::IsBrace(currentChar))
					{
						if (!tempToken.empty())
						{
							AddToTokens(tempToken);
						}
						AddToTokens(currentChar);
						continue;
					}
					tempToken.push_back(currentChar);
				}
				mTempTokenList.push_back(endOfStatement);
			}
			void Analyze()
			{
				for (int i = 0; i < mTempTokenList.size(); ++i) {
					mTokenTypes.push_back(TokenType::None);
				}
				bool keySet = false;
				bool isKeyDefinition = false;
				bool isClassDefinition = false;
				bool isArrayDefinition = false;

				for (int i = 0; i < mTempTokenList.size(); ++i) {
					if (mTokenTypes.at(i) == TokenType::EndOfStatement)
					{
						keySet = false;
						isKeyDefinition = false;
						isClassDefinition = false;
						isArrayDefinition = false;
						continue;
					}

					if (mTokenTypes.at(i) != TokenType::None) {
						continue;
					}

					if (mTempTokenList.at(i).compare(endOfStatement) == 0)
					{
						mTokenTypes[i] = TokenType::EndOfStatement;
						if (!(isClassDefinition || isArrayDefinition))
						{
							isKeyDefinition = true;
						}
						i = FindLastEOS(i);
					}
					else if (isArrayDefinition && !keySet)
					{
						mTokenTypes[i] = TokenType::ArrayKey;
						keySet = true;
					}
					else if (isClassDefinition && !keySet)
					{
						mTokenTypes[i] = TokenType::ClassKey;
						keySet = true;
					}
					else if (isKeyDefinition)
					{
						if (keySet)
						{
							mTokenTypes[i] = TokenType::Value;
						}
						else
						{
							mTokenTypes[i] = TokenType::Key;
							keySet = true;
						}

					}
					else if (Utils::IsOpenerSquare(mTempTokenList.at(i)[0]))
					{
						mTokenTypes[i] = TokenType::ArrayOpener;
						isKeyDefinition = false;
						isClassDefinition = false;
						isArrayDefinition = true;
					}
					else if (Utils::IsOpenerCurly(mTempTokenList.at(i)[0]))
					{
						mTokenTypes[i] = TokenType::ClassOpener;
						isKeyDefinition = false;
						isClassDefinition = true;
						isArrayDefinition = false;
					}
					else if (Utils::IsCloserSquare(mTempTokenList.at(i)[0]))
					{
						mTokenTypes[i] = TokenType::ArrayCloser;
					}
					else if (Utils::IsCloserCurly(mTempTokenList.at(i)[0]))
					{
						mTokenTypes[i] = TokenType::ClassCloser;
					}

				}
			}
			std::shared_ptr<DataTree> GenerateStructure()
			{
				std::shared_ptr<DataTree> root(new DataTree());
				std::shared_ptr<DataTree> tempRoot = root;
				size_t index = 0;

				while (index < mTokenTypes.size())
				{
					if (mTokenTypes.at(index) == TokenType::Key)
					{
						RKey(tempRoot, index);
						continue;
					}

					if (mTokenTypes.at(index) == TokenType::ClassKey)
					{
						RClass(tempRoot, index);
						continue;
					}

					if (mTokenTypes.at(index) == TokenType::ArrayKey)
					{
						RArray(tempRoot, index);
						continue;
					}
					++index;
				}

				return root;
			}


			void PrintTokenList()
			{
				auto it{ mTempTokenList.begin() };
				auto jt{ mTokenTypes.begin() };
				while (it != mTempTokenList.end() || jt != mTokenTypes.end()) {
					LOGI << *it << "\t\t\t" << TokenTypeAsString(*jt);
					++it;
					++jt;
				}
			}

		private:
			std::wifstream mFile;
			std::wstring mFileName;

			std::vector<std::wstring> mTempTokenList;
			std::vector<TokenType> mTokenTypes;

			int FindLastEOS(int i)
			{
				for (int j = i - 1; j >= 0; --j) {
					if (mTempTokenList[j] == endOfStatement)
					{
						return j;
					}
				}
				return -1;
			}

			void AddToTokens(std::vector<wchar_t>& tempToken)
			{
				std::wstring tempTokenWString;
				for (auto it = tempToken.begin(); it != tempToken.end(); ++it) {
					tempTokenWString.push_back(*it);
				}
				mTempTokenList.push_back(tempTokenWString);
				tempToken.clear();
			}
			void AddToTokens(wchar_t tempToken)
			{
				std::wstring tempWStr;
				tempWStr.clear();
				tempWStr.push_back(tempToken);
				mTempTokenList.push_back(tempWStr);
			}

			std::wstring TokenTypeAsString(TokenType type)
			{
				switch (type)
				{
				case TokenType::None: return L"NONE";
				case TokenType::Key: return key;
				case TokenType::Value: return value;
				case TokenType::ClassKey: return classKey;
				case TokenType::ClassOpener: return classOpener;
				case TokenType::ClassCloser: return classCloser;
				case TokenType::ArrayKey: return arrayKey;
				case TokenType::ArrayOpener: return arrayOpener;
				case TokenType::ArrayCloser: return arrayCloser;
				case TokenType::EndOfStatement: return endOfStatement;
				default: throw std::invalid_argument("Unimplemented item");
				}

			}

			void RKey(std::shared_ptr<DataTree> root, size_t& index)
			{
				std::shared_ptr<DataTree> tempKey = std::make_shared<DataTree>();
				tempKey->parent = root;
				if (root->type != DataType::Array)
				{
					tempKey->name = mTempTokenList.at(index);
					++index;
				}
				tempKey->type = DataType::Key;

				std::wstringstream tempValue;
				tempValue.clear();

				while (mTokenTypes.at(index) != TokenType::EndOfStatement)
				{
					tempValue << mTempTokenList.at(index) << ' ';
					++index;
				}

				std::wstring tempValueString = tempValue.str();
				tempValue.clear();
				tempKey->value = JadsVal(tempValueString.substr(0, tempValueString.length() - 1));

				root->childs.push_back(tempKey);
			}
			void RClass(std::shared_ptr<DataTree> root, size_t& index)
			{
				std::shared_ptr<DataTree> tempKey = std::make_shared<DataTree>();
				tempKey->parent = root;
				tempKey->type = DataType::Class;

				if (root->type != DataType::Array)
				{
					tempKey->name = mTempTokenList.at(index);
				}

				++index;

				while (mTokenTypes.at(index) != TokenType::ClassCloser)
				{
					if (mTokenTypes.at(index) == TokenType::Key)
					{
						RKey(tempKey, index);
						continue;
					}

					if (mTokenTypes.at(index) == TokenType::ClassKey)
					{
						RClass(tempKey, index);
						continue;
					}

					if (mTokenTypes.at(index) == TokenType::ArrayKey)
					{
						RArray(tempKey, index);
						continue;
					}
					++index;
				}
				++index;

				root->childs.push_back(tempKey);
			}
			void RArray(std::shared_ptr<DataTree> root, size_t& index)
			{
				std::shared_ptr<DataTree> tempKey = std::make_shared<DataTree>();
				tempKey->parent = root;
				tempKey->type = DataType::Array;

				if (root->type != DataType::Array)
				{
					tempKey->name = mTempTokenList.at(index);
				}

				while (mTokenTypes.at(index) != TokenType::EndOfStatement)
				{
					++index;
				}

				while (mTokenTypes.at(index) != TokenType::ArrayCloser)
				{
					if (mTokenTypes.at(index) == TokenType::Key)
					{
						RKey(tempKey, index);
						continue;
					}

					if (mTokenTypes.at(index) == TokenType::ClassOpener)
					{
						RClass(tempKey, index);
						continue;
					}

					if (mTokenTypes.at(index) == TokenType::ArrayOpener)
					{
						RArray(tempKey, index);
						++index;
						continue;
					}
					++index;
				}

				root->childs.push_back(tempKey);
			}

			const std::wstring key = L"KEY";
			const std::wstring value = L"VALUE";
			const std::wstring classKey = L"CLASS";
			const std::wstring classOpener = L"CLASS_OPENER";
			const std::wstring classCloser = L"CLASS_CLOSER";
			const std::wstring arrayKey = L"ARRAY";
			const std::wstring arrayOpener = L"ARRAY_OPENER";
			const std::wstring arrayCloser = L"ARRAY_CLOSER";
			const std::wstring arrayElementSeperator = L"ARRAY_SEPERATOR";
			const std::wstring endOfStatement = L"__END_OF_STATEMENT__";
	};
}

#endif