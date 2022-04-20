#ifndef JADS_VAL_20220405
#define JADS_VAL_20220405

#include "Utils.h"

#include <iostream>
#include <logger/Log.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace jads
{
	class JadsVal
	{
		public:
			JadsVal()
			{
				mValue.clear();
			}
			JadsVal(std::wstring_view val)
			{
				mValue = std::wstring(val);
			}

			void SetValue(std::wstring_view newVal)
			{
				mValue = std::wstring(newVal);
			}

			std::wstring AsString()
			{
				return mValue;
			}
			wchar_t AsCharacter()
			{
				return mValue[0];
			}
			int AsInteger()
			{
				int tempValue = 0;

				if (Utils::IsInteger(mValue))
				{
					size_t length = mValue.length();

					size_t negative = 0;
					if (mValue[0] == L'-')
					{
						negative = 1;
					}

					for (int i = 0; i + negative < length; ++i)
					{
						tempValue += static_cast<int>(
							Utils::PowerOf10(static_cast<int>(length - 1 - i - negative))
							* (static_cast<int>(mValue[i + negative]) - L'0')
							);
					}
					if (negative) tempValue *= -1;
				}
				else if (Utils::IsFractional(mValue))
				{
					size_t pointOfDot = mValue.find(L'.');

					size_t negative = 0;
					if (mValue[0] == L'-')
					{
						negative = 1;
					}
					for (size_t i = 0; i + negative < pointOfDot; ++i)
					{
						tempValue += static_cast<int>(
							Utils::PowerOf10(static_cast<int>(pointOfDot - 1 - i - negative))
							* (static_cast<int>(mValue[i + negative]) - L'0')
							);
					}
					if (negative) tempValue *= -1;
				}

				return tempValue;
			}
			double AsDouble()
			{
				double tempValue = 0.0f;
				if (Utils::IsFractional(mValue))
				{
					size_t pointOfDot = mValue.find(L'.');

					if (pointOfDot == std::wstring::npos)
					{
						pointOfDot = mValue.length();
					}

					size_t negative = 0;
					if (mValue[0] == L'-')
					{
						negative = 1;
					}

					for (int i = 0; i + negative < pointOfDot; ++i)
					{
						tempValue += ((mValue[i + negative]) - L'0') * Utils::PowerOf10(static_cast<int>(pointOfDot - i - 1 - negative));
					}
					for (int i = static_cast<int>(pointOfDot) + 1; i + negative < mValue.length(); ++i)
					{
						tempValue += ((mValue[i + negative]) - L'0') * Utils::PowerOf10(static_cast<int>(pointOfDot - i - negative));
					}
					if (negative) tempValue *= -1;
				}

				return tempValue;
			}
			float AsFloat()
			{
				float tempValue = 0.0f;
				if (Utils::IsFractional(mValue))
				{
					size_t pointOfDot = mValue.find(L'.');

					if (pointOfDot == std::wstring::npos)
					{
						pointOfDot = mValue.length();
					}

					size_t negative = 0;
					if (mValue[0] == L'-')
					{
						negative = 1;
					}

					for (int i = 0; i + negative < pointOfDot; ++i)
					{
						tempValue += static_cast<float>(
							(mValue[i + negative] - L'0') * Utils::PowerOf10(static_cast<int>(pointOfDot - i - 1 - negative))
							);
					}
					for (int i = static_cast<int>(pointOfDot) + 1; i + negative < mValue.length(); ++i)
					{
						tempValue += static_cast<float>(
							(mValue[i + negative] - L'0') * Utils::PowerOf10(static_cast<int>(pointOfDot - i - negative))
							);
					}
					if (negative) tempValue *= -1;
				}
				return tempValue;
			}
			bool AsBoolean()
			{
				return Utils::IsBoolean(mValue);
			}

		private:
			std::wstring mValue;
	};
}



#endif

