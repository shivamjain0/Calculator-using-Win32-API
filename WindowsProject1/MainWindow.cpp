#include "framework.h"
#include "WindowsProject1.h"
#include<string>
#include <sstream>
#include<iostream>
#include"WindowsProject1.cpp"

class CalculationsHistory {
protected:
	int CalculationCountInHis;
	std::wstring history[5];
	int ind;

public:

	CalculationsHistory() {
		ind = -10;
		CalculationCountInHis = 0;
	}

	void OverrideHistory() {

		if (ind == -10 || ind == 4 ) return;
		int curInd = ind;
		
		if (curInd < CalculationCountInHis - 1) {
			for (int i = curInd; i < 5; i++) {
				if (history[i] == L"") break;
				history[i] = L"";
			}
			CalculationCountInHis = curInd ;
		}
	}

	void SetIndexIfScreenCleared() {

		int nonEmptyWStringCount = 0;
		for (const auto& wstr : history) {
			if (!wstr.empty()) {
				++nonEmptyWStringCount;
			}
		}

		ind = nonEmptyWStringCount;
	}

	void DecHisCount() {
		CalculationCountInHis--;
	}

	void IncHisCount() {
		CalculationCountInHis++;
	}

	void AllClrHistoryCount() {
		CalculationCountInHis = 0;
		for (int i = 0; i < 5; i++) {
			history[i] = L"";
		}
	}

	void AddCalculationHistory(std::wstring displayedString) {

		ind = -10;
		int n = CalculationCountInHis;
		if (n == 6) {

			std::wstring temp[5];

			for (int i = 0; i < 4; i++) {
				temp[i] = history[i+1];
			}
			temp[4] = displayedString;


			for (int i = 0; i < 5; i++) {
				history[i] = temp[i];
			}

			DecHisCount();
		}
		else {

			if (n >= 1 && n <= 5 )
				history[n-1] = displayedString;
		}
	}

	BOOL ForwardHistory( HWND hOutput ) {

		int n = CalculationCountInHis;

		if (ind >= CalculationCountInHis-1 )
			return FALSE;

		// No Forward History present
		if ( ind == -10 || ind == 4 )
			return FALSE;

		if (ind >= 0 && ind <= 3) {

			ind++;
			SetWindowTextW(hOutput, history[ind].c_str());

			return TRUE;
		}

		return FALSE;
	}

	BOOL BackwardHistory( HWND hOutput ) {

		if (ind == -10) {
			// subtract 1 -> as we have to display prev value
			ind = CalculationCountInHis-1;
		}

		// No more history present
		if (ind <= -1)
			return FALSE;

		if (ind >= 1 && ind <= 5) {

			ind--;
			SetWindowTextW(hOutput, history[ind].c_str());

			return TRUE;
		}

		return FALSE;
	}
};

