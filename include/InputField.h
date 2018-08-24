#pragma once

class InputField
{
	public:
		static const int MAX_TEXT = 256;

		InputField();

		void Draw();

		void SetPosition(int x, int y);
		void SetLabel(const char* label);

		const char* GetText();

	private:
		int m_x, m_y;
		char m_text[MAX_TEXT];
		const char* m_label;
};

inline void InputField::SetPosition(int x, int y)
{
	m_x = x;
	m_y = y;
}

inline void InputField::SetLabel(const char* label)
{
	m_label = label;
}

inline const char* InputField::GetText()
{
	return m_text;
}
