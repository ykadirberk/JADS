#ifndef UTILS_20220420
#define UTILS_20220420

#include <string>

namespace jads
{
	class Utils
	{
		public:
			static bool IsComma(wchar_t ch)
			{
				return (ch == L',');
			}
			static bool IsWhiteSpace(wchar_t ch)
			{
				return (ch == L' ' || ch == L'\t' || IsDeadEnd(ch));
			}
			static bool IsDeadEnd(wchar_t ch)
			{
				return (ch == L'\n');
			}

			static bool IsQuote(wchar_t ch)
			{
				return (ch == L'\"' || ch == L'\'');
			}
			static bool IsBackSlash(wchar_t ch)
			{
				return (ch == L'\\');
			}

			static wchar_t GetEscapeSequence(wchar_t ch)
			{
				switch (ch)
				{
					case L'n': return L'\n';
					case L'\\': return L'\\';
					case L't': return L'\t';
					case L'v': return L'\v';
					case L'\'': return L'\'';
					case L'b': return L'\b';
					case L'\"': return L'\"';
					case L'r': return L'\r';
					case L'0': return L'\0';
					case L'f': return L'\f';
					default: return '\\';
				}
			}

			static bool IsOpenerSquare(wchar_t ch)
			{
				return (ch == L'[');
			}
			static bool IsCloserSquare(wchar_t ch)
			{
				return (ch == L']');
			}
			static bool IsOpenerCurly(wchar_t ch)
			{
				return (ch == L'{');
			}
			static bool IsCloserCurly(wchar_t ch)
			{
				return (ch == L'}');
			}
			static bool IsSquare(wchar_t ch)
			{
				return (IsOpenerSquare(ch) || IsCloserSquare(ch));
			}
			static bool IsCurly(wchar_t ch)
			{
				return (IsOpenerCurly(ch) || IsCloserCurly(ch));
			}
			static bool IsBrace(wchar_t ch)
			{
				return (IsSquare(ch) || IsCurly(ch));
			}

			static bool IsInteger(std::wstring_view mValue)
			{
				for (int i = 0; i < mValue.length(); ++i)
				{
					if (!(mValue[i] <= L'9' && mValue[i] >= L'0' || mValue[i] == L'-'))
					{
						return false;
					}
				}
				return true;
			}
			static bool IsFractional(std::wstring_view mValue)
			{
				for (int i = 0; i < mValue.length(); ++i)
				{
					if (!((mValue[i] <= L'9' && mValue[i] >= L'0') || mValue[i] == L'.' || mValue[i] == L'-'))
					{
						return false;
					}
				}
				return true;
			}
			static bool IsBoolean(std::wstring_view mValue)
			{
				if (mValue.compare(L"true") == 0 || mValue[0] == '1') return true;
				return false;
			}
			static double PowerOf10(int p)
			{
				double tempValue = 1;
				if (p > 0)
				{
					for (int i = 0; i < p; ++i)
					{
						tempValue *= 10;
					}
				}
				if (p < 0)
				{
					for (int i = p; i < 0; ++i)
					{
						tempValue /= 10;
					}
				}

				return tempValue;
			}

			static int AsInteger(std::wstring_view tVal) 
			{
				int tempValue = 0;

				if (IsInteger(tVal))
				{
					size_t length = tVal.length();
					for (int i = 0; i < length; ++i)
					{
						tempValue += static_cast<int>(
							PowerOf10(static_cast<int>(length - i - 1)) * (static_cast<int>(tVal[i]) - L'0')
							);
					}
				}
				return tempValue;
			}
	};
}

#endif