class MainWindow : public BaseWindow<MainWindow>
{
public:
	PCWSTR  ClassName() const { return L"Sample Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void addControls(HWND);
	void Resize(HWND);
	void KeysHandling(WPARAM);
	BOOL MouseHandling(WPARAM);
	BOOL IsDisplayStringOutOfLimit(wchar_t displayString[]);
	void DisplayCommas();
	void RemoveCommas();
		
protected:
	std::wstring oprtr[4] = { L"+", L"-", L"*", L"/" };
	int operationToPerform = -1;
	BOOL isEqualToPressed = FALSE;

	HWND hOutput;
	HWND b[6][4];
	HWND HF, HB;
	HFONT hFont;
	HFONT buttonFont[6][4];


	CalculationsHistory obj;
};

void MainWindow::RemoveCommas() {
	wchar_t displayedText[50];
	GetWindowTextW(hOutput, displayedText, 50);
	std::wstring convertedString(displayedText);

	std::wstring s1;
	for (int i = 0; i < convertedString.size(); i++) {
		if (convertedString[i] == L',')
			continue;
		s1 += convertedString[i];
	}

	SetWindowTextW(hOutput, s1.c_str());
}

std::wstring AddCommas(std::wstring str) {

	std::wstring s = str;

	std::wstring result1, stringAfterComma;
	int isFirstCharMinus = 0;

	for (int i = 0; i < s.size(); i++) {

		if (s[i] == L'e') return str;

		if (i == 0 && s[i] == L'-') {
			isFirstCharMinus = 1;
			continue;
		}
		else if (s[i] == L'.') {
			stringAfterComma = s.substr(i);
			break;
		}
		else result1 += s[i];
	}

	s = result1;

	if (s.size() > 3) {

		std::reverse(s.begin(), s.end());

		std::wstring stringWithCommas;
		for (int i = 0; i < s.size(); i++) {
			if (i != 0 && i % 3 == 0) {
				stringWithCommas += L',';
			}
			stringWithCommas += s[i];
		}

		s = stringWithCommas;

		std::reverse(s.begin(), s.end());
	}
	else return str;

	s += stringAfterComma;
	if (isFirstCharMinus) s = L'-'+s;
	return s;
}

void MainWindow::DisplayCommas() {

	wchar_t displayedText[50];
	GetWindowTextW(hOutput, displayedText, 50);
	std::wstring convertedString(displayedText);

	std::wstring s1, s2, op;
	int flag = 0;
	for (int i = 0; i < convertedString.size() ; i++) {

		if (convertedString[i] == L'e') {
			s1 += convertedString[i];
			s1 += convertedString[i+1];
			i++;
			continue;
		}

		if (i > 0 && (convertedString[i] == L'+' || convertedString[i] == L'-'
			|| convertedString[i] == L'*' || convertedString[i] == L'/'))
		{

			// If negation is present with 2nd operand , then skip this condtition
			if (flag == 0) {
				op = convertedString[i];
				flag = 1;
				continue;
			}
		}
		if (!flag)
			s1 += convertedString[i];
		else
			s2 += convertedString[i];

	}

	s1 = AddCommas(s1);
	s2 = AddCommas(s2);

	std::wstring res = s1 + op + s2;
	SetWindowTextW(hOutput, res.c_str());
}

BOOL IsDisplayedValueZero(std::wstring displayedString) {

	int n = (int)(displayedString.size());

	for (int i = 0; i < n; i++) {
		if (displayedString[i] == L'.')
			return FALSE;
	}
	if (n > 0 &&  ( displayedString[n-1] == L'+' || displayedString[n - 1] == L'+' ||
		               displayedString[n - 1] == L'-' || displayedString[n - 1] == L'*' || displayedString[n - 1] == L'/')) {
		return FALSE;
	}

	float result;
	std::wstringstream(displayedString) >> result;

	if ( result == 0.0f  )
		return TRUE;
	return FALSE;
}

int CheckForNumOfDotsInOneOperand( std::wstring displayedString ) {
	int countOfDots = 0;
	int size = (int)displayedString.size();
	for (int i = 0; i < size; i++) {
		if (displayedString[i] == L'.')
			countOfDots++;

		if (displayedString[i] == L'+' || displayedString[i] == L'-'
			|| displayedString[i] == L'*' || displayedString[i] == L'/')
			countOfDots = 0;
		if (countOfDots > 1)
			return TRUE;
	}

	if (countOfDots == 1) return TRUE;
	return FALSE;
}

void MainWindow::Resize( HWND m_hwnd) {
	RECT rect;
	GetClientRect(m_hwnd,&rect);

	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	int xMargin = static_cast<int>(0.02*windowWidth);
	int yMarign = static_cast<int>(0.015*windowHeight);

	int xCoordinate = xMargin;
	int yCoordinate = static_cast<int>(0.1814*windowHeight);

	int buttonWidth = static_cast<int>(0.225*windowWidth);
	int buttonHeight = static_cast<int>(0.1214*windowHeight);

	int dispalyStringHeight = static_cast<int>(0.1214*windowHeight);
	int dispalyStringWidth = static_cast<int>(0.96*windowWidth);


	for (int i = 0; i < 6; i++) {

		xCoordinate = xMargin;

		for (int j = 0; j < 4; j++) {

			SetWindowPos(b[i][j], NULL, 
				rect.left + xCoordinate, 
				rect.top + yCoordinate, 
				buttonWidth,
				buttonHeight,
				SWP_NOZORDER | SWP_NOREPOSITION);

			xCoordinate += buttonWidth + xMargin;
		}
		yCoordinate += buttonHeight + yMarign;
	}

	SetWindowPos(hOutput, NULL,
		rect.left + static_cast<int>(0.02*windowWidth),
		rect.top + static_cast<int>(0.015*windowHeight),
		static_cast<int>(0.96*windowWidth),
		static_cast<int>(0.1214*windowHeight),
		SWP_NOZORDER | SWP_NOREPOSITION);

	
	// Adjusting the size of the dispalying characters w.r.t the resizing and adding new characters
	// *****************************************************************************************************************

	int dispalyStringFontHeight = static_cast<int>(0.1150*windowHeight);
	int dispalyStringFontWidth = static_cast<int>((dispalyStringFontHeight*4)/10);

	wchar_t displayedText[50];
	GetWindowTextW(hOutput, displayedText, 50);
	int noOfDisplayedchar = (int)wcslen(displayedText);

	int res = static_cast<int>(noOfDisplayedchar* dispalyStringFontWidth);

	while ( res > static_cast<int>((dispalyStringWidth*75)/100) ) {

		dispalyStringFontWidth -= 1; 
		res = static_cast<int>(noOfDisplayedchar* dispalyStringFontWidth);
	}

	res /= (noOfDisplayedchar > 0)?noOfDisplayedchar: noOfDisplayedchar+1; // Add 1 to handle 
	res = static_cast<int>((res*10)/4);

	// *****************************************************************************************************************

	hFont = CreateFontW(res, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"Arial");
	if (hFont != NULL)
	{
		SendMessage(hOutput, WM_SETFONT, (WPARAM)hFont, TRUE);
	}

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			buttonFont[i][j] = CreateFontW(static_cast<int>((windowWidth/35 + windowHeight/25 )), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE, L"Arial");

			if (buttonFont[i][j] != NULL)
			{
				SendMessage(b[i][j], WM_SETFONT, (WPARAM)buttonFont[i][j], TRUE);
			}
		}
	}

}

std::wstring formatFloat(float value) {
	std::wstringstream wss;
	wss << value;

	// Convert the string back to a float to remove trailing zeros
	float formattedValue;
	wss >> formattedValue;

	std::wstringstream formattedWss;
	formattedWss << formattedValue;

	return formattedWss.str();
}

BOOL MainWindow::IsDisplayStringOutOfLimit(wchar_t displayString[]) {
	/*
	RemoveCommas();


	wchar_t displayedText[50];
	GetWindowTextW(hOutput, displayedText, 50);

	int length = (int)wcslen(displayedText);
	if (length > 30) {
		DisplayCommas();
		return TRUE;
	}
	DisplayCommas();
	return FALSE;
	*/
	int length = (int)wcslen(displayString);
	if (length > 32) return TRUE;
	return FALSE;
}

BOOL MainWindow::MouseHandling(WPARAM wParam) {

	int buttonId = static_cast<int>(wParam);

	// Functionality for Numeric Values
	if (buttonId >= 0 && buttonId <= 9) {

		wchar_t concatenatedString[50];

		wchar_t displayedText[50];
		GetWindowTextW(hOutput, displayedText, 50);

		// Check for Displayed String Limit
		if (IsDisplayStringOutOfLimit(displayedText)) return TRUE;

		// If Equals To is pressed that make displayed text empty
		if (isEqualToPressed) {
			wcsncpy_s(displayedText, L"", sizeof(displayedText) / sizeof(displayedText[0]));
			obj.SetIndexIfScreenCleared();
			isEqualToPressed = FALSE;
		}

		else if (IsDisplayedValueZero(displayedText)) {
			wcsncpy_s(displayedText, L"", sizeof(displayedText) / sizeof(displayedText[0]));
		}

		swprintf_s(concatenatedString, L"%s%d", displayedText, buttonId);

		SetWindowTextW(hOutput, concatenatedString);

		RemoveCommas();
		DisplayCommas();
	}

	// Functionality for Operators
	else if (buttonId >= 14 && buttonId <= 17) {

		wchar_t displayedText[50];
		GetWindowTextW(hOutput, displayedText, 50);
		std::wstring displayedString(displayedText);
		
		// Check for Displayed String Limit
		if (IsDisplayStringOutOfLimit(displayedText)) return TRUE;

		int displayedTextLength = (int)displayedString.size();

		// Check if last char is operator then override it , if any other operator is pressed
		int flag = 0;
		if ((displayedTextLength > 2 && displayedText[0] == L'-' ) || (displayedTextLength > 1 && displayedText[0] != L'-')) {

			int lastIndex = displayedTextLength - 1;
			if (displayedText[lastIndex] == L'+' || displayedText[lastIndex] == L'-'
				|| displayedText[lastIndex] == L'*' || displayedText[lastIndex] == L'/') {

				displayedString.erase(displayedTextLength - 1);
				flag = 1;
			}
			isEqualToPressed = FALSE;
		}

		int count = 0;
		if (displayedTextLength > 1) {
			int i = 0;
			if (displayedText[0] == L'-') i = 1;

			for (i; i < displayedTextLength; i++) {

				if (displayedText[i] == L'e') { i++; continue; }

				if ((displayedText[i] == L'+' || displayedText[i] == L'-'
					|| displayedText[i] == L'*' || displayedText[i] == L'/') && i != displayedTextLength - 1)
					return FALSE;
			}
		}

		int oprtrID = buttonId - 14;
		operationToPerform = oprtrID;

		std::wstring concatenatedString = displayedString + oprtr[oprtrID];
		LPCWSTR lpwstr = concatenatedString.c_str();
		SetWindowTextW(hOutput, lpwstr);

		isEqualToPressed = FALSE;
	}

	// Functionality for Equals To operator
	else if (buttonId == 13) {

		RemoveCommas();

		wchar_t displayedText[50];
		GetWindowTextW(hOutput, displayedText, 50);
		std::wstring convertedString(displayedText);

		std::wstring s1 = L"", s2 = L"";

		int flag = 0;
		for (int i = 0; i < convertedString.size() ; i++) {

			if (displayedText[i] == L'e') {
				s1 += convertedString[i];
				s1 += convertedString[i+1];
				i++;
				continue;
			}

			if (i > 0 && (displayedText[i] == L'+' || displayedText[i] == L'-'
				|| displayedText[i] == L'*' || displayedText[i] == L'/'))
			{

				// If negation is present with 2nd operand , then skip this condtition
				if (flag == 0) {
					flag = 1;
					continue;
				}
			}
			if (!flag)
				s1 += convertedString[i];
			else
				s2 += convertedString[i];

		}
		if (flag == 0) return 0;
		if (s2 == L"") { 
			DisplayCommas();
			return TRUE; 
		}

		float val1;
		float val2;

		std::wstringstream(s1.c_str()) >> val1;
		std::wstringstream(s2.c_str()) >> val2;

		float result;
		if (operationToPerform == 0)
			result = val1 + val2;
		else if (operationToPerform == 1)
			result = val1 - val2;
		else if (operationToPerform == 2)
			result = val1 * val2;
		else if (operationToPerform == 3) {
			if (val2 == 0.0f) {
				DisplayCommas();
				MessageBox(NULL, L"ERROR: Cannot divide by zero", L"ERROR", MB_OK);
				return TRUE;
			}
			result = val1 / val2;
		}

		std::wstring s = formatFloat(result);
		SetWindowTextW(hOutput, s.c_str());

		obj.OverrideHistory();
		obj.IncHisCount();
		obj.AddCalculationHistory(s.c_str());

		isEqualToPressed = TRUE;
		Resize(m_hwnd);

		DisplayCommas();
		return TRUE;
	}

	// Functionality for BackSpace
	else if (buttonId == 18) {

		// After Equal to operation , text is not back spaced
		if (isEqualToPressed) return TRUE;

		wchar_t displayedText[50];
		GetWindowTextW(hOutput, displayedText, 50);
		if (!displayedText) return TRUE;

		int length = (int)wcslen(displayedText);

		// Check if the string is not empty
		if (length > 0) {
			// Replace the last character with the null terminator
			displayedText[length - 1] = L'\0';

			// If operator is present with the numeric to clear, then clear the operator also
			if (wcslen(displayedText) > 0) {
				if (displayedText[length - 2] == L'+' || displayedText[length - 2] == L'-') 
					displayedText[length - 2] = L'\0';
			}
		}

		if (displayedText[0] == L'\0') displayedText[0] = L'0';
		SetWindowTextW(hOutput, displayedText); 

		RemoveCommas();
		DisplayCommas();
	}

	// Functionality for Dot
	else if (buttonId == 12) {
		wchar_t concatenatedString[20];

		wchar_t displayedText[20];
		GetWindowTextW(hOutput, displayedText, 20);

		// Check for Displayed String Limit
		if (IsDisplayStringOutOfLimit(displayedText)) return TRUE;

		if (isEqualToPressed) {
			SetWindowTextW(hOutput, L".");
			//wcsncpy_s(displayedText, L"", sizeof(displayedText) / sizeof(displayedText[0]));
			isEqualToPressed = FALSE;
			return TRUE;
		}

		std::wstring displayedString(displayedText);
		if (CheckForNumOfDotsInOneOperand(displayedString)) return TRUE;

		// Fucntion to concatenate two wchar_t
		swprintf_s(concatenatedString, L"%s%s", displayedText, L".");

		SetWindowTextW(hOutput, concatenatedString);
	}

	// Functionality for C (Clear) 
	else if (buttonId == 11) {
		SetWindowTextW(hOutput, L"0");
		obj.SetIndexIfScreenCleared();
	}

	// Functionality for Forward History 
	else if (buttonId == 19) {
	obj.ForwardHistory(hOutput);
	}

	// Functionality for Backward History 
	else if (buttonId == 20) {
		obj.BackwardHistory(hOutput);
	}

	// Functionality for All Clear History
	else if (buttonId == 10) {
		obj.AllClrHistoryCount();
		SetWindowText(hOutput, L"0");
	}

	// Functionality for Square Root
	else if (buttonId == 22) {

		RemoveCommas();

		wchar_t displayedText[50];
		GetWindowTextW(hOutput, displayedText, 50);
		std::wstring displayedString(displayedText);

		std::wstring res;
		int displayedTextLength = (int)displayedString.size();

		// If displayed string is empty, then do nothing and return
		if (displayedTextLength == 0) return TRUE;

		// If only 1 value is present and its operator, then do nothing and return
		if (displayedTextLength == 1 && (displayedText[0] == L'+' || displayedText[0] == L'-'
			|| displayedText[0] == L'*' || displayedText[0] == L'/')) 
			return TRUE;

		int i = 0;
		int flag = 0;
		// If number is negative, then make flag = 1
		if (displayedTextLength > 1 && displayedText[0] == L'-') {
			i = 1;
			flag = 1;
		}

		for ( i ; i < displayedTextLength; i++ ) {

			if (displayedText[i] == L'e') {
				res += displayedText[i];
				res += displayedText[i + 1];
				i++;
				continue;
			}

			if (displayedText[i] == L'+' || displayedText[i] == L'-'
			|| displayedText[i] == L'*' || displayedText[i] == L'/') {

				if (i == displayedTextLength - 1)
					continue;
				else {
					DisplayCommas();
					return TRUE;
				}
			}

			res += displayedText[i];
		}

		float finalRes;
		std::wstringstream(res.c_str()) >> finalRes;

		if (flag) finalRes = finalRes *(-1);

		finalRes = std::sqrt(finalRes);
		std::wstring s = formatFloat(finalRes);
		SetWindowTextW(hOutput, s.c_str());

		DisplayCommas();

		obj.OverrideHistory();
		obj.IncHisCount();
		obj.AddCalculationHistory(s.c_str());

		isEqualToPressed = TRUE;
	}

	// Functionality for Square a number
	else if (buttonId == 23) {

		RemoveCommas();

		wchar_t displayedText[50];
		GetWindowTextW(hOutput, displayedText, 50);
		std::wstring displayedString(displayedText);

		std::wstring res;
		int displayedTextLength = (int)displayedString.size();

		// If displayed string is empty, then do nothing and return
		if (displayedTextLength == 0) return TRUE;

		// If only 1 value is present and its operator, then do nothing and return
		if (displayedTextLength == 1 && (displayedText[0] == L'+' || displayedText[0] == L'-'
			|| displayedText[0] == L'*' || displayedText[0] == L'/'))
			return TRUE;

		for ( int i = 0; i < displayedTextLength; i++ ) {

			if (displayedText[i] == L'e') {
				res += displayedText[i];
				res += displayedText[i + 1];
				i++;
				continue;
			}

			if (displayedText[i] == L'+' || displayedText[i] == L'-'
				|| displayedText[i] == L'*' || displayedText[i] == L'/') {

				// Do nothing if operator is at the starting or at the end
				if ( i == 0 || i == displayedTextLength - 1)
					continue;
				else {
					DisplayCommas();
					return TRUE;
				}
			}

			res += displayedText[i];
		}

		float finalRes;
		std::wstringstream(res.c_str()) >> finalRes;

		finalRes *= finalRes;
		std::wstring s = formatFloat(finalRes);
		SetWindowTextW(hOutput, s.c_str());

		DisplayCommas();

		obj.OverrideHistory();
		obj.IncHisCount();
		obj.AddCalculationHistory(s.c_str());

		isEqualToPressed = TRUE;
	}

	// Functionality for Negation
	else if (buttonId == BNEGATION) {

		wchar_t displayedText[50];
		GetWindowTextW(hOutput, displayedText, 50);
		std::wstring displayedString(displayedText);

		// Check for Displayed String Limit
		if (IsDisplayStringOutOfLimit(displayedText)) return TRUE;

		int displayedTextLength = (int)displayedString.size();
		// If displayed string is empty, then do nothing and return
		if (displayedTextLength == 0) return TRUE;

		if (displayedTextLength > 1 && (displayedText[displayedTextLength - 1] == L'+' || displayedText[displayedTextLength - 1] == L'-'
			|| displayedText[displayedTextLength - 1] == L'*' || displayedText[displayedTextLength - 1] == L'/')) {
			return TRUE;
		}

		std::wstring s1 = L"", s2 = L"";
		int flag = 0;
		for (int i = 0; i < displayedString.size(); i++) {

			if (displayedText[i] == L'e') {
				s1 += displayedString[i];
				s1 += displayedString[i+1];
				i++;
				continue;
			}

			if (i > 0 && (displayedText[i] == L'+' || displayedText[i] == L'-'
				|| displayedText[i] == L'*' || displayedText[i] == L'/'))
			{
				if (flag != 1) {
					flag = 1;
					s1 += displayedString[i];
					continue;
				}
			}
			if (!flag)
				s1 += displayedString[i];
			else
				s2 += displayedString[i];

		}

		if (s2 == L"") {
			if (flag == 1) return TRUE;
			else {
				if (s1[0] == L'-') s1 = s1.substr(1);
				else s1 = L"-" + s1;
			}
		}
		else {
			if (s2[0] == L'-') s2 = s2.substr(1);
			else s2 = L"-" + s2;
		}

		std::wstring res = s1 + s2;
		SetWindowTextW(hOutput, res.c_str());
	}

	Resize(m_hwnd);
	return TRUE;
}

void MainWindow::KeysHandling(WPARAM w) {
		WPARAM wp;
		switch (w)
		{
			case VK_NUMPAD0:
			case 0x30:
				wp = B0;
				break;

			case VK_NUMPAD1:
			case  0x31:
				wp = B1;
				break;

			case VK_NUMPAD2:
			case 0x32:
				wp = B2;
				break;

			case VK_NUMPAD3:
			case 0x33:
				wp = B3;
				break;

			case VK_NUMPAD4:
			case 0x34:
				wp = B4;
				break;

			case VK_NUMPAD5:
			case 0x35:
				wp = B5;
				break;

			case VK_NUMPAD6:
			case 0x36:
				wp = B6;
				break;

			case VK_NUMPAD7:
			case 0x37:
				wp = B7;
				break;

			case VK_NUMPAD8:
			case 0x38:
				wp = B8;
				break;

			case VK_NUMPAD9:
			case 0x39:
				wp = B9;
				break;
			case VK_MULTIPLY:
				wp = BMULTI;
				break;
			case VK_ADD:
				wp = BADD;
				break;

			case VK_SUBTRACT:
				wp = BSUB;
				break;

			case VK_DIVIDE:
				wp = BDIV;
				break;

			case VK_DECIMAL:
				wp = BDOT;
				break;

			case VK_RETURN:
				wp = BEQUAL;
				break;

			case VK_BACK:
				wp = BBKS;
				break;

			default:
				return;
		}
		MouseHandling(wp);
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		addControls(m_hwnd);
		break;

	case WM_COMMAND:
		if (wParam < 24) {
			MouseHandling(wParam);
			SetFocus(m_hwnd);
			break;
		}
		else 
			return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		break;

	case WM_GETMINMAXINFO:
	{
		int RECT_WIDTH = 480, RECT_HEIGHT = 590;
		MINMAXINFO* minMaxInfo = reinterpret_cast<MINMAXINFO*>(lParam);

		minMaxInfo->ptMinTrackSize.x = RECT_WIDTH;
		minMaxInfo->ptMinTrackSize.y = RECT_HEIGHT;

		return 0;
	}

	case WM_KEYDOWN:
		KeysHandling(wParam);
		break;

	case WM_SIZE:
		Resize(m_hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(m_hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(m_hwnd, &ps);
	}
	return 0;

	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
	return TRUE;
}

void MainWindow::addControls(HWND hWnd) {

	hFont = CreateFontW(70, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"Arial");

	if (hFont != NULL)
	{
		hOutput = CreateWindowW(L"STATIC", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_RIGHT | ES_AUTOHSCROLL,
			20, 10, 430, 70, hWnd, NULL, NULL, NULL);

		SendMessage(hOutput, WM_SETFONT, (WPARAM)hFont, TRUE);
	}

	const wchar_t *buttons[6][4] = { {L"AC",L"C",L"\u232b",L"±"},
									 {L"x²",L"√",L"HB",L"HF"},
									 {L"1",L"2",L"3",L"/"},
									 {L"4",L"5",L"6",L"*"},
									 {L"7",L"8",L"9",L"-"},
									 {L".",L"0",L"=",L"+"} };

	int buttonIDs[6][4] = { {BAC,BC,BBKS,BNEGATION},
							{BSQ,BSQRT,BBKWHIS,BFWHIS},
							{B1,B2,B3,BDIV},
							{B4,B5,B6,BMULTI},
							{B7,B8,B9,BSUB},
							{BDOT,B0,BEQUAL,BADD} };

	int margin = 10;
	int x = 20, y = 120;

	for (int i = 0; i < 6; i++) {
		x = 20;
		for (int j = 0; j < 4; j++) {

			b[i][j] = CreateWindowW(L"Button", buttons[i][j], WS_VISIBLE | WS_CHILD | WS_BORDER, x, y, 100, 60,
				hWnd, (HMENU)buttonIDs[i][j], NULL, NULL);
			x += 100 + margin;
		}
		y += 60 + margin;
	}

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			buttonFont[i][j] = CreateFontW(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE, L"Arial");

			if (buttonFont[i][j] != NULL)
			{
				SendMessage(b[i][j], WM_SETFONT, (WPARAM)buttonFont[i][j], TRUE);
			}
		}
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	MainWindow win;

	if (!win.Create(L"Learn to Program Windows", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